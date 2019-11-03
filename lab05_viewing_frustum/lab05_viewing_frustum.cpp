#include "lab05_framework.hpp"

/*****************************************************************************/
// Scene Objects
/*****************************************************************************/

Object gSceneRoot;
auto *gAxis = gSceneRoot.addChild<Axis>();
auto *gGround = gSceneRoot.addChild<MeshGround>();
auto *gCube = gSceneRoot.addChild<Cube>();

auto *gLeftCamera = gSceneRoot.addChild<PerspectiveCamera>();
auto *gRightCamera = gSceneRoot.addChild<PerspectiveCamera>();

/*****************************************************************************/
// Scene Creation
/*****************************************************************************/

void updateCamera()
{
    gLeftCamera->setAspect(
        static_cast<float>(gFramebufferWidth / 2) / gFramebufferHeight
    );
}

void initScene()
{
    updateCamera();
    gLeftCamera->position().z = 5;
}

/*****************************************************************************/
// Scene Update
/*****************************************************************************/

void update(float dt)
{
}

/*****************************************************************************/
// Scene Rendering
/*****************************************************************************/

void drawLeftViewport(float dt)
{
    glViewport(0, 0, gFramebufferWidth / 2, gFramebufferHeight);

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    // Reset the matrix
    glLoadIdentity();
    // Apply projection matrix
    gLeftCamera->applyProjection();

    glMatrixMode(GL_MODELVIEW);
    // Reset the matrix
    glLoadIdentity();
    // Apply camera world-to-local transformation
    gLeftCamera->applyWorldToLocalMatrix();
    // Draw the complete scene hierarchy
    gSceneRoot.drawHierarchyTransformed(dt);
}

void render(float dt)
{
    // Clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawLeftViewport(dt);
}

/*****************************************************************************/
// Window Management
/*****************************************************************************/

int gFramebufferWidth = 1280;
int gFramebufferHeight = 720;
const char *gWindowTitle = "Lab 5";

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
        gLeftCamera->orientation().y -= (float)dx * 0.1f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(!action) return;

    float step = 0.1f;

    switch(key)
    {
        case GLFW_KEY_H:
            gSceneRoot.printObjectHierarchy();
            break;

        case GLFW_KEY_I:
            gLeftCamera->position().z -= step;
            break;
        case GLFW_KEY_J:
            gLeftCamera->position().x -= step;
            break;
        case GLFW_KEY_K:
            gLeftCamera->position().z += step;
            break;
        case GLFW_KEY_L:
            gLeftCamera->position().x += step;
            break;
        case GLFW_KEY_U:
            gLeftCamera->position().y -= step;
            break;
        case GLFW_KEY_O:
            gLeftCamera->position().y += step;
            break;
    }
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
}
