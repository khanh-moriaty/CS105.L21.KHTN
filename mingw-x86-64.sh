gcc -w -I. -I GLEW/include/ -I GLFW/include/ -I GLM/include/ -I SOIL2/include/ -o main.exe ./Application.cpp ./GLM/include/glm/detail/glm.cpp ./SOIL2/include/SOIL2.c ./SOIL2/include/image_DXT.c ./SOIL2/include/image_helper.c ./SOIL2/include/wfETC.c -L. -lopengl32 -lglfw3 -lglew32 -lstdc++  
