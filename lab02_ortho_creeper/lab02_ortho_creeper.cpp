#ifdef _WIN32
#   pragma comment(lib, "OpenGL32.lib")
#   define WIN32_LEAN_AND_MEAN
#   include <Windows.h>
#   undef near
#   undef far
#   undef max
#   undef min
#endif

#include <GLFW/glfw3.h>
#include <gl/GLU.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

/******************************************************************************/
// Object
/******************************************************************************/

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

/******************************************************************************/
// Camera
/******************************************************************************/

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

    void applyProjection() const override
    {
        // TODO: implement
        glOrtho(mLeft, mRight, mBottom, mTop, mNear, mFar);
    }
};

/******************************************************************************/
// Creeper
/******************************************************************************/

class Creeper : public Object
{
    // This is a creeper head
    const unsigned char PIXELS[64][3] = {
        { 159, 205, 165 }, { 15, 169, 19 }, { 65, 179, 76 }, { 157, 199, 157 },
        { 210, 210, 210 }, { 116, 220, 133 }, { 110, 204, 125 }, { 74, 144, 77 },
        { 38, 147, 41 }, { 89, 197, 95 }, { 54, 183, 65 }, { 101, 201, 110 },
        { 72, 145, 76 }, { 93, 220, 112 }, { 85, 188, 101 }, { 210, 210, 210 },
        { 54, 197, 71 }, { 23, 84, 31 }, { 49, 88, 56 }, { 140, 215, 147 },
        { 86, 152, 87 }, { 24, 78, 30 }, { 49, 88, 56 }, { 220, 220, 220 },
        { 61, 203, 76 }, { 24, 78, 30 }, { 0, 0, 0 }, { 85, 207, 103 },
        { 57, 178, 71 }, { 0, 0, 0 }, { 20, 58, 23 }, { 103, 206, 120 },
        { 130, 210, 137 }, { 87, 183, 101 }, { 142, 215, 148 }, { 20, 79, 28 },
        { 22, 76, 29 }, { 192, 210, 195 }, { 69, 140, 72 }, { 76, 208, 94 },
        { 154, 210, 164 }, { 78, 151, 81 }, { 49, 88, 56 }, { 0, 0, 0 },
        { 0, 0, 0 }, { 24, 85, 31 }, { 85, 215, 103 }, { 187, 210, 191 },
        { 187, 226, 194 }, { 102, 161, 103 }, { 0, 0, 0 }, { 0, 0, 0 },
        { 0, 0, 0 }, { 0, 0, 0 }, { 80, 188, 95 }, { 77, 152, 81 },
        { 85, 215, 103 }, { 112, 222, 130 }, { 20, 58, 23 }, { 132, 210, 147 },
        { 168, 208, 174 }, { 22, 76, 29 }, { 55, 142, 59 }, { 87, 207, 104 },
    };
    float pixelSize = 20;

public:
    void draw(float dt) override
    {
        float xcur = 0;
        float ycur = 0;

        glBegin(GL_QUADS);
        auto idx = 0;
        for (auto i = 0; i < 8; ++i)
        {
            for (auto j = 0; j < 8; ++j)
            {
                glColor3ubv(PIXELS[idx++]);

                glVertex2f(xcur, + ycur);
                glVertex2f(xcur + pixelSize, ycur);
                glVertex2f(xcur + pixelSize, ycur + pixelSize);
                glVertex2f(xcur, ycur + pixelSize);
                xcur += pixelSize;
            }
            ycur += pixelSize;
            // REMEMBER TO RESET THE X CURSOR!
            xcur = 0;
        }
        glEnd();
    }
};

/******************************************************************************/
// main()
/******************************************************************************/

int gWindowWidth = 1280;
int gWindowHeight = 720;
const char *gWindowTitle = "Cube";

OrthogonalCamera gCamera;
Creeper gCreeper;

void window_size_callback(GLFWwindow* window, int width, int height)
{
    gWindowWidth = width;
    gWindowHeight = height;
    gCamera.setRight(gWindowWidth);
    gCamera.setTop(gWindowHeight);
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    gCreeper.position().x = xpos;
    gCreeper.position().y = ypos;
}

void initScene()
{
}

void update(float dt)
{
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
    gCreeper.applyLocalToWorld();

    // TODO: Draw the cube
    gCreeper.draw(dt);
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
    glfwSetCursorPosCallback(window, cursor_position_callback);

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
