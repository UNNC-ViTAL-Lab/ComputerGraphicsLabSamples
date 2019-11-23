#pragma once

#include <gl/glew.h>

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
#   define GLFW_EXPOSE_NATIVE_WIN32
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
#include <fstream>
#include <sstream>

// GLFW & GLM headers
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
// Requried for glm::type_ptr
#include <glm/gtc/type_ptr.hpp>
// Convers glm vector to string
#include <glm/gtx/string_cast.hpp>

// ImGUI
#include "imgui-1.73/imgui.cpp"
#include "imgui-1.73/imgui_widgets.cpp"
#include "imgui-1.73/imgui_draw.cpp"
#include "imgui-1.73/imgui_impl_opengl2.cpp"
#include "imgui-1.73/imgui_impl_glfw.cpp"

/*****************************************************************************/
// Texture
/*****************************************************************************/

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Read https://learnopengl.com/Getting-started/Textures for more details
// on textures.
class Texture
{
    GLuint mTextureId = 0;
    bool mLoaded = false;

    void create()
    {
        glGenTextures(1, &mTextureId);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

public:
    ~Texture()
    {
        if(mTextureId)
        {
            glDeleteTextures(1, &mTextureId);
            mTextureId = 0;
        }
    }

    void loadFromFile(const char *path)
    {
        if(mTextureId == 0) create();
        int width, height, num_channels;
        auto data = stbi_load(path, &width, &height, &num_channels, 4);
        glBindTexture(GL_TEXTURE_2D, mTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);
        mLoaded = true;
    }

    GLuint textureId() const { return mTextureId; }
    bool loaded() const { return mLoaded; }
};

/*****************************************************************************/
// Shader
/*****************************************************************************/

std::string readFileToString(const char *path)
{
    std::string code;
    std::ifstream input(path, std::ios::in);
    if(input.is_open())
    {
        std::stringstream buf;
        buf << input.rdbuf();
        code = buf.str();
        input.close();
    }
    else
    {
        throw std::runtime_error { "Could not open file." };
    }
    return code;
}

class Shader
{
    GLuint mProgram = 0;

    void deleteProgram()
    {
        if(mProgram)
        {
            glDeleteProgram(mProgram);
            mProgram = 0;
        }
    }

public:
    void load(const char *vertex_file_path, const char *fragment_file_path)
    {
        deleteProgram();

        // Original Source Code: http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/

        // Create the shaders
        GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

        // Read the Vertex Shader code from the file
        const auto vertex_shader_code = readFileToString(vertex_file_path);

        // Read the Fragment Shader code from the file
        const auto fragment_shader_code = readFileToString(fragment_file_path);

        GLint Result = GL_FALSE;
        int InfoLogLength;

        // Compile Vertex Shader
        printf("Compiling shader : %s\n", vertex_file_path);
        char const *vertex_source_pointer = vertex_shader_code.c_str();
        glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, NULL);
        glCompileShader(vertex_shader_id);

        // Check Vertex Shader
        glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if(InfoLogLength > 0)
        {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(vertex_shader_id, InfoLogLength, NULL,
                &VertexShaderErrorMessage[0]);
            printf("%s\n", &VertexShaderErrorMessage[0]);
        }

        // Compile Fragment Shader
        printf("Compiling shader : %s\n", fragment_file_path);
        char const *FragmentSourcePointer = fragment_shader_code.c_str();
        glShaderSource(fragment_shader_id, 1, &FragmentSourcePointer, NULL);
        glCompileShader(fragment_shader_id);

        // Check Fragment Shader
        glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if(InfoLogLength > 0)
        {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(fragment_shader_id, InfoLogLength, NULL,
                &FragmentShaderErrorMessage[0]);
            printf("%s\n", &FragmentShaderErrorMessage[0]);
        }

        // Link the program
        printf("Linking program\n");
        GLuint ProgramID = glCreateProgram();
        glAttachShader(ProgramID, vertex_shader_id);
        glAttachShader(ProgramID, fragment_shader_id);
        glLinkProgram(ProgramID);

        // Check the program
        glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if(InfoLogLength > 0)
        {
            std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
            glGetProgramInfoLog(ProgramID, InfoLogLength, NULL,
                &ProgramErrorMessage[0]);
            printf("%s\n", &ProgramErrorMessage[0]);
        }

        glDetachShader(ProgramID, vertex_shader_id);
        glDetachShader(ProgramID, fragment_shader_id);

        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);

