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

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 persp = cam.getProj();
    glm::mat4 view = cam.getView();

    model = glm::scale(model, glm::vec3(40.0f, 40.0f, 40.0f));

    glm::mat4 mvp = persp * view * model;
    glm::mat4 normal = glm::inverse(view * model);
    normal = glm::transpose(normal);

    unsigned int mvpLoc = glGetUniformLocation(program, "u_MVPMatrix");
    unsigned int modelLoc = glGetUniformLocation(program, "u_ModelMatrix");
    unsigned int normalLoc = glGetUniformLocation(program, "u_NormalMatrix");

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(normalLoc, 1, GL_FALSE, glm::value_ptr(normal));

    glm::vec3 lightDir = glm::vec3(0.0, 0.0, -1.0);
    glm::vec3 lightCol = glm::vec3(1.0, 1.0, 1.0);

    unsigned int camLoc = glGetUniformLocation(program, "u_Camera");
    unsigned int lightDirLoc = glGetUniformLocation(program, "u_LightDirection");
    unsigned int lightColLoc = glGetUniformLocation(program, "u_LightColor");

    glm::vec3 cPos = mvp * glm::vec4(cam.position.x, cam.position.y, cam.position.z, 1.0);

    glUniform3f(camLoc, cPos.x, cPos.y, cPos.z);
    glUniform3f(lightDirLoc, lightDir.x, lightDir.y, lightDir.z);
    glUniform3f(lightColLoc, lightCol.x, lightCol.y, lightCol.z);


    unsigned int roughLoc = glGetUniformLocation(program, "u_MetallicRoughnessValues");
    unsigned int baseLoc = glGetUniformLocation(program, "u_BaseColorFactor");

    glUniform2f(roughLoc, 1.0, 1.0);
    glUniform4f(baseLoc, 1.0, 1.0, 1.0, 1.0);
    glUniform1f(glGetUniformLocation(program, "u_NormalScale"), 1.0f);
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