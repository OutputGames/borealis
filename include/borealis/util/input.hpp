#if !defined(INPUT_HPP)
#define INPUT_HPP

#include <GLFW/glfw3.h>
#include <string>
#include <cstring>

namespace brl
{
    struct InputMgr {
        static void init(GLFWwindow* w);
        static void update();

        static bool getKey(int key);
        static bool getKeyDown(int key);
        static bool getKeyUp(int key);

        static bool getMouseButton(int button);
        static bool getMouseButtonDown(int button);
        static bool getMouseButtonUp(int button);

        static float GetMouseX() { return mouseX; }
        static float GetMouseY() { return mouseY; }
        static float GetMouseDeltaX() { return mouseX - prevMouseX; }
        static float GetMouseDeltaY() { return mouseY - prevMouseY; }

        static float getAxisRaw(std::string axis);

        static GLFWwindow* window;
    private:

        static bool keys[GLFW_KEY_LAST];
        static bool keysPrev[GLFW_KEY_LAST];
        static bool mouseButtons[GLFW_MOUSE_BUTTON_LAST];
        static bool mouseButtonsPrev[GLFW_MOUSE_BUTTON_LAST];
        static float mouseX, mouseY, prevMouseX, prevMouseY;
    };
} // namespace brl


#endif // INPUT_HPP
