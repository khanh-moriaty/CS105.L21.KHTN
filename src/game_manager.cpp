#pragma once
#include "game_manager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <HTTPRequest.hpp>
#include <Base64.h>
#include <sstream>

GameManager *GameManager::gameManager = NULL;

GameManager *GameManager::getInstance()
{
    if (gameManager == NULL)
    {
        gameManager = new GameManager(1200, 900);
    }
    return gameManager;
}

GameManager::GameManager(const int SCR_WIDTH, const int SCR_HEIGHT)
{
    this->SCR_WIDTH = SCR_WIDTH;
    this->SCR_HEIGHT = SCR_HEIGHT;
    this->lastX = -1;
    this->lastY = -1;
    this->camera = new Camera(glm::vec3(2.0f, 3.0f, 2.0f), glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch);

    this->running = true;
    this->blockCam = 0;
    this->power = 0.f;
    init();

    this->textureManager = new TextureManager();
    this->textureManager->initTexture();

    this->objectManager = new ObjectManager(this->textureManager);
    this->objectManager->initRoom();

    this->objectManager->addLight(0, glm::vec3(0.f, 3.f, 0.f));
    this->objectManager->addLight(1, glm::vec3(-.5f, 3.f, 0.f));
    this->objectManager->addLight(2, glm::vec3(+.5f, 3.f, 0.f));
  
    this->reset();

    this->shader = new Shader("src/shaders/vertex_core.glsl", "src/shaders/fragment_core.glsl");
    this->shader->use_Program();
    initMatrices();
}

#include "objects/game_objects/ball.h"
void GameManager::reset()
{
    this->objectManager->removeBalls();
    this->objectManager->addBall(0, glm::vec3(+1.f, 1.f, +0.f));

    for (int i=5; i>0; i--){
        for (int j=0; j<i; j++) {
            float zPos;
            // if (i%2 == 1) {
                zPos = (2*j - i) * (Ball::RADIUS);
            // }
            int x = 16-i*(i+1)/2+j;
            //if(x==2 || x==0)
                this->objectManager->addBall((x), glm::vec3(-1.5f + Ball::RADIUS*2*(6-i), 1.f, zPos));
        }
    }
}
GameManager::~GameManager()
{
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

    delete this->camera;
    delete this->shader;
    delete this->textureManager;
    delete this->objectManager;
}

void GameManager::init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glEnable(GL_MULTISAMPLE);  

    // glfw window creation
    // --------------------
    this->window = glfwCreateWindow(this->SCR_WIDTH, this->SCR_HEIGHT, "The Best Billiard Game", NULL, NULL);
    if (this->window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        this->running = false;
        return;
    }

    glfwGetFramebufferSize(this->window, &this->w_buffer, &this->h_buffer);
    glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //ko hien thi con tro chuot
    glfwSetCursorPosCallback(this->window, mouse_callback);
    glfwSetScrollCallback(this->window, scroll_callback);
    glfwSetDropCallback(this->window, drop_callback);
    glfwMakeContextCurrent(this->window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        printf("Error\n");
        this->running = false;
        return;
    }
    glEnable(GL_DEPTH_TEST);
}

void GameManager::initMatrices()
{
    //init matrix
    this->ModelMatrix = glm::mat4(1.f);
    this->ViewMatrix = this->camera->GetViewMatrix();

    float fov = 90.f;
    this->ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(w_buffer) / h_buffer, nearPlane, farPlane);

    this->shader->set_Mat4fv(this->ModelMatrix, "ModelMatrix");
    this->shader->set_Mat4fv(this->ViewMatrix, "ViewMatrix");
    this->shader->set_Mat4fv(this->ProjectionMatrix, "ProjectionMatrix");
    this->shader->set_3fv(this->objectManager->getLight0(), "lightPos0");
    this->shader->set_3fv(this->objectManager->getLight1(), "lightPos1");
    this->shader->set_3fv(this->objectManager->getLight2(), "lightPos2");
    this->shader->set_3fv(this->camera->GetPos(), "camPosition");
}

bool GameManager::isRunning()
{
    return this->running && !glfwWindowShouldClose(this->window);
}

void GameManager::update()
{
    float currentFrame = glfwGetTime();
    this->deltaTime = currentFrame - this->lastFrame;
    this->lastFrame = currentFrame;
    // input
    // -----

    this->processInput(this->window);
    this->objectManager->update();

    this->shader->set_Mat4fv(this->ModelMatrix, "ModelMatrix");
    glfwGetFramebufferSize(this->window, &w_buffer, &h_buffer);
    this->ProjectionMatrix = glm::perspective(glm::radians(this->camera->Zoom), static_cast<float>(w_buffer) / h_buffer, nearPlane, farPlane);
    this->shader->set_Mat4fv(this->ProjectionMatrix, "ProjectionMatrix");
    this->ViewMatrix = this->camera->GetViewMatrix();
    this->shader->set_Mat4fv(this->ViewMatrix, "ViewMatrix");
}

