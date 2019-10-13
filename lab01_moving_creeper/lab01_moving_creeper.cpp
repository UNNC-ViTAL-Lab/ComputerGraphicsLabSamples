#include <GLFW/glfw3.h>

#ifdef _WIN32
#   pragma comment(lib, "OpenGL32.lib")
#endif

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

float xOffset = 0;
float yOffset = 0;

float xSize = 1280;
float ySize = 720;

float pixelSize = 20;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    // We will explain this conversion in the next lab
    xOffset = xpos / xSize * 2 - 1;
    yOffset = 1 - ypos / ySize * 2;
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    xSize = width;
    ySize = height;
}

void render()
{
    glViewport(0, 0, xSize, ySize);

    float xstep = 2.f / xSize * pixelSize;
    float ystep = -2.f / ySize * pixelSize;
    float xcur = 0;
    float ycur = 0;

    glBegin(GL_QUADS);
    auto idx = 0;
    for (auto i = 0; i < 8; ++i)
    {
        for (auto j = 0; j < 8; ++j)
        {
            // There are several formats of color values for glColor() series of function.
            // Our data ranges from 0 to 255, so make sure you use the correct one!
            // See https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glColor.xml
            // for more details.

            // And the details are pasted here:

            // Current color values are stored in floating-point format, with unspecified 
            // mantissa and exponent sizes. Unsigned integer color components, when specified,
            // are linearly mapped to floating-point values such that the largest representable 
            // value maps to 1.0 (full intensity), and 0 maps to 0.0 (zero intensity). Signed
            // integer color components, when specified, are linearly mapped to floating-point 
            // values such that the most positive representable value maps to 1.0, and the most 
            // negative representable value maps to -1.0 . (Note that this mapping does not 
            // convert 0 precisely to 0.0.) Floating-point values are mapped directly.
            glColor3ubv(PIXELS[idx++]);

            // We will try better ways of dealing with the offsets in the next lab.
            glVertex2f(xOffset + xcur, yOffset + ycur);
            glVertex2f(xOffset + xcur + xstep, yOffset + ycur);
            glVertex2f(xOffset + xcur + xstep, yOffset + ycur + ystep);
            glVertex2f(xOffset + xcur, yOffset + ycur + ystep);
            xcur += xstep;
        }
        ycur += ystep;
        // REMEMBER TO RESET THE X CURSOR!
        xcur = 0;
    }
    glEnd();
}

int main(void)
{
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(xSize, ySize, "Creeper", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    // Enable vertical sync
    glfwSwapInterval(1);

    // Register our callbacks so we can know when the events occur.
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Clear the framebuffer
        glClear(GL_COLOR_BUFFER_BIT);

        render();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events, this will call into the callbacks
        // we set before when related events occur.
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
