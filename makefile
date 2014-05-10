emscripten:
	emcc main.cpp app.cpp common.cpp renderer.cpp stb_image.cpp -s TOTAL_MEMORY=134217728 -s EXPORTED_FUNCTIONS="['_main','_setAppValue']" -o build/index_plain.html -std=c++11 -I. --preload-file assets
