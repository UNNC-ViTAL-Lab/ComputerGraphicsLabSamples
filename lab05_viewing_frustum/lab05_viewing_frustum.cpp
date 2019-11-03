#include "lab05_framework.hpp"

extern float gHalfWidth;

/*****************************************************************************/
// Scene Objects
/*****************************************************************************/

Object gSceneRoot;
auto *gAxis = gSceneRoot.addChild<Axis>();
auto *gGround = gSceneRoot.addChild<MeshGround>();
// auto *gCube = gSceneRoot.addChild<Cube>();
Cube gFrustum { 1 };

auto *gLeftCamera = gSceneRoot.addChild<PerspectiveCamera>();
auto *gRightCamera = gSceneRoot.addChild<OrthogonalCamera>();

/*****************************************************************************/
// Scene Creation
/*****************************************************************************/

void updateCamera()
{
    const float aspect = gHalfWidth / gFramebufferHeight;

    gLeftCamera->setAspect(aspect);

    const float view_size = 50;
    gRightCamera->setLeft(-view_size * aspect);
    gRightCamera->setRight(view_size * aspect);
    gRightCamera->setTop(view_size);
    gRightCamera->setBottom(-view_size);
}

void initScene()
{
    updateCamera();

    gLeftCamera->position().z = 5;
    gLeftCamera->setZFar(1000);

    gRightCamera->orientation().x = -90;
    gRightCamera->position().y = 5;
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

    if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
        gLeftCamera->position() += move;
    else
        gRightCamera->position() += move;
}

/*****************************************************************************/
// Scene Rendering
/*****************************************************************************/

void drawLeftViewport(float dt)
{
    glViewport(0, 0, gHalfWidth, gFramebufferHeight);


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

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Draw the viewing frustum of the right camera

    glPushMatrix();
    // NDC->View->World
    gRightCamera->applyLocalToWorldMatrix();
    gRightCamera->applyInverseProjectionMatrix();
    gFrustum.draw(dt);
    glPopMatrix();

    // Draw scene objects

    glEnable(GL_DEPTH_TEST);
    // Draw the complete scene hierarchy
    gSceneRoot.drawHierarchyTransformed(dt);
}

void drawRightViewport(float dt)
{
    glViewport(gHalfWidth, 0, gHalfWidth, gFramebufferHeight);

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    // Reset the matrix
    glLoadIdentity();
    // Apply projection matrix
    gRightCamera->applyProjectionMatrix();

    glMatrixMode(GL_MODELVIEW);
    // Reset the matrix
    glLoadIdentity();
    // Apply camera world-to-local transformation
    gRightCamera->applyWorldToLocalMatrix();

    // Draw the complete scene hierarchy
    gSceneRoot.drawHierarchyTransformed(dt);
}

void render(float dt)
{
    // Clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    drawLeftViewport(dt);
    glPopAttrib();

    glPushAttrib(GL_ALL_ATTRIB_BITS);
    drawRightViewport(dt);
    glPopAttrib();
}

/*****************************************************************************/
// Window Management
/*****************************************************************************/

float gFramebufferWidth = 1280;
float gFramebufferHeight = 720;
float gHalfWidth = gFramebufferWidth / 2;
const char *gWindowTitle = "Lab 5";

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // Record the correct size of the viewport
    gFramebufferWidth = width;
    gFramebufferHeight = height;
    gHalfWidth = gFramebufferWidth / 2;
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

    switch(key)
    {
        case GLFW_KEY_H:
            gSceneRoot.printObjectHierarchy();
            break;

        default: ;
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
