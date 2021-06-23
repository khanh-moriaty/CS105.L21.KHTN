CC 				= gcc
CFLAGS 			= -w
CPPFLAGS 		= -std=c++0x -DGLEW_NO_GLU -DGLEW_STATIC -DGLEW_BUILD -O2
CINCLUDES 		= -I src/ -I dependencies/GLEW/include/ -I dependencies/GLFW/include/ -I dependencies/GLM/include/ \
					-I dependencies/SOIL2/include/ -I dependencies/include

ifeq ($(OS),Windows_NT)
	LDFLAGS 	= -L. -lopengl32 -lglu32 -lgdi32 -lstdc++ -lwsock32 -lWs2_32
	TARGET		= BilliardsGame.exe
else
	LDFLAGS 	= -L. -lGL -lGLU -lm -ldl -lX11 -lpthread -lstdc++
	TARGET		= BilliardsGame
endif

CPPSOURCES		= $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*/*.cpp)
CSOURCES		= $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*/*/*.c)
CPPDEPS			= dependencies/GLM/include/glm/detail/glm.cpp
CDEPS	 		= dependencies/SOIL2/include/SOIL2.c dependencies/SOIL2/include/image_DXT.c \
					dependencies/SOIL2/include/image_helper.c dependencies/SOIL2/include/wfETC.c
COBJECTS		= $(patsubst %.c, %.o, $(CSOURCES)) $(patsubst %.c, %.o, $(CDEPS)) 
CPPOBJECTS		= $(patsubst %.cpp, %.o, $(CPPSOURCES)) $(patsubst %.cpp, %.o, $(CPPDEPS)) 

GLEW_SOURCE		= dependencies/GLEW/src/glew.c
GLEW_OBJECT		= $(patsubst %.c, %.o, $(GLEW_SOURCE))
GLEW_INCLUDE	= -I dependencies/GLEW/include/

# GLFW_SOURCE		= $(wildcard dependencies/GLFW/src/*.c)
GLFW_SOURCE_COMMON	= context.c init.c input.c monitor.c vulkan.c window.c
GLFW_INCLUDE	= -I dependencies/GLFW/include/ -I dependencies/GLFW/src/ -I dependencies/GLFW/deps/mingw/
ifeq ($(OS),Windows_NT)
	GLFW_FLAGS		= -D_GLFW_WIN32
	GLFW_SOURCE_FILES = $(GLFW_SOURCE_COMMON) win32_init.c win32_joystick.c win32_monitor.c win32_time.c \
					win32_thread.c win32_window.c wgl_context.c egl_context.c osmesa_context.c
else
	GLFW_FLAGS		= -D_GLFW_X11
	GLFW_SOURCE_FILES = $(GLFW_SOURCE_COMMON) x11_init.c x11_monitor.c x11_window.c xkb_unicode.c posix_time.c \
					posix_thread.c glx_context.c egl_context.c osmesa_context.c linux_joystick.c
endif
GLFW_SOURCE		= $(patsubst %, dependencies/GLFW/src/%, $(GLFW_SOURCE_FILES))
GLFW_OBJECT		= $(patsubst %.c, %.o, $(GLFW_SOURCE))

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

install: $(TARGET) clean

lib: $(GLFW_OBJECT)

# Build glew
$(GLEW_OBJECT): $(GLEW_SOURCE)
	gcc -c $(CFLAGS) $(GLEW_INCLUDE) $^ -o $@

# Build glfw
$(GLFW_OBJECT): %.o: %.c
	gcc -c $(CFLAGS) $(GLFW_FLAGS) $(GLFW_INCLUDE) $< -o $@

# Build objects from C sources
$(COBJECTS): %.o: %.c
	$(CC) -c $(CFLAGS) $< $(CINCLUDES) -o $@

# Build objects from CPP sources
$(CPPOBJECTS): %.o: %.cpp
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< $(CINCLUDES) -o $@

# Make executable
$(TARGET): $(COBJECTS) $(CPPOBJECTS) $(GLEW_OBJECT) $(GLFW_OBJECT)
	$(CC) -o $@ $^ $(CINCLUDES) $(LDFLAGS)

RM_LIST := $(COBJECTS) $(CPPOBJECTS) $(GLEW_OBJECT) $(GLFW_OBJECT)
ifeq ($(OS),Windows_NT)
	RM 		= cmd //C del //Q //F
	RM_DIR 	= cmd //C rmdir //Q //S
	RM_LIST := $(subst /,\,$(RM_LIST))
else
	RM 		= rm -f
	RM_DIR 	= rm -f -r
endif

# Clean object files when done
clean:
	$(RM) $(RM_LIST)