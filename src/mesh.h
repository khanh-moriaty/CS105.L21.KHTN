#pragma once
#include "vertex.h"
#include "primitives.h"
#include "shaders/shader.h"
#include "texture.h"

class Mesh
{
private:
	Vertex* vertexArray;
	unsigned nrOfVertices;
	GLuint* indexArray;
	unsigned nrOfIndices;

	Texture texture;

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	glm::vec3 position;
	glm::vec3 origin;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 ModelMatrix;

	void updateUniforms(Shader* shader) {shader->set_Mat4fv(this->ModelMatrix, "ModelMatrix");}

	void initVAO();

	void updateModelMatrix();

public:
	Mesh(
		Vertex* vertexArray,
		const unsigned& nrOfVertices,
		GLuint* indexArray,
		const unsigned& nrOfIndices,
		const Texture& texture,
		glm::vec3 position,
		glm::vec3 origin,
		glm::vec3 rotation,
		glm::vec3 scale);

	Mesh(
		Primitive* primitive,
		const Texture& texture,
		glm::vec3 position,
		glm::vec3 origin,
		glm::vec3 rotation,
		glm::vec3 scale);

	Mesh(const Mesh& obj);

	~Mesh();

	void render(Shader* shader);

	//Accessors

	//Modifiers
	void setPosition(const glm::vec3 position) {this->position = position;}
	void setOrigin(const glm::vec3 origin) {this->origin = origin;}
	void setRotation(const glm::vec3 rotation) {this->rotation = rotation;}
	void setScale(const glm::vec3 scale) {this->scale = scale;}

	//Functions

	void move(const glm::vec3 position) {this->position += position;}
	void rotate(const glm::vec3 rotation) {this->rotation += rotation;}
	void scaleUp(const glm::vec3 scale) {this->scale += scale;}

	void update() {}
};