#ifdef _WIN32
#   pragma comment(lib, "OpenGL32.lib")
#   pragma comment(lib, "GLU32.lib")
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#   undef near
#   undef far
#   undef max
#   undef min
#   include <GL/gl.h>
#   include <GL/glu.h>
#endif

#ifdef __APPLE__
#   define GL_SILENCE_DEPRECATION
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/*****************************************************************************/
// Object
/*****************************************************************************/

class Object
{
protected:
    /**
     * \brief Origin of the object in Cartesian coordinates.
     */
    glm::vec3 mPosition = { 0, 0, 0 };

    /**
     * \brief Orientation of the object using Euler angles.
     */
    glm::vec3 mOrientation = { 0, 0, 0 };

    /**
     * \brief Scaling of the object on each axis.
     */
    glm::vec3 mScaling = { 1, 1, 1 };

public:
    virtual ~Object() = default;

    glm::vec3 & position() { return mPosition; }
    glm::vec3 & orientation() { return mOrientation; }
    glm::vec3 & scaling() { return mScaling; }

    void setTransformation(
        const glm::vec3 &position = { 0, 0, 0 },
        const glm::vec3 &orientation = { 0, 0, 0 },
        const glm::vec3 &scaling = { 1, 1, 1, })
    {
        mPosition = position;
        mOrientation = orientation;
        mScaling = scaling;
    }

    /**
     * \brief Concatenate the matrix for converting positions into the local
     * space of this object. Useful for cameras.
     */
    void applyWorldToLocal() const
    {
        // TODO: implement
        // keep in mind how the matrix stack & transformation works
        glScalef(1 / mScaling.x, 1 / mScaling.y, 1 / mScaling.z);
        glRotatef(-mOrientation.x, 1, 0, 0);
        glRotatef(-mOrientation.y, 0, 1, 0);
        glRotatef(-mOrientation.z, 0, 0, 1);
        glTranslatef(-mPosition.x, -mPosition.y, -mPosition.z);
    }

    /**
     * \brief Concatenate the matrix for converting positions from the local
     * space of the object into the world space. Useful for renderable objects.
     */
    void applyLocalToWorld() const
    {
        // TODO: implement
        glTranslatef(mPosition.x, mPosition.y, mPosition.z);
        glRotatef(mOrientation.z, 0, 0, 1);
        glRotatef(mOrientation.y, 0, 1, 0);
        glRotatef(mOrientation.x, 1, 0, 0);
        glScalef(mScaling.x, mScaling.y, mScaling.z);
    }

    /**
     * \brief Issue drawing commands.
     * \param dt
     */
    virtual void draw(float dt) = 0;

    /**
     * \brief Optional methods for updating your object with time.
     * \param dt
     */
    virtual void update(float dt) { }
};

/*****************************************************************************/
// Camera
/*****************************************************************************/

class Camera : public Object
{
public:
    void draw(float dt) override
    {
        // Usually we don't draw the camera itself.
    }

    /**
     * \brief Apply the projection matrix.
     */
    virtual void applyProjection() const = 0;
};

/*****************************************************************************/
// PerspectiveCamera
/*****************************************************************************/

class PerspectiveCamera : public Camera
{
    float mFov = 60;
    float mAspect = 16 / 9.f;
    float mZNear = 0.5f;
    float mZFar = 100;

public:
    PerspectiveCamera() = default;

    PerspectiveCamera(float fov, float aspect, float near, float far)
        : mFov(fov)
        , mAspect(aspect)
        , mZNear(near)
        , mZFar(far)
    {
    }

    void setFov(float fov) { mFov = fov; }
    void setAspect(float aspect) { mAspect = aspect; }
    void setZNear(float near) { mZNear = near; }
    void setZFar(float far) { mZFar = far; }

    void applyProjection() const override
    {
        // TODO: implement
        gluPerspective(mFov, mAspect, mZNear, mZFar);
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
        // TODO: implement
        glm::vec3 v[] = {
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
// main()
/*****************************************************************************/

int gWindowWidth = 1280;
int gWindowHeight = 720;
const char *gWindowTitle = "Cube";

PerspectiveCamera gCamera;
Cube gCube;

void window_size_callback(GLFWwindow* window, int width, int height)
{
    gWindowWidth = width;
    gWindowHeight = height;
    gCamera.setAspect(static_cast<float>(gWindowWidth) / gWindowHeight);
}

void initScene()
{
    // TODO: set the positions of your cube and camera
    gCamera.position().z = 5;
}

void update(float dt)
{
    gCube.orientation().y += dt * 10;
}

void render(float dt)
{
    glViewport(0, 0, gWindowWidth, gWindowHeight);
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
    gCamera.applyWorldToLocal();
    // Apply cube local-to-world transformation
    gCube.applyLocalToWorld();

    // TODO: Draw the cube
    gCube.draw(dt);
}

int main(void)
{
    GLFWwindow *window = nullptr;

    // Initialize the library
    if(!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(
        gWindowWidth, gWindowHeight, gWindowTitle,
        nullptr, nullptr);
    // If you cannot create the window, update your graphics driver first!
    if(!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    // Enable vertical sync
    glfwSwapInterval(1);

    // Register our callbacks so we can know when the events occur.
    // glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    initScene();

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

    glfwTerminate();
    return 0;
}
