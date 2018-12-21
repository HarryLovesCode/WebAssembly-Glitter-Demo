#include "camera.hpp"
#include "glitter.hpp"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

Camera::Camera()
{
    position = glm::vec3(0.0f, 1.14f, -3.0f);
    globalUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = 180.0f;
    pitch = 0.0f;
    sensitivity = 0.01f;
    speed = 0.3f;
}

void Camera::init(GLFWwindow *window)
{
    this->window = window;
}

void Camera::update()
{
    mouseMove();

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        keyboardPress(FORWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        keyboardPress(BACKWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        keyboardPress(LEFT);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        keyboardPress(RIGHT);
    }

    front = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))));

    right = glm::normalize(glm::cross(front, globalUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::mouseMove()
{
    double x = 0.0;
    double y = 0.0;

    glfwGetCursorPos(window, &x, &y);

    float width = (float)wWidth;
    float height = (float)wHeight;
    float xCoord = (2.0f * x - width) / width;
    float yCoord = (2.0f * y - height) / height;

    yaw = xCoord * M_PI;
    pitch = yCoord * M_PI;
}

void Camera::keyboardPress(CameraDirection direction)
{
    if (direction == FORWARD)
    {
        position += front * speed;
    }
    else if (direction == BACKWARD)
    {
        position -= front * speed;
    }
    else if (direction == LEFT)
    {
        position -= right * speed;
    }
    else if (direction == RIGHT)
    {
        position += right * speed;
    }
}

glm::mat4 Camera::getView()
{
    glm::mat4 transform = glm::mat4(1.0);
    transform = glm::rotate(transform, yaw, glm::vec3(0.0, 1.0, 0.0));
    transform = glm::rotate(transform, pitch, glm::vec3(1.0, 0.0, 0.0));
    transform = glm::translate(transform, glm::vec3(0.0, 0.0, 0.0));

    glm::mat4 lookAt = glm::lookAt(position, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    return lookAt * transform;
}

glm::mat4 Camera::getProj()
{
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    return glm::perspective(1.0, (double)width / (double)height, 0.1, 100.0);
}