#include "app.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>

using namespace glm;

vec3 toVec3(const std::string& str)
{
    std::stringstream ss(str);
    vec3 result;
    ss >> result.x;
    ss >> result.y;
    ss >> result.z;
    std::cout << result.x << " " << result.y << " " << result.z << std::endl;
    return result;
}

void App::setValue(const std::string& param, const std::string& value)
{
    std::cout << param << "|" << value << std::endl;

    if (param == "roughness")
        roughness = std::stof(value);
    else if (param == "F0")
        F0 = toVec3(value);
    else if (param == "kd")
        kd = toVec3(value);
    else if (param == "diff")
        showDifference = (value == "true");
    else if (param == "leftD")
        leftD = value;
    else if (param == "leftG")
        leftG = value;
    else if (param == "rightD")
        rightD = value;
    else if (param == "rightG")
        rightG = value;
    else if (param == "gamma")
        gamma = std::stof(value);
    else if (param == "mesh")
        currentMeshInd = (value == "Walt")? 0 : 1;

    assert(roughness >= 0.f and roughness <= 1.f);
    assert(gamma >= 1.f and gamma <= 2.5f);

    std::vector<std::string>::const_iterator itD, itG;
    itD = std::find(Dnames.begin(), Dnames.end(), leftD);
    itG = std::find(Gnames.begin(), Gnames.end(), leftG);
    assert(itD != Dnames.end());
    assert(itG != Gnames.end());
    leftShaderIndex = std::distance(Dnames.begin(), itD)*numGs +
                      std::distance(Gnames.begin(), itG);

    itD = std::find(Dnames.begin(), Dnames.end(), rightD);
    itG = std::find(Gnames.begin(), Gnames.end(), rightG);
    assert(itD != Dnames.end());
    assert(itG != Gnames.end());
    rightShaderIndex = std::distance(Dnames.begin(), itD)*numGs +
                       std::distance(Gnames.begin(), itG);

    std::cout << leftShaderIndex << " " << rightShaderIndex << std::endl;
}

App::~App()
{
    delete renderer;
}

bool App::checkPlatform()
{
    return true;
}

bool App::setup()
{
    const bool platformOk = checkPlatform();
    if (!platformOk)
        return false;

    renderer = new Renderer(canvasWidth, canvasHeight);

    meshes[0]     = renderer->addMesh("assets/walt.rawmesh");
    meshes[1]     = renderer->addMesh("assets/icosphere.rawmesh");
    tonemapShader = renderer->addShader({}, {"assets/tonemap.fs"});
    diffShader    = renderer->addShader({}, {"assets/diff.fs"});

    for (int i = 0; i < Dfiles.size(); i++) {
        for (int j = 0; j < Gfiles.size(); j++) {
            std::vector<std::string> fsParts = {
                "assets/" + Dfiles[i] + ".part",
                "assets/" + Gfiles[j] + ".part",
                "assets/Fschlick.part",
                "assets/mesh.fs"};
            meshShaders.push_back(renderer->addShader({"assets/mesh.vs"}, fsParts));
            std::cout << (i*numGs + j) << " " << Dnames.at(i) << " " << Gnames.at(j) << std::endl;
        }
    }
    setValue("dummy", "dummy");

    framebuffer = renderer->addFramebuffer();
    fbColor = renderer->addEmptyTexture(canvasWidth, canvasHeight, PixelFormat::Rgb, PixelType::Float);
    fbDepth = renderer->addRenderbuffer(canvasWidth, canvasHeight, PixelFormat::Depth16);
    renderer->attachTextureToFramebuffer(framebuffer, fbColor);
    renderer->attachRenderbufferToFramebuffer(framebuffer, fbDepth);
    return true;
}