        mProgram = ProgramID;
    }

    ~Shader()
    {
        deleteProgram();
    }

    GLuint programId() const { return mProgram; }
};

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

    Texture *mTexture = nullptr;
    Shader *mShader = nullptr;

public:
    // A class intended for inheriting must have a virtual destructor to
    // maintain correct destruction behavior.
    virtual ~Object() = default;

    // Below are setters & getters for member variables.

    void setTexture(Texture *texture) { mTexture = texture; }
    void setShader(Shader *shader) { mShader = shader; }

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

    auto localToWorldMatrix() const
    {
        auto s = glm::scale(glm::mat4(1), mScaling);
        // glm::rotate uses radians but glRotate uses degrees...!!! wth
        auto rx = glm::rotate(s,
            glm::radians(mOrientation.x), glm::vec3(1, 0, 0));
        auto ry = glm::rotate(rx,
            glm::radians(mOrientation.y), glm::vec3(0, 1, 0));
        auto rz = glm::rotate(ry,
            glm::radians(mOrientation.z), glm::vec3(0, 0, 1));
        auto t = glm::translate(rz, mPosition);

        // t * rz * ry * rx * s;

        return t;
    }

    auto worldToLocalMatrix() const
    {
        auto t = glm::translate(glm::mat4(1), -mPosition);
        auto rz = glm::rotate(t,
            glm::radians(-mOrientation.z), glm::vec3(0, 0, 1));
        auto ry = glm::rotate(rz,
            glm::radians(-mOrientation.y), glm::vec3(0, 1, 0));
        auto rx = glm::rotate(ry,
            glm::radians(-mOrientation.x), glm::vec3(1, 0, 0));
        auto s = glm::scale(rx, 1.f / mScaling);

        // s * rx * ry * rz * t;

        return s;
    }

    virtual void emitControlWidgets()
    {
        ImGui::DragFloat3("Position", &mPosition.x, 0.01f);
        ImGui::DragFloat3("Scaling", &mScaling.x, 0.01f);
        ImGui::DragFloat3("Orientation", &mOrientation.x, 0.01f);
    }

    void renderControlWidgetHierarchy()
    {
        if(ImGui::CollapsingHeader(
            typeid(*this).name(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::PushID(this);
            emitControlWidgets();
            ImGui::Indent(64);
            for(auto &&c : mChildObjects)
            {
                c->renderControlWidgetHierarchy();
            }
            ImGui::Unindent(64);
            ImGui::PopID();
        }
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
        if(mShader) glUseProgram(mShader->programId());
        if(mTexture) glBindTexture(GL_TEXTURE_2D, mTexture->textureId());
        // Call the draw() function of the derived class.
        draw(dt);
        if(mTexture) glBindTexture(GL_TEXTURE_2D, 0);
        if(mShader) glUseProgram(0);
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
    glTexCoord2f(0, 0); \
    glVertex3fv(value_ptr(array[a])); \
    glTexCoord2f(1, 0); \
    glVertex3fv(value_ptr(array[b])); \
    glTexCoord2f(1, 1); \
    glVertex3fv(value_ptr(array[c])); \
    glTexCoord2f(0, 1); \
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

/*****************************************************************************/
// Sphere
/*****************************************************************************/

class Sphere : public Object
{
    GLUquadric *mQuadric = gluNewQuadric();
    float mRadius = 1;

public:
    Sphere() = default;

    Sphere(float radius)
        : mRadius(radius)
    {
        gluQuadricTexture(mQuadric, GL_TRUE);
    }

    ~Sphere()
    {
        gluDeleteQuadric(mQuadric);
    }

    void draw(float dt) override
    {
        gluSphere(mQuadric, mRadius, 64, 64);
    }
};

void installCallbacks(GLFWwindow *window);
void initScene();
void render(float dt);
void update(GLFWwindow *window, float dt);
void mainLoop(GLFWwindow *window);

extern int gFramebufferWidth;
extern int gFramebufferHeight;
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

        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        update(window, dt);
        // Set up the camera and draw our scene
        render(dt);

        // Rendering
        ImGui::Render();
        glViewport(0, 0, gFramebufferWidth, gFramebufferHeight);
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

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
    glfwGetFramebufferSize(window, &gFramebufferWidth, &gFramebufferHeight);

    glfwMakeContextCurrent(window);
    glewInit();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL2_Init();

    installCallbacks(window);

    // Make the window's context current
    glfwMakeContextCurrent(window);
    // Enable vertical sync
    glfwSwapInterval(1);

    initScene();
    mainLoop(window);

    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
