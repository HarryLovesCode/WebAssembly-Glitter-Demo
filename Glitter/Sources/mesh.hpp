#ifndef MESH_HPP
#define MESH_HPP

#include "shader.hpp"

#include <GLES3/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

class Vertex
{
  public:
    Vertex();

    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoords;
};

class Texture
{
  public:
    Texture();

    GLuint load(std::string const &path);

    GLuint id;
    std::string type;
    std::string path;
};

class Mesh
{
  private:
    void setupMesh();

    GLuint VBO;
    GLuint EBO;

  public:
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void draw(Shader shader);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO;
};

#endif