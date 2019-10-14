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

// --------------------------------------------------------------------
// Window

int gFramebufferWidth = 1280;
int gFramebufferHeight = 720;
const char *gWindowTitle = "Cube";

// --------------------------------------------------------------------
// Perspective Camera

float gFov = 60;
float gAspect = 16 / 9.f;
float gZNear = 0.5f;
float gZFar = 100;
glm::vec3 gCameraPosition { 0, 0, 5 };

// --------------------------------------------------------------------
// Cube

float gCubeHalfSize = 0.5f;
glm::vec3 gCubeOrientation { 0, 0, 0 };

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    gFramebufferWidth = width;
    gFramebufferHeight = height;
    gAspect = static_cast<float>(gFramebufferWidth) / gFramebufferHeight;
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

    // Make the window's context current
    glfwMakeContextCurrent(window);
    // Enable vertical sync
    glfwSwapInterval(1);

    // Register our callbacks so we can know when the events occur.
    // glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    auto time = glfwGetTime();

    // Loop until the user closes the window
    while(!glfwWindowShouldClose(window))
    {
        const auto new_time = glfwGetTime();
        // Elapsed time since last frame, in seconds.
        const auto dt = new_time - time;
        time = new_time;

        // --------------------------------------------------------------------

        // rotate the cube with time
        gCubeOrientation.y += dt * 10;

        // --------------------------------------------------------------------

        // Clear the framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Set up the camera and draw our scene
        // render(static_cast<float>(dt));

        // --------------------------------------------------------------------

        glViewport(0, 0, gFramebufferWidth, gFramebufferHeight);
        glEnable(GL_DEPTH_TEST);

        // --------------------------------------------------------------------

        glMatrixMode(GL_PROJECTION);
        // Reset the matrix
        glLoadIdentity();
        // Apply projection matrix
        // gCamera.applyProjection();

        gluPerspective(gFov, gAspect, gZNear, gZFar);

        // --------------------------------------------------------------------

        glMatrixMode(GL_MODELVIEW);
        // Reset the matrix
        glLoadIdentity();
        // Apply camera world-to-local transformation
        // gCamera.applyWorldToLocal();
        glTranslatef(-gCameraPosition.x, -gCameraPosition.y, -gCameraPosition.z);

        // --------------------------------------------------------------------

        // Apply cube local-to-world transformation
        // gCube.applyLocalToWorld();
        glRotatef(gCubeOrientation.z, 0, 0, 1);
        glRotatef(gCubeOrientation.y, 0, 1, 0);
        glRotatef(gCubeOrientation.x, 1, 0, 0);

        // --------------------------------------------------------------------

        // TODO: Draw the cube
        // gCube.draw(dt);

        glm::vec3 v[] = {
            { gCubeHalfSize, gCubeHalfSize, gCubeHalfSize },
            { gCubeHalfSize, gCubeHalfSize, -gCubeHalfSize },
            { gCubeHalfSize, -gCubeHalfSize, gCubeHalfSize },
            { gCubeHalfSize, -gCubeHalfSize, -gCubeHalfSize },
            { -gCubeHalfSize, gCubeHalfSize, gCubeHalfSize },
            { -gCubeHalfSize, gCubeHalfSize, -gCubeHalfSize },
            { -gCubeHalfSize, -gCubeHalfSize, gCubeHalfSize },
            { -gCubeHalfSize, -gCubeHalfSize, -gCubeHalfSize },
        };

        glBegin(GL_QUADS);
            // top
            glColor3ub(169, 102, 194);
            glVertex3fv(value_ptr(v[1]));
            glVertex3fv(value_ptr(v[0]));
            glVertex3fv(value_ptr(v[4]));
            glVertex3fv(value_ptr(v[5]));
            // bottom
            glColor3ub(164, 33, 14);
            glVertex3fv(value_ptr(v[2]));
            glVertex3fv(value_ptr(v[3]));
            glVertex3fv(value_ptr(v[7]));
            glVertex3fv(value_ptr(v[6]));
            // left
            glColor3ub(228, 69, 147);
            glVertex3fv(value_ptr(v[5]));
            glVertex3fv(value_ptr(v[4]));
            glVertex3fv(value_ptr(v[6]));
            glVertex3fv(value_ptr(v[7]));
            // right
            glColor3ub(136, 157, 210);
            glVertex3fv(value_ptr(v[0]));
            glVertex3fv(value_ptr(v[1]));
            glVertex3fv(value_ptr(v[3]));
            glVertex3fv(value_ptr(v[2]));
            // front
            glColor3ub(138, 250, 122);
            glVertex3fv(value_ptr(v[1]));
            glVertex3fv(value_ptr(v[5]));
            glVertex3fv(value_ptr(v[7]));
            glVertex3fv(value_ptr(v[3]));
            // back
            glColor3ub(1, 37, 146);
            glVertex3fv(value_ptr(v[4]));
            glVertex3fv(value_ptr(v[0]));
            glVertex3fv(value_ptr(v[2]));
            glVertex3fv(value_ptr(v[6]));
        glEnd();

        // --------------------------------------------------------------------

        // Swap front and back buffers
        glfwSwapBuffers(window);
        // Poll for and process events, this will call into the callbacks
        // we set before when related events occur.
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
