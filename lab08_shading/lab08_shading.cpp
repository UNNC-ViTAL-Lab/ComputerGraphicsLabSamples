#include "lab08_framework.hpp"

/*****************************************************************************/
// Scene Objects
/*****************************************************************************/

struct Material
{
    // These are default values as said in
    // https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glMaterial.xml

    glm::vec4 ambient { 0.2f, 0.2f, 0.2f, 1.0f };
    glm::vec4 diffuse { 0.8f, 0.8f, 0.8f, 1.0f };
    glm::vec4 specular { 0, 0, 0, 1 };
    glm::vec4 emission { 0, 0, 0, 1 };
    float shininess = 0;

    void emitControlWidgets()
    {
        ImGui::SliderFloat4("Ambient", &ambient.x, 0, 1);
        ImGui::SliderFloat4("Diffuse", &diffuse.x, 0, 1);
        ImGui::SliderFloat4("Specular", &specular.x, 0, 1);
        ImGui::SliderFloat4("Emission", &emission.x, 0, 1);
        ImGui::SliderFloat("Shininess", &shininess, 0, 128);
    }

    void apply()
    {
        glMaterialfv(GL_FRONT, GL_AMBIENT, &ambient.x);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, &diffuse.x);
        glMaterialfv(GL_FRONT, GL_SPECULAR, &specular.x);
        glMaterialfv(GL_FRONT, GL_EMISSION, &emission.x);
        glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);
    }
};

class Light : public Object
{
public:
    // See https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glLight.xml

    glm::vec4 ambient { 0, 0, 0, 1 };
    glm::vec4 diffuse { 1, 1, 1, 1 };
    glm::vec4 specular { 1, 1, 1, 1 };
    bool directional = true;
    int light_idx = GL_LIGHT0;

    Light() = default;

    explicit Light(int light_idx)
        : light_idx(light_idx)
    {
    }

    void emitControlWidgets() override
    {
        Object::emitControlWidgets();

        ImGui::SliderFloat4("Ambient", &ambient.x, 0, 1);
        ImGui::SliderFloat4("Diffuse", &diffuse.x, 0, 1);
        ImGui::SliderFloat4("Specular", &specular.x, 0, 1);
        ImGui::Checkbox("Directional", &directional);
    }

    void draw(float dt) override
    {
        glLightfv(light_idx, GL_AMBIENT, &ambient.x);
        glLightfv(light_idx, GL_DIFFUSE, &diffuse.x);
        glLightfv(light_idx, GL_SPECULAR, &specular.x);
        glm::vec4 pos { mPosition, directional ? 0 : 1 };
        glLightfv(light_idx, GL_POSITION, &pos.x);
    }
};

Object gSceneRoot;
auto *gAxis = gSceneRoot.addChild<Axis>();
auto *gGround = gSceneRoot.addChild<MeshGround>();
Material gMaterial;

// An observer camera which shows the world and the camera you are tweaking
auto *gLeftCamera = gSceneRoot.addChild<PerspectiveCamera>();

Object *activeObject(GLFWwindow *window)
{
    return gLeftCamera;
}

// Texture gCubeTex;
// Shader mBlinnPhong;

/*****************************************************************************/
// Scene Creation
/*****************************************************************************/

void updateCamera()
{
    const float aspect = 1.f * gFramebufferWidth / gFramebufferHeight;

    gLeftCamera->setAspect(aspect);
}

void initScene()
{
    updateCamera();

    gLeftCamera->position().z = 20;
    gLeftCamera->setZFar(1000);
    gLeftCamera->addChild<Axis>();

    gSceneRoot.addChild<Light>()->position().z = 10;
    gSceneRoot.addChild<Sphere>(5.f);

    // gCubeTex.create();
    // mBlinnPhong.load("blinn.vert", "blinn.frag");
    // gCube->setShader(&mBlinnPhong);
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

    glm::vec4 move { 0, 0, 0, 0 };

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

    auto active_object = activeObject(window);

    move = active_object->localToWorldMatrix() * move;
    active_object->position() += glm::vec3(move);
}

/*****************************************************************************/
// Scene Rendering
/*****************************************************************************/

bool gEnableNormalize = false;
bool gEnableLighting = true;
// enable first light, disable others
bool gEnabledLights[8] { true, false };

void drawViewport(float dt)
{
    glViewport(0, 0, gFramebufferWidth, gFramebufferHeight);

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
    glEnable(GL_TEXTURE_2D);
    // Normalize the normals after scaling the objects to get correct lighting
    if(gEnableNormalize) glEnable(GL_NORMALIZE);
    if(gEnableLighting) glEnable(GL_LIGHTING);
    for(auto i = 0; i < 8; ++i)
    {
        if(gEnabledLights[i])
            glEnable(GL_LIGHT0 + i);
    }

    gMaterial.apply();
    gSceneRoot.drawHierarchyTransformed(dt);

    using namespace ImGui;
    if(Begin("Scene Control"))
    {
        if(Button("100% Font Size"))
        {
            SetWindowFontScale(1.f);
        } SameLine();
        if(Button("200% Font Size"))
        {
            SetWindowFontScale(2.f);
        } SameLine();
        if(Button("300% Font Size"))
        {
            SetWindowFontScale(3.f);
        }

        if(CollapsingHeader("OpenGL Options", ImGuiTreeNodeFlags_DefaultOpen))
        {
            PushID("ogl");
            Checkbox("GL_NORMALIZE", &gEnableNormalize);
            SameLine();
            Checkbox("GL_LIGHTING", &gEnableLighting);
            for(auto i = 0; i < 8; ++i)
            {
                static char name[] = "GL_LIGHT#";
                name[8] = '0' + i;
                Checkbox(name, &gEnabledLights[i]);
                if((i + 1) % 4 != 0) SameLine();
            }
            PopID();
        }

        if(CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            PushID("mat");
            gMaterial.emitControlWidgets();
            PopID();
        }

        gSceneRoot.renderControlWidgetHierarchy();
    }
    End();
}

void render(float dt)
{
    // Clear the framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Save all current attributes
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    drawViewport(dt);
    glPopAttrib();
}

/*****************************************************************************/
// Window Management
/*****************************************************************************/

int gFramebufferWidth = 1280;
int gFramebufferHeight = 720;
const char *gWindowTitle = "Lab 8";

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    // Record the correct size of the viewport
    gFramebufferWidth = width;
    gFramebufferHeight = height;
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
    {
        activeObject(window)->orientation().y -= (float)dx * 0.1f;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    if(!action) return;

    switch(key)
    {
        case GLFW_KEY_H:
            gSceneRoot.printObjectHierarchy();
            break;

        default: ;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

void character_callback(GLFWwindow* window, unsigned int codepoint)
{
    ImGui_ImplGlfw_CharCallback(window, codepoint);
}

/*****************************************************************************/
// File Drop Handler
/*****************************************************************************/

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    // gCubeTex.loadFromFile(paths[0]);
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
    glfwSetDropCallback(window, drop_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCharCallback(window, character_callback);
}
