#ifndef __APP_HPP__
#define __APP_HPP__

#include "renderer.hpp"

#include <GL/glew.h>
#include <GL/glfw.h>
#ifdef EMSCRIPTEN
#include <emscripten/emscripten.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class App {
public:
    App(int canvasWidth, int canvasHeight): canvasWidth(canvasWidth), canvasHeight(canvasHeight) {}
    ~App();

    bool checkPlatform();
    bool setup();
    void drawFrame();

    void onKey(int key, int action);
    void onChar(int key, int action);
    void onMousePos(int x, int y);
    void onMouseButton(int button, int action);
    void onMouseWheel(int pos);

    void setValue(const std::string& param, const std::string& value);

private:
    int canvasWidth, canvasHeight;
    int mouseStartX, mouseStartY;
    bool dragging = false;
    float cameraPhi   = 0.f;
    float cameraTheta = PI2;
    float cameraR     = 3.f;

    const std::vector<std::string> Dnames = {"Ashikhmin-Shirley", "Beckmann", "Blinn", "Trowbridge-Reitz"};
    const std::vector<std::string> Gnames = {"Beckmann-Walter", "Implicit", "Cook-Torrance", "Trowbridge-Reitz-Walter",
                                             "Kelemen-Kalos"};
    const std::vector<std::string> Dfiles = {"Dashikhmin", "Dbeckmann", "Dblinn", "Dtr"};
    const std::vector<std::string> Gfiles = {"Gbeckmannwalter", "Gimplicit", "Gcooktorrance", "Gtrwalter", "Gkelemen"};
    const int numDs = Dfiles.size();
    const int numGs = Gfiles.size();
    std::string leftD = "Blinn";
    std::string leftG = "Implicit";
    std::string rightD = "Blinn";
    std::string rightG = "Implicit";
    int leftShaderIndex, rightShaderIndex;

    Renderer* renderer = nullptr;
    MeshID meshes[3];
    int currentMeshInd = 0;
    FramebufferID framebuffer;
    TextureID fbColor;
    RenderbufferID fbDepth;
    ShaderID tonemapShader;
    ShaderID diffShader;
    std::vector<ShaderID> meshShaders;
    bool showDifference = false;
    float roughness = 0.5f;
    glm::vec3 F0 = glm::vec3(0.05f);
    glm::vec3 kd = glm::vec3(0.42f, 0.008f, 0.008f);
    float gamma = 2.2f;

    std::string cmd, previousCmd;
};

#endif
