#include "framework.hpp"

/*****************************************************************************/
// Axis
/*****************************************************************************/

class Axis : public Object
{
    float mSize = 0.1f;

public:
    Axis() = default;

    Axis(float size)
        : mSize(size)
    {
    }

    void setSize(float size) { mSize = size; }

    void draw(float dt) override
    {
        glBegin(GL_LINES);
            // x-axis
            glColor3f(1, 0, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(1, 0, 0);
            // y-axis
            glColor3f(0, 1, 0);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 1, 0);
            // z-axis
            glColor3f(0, 0, 1);
            glVertex3f(0, 0, 0);
            glVertex3f(0, 0, 1);
        glEnd();
    }
};


/*****************************************************************************/
// MeshGround
/*****************************************************************************/

class MeshGround : public Object
{
    float mSize = 50.f;
    float mStep = 5.f;
    float mHeight = -2.5f;

public:
    MeshGround() = default;

    MeshGround(float size, float step, float height)
        : mSize(size)
        , mStep(step)
        , mHeight(height)
    {
    }

    void setSize(float size) { mSize = size; }
    void setStep(float step) { mStep = step; }
    void setHeight(float height) { mHeight = height; }

    void draw(float dt) override
    {
        glBegin(GL_LINES);
            glColor3f(0.75f, 0.75f, 0.75f);
            // draw horizontal lines
            for(float i = -mSize; i <= mSize; i += mStep)
            {
                glVertex3f(-mSize, mHeight, i);
                glVertex3f(mSize, mHeight, i);
            }
            // draw deep lines
            for(float i = -mSize; i <= mSize; i += mStep)
            {
                glVertex3f(i, mHeight, -mSize);
                glVertex3f(i, mHeight, mSize);
            }
        glEnd();
    }
};

/*****************************************************************************/
// Cube
/*****************************************************************************/

#define QUAD_INDICES(array, a, b, c, d) \
    glVertex3fv(value_ptr(array[a])); \
    glVertex3fv(value_ptr(array[b])); \
    glVertex3fv(value_ptr(array[c])); \
    glVertex3fv(value_ptr(array[d])); \
/**/

class Cube : public Object
{
    float mHalfSize = 0.5f;

public:
    Cube() = default;

    Cube(float half_size)
        : mHalfSize(half_size)
    {
    }

    void setHalfSize(float half_size) { mHalfSize = half_size; }

    void draw(float dt) override
    {
        const glm::vec3 v[] = {
            { mHalfSize, mHalfSize, mHalfSize },
            { mHalfSize, mHalfSize, -mHalfSize },
            { mHalfSize, -mHalfSize, mHalfSize },
            { mHalfSize, -mHalfSize, -mHalfSize },
            { -mHalfSize, mHalfSize, mHalfSize },
            { -mHalfSize, mHalfSize, -mHalfSize },
            { -mHalfSize, -mHalfSize, mHalfSize },
            { -mHalfSize, -mHalfSize, -mHalfSize },
        };

        glBegin(GL_QUADS);
        // top
        glColor3ub(169, 102, 194);
        QUAD_INDICES(v, 1, 0, 4, 5);
        // bottom
        glColor3ub(164, 33, 14);
        QUAD_INDICES(v, 2, 3, 7, 6);
        // left
        glColor3ub(228, 69, 147);
        QUAD_INDICES(v, 5, 4, 6, 7);
        // right
        glColor3ub(136, 157, 210);
        QUAD_INDICES(v, 0, 1, 3, 2);
        // front
        glColor3ub(138, 250, 122);
        QUAD_INDICES(v, 1, 5, 7, 3);
        // back
        glColor3ub(1, 37, 146);
        QUAD_INDICES(v, 4, 0, 2, 6);
        glEnd();
    }
};

/*****************************************************************************/
// Game Logics
/*****************************************************************************/

int gFramebufferWidth = 1280;
int gFramebufferHeight = 720;
const char *gWindowTitle = "Cube";

PerspectiveCamera gCamera;
Axis gAxis;
MeshGround gGround;
Cube gCube;

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    gFramebufferWidth = width;
    gFramebufferHeight = height;
    gCamera.setAspect(
        static_cast<float>(gFramebufferWidth) / gFramebufferHeight
    );
}

