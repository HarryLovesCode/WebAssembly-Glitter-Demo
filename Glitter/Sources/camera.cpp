#include "camera.hpp"

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera()
{
    position = glm::vec3(0.0f, 1.14f, -3.0f);
    globalUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = 180.0f;
    pitch = 0.0f;
    sensitivity = 0.01f;
    speed = 0.3f;
    lastX = 0.0;
    lastY = 0.0;
    lastPos = position;
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

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(front);
    right = glm::normalize(glm::cross(front, globalUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::mouseMove()
{
    double x, y;

    glfwGetCursorPos(window, &x, &y);

    float xCoord = (2.0 * x) / 1280.0;
    float yCoord = (2.0 * y - 720.0) / 720.0;

    yaw = xCoord * 3.14159264f;
    pitch = yCoord * 3.14159264f;
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
    glm::mat4 out = glm::mat4(1.0);
    out = glm::rotate(out, yaw, glm::vec3(0.0, 1.0, 0.0));
    out = glm::rotate(out, pitch, glm::vec3(1.0, 0.0, 0.0));
    out = glm::translate(out, glm::vec3(0.0, 0.0, 0.0));

    glm::mat4 tmp = glm::lookAt(position, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

    return tmp * out;
}

glm::mat4 Camera::getProj()
{
    int width, height;

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    return glm::perspective(1.0, (double)width / (double)height, 0.1, 100.0);
}