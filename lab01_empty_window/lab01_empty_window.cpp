// Original Code:
// https://www.glfw.org/documentation.html

#include <GLFW/glfw3.h>

void render()
{
    // We will be doing something here in the future
    glBegin(GL_LINE_LOOP);
        glColor3d(1, 0, 0);
        glVertex2d(0, 0.5);
        glColor3d(0, 1, 0);
        glVertex2d(0.5, -0.5);
        glColor3d(0, 0, 1);
        glVertex2d(-0.5, -0.5);
    glEnd();
}

int main(void)
{
    GLFWwindow *window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(1280, 720, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    // Enable vertical sync
    glfwSwapInterval(1);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Clear the framebuffer
        glClear(GL_COLOR_BUFFER_BIT);

        render();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
