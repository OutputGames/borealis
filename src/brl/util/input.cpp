#include "borealis/util/input.hpp"

GLFWwindow* brl::InputMgr::window = nullptr;
bool brl::InputMgr::keys[GLFW_KEY_LAST] = {};
bool brl::InputMgr::keysPrev[GLFW_KEY_LAST] = {};
bool brl::InputMgr::mouseButtons[GLFW_MOUSE_BUTTON_LAST] = {};
bool brl::InputMgr::mouseButtonsPrev[GLFW_MOUSE_BUTTON_LAST] = {};
float brl::InputMgr::mouseX = 0, brl::InputMgr::mouseY = 0;
float brl::InputMgr::prevMouseX = 0, brl::InputMgr::prevMouseY = 0;

void brl::InputMgr::init(GLFWwindow* w)
{
    window = w;
    memset(keys, 0, sizeof(keys));
    memset(keysPrev, 0, sizeof(keysPrev));
}

void brl::InputMgr::update()
{
    memcpy(keysPrev, keys, sizeof(keys));
    memcpy(mouseButtonsPrev, mouseButtons, sizeof(mouseButtons));

    prevMouseX = mouseX;
    prevMouseY = mouseY;
    double mx = 0;
    double my;

    glfwGetCursorPos(window, &mx, &my);
    mouseX = mx;
    mouseY = my;

}

bool brl::InputMgr::getKey(int key)
{
    return keys[key] = (glfwGetKey(window, key) == GLFW_PRESS);
}

bool brl::InputMgr::getKeyDown(int key)
{
    return getKey(key) && !keysPrev[key];
}

bool brl::InputMgr::getKeyUp(int key)
{
    return !getKey(key) && keysPrev[key];
}

bool brl::InputMgr::getMouseButton(int button)
{
    return mouseButtons[button] = (glfwGetMouseButton(window, button) == GLFW_PRESS);
}

bool brl::InputMgr::getMouseButtonDown(int button)
{
    return getMouseButton(button) && !mouseButtonsPrev[button];
}

bool brl::InputMgr::getMouseButtonUp(int button)
{
    return !getMouseButton(button) && mouseButtonsPrev[button];
}

float brl::InputMgr::getAxisRaw(std::string axis)
{
    if (axis == "Horizontal")
    {
        return (getKey(GLFW_KEY_D) ? 1.0f : 0.0f) - (getKey(GLFW_KEY_A) ? 1.0f : 0.0f);
    }
    if (axis == "Vertical")
    {
        return (getKey(GLFW_KEY_W) ? 1.0f : 0.0f) - (getKey(GLFW_KEY_S) ? 1.0f : 0.0f);
    }
    return 0.0f;
}