double gLastMouseX = 0;
double gLastMouseY = 0;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    auto dx = xpos - gLastMouseX;
    auto dy = ypos - gLastMouseY;
    gLastMouseX = xpos;
    gLastMouseY = ypos;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
        gCamera.orientation().y -= (float)dx * 0.1f;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(!action) return;

    float step = 0.1f;

    switch(key)
    {
        case GLFW_KEY_W:
            gCube.position().z -= step;
            break;
        case GLFW_KEY_A:
            gCube.position().x -= step;
            break;
        case GLFW_KEY_S:
            gCube.position().z += step;
            break;
        case GLFW_KEY_D:
            gCube.position().x += step;
            break;
        case GLFW_KEY_SPACE:
            gCube.position().y += step;
            break;
        case GLFW_KEY_LEFT_CONTROL:
            gCube.position().y -= step;
            break;

        case GLFW_KEY_Q:
            gCube.orientation().y += 10;
            break;
        case GLFW_KEY_E:
            gCube.orientation().y -= 10;
            break;
        case GLFW_KEY_LEFT:
            gCube.orientation().z += 10;
            break;
        case GLFW_KEY_RIGHT:
            gCube.orientation().z -= 10;
            break;
        case GLFW_KEY_UP:
            gCube.orientation().x -= 10;
            break;
        case GLFW_KEY_DOWN:
            gCube.orientation().x += 10;
            break;

        case GLFW_KEY_1:
            gCube.scaling() = { 1, 1, 1 };
            break;
        case GLFW_KEY_2:
            gCube.scaling() = { 2, 2, 2 };
            break;
        case GLFW_KEY_3:
            gCube.scaling() = { 5, 5, 5 };
            break;
        case GLFW_KEY_4:
            gCube.scaling().y *= 1.1f;
            break;
        case GLFW_KEY_5:
            gCube.scaling().y /= 1.1f;
            break;

        case GLFW_KEY_I:
            gCamera.position().z -= step;
            break;
        case GLFW_KEY_J:
            gCamera.position().x -= step;
            break;
        case GLFW_KEY_K:
            gCamera.position().z += step;
            break;
        case GLFW_KEY_L:
            gCamera.position().x += step;
            break;
        case GLFW_KEY_U:
            gCamera.position().y -= step;
            break;
        case GLFW_KEY_O:
            gCamera.position().y += step;
            break;
    }
}

void initScene()
{
    // TODO: set the positions of your cube and camera

    // I already put the camera at a good position. You only have to
    // move the cube.
    gCamera.position().z = 5;
}

glm::vec3 gCameraCenter;

void update(float dt)
{
    // gCube.orientation().y += dt * 10;
    gCameraCenter = gCube.position();
}

void render(float dt)
{
    glViewport(0, 0, gFramebufferWidth, gFramebufferHeight);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    // Reset the matrix
    glLoadIdentity();
    // Apply projection matrix
    gCamera.applyProjection();

    glMatrixMode(GL_MODELVIEW);
    // Reset the matrix
    glLoadIdentity();
    // Apply camera world-to-local transformation
    // glTranslatef(-gCameraCenter.x, -gCameraCenter.y, -gCameraCenter.z);
    gCamera.applyWorldToLocal();
    // This is a new function which applies the transformation automatically
    gAxis.drawTransformed(dt);
    gGround.drawTransformed(dt);
    gCube.drawTransformed(dt);
}

void installCallbacks(GLFWwindow *window)
{
    // Register our callbacks so we can know when the events occur.
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
}

void mainLoop(GLFWwindow *window)
{
    auto time = glfwGetTime();

    // Loop until the user closes the window
    while(!glfwWindowShouldClose(window))
    {
        const auto new_time = glfwGetTime();
        // Elapsed time since last frame, in seconds.
        const auto dt = new_time - time;
        time = new_time;

        update(static_cast<float>(dt));
        // Clear the framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Set up the camera and draw our scene
        render(static_cast<float>(dt));
        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Poll for and process events, this will call into the callbacks
        // we set before when related events occur.
        glfwPollEvents();
    }
}
