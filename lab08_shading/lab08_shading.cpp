#include "lab08_framework.hpp"

/*****************************************************************************/
// Scene Objects
/*****************************************************************************/

Object gSceneRoot;
auto *gAxis = gSceneRoot.addChild<Axis>();
auto *gGround = gSceneRoot.addChild<MeshGround>();
auto *gCube = gSceneRoot.addChild<Cube>();

// An observer camera which shows the world and the camera you are tweaking
auto *gLeftCamera = gSceneRoot.addChild<PerspectiveCamera>();

Object *activeObject(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_Z))
        return gCube;
    return gLeftCamera;
}

// Texture gCubeTex;
Shader mBlinnPhong;

/*****************************************************************************/
// Scene Creation
/*****************************************************************************/

void updateCamera()
{
    const float aspect = 1.f * gFramebufferWidth / gFramebufferHeight;

    gLeftCamera->setAspect(aspect);
}

void initScene()
{
    updateCamera();

    gLeftCamera->position().z = 20;
    gLeftCamera->setZFar(1000);

    // gCubeTex.create();
    // mBlinnPhong.load("blinn.vert", "blinn.frag");
    // gCube->setShader(&mBlinnPhong);
}

/*****************************************************************************/
// Scene Update
/*****************************************************************************/

void update(GLFWwindow *window, float dt)
{
    auto multiplier = 1.f;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        multiplier *= 10;
    if(glfwGetKey(window, GLFW_KEY_LEFT_ALT))
        multiplier /= 10;
    const auto step = 0.1f * multiplier;

    glm::vec3 move { 0, 0, 0 };

    if(glfwGetKey(window, GLFW_KEY_W))
        move.z -= step;
    if(glfwGetKey(window, GLFW_KEY_A))
        move.x -= step;
    if(glfwGetKey(window, GLFW_KEY_S))
        move.z += step;
    if(glfwGetKey(window, GLFW_KEY_D))
        move.x += step;
    if(glfwGetKey(window, GLFW_KEY_Q))
        move.y -= step;
    if(glfwGetKey(window, GLFW_KEY_E))
        move.y += step;

    activeObject(window)->position() += move;
}

/*****************************************************************************/
// Scene Rendering
/*****************************************************************************/

void drawViewport(float dt)
{
    glViewport(0, 0, gFramebufferWidth, gFramebufferHeight);

    glMatrixMode(GL_PROJECTION);
    // Reset the matrix
    glLoadIdentity();
    // Apply projection matrix
    gLeftCamera->applyProjectionMatrix();

    glMatrixMode(GL_MODELVIEW);
    // Reset the matrix
    glLoadIdentity();
    // Apply camera world-to-local transformation
    gLeftCamera->applyWorldToLocalMatrix();

    // Draw the scene hierarchy
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    // glEnable(GL_LIGHTING);

    gSceneRoot.drawHierarchyTransformed(dt);
}

void render(float dt)
{
    // Clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Save all current attributes
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    drawViewport(dt);
    glPopAttrib();
}

/*****************************************************************************/
// Window Management
/*****************************************************************************/

int gFramebufferWidth = 1280;
int gFramebufferHeight = 720;
const char *gWindowTitle = "Lab 8";

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // Record the correct size of the viewport
    gFramebufferWidth = width;
    gFramebufferHeight = height;
    updateCamera();
}

/*****************************************************************************/
// Input Handling
/*****************************************************************************/

double gLastMouseX = 0;
double gLastMouseY = 0;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    // record the relative movement of the mouse
    auto dx = xpos - gLastMouseX;
    auto dy = ypos - gLastMouseY;
    gLastMouseX = xpos;
    gLastMouseY = ypos;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
    {
        activeObject(window)->orientation().y -= (float)dx * 0.1f;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    if(!action) return;

    switch(key)
    {
        case GLFW_KEY_H:
            gSceneRoot.printObjectHierarchy();
            break;

        default: ;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
    ImGui_ImplGlfw_CharCallback(window, codepoint);
}

/*****************************************************************************/
// File Drop Handler
/*****************************************************************************/

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    // gCubeTex.loadFromFile(paths[0]);
}

/*****************************************************************************/
// Install Callbacks
/*****************************************************************************/

void installCallbacks(GLFWwindow *window)
{
    // Register our callbacks so we can know when the events occur.
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetDropCallback(window, drop_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCharCallback(window, character_callback);
}
