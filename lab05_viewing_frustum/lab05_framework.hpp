#pragma once

// Linking with OpenGL libraries in Windows
#ifdef _WIN32
#   pragma comment(lib, "OpenGL32.lib")
#   pragma comment(lib, "GLU32.lib")
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
// These macros defined by Windows can cause troubles, so get rid of them.
#   undef near
#   undef far
#   undef max
#   undef min
#   include <GL/gl.h>
#   include <GL/glu.h>
#endif

// MacOS has different headers for OpenGL
#ifdef __APPLE__
#   define GL_SILENCE_DEPRECATION
#   include <OpenGL/gl.h>
#   include <OpenGL/glu.h>
#endif

#include <vector>
#include <iostream>
#include <iterator>
#include <memory>

// GLFW & GLM headers
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
// Requried for glm::type_ptr
#include <glm/gtc/type_ptr.hpp>
// Convers glm vector to string
#include <glm/gtx/string_cast.hpp>

/*****************************************************************************/
// Object
/*****************************************************************************/

/**
 * \brief Base class of all objects that can modify the OpenGL states and
 * issueing drawing commands, including renderable objects, camera, or joints.
 */
class Object
{
protected:
    Object *mParent = nullptr;

    /**
     * \brief This variable stores the translation distance of the object
     * relative to the parent coordinate system. The value will be used with
     * glTranslate() when applying transformations.
     */
    glm::vec3 mPosition = { 0, 0, 0 };

    /**
     * \brief The angle of rotations applied around each axis X, Y, and Z.
     * You may override the transformation functions to change the order
     * they are applied.
     */
    glm::vec3 mOrientation = { 0, 0, 0 };

    /**
     * \brief The scaling of the object along each axis.
     */
    glm::vec3 mScaling = { 1, 1, 1 };

    // Note that these three variables only store the value. Changing them
    // does not magically lead to tranformation of the object. You must apply
    // the transformations using glXXXXX() functions.

    // Child objects whose parent coodinate system is this object.
    std::vector<std::unique_ptr<Object>> mChildObjects;

public:
    // A class intended for inheriting must have a virtual destructor to
    // maintain correct destruction behavior.
    virtual ~Object() = default;

    // Below are setters & getters for member variables.

    // Returns a reference to the variable storing translation.
    // You can directly modify the value via the reference.
    glm::vec3 & position() { return mPosition; }
    // Returns a reference to the rotation values.
    glm::vec3 & orientation() { return mOrientation; }
    // Returns a reference to the scaling values.
    glm::vec3 & scaling() { return mScaling; }

    // A helper function for setting the transformations. This is not applied
    // to OpenGL directly.
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
     * \brief Apply the transformation which transforms the coordinates from
     * the local coordinate system of this object to the coordinate system
     * of the parent object to the matrix stack. If this object is the root
     * object, it is equivalent to applying local-to-world transformation.
     * Remember to switch to modelview matrix mode before calling.
     * You can override this function in derived classes if you want a
     * different order of transformations.
     */
    virtual void applyLocalToParentMatrix() const
    {
        // The transformations are done in the following order:
        // Scale
        // Rotate around X-axis
        // Rotate around Y-axis
        // Rotate around Z-axis
        // Translate
        glTranslatef(mPosition.x, mPosition.y, mPosition.z);
        glRotatef(mOrientation.z, 0, 0, 1);
        glRotatef(mOrientation.y, 0, 1, 0);
        glRotatef(mOrientation.x, 1, 0, 0);
        glScalef(mScaling.x, mScaling.y, mScaling.z);
    }

    /**
    * \brief Apply the inverse transformation which transforms the coordinates
    * from parent coordinate system into the local coordinate system of
    * this object. Useful for cameras. If this object is the root
    * object, it is equivalent to applying world-to-local transformation.
    * Remember to switch to modelview matrix mode before calling.
    * You can override this function in derived classes if you want a
    * different order of transformations.
    */
    virtual void applyParentToLocalMatrix() const
    {
        // The transformations are done in the following order:
        // Translate back
        // Rotate around Z-axis in reverse direction
        // Rotate around Y-axis in reverse direction
        // Rotate around X-axis in reverse direction
        // Scale back
        glScalef(1 / mScaling.x, 1 / mScaling.y, 1 / mScaling.z);
        glRotatef(-mOrientation.x, 1, 0, 0);
        glRotatef(-mOrientation.y, 0, 1, 0);
        glRotatef(-mOrientation.z, 0, 0, 1);
        glTranslatef(-mPosition.x, -mPosition.y, -mPosition.z);
    }