void GameManager::render()
{
    // render
    // ------
    glClearColor(0.f, 0.f, 0.f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    this->objectManager->render(this->shader);

    glfwSwapBuffers(this->window);
    glfwPollEvents();
}

void GameManager::processInput(GLFWwindow *window)
{   
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        this->camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        this->camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        this->camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        this->camera->ProcessKeyboard(RIGHT, deltaTime);
    //replay
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        this->reset();
    }
    //set camera pos at bida 0 pos, and block Camera 
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        //this->camera->setPos(glm::vec3(-1.5f, 1.f, -0.5f), glm::vec3(0.f, 1.f, 0.f), 0.f, 0.f);
        if(this->blockCam == 0){
            float radius = 1.f;
            float camX   = sin(glfwGetTime()) * radius;
            float camZ   = cos(glfwGetTime()) * radius;
            float yaw = this->camera->Yaw;
            float pitch = this->camera->Pitch;
            glm::vec3 pos = this->objectManager->getBall(0)->getPos();
            glm::vec3 dir = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), 
                                        sin(glm::radians(pitch)),
                                        sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
            this->camera->setViewMat(glm::normalize(-dir)*radius + pos, this->camera->Yaw, this->camera->Pitch);
        }
        this->blockCam = 1;
    }
    //set power of stick
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        this->power = 1.f;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        this->power = 2.f;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        this->power = 3.f;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        this->power = 4.f;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        this->power = 5.f;
    }
    //set the diretion of stick
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        glm::vec3 direction = glm::vec3(this->camera->Front.x, 0, this->camera->Front.z);
        //float power = 3.f;
        glm::vec3 pos = this->objectManager->getBall(0)->getPos();
        this->objectManager->removeBall(0);
        this->objectManager->addBall(0, pos, direction, this->power);
        this->blockCam = 0;

    }
    //not block Camera
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
    {
        this->blockCam = 0;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
    {
        this->objectManager->removeBall(0);
        this->objectManager->addBall(0, glm::vec3(1.f, 1.f, 0.f));
    }
    
}

void GameManager::mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    GameManager *gameManager = GameManager::getInstance();

    if (gameManager->lastX == -1)
    {
        gameManager->setMouseXY(xpos, ypos);
    }

    float xoffset = xpos - gameManager->getLastX();
    float yoffset = gameManager->getLastY() - ypos; // reversed since y-coordinates go from bottom to top

    gameManager->setMouseXY(xpos, ypos);

    if(gameManager->blockCam==1){
        float radius = 1.f;
        float camX   = sin(glfwGetTime()) * radius;
        float camZ   = cos(glfwGetTime()) * radius;
        float yaw = gameManager->camera->Yaw + xoffset*0.1f;
        float pitch = gameManager->camera->Pitch;
        glm::vec3 pos = gameManager->objectManager->getBall(0)->getPos();
        glm::vec3 dir = glm::vec3(cos(glm::radians(yaw)) * cos(glm::radians(pitch)), 
                                    sin(glm::radians(pitch)),
                                    sin(glm::radians(yaw)) * cos(glm::radians(pitch)));
        gameManager->camera->setViewMat(glm::normalize(-dir)*radius + pos, yaw, pitch);
    }
    else
        gameManager->getCamera()->ProcessMouseMovement(xoffset, yoffset);
}

void GameManager::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    GameManager *gameManager = GameManager::getInstance();
    gameManager->getCamera()->ProcessMouseScroll(yoffset);
}

void GameManager::drop_callback(GLFWwindow* window, int count, const char** fileList) {
    if (count != 1) return;
    // std::cout << fileList[0] << std::endl;
    // you can pass http::InternetProtocol::V6 to Request to make an IPv6 request
    std::string url = std::string("127.0.0.1:5678/") + macaron::Base64::Encode(fileList[0]);
    std::cout << url << std::endl;
    http::Request request(url);

    // send a get request
    const auto response = request.send("GET");
    const std::string result = std::string(response.body.begin(), response.body.end());
    std::stringstream ss(result);
    std::string line;

    GameManager *gameManager = GameManager::getInstance();
    gameManager->objectManager->removeBalls();
    int ballNumber = 0;

    while(std::getline(ss, line, '\n')){
        std::stringstream ss_line(line);
        float a, b;
        ss_line >> a >> b;
        std::cout << "line: " << line << std::endl;
        gameManager->objectManager->addBall(ballNumber % 16, glm::vec3(a * 3.3f - 2.1f, 1.f, b * 1.6f - 0.75f));
        ballNumber+=7;
    }

    gameManager->camera->setViewMat(glm::vec3(2.0f, 3.0f, 2.0f), -135.f, -30.f);
}