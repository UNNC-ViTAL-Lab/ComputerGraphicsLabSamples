#include "lab04_framework.hpp"

/*****************************************************************************/
// Scene Objects
/*****************************************************************************/

Object gSceneRoot;
OrthogonalCamera *gCamera = gSceneRoot.addChild<OrthogonalCamera>();
// Axis gAxis;
// MeshGround gGround;
// Cube gCube;

/*****************************************************************************/
// Scene Creation
/*****************************************************************************/

void initScene()
{
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

int a = 3;
int b = -121;

void turtle()
{
    // // Move to the starting point
    // glTranslatef(520, 0, 0);
    // glRotatef(90, 0, 0, 1);
    // glTranslatef(240, 0, 0);
    // // Draw a box
    // for(int i = 0; i < 4; ++i)
    // {
    //     // Pen down & move forward
    //     glBegin(GL_LINES);
    //     glVertex2f(0, 0);
    //     glVertex2f(240, 0);
    //     glEnd();
    //     glTranslatef(240, 0, 0);
    //     // Turn right
    //     glRotatef(-90, 0, 0, 1);
    // }
    glTranslatef(640, 0, 0);
    glRotatef(90, 0, 0, 1);
    glTranslatef(360, 0, 0);
    for(int i = 0; i < 200; ++i)
    {
        // Pen down & move forward
        glBegin(GL_LINES);
            glVertex2f(0, 0);
            glVertex2f(i * a, 0);
        glEnd();
        glTranslatef(i * a, 0, 0);
        // Turn right
        glRotatef(b, 0, 0, 1);
    }
}

void render(float dt)
{
    // Clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, gFramebufferWidth, gFramebufferHeight);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    // Reset the matrix
    glLoadIdentity();
    // Apply projection matrix
    gCamera->applyProjection();

    glMatrixMode(GL_MODELVIEW);
    // Reset the matrix
    glLoadIdentity();
    // Apply camera world-to-local transformation
    // gCamera->applyLocalToParentMatrix();
    // This is a new function which applies the transformation automatically
    // gAxis.drawTransformed(dt);
    // gGround.drawTransformed(dt);
    // gCube.drawTransformed(dt);

    turtle();
}

/*****************************************************************************/
// Window Management
/*****************************************************************************/

int gFramebufferWidth = 1280;
int gFramebufferHeight = 720;
const char *gWindowTitle = "Lab 4";

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // Record the correct size of the viewport
    gFramebufferWidth = width;
    gFramebufferHeight = height;
    // gCamera.setAspect(
    //     static_cast<float>(gFramebufferWidth) / gFramebufferHeight
    // );
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
    // if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
    //     gCamera.orientation().y -= (float)dx * 0.1f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(action != GLFW_PRESS) return;

    switch(key)
    {
        case GLFW_KEY_H:
            gSceneRoot.printObjectHierarchy();
            break;

        case GLFW_KEY_1:
            --a;
            break;

        case GLFW_KEY_2:
            ++a;
            break;

        case GLFW_KEY_3:
            --b;
            break;

        case GLFW_KEY_4:
            ++b;
            break;


        case GLFW_KEY_5:
            std::cout << "a = " << a << " b =" << b << std::endl;
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
