cp glfw3_x86-64.dll glfw3.dll
cp glew32_x86-64.dll glew32.dll
gcc -w -std=c++0x -I. -I GLEW/include/ -I GLFW/include/ -I GLM/include/ -I SOIL2/include/ -o main.exe ./Application.cpp ./GLM/include/glm/detail/glm.cpp ./SOIL2/include/SOIL2.c ./SOIL2/include/image_DXT.c ./SOIL2/include/image_helper.c ./SOIL2/include/wfETC.c -L. -lglfw3 -lglew32 -lopengl32 -lstdc++  
