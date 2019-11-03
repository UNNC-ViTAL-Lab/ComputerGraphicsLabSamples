#pragma once

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
    virtual void applyWorldToLocal() const
    {
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
    virtual void applyLocalToWorld() const
    {
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

    void drawTransformed(float dt)
    {
        // Save the current matrix
        glPushMatrix();
        // Apply cube local-to-world transformation
        applyLocalToWorld();
        draw(dt);
        // Restore to last saved matrix
        glPopMatrix();
    }

    /**
     * \brief Optional methods for updating your object with time.
     * \param dt
     */
    virtual void update(float dt)
    {
    }
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
    virtual void applyProjectionMatrix() const = 0;
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

    void applyProjectionMatrix() const override
    {
        gluPerspective(mFov, mAspect, mZNear, mZFar);
    }
};

/******************************************************************************/
// OrthogonalCamera
/******************************************************************************/

class OrthogonalCamera : public Camera
{
    float mLeft = 0;
    float mRight = 1280;
    float mTop = 0;
    float mBottom = 720;
    float mNear = -10;
    float mFar = 10;

public:
    OrthogonalCamera() = default;

    OrthogonalCamera(
        float left,
        float right,
        float top,
        float bottom,
        float near,
        float far)
        : mLeft(left)
        , mRight(right)
        , mTop(top)
        , mBottom(bottom)
        , mNear(near)
        , mFar(far)
    {
    }

    void setLeft(float left) { mLeft = left; }
    void setRight(float right) { mRight = right; }
    void setTop(float top) { mTop = top; }
    void setBottom(float bottom) { mBottom = bottom; }
    void setNear(float near) { mNear = near; }
    void setFar(float far) { mFar = far; }

    void applyProjectionMatrix() const override
    {
        glOrtho(mLeft, mRight, mBottom, mTop, mNear, mFar);
    }
};


void installCallbacks(GLFWwindow* window);
void initScene();
void render(float dt);
void update(float dt);
void mainLoop(GLFWwindow* window);

extern int gFramebufferWidth;
extern int gFramebufferHeight;
extern const char *gWindowTitle;

int main(void)
{
    GLFWwindow *window = nullptr;

    // Initialize the library
    if(!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(
        gFramebufferWidth, gFramebufferHeight, gWindowTitle,
        nullptr, nullptr);
    // If you cannot create the window, update your graphics driver first!
    if(!window)
    {
        glfwTerminate();
        return -1;
    }

    installCallbacks(window);

    // Make the window's context current
    glfwMakeContextCurrent(window);
    // Enable vertical sync
    glfwSwapInterval(1);

    initScene();
    mainLoop(window);

    glfwTerminate();
    return 0;
}
