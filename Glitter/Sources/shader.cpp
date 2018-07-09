#include "shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <iostream>
#include <vector>

Shader::Shader() {}

void Shader::init(std::string const &vs, std::string const &fs)
{
    program = glCreateProgram();

    load(vs);
    load(fs);
    link();
}

void Shader::load(std::string const &filename)
{
    GLint success = GL_FALSE;
    std::ifstream in(filename);
    std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    GLuint shader = create(filename);

    const char *source = contents.c_str();

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == GL_FALSE)
    {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<GLchar> errorLog(logLength);
        glGetShaderInfoLog(shader, logLength, NULL, &errorLog[0]);

        std::cout << filename << " : " << &errorLog[0] << std::endl;
        glDeleteShader(shader);

        return;
    }

    glAttachShader(program, shader);
}

void Shader::link()
{
    GLint success = GL_FALSE;

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<GLchar> errorLog(logLength);
        glGetProgramInfoLog(program, logLength, NULL, &errorLog[0]);

        std::cout << &errorLog[0] << std::endl;
        glDeleteProgram(program);

        return;
    }
}

void Shader::activate(Camera cam)
{
    glUseProgram(program);

    glm::mat4 model = glm::mat4(1.0);
    glm::mat4 persp = cam.getProj();
    glm::mat4 view = cam.getView();

    model = scale(model, glm::vec3(40.0, 40.0, 40.0));

    unsigned int transLoc = glGetUniformLocation(program, "mView");
    unsigned int perspLoc = glGetUniformLocation(program, "mProjection");
    unsigned int modelLoc = glGetUniformLocation(program, "mModel");

    glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(perspLoc, 1, GL_FALSE, glm::value_ptr(persp));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::vec3 lightPositions[] = {
        glm::vec3(-10.0f, 10.0f, -10.0f),
        glm::vec3(10.0f, 10.0f, -10.0f),
        glm::vec3(-10.0f, -10.0f, -10.0f),
        glm::vec3(10.0f, -10.0f, -10.0f)};

    glm::vec3 lightColors[] = {
        glm::vec3(1000.0f, 1000.0f, 1000.0f),
        glm::vec3(1000.0f, 1000.0f, 1000.0f),
        glm::vec3(1000.0f, 1000.0f, 1000.0f),
        glm::vec3(1000.0f, 1000.0f, 1000.0f)};

    unsigned int camLoc = glGetUniformLocation(program, "vCamPos");
    glUniform3f(camLoc, cam.position.x, cam.position.y, cam.position.z);

    for (unsigned int i = 0; i < 4; ++i)
    {
        std::string posName = "vLightPos[" + std::to_string(i) + "]";
        std::string colName = "vLightCol[" + std::to_string(i) + "]";
        unsigned int posLoc = glGetUniformLocation(program, posName.c_str());
        unsigned int colLoc = glGetUniformLocation(program, colName.c_str());

        glUniform3f(posLoc, lightPositions[i][0], lightPositions[i][1], lightPositions[i][2]);
        glUniform3f(colLoc, lightColors[i][0], lightColors[i][1], lightColors[i][2]);
    }
}

void Shader::deactivate()
{
    glUseProgram(NULL);
}

GLuint Shader::create(std::string const &filename)
{
    auto index = filename.rfind(".");
    auto ext = filename.substr(index + 1);

    if (ext == "vert")
    {
        return glCreateShader(GL_VERTEX_SHADER);
    }
    else if (ext == "frag")
    {
        return glCreateShader(GL_FRAGMENT_SHADER);
    }
    else
    {
        return false;
    }
}