void App::drawFrame()
{
    renderer->liveReloadUpdate();

    const vec3 worldUp = vec3(0.f, 1.f, 0.f);
    const vec3 cameraPosition = cameraR * vec3(
            sin(cameraTheta)*sin(cameraPhi),
            cos(cameraTheta),
            sin(cameraTheta)*cos(cameraPhi)
            );
    const mat4 view = glm::lookAt(cameraPosition,
                             vec3(0.f, 0.f, 0.f),
                             worldUp);
    const int viewportWidth = 512;
    const int viewportHeight = 512;
    const float ratio = static_cast<float>(viewportWidth) / viewportHeight;
    const mat4 projection = glm::perspective(60.f, ratio, 0.5f, 100.f);
    const mat4 mvp = projection * view;

    renderer->setFramebuffer(framebuffer);
    renderer->setViewport(0, 0, canvasWidth, canvasHeight);
    glClearColor(0.95f, 0.95f, 0.95f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    renderer->setViewport(0,0, viewportWidth,viewportHeight);
    renderer->setShader(meshShaders[leftShaderIndex]);
    renderer->setUniform4x4fv("mvp", 1, &mvp[0][0]);
    renderer->setUniform3fv("viewOrigin", 1, &cameraPosition[0]);
    renderer->setUniform3fv("F0", 1, &F0[0]);
    renderer->setUniform1f("roughness", roughness);
    renderer->setUniform3fv("kd", 1, &kd[0]);
    renderer->drawMesh(meshes[currentMeshInd]);

    renderer->setViewport(viewportWidth,0, viewportWidth,viewportHeight);
    renderer->setShader(meshShaders[rightShaderIndex]);
    renderer->setUniform4x4fv("mvp", 1, &mvp[0][0]);
    renderer->setUniform3fv("viewOrigin", 1, &cameraPosition[0]);
    renderer->setUniform3fv("F0", 1, &F0[0]);
    renderer->setUniform1f("roughness", roughness);
    renderer->setUniform3fv("kd", 1, &kd[0]);
    renderer->drawMesh(meshes[currentMeshInd]);

    // Post
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    renderer->setDefaultFramebuffer();
    renderer->setViewport(0,0, viewportWidth,viewportHeight);
    renderer->setShader(tonemapShader);
    renderer->setTexture(0, fbColor);
    renderer->setUniform1i("fb", 0);
    vec2 uvOff = vec2(0.f, 0.f);
    renderer->setUniform2fv("uvOff", 1, &uvOff[0]);
    renderer->drawScreenQuad();

    renderer->setViewport(viewportWidth,0, viewportWidth,viewportHeight);
    if (showDifference) {
        renderer->setShader(diffShader);
        renderer->setTexture(0, fbColor);
        renderer->setUniform1i("fb", 0);
        renderer->drawScreenQuad();
    }
    else {
        renderer->setShader(tonemapShader);
        renderer->setTexture(0, fbColor);
        renderer->setUniform1i("fb", 0);
        uvOff = vec2(0.5f, 0.f);
        renderer->setUniform2fv("uvOff", 1, &uvOff[0]);
        renderer->setUniform1f("gamma", gamma);
        renderer->drawScreenQuad();
    }
}

void App::onKey(int key, int action)
{
}

void App::onChar(int key, int action)
{
}

void App::onMousePos(int x, int y)
{
    if (dragging) {
        float dx = static_cast<float>(x-mouseStartX);
        float dy = static_cast<float>(y-mouseStartY);
        mouseStartX = x;
        mouseStartY = y;
        cameraPhi   -= (dx / canvasWidth)  * TwoPI;
        cameraTheta -= (dy / canvasHeight) * PI;
        cameraTheta = glm::clamp(cameraTheta, 0.001f, PI-0.001f);
    }
}

void App::onMouseButton(int button, int action)
{
    if (action == GLFW_PRESS and button == GLFW_MOUSE_BUTTON_LEFT) {
        int xpos, ypos;
        glfwGetMousePos(&xpos, &ypos);
        mouseStartX = xpos;
        mouseStartY = ypos;
        dragging = true;
    }
    else {
        dragging = false;
    }
}

void App::onMouseWheel(int pos)
{
}