    void applyLocalToWorldMatrix() const
    {
        if(mParent) mParent->applyLocalToWorldMatrix();
        applyLocalToParentMatrix();
    }

    void applyWorldToLocalMatrix() const
    {
        applyParentToLocalMatrix();
        if(mParent) mParent->applyWorldToLocalMatrix();
    }

    /**
     * \brief Issue drawing commands like glBegin() & glEnd().
     * \param dt The elapsed time since last frame. Useful for animations.
     */
    virtual void draw(float dt)
    {
    }

    void drawHierarchyTransformed(float dt)
    {
        // Save the current matrix
        glPushMatrix();
        // Apply cube local-to-parent transformation, might be overridden in
        // derived classes.
        applyLocalToParentMatrix();
        // Call the draw() function of the derived class.
        draw(dt);
        for(auto &&c : mChildObjects)
        {
            c->drawHierarchyTransformed(dt);
        }
        // Restore to last saved matrix
        glPopMatrix();
    }

    /**
     * \brief A helper function which draws this object with its tranformations.
     * \param dt The elapsed time since last frame. Useful for animations.
     * This function is NOT overridable.
     */
    void drawTransformed(float dt)
    {
        // Save the current matrix
        glPushMatrix();
        // Apply cube local-to-parent transformation, might be overridden in
        // derived classes.
        applyLocalToParentMatrix();
        // Call the draw() function of the derived class.
        draw(dt);
        // Restore to last saved matrix
        glPopMatrix();
    }

    /**
     * \brief Optional methods for updating your object with time.
     * \param dt The elapsed time since last frame. Useful for animations.
     */
    virtual void update(float dt)
    {
    }

    /**
     * \brief Add a child object to this object.
     * \tparam T The type of the child.
     * \tparam Args
     * \param args The parameters used to construct the child.
     */
    template <typename T, typename... Args> // <- a variadic template
    T * addChild(Args &&...args)
    {
        // Ensure you are creating an Object or derived type
        static_assert(std::is_base_of_v<Object, T>,
            "T is not a derived type of Object!");
        // Create the child object and push it into the children vector
        mChildObjects.push_back(std::make_unique<T>(
            std::forward<Args>(args)...));
        // Link the child with parent
        mChildObjects.back()->mParent = this;
        // Return a pointer to the newly created child
        return static_cast<T*>(mChildObjects.back().get());
    }

#define INDENT(i) \
std::fill_n(std::ostream_iterator<char>(std::cout), i, ' ') \
/**/

    /**
     * \brief A helper function which prints the hierarchy of the objects.
     * It might be useful for debugging.
     * \param indentation
     */
    void printObjectHierarchy(int indentation = 0) const
    {
        INDENT(indentation);
        std::cout << typeid(*this).name() << std::endl;
        printInfo(indentation + 2);
        for(auto &&child : mChildObjects)
        {
            child->printObjectHierarchy(indentation + 2);
        }
    }

    /**
     * \brief Overridable function for printing type-specific information
     * which helps you to debug.
     */
    virtual void printInfo(int indentation) const
    {
        INDENT(indentation);
        std::cout << "Scaling  = " << to_string(mScaling) << std::endl;
        INDENT(indentation);
        std::cout << "Rotation = " << to_string(mOrientation) << std::endl;
        INDENT(indentation);
        std::cout << "Position = " << to_string(mPosition) << std::endl;
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
        // Usually we don't draw the cameras.
    }

    /**
     * \brief Apply the projection matrix.
     */
    virtual void applyProjectionMatrix() const = 0;

    virtual void applyInverseProjectionMatrix() const = 0;
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

