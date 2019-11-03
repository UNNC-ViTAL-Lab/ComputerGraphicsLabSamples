#include "lab05_framework.hpp"

/*****************************************************************************/
// Scene Objects
/*****************************************************************************/

Object gSceneRoot;
auto *gAxis = gSceneRoot.addChild<Axis>();
auto *gGround = gSceneRoot.addChild<MeshGround>();
auto *gCube = gSceneRoot.addChild<Cube>();

// An observer camera which shows the world and the camera you are tweaking
auto *gLeftCamera = gSceneRoot.addChild<PerspectiveCamera>();

// Two cameras for you to experiment with :)
auto *gRightCameraOrtho = gSceneRoot.addChild<OrthogonalCamera>();
auto *gRightCameraPersp = gSceneRoot.addChild<PerspectiveCamera>();

Camera *gCameras[] {
    gRightCameraOrtho,
    gRightCameraPersp,
};

int gCameraSelection = 0;

Camera *activeCamera()
{
    return gCameras[gCameraSelection];
}

// Shows the viewing frustum of the camera on the right side
Cube gFrustum { 1 };

/*****************************************************************************/
// Scene Creation
/*****************************************************************************/

void updateCamera()
{
    const float aspect = 1.f * gHalfWidth / gFramebufferHeight;

    gLeftCamera->setAspect(aspect);
    gRightCameraPersp->setAspect(aspect);

    const float view_size = 10;
    gRightCameraOrtho->setLeft(-view_size * aspect);
    gRightCameraOrtho->setRight(view_size * aspect);
    gRightCameraOrtho->setTop(view_size);
    gRightCameraOrtho->setBottom(-view_size);
}

void initScene()
{
    updateCamera();

    gLeftCamera->position().z = 10;
    gLeftCamera->setZFar(1000);

    gRightCameraOrtho->orientation().x = -90;
    gRightCameraOrtho->position().y = 5;

    gRightCameraPersp->setZFar(10);
    gRightCameraPersp->setFovY(45);
    gRightCameraPersp->position().z = 5;

    gFrustum.setAlpha(128);

    // References of the positions of the cameras
    gRightCameraOrtho->addChild<Axis>(0.05f);
    gRightCameraPersp->addChild<Axis>(0.05f);
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
        activeCamera()->position() += move;
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

    // Draw the scene hierarchy
    glEnable(GL_DEPTH_TEST);
    gSceneRoot.drawHierarchyTransformed(dt);

    // Draw the viewing frustum of the right camera
    // Only test on the depth but not overwrite it so that all faces
    // of the viewing frustum could be drawn
    glDepthMask(GL_FALSE);
    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Transform the NDC cube from NDC->View->World
    // Recall that in NDC space everything is in the range of [-1, 1] on every
    // axis.
    activeCamera()->applyLocalToWorldMatrix();
    activeCamera()->applyInverseProjectionMatrix();
    gFrustum.draw(dt);
}

void drawRightViewport(float dt)
{
    glViewport(gHalfWidth, 0, gHalfWidth, gFramebufferHeight);

    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    // Reset the matrix
    glLoadIdentity();
    // Apply projection matrix
    activeCamera()->applyProjectionMatrix();

    glMatrixMode(GL_MODELVIEW);
    // Reset the matrix
    glLoadIdentity();
    // Apply camera world-to-local transformation
    activeCamera()->applyWorldToLocalMatrix();

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

int gFramebufferWidth = 1280;
int gFramebufferHeight = 720;
int gHalfWidth = gFramebufferWidth / 2;
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

        case GLFW_KEY_SPACE:
            gCameraSelection = (gCameraSelection + 1) % 2;
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