    // Constructor with parameters
    PerspectiveCamera(float fov, float aspect, float near, float far)
        : mFov(fov)
        , mAspect(aspect)
        , mZNear(near)
        , mZFar(far)
    {
    }

    void setFovY(float fov) { mFov = fov; }
    void setAspect(float aspect) { mAspect = aspect; }
    void setZNear(float near) { mZNear = near; }
    void setZFar(float far) { mZFar = far; }

    void applyProjectionMatrix() const override
    {
        gluPerspective(mFov, mAspect, mZNear, mZFar);
    }

    void applyInverseProjectionMatrix() const override
    {
        auto mat = glm::perspective(mFov, mAspect, mZNear, mZFar);
        mat = inverse(mat);
        glMultMatrixf(value_ptr(mat));
    }
};

/******************************************************************************/
// OrthogonalCamera
/******************************************************************************/

class OrthogonalCamera : public Camera
{
    float mLeft = 0;
    float mRight = 1280;
    float mTop = 720;
    float mBottom = 0;
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

    void applyInverseProjectionMatrix() const override
    {
        auto mat = glm::ortho(mLeft, mRight, mBottom, mTop, mNear, mFar);
        mat = inverse(mat);
        glMultMatrixf(value_ptr(mat));
    }

    void flipY()
    {
        std::swap(mTop, mBottom);
    }
};

/*****************************************************************************/
// Axis
/*****************************************************************************/

/**
 * \brief Colored axes showing the coordinate system. Useful for debugging.
 */
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

/**
 * \brief A customizable ground made using line mesh. Useful for perceving the
 * position of other objects.
 */
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

/**
 * \brief A unit cube.
 */
class Cube : public Object
{
    float mHalfSize = 0.5f;
    int mAlpha = 255;

public:
    Cube() = default;

    Cube(float half_size)
        : mHalfSize(half_size)
    {
    }

    void setHalfSize(float half_size) { mHalfSize = half_size; }
    void setAlpha(int alpha) { mAlpha = alpha; }

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
        glColor4ub(169, 102, 194, mAlpha);
        // This is calling the macro defined above!
        QUAD_INDICES(v, 1, 0, 4, 5);
        // bottom
        glColor4ub(164, 33, 14, mAlpha);
        QUAD_INDICES(v, 2, 3, 7, 6);
        // left
        glColor4ub(228, 69, 147, mAlpha);
        QUAD_INDICES(v, 5, 4, 6, 7);
        // right
        glColor4ub(136, 157, 210, mAlpha);
        QUAD_INDICES(v, 0, 1, 3, 2);
        // front
        glColor4ub(138, 250, 122, mAlpha);
        QUAD_INDICES(v, 1, 5, 7, 3);
        // back
        glColor4ub(1, 37, 146, mAlpha);
        QUAD_INDICES(v, 4, 0, 2, 6);
        glEnd();
    }
};

void installCallbacks(GLFWwindow *window);
void initScene();
void render(float dt);
void update(GLFWwindow *window, float dt);
void mainLoop(GLFWwindow *window);

extern float gFramebufferWidth;
extern float gFramebufferHeight;
extern const char *gWindowTitle;

void mainLoop(GLFWwindow *window)
{
    auto time = glfwGetTime();

    // Loop until the user closes the window
    while(!glfwWindowShouldClose(window))
    {
        const auto new_time = glfwGetTime();
        // Elapsed time since last frame, in seconds.
        const auto dt = static_cast<float>(new_time - time);
        time = new_time;

        update(window, dt);
        // Set up the camera and draw our scene
        render(dt);
        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Poll for and process events, this will call into the callbacks
        // we set before when related events occur.
        glfwPollEvents();
    }
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
        gFramebufferWidth, gFramebufferHeight, gWindowTitle,
        nullptr, nullptr);
    // If you cannot create the window, update your graphics driver first!
    if(!window)
    {
        glfwTerminate();
        return -1;
    }
    // Get the correct size of the framebuffer.
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    gFramebufferWidth = w;
    gFramebufferHeight = h;

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
