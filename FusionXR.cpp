// Copyright (c) 2017-2020 The Khronos Group Inc.
// Copyright (c) 2020 ReliaSolve LLC.
//
// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <stdlib.h>
#include <string>
#include <iostream>
#include <array>
#include <map>

#include "pch.h"
#include "common.h"
#include "gfxwrapper_opengl.h"
#include "xr_linear.h"
// Include the file that describes the cubes we draw in this example.
//#include "geometry.h"
#define GLFW_INCLUDE_GLU  
#define GLFW_EXPOSE_NATIVE_WGL 
#define GLFW_EXPOSE_NATIVE_WIN32
//#include "GLFW/glfw3.h"
//#include "GLFW/glfw3native.h"
#include "FusionXR.h"


//#include <irrlicht.h>


unsigned g_verbosity = 1;
bool renderForPc=false;
bool g_sessionRunning = false;
static void Usage(std::string name)
{
    std::cout << "Usage: " << name << " [--verbosity V]" << std::endl;
    std::cout << "       --verbosity: Set V to 0 for silence, higher for more info (default " << g_verbosity << ")" << std::endl;
}

//============================================================================================
// Helper functions.

namespace Math {
    namespace Pose {
        XrPosef Identity() {
            XrPosef t{};
            t.orientation.w = 1;
            return t;
        }

        XrPosef Translation(const XrVector3f& translation) {
            XrPosef t = Identity();
            t.position = translation;
            return t;
        }

        XrPosef RotateCCWAboutYAxis(float radians, XrVector3f translation) {
            XrPosef t = Identity();
            t.orientation.x = 0.f;
            t.orientation.y = std::sin(radians * 0.5f);
            t.orientation.z = 0.f;
            t.orientation.w = std::cos(radians * 0.5f);
            t.position = translation;
            return t;
        }
    }  // namespace Pose
}  // namespace Math

//============================================================================================
// Code to handle knowing which spaces things are rendered in.
/// @todo If you only want to render in world space, all of the space-choosing machinery
/// can be removed.  Note that the hands are spaces in addition to the application-defined ones.

// Maps from the space back to its name so we can know what to render in each
std::map<XrSpace, std::string> g_spaceNames;

// Description of one of the spaces we want to render in, along with a scale factor to
// be applied in that space.  In the original example, this is used to position, orient,
// and scale cubes to various spaces including hand space.
struct Space {
    XrPosef Pose;           ///< Pose of the space relative to g_appSpace
    XrVector3f Scale;       ///< Scale hint for the space
    std::string Name;       ///< An identifier so we can know what to render in each space
};

//============================================================================================
// OpenGL state and functions.

constexpr float DarkSlateGray[] = { 0.184313729f, 0.309803933f, 0.309803933f, 1.0f };

#ifdef XR_USE_PLATFORM_WIN32
XrGraphicsBindingOpenGLWin32KHR g_graphicsBinding{ XR_TYPE_GRAPHICS_BINDING_OPENGL_WIN32_KHR };
#elif defined(XR_USE_PLATFORM_XLIB)
XrGraphicsBindingOpenGLXlibKHR g_graphicsBinding{ XR_TYPE_GRAPHICS_BINDING_OPENGL_XLIB_KHR };
#elif defined(XR_USE_PLATFORM_XCB)
XrGraphicsBindingOpenGLXcbKHR g_graphicsBinding{ XR_TYPE_GRAPHICS_BINDING_OPENGL_XCB_KHR };
#elif defined(XR_USE_PLATFORM_WAYLAND)
XrGraphicsBindingOpenGLWaylandKHR g_graphicsBinding{ XR_TYPE_GRAPHICS_BINDING_OPENGL_WAYLAND_KHR };
#endif

ksGpuWindow g_window{};

std::list<std::vector<XrSwapchainImageOpenGLKHR>> g_swapchainImageBuffers;
GLuint g_swapchainFramebuffer{ 0 };
GLuint g_program{ 0 };
GLint g_modelViewProjectionUniformLocation{ 0 };
GLint g_vertexAttribCoords{ 0 };
GLint g_vertexAttribColor{ 0 };
GLuint g_vao{ 0 };
GLuint g_cubeVertexBuffer{ 0 };
GLuint g_cubeIndexBuffer{ 0 };

// Map color buffer to associated depth buffer. This map is populated on demand.
std::map<uint32_t, uint32_t> g_colorToDepthMap;

static const char* VertexShaderGlsl = R"_(
    #version 410

    in vec3 VertexPos;
    in vec3 VertexColor;

    out vec3 PSVertexColor;

    uniform mat4 ModelViewProjection;

    void main() {
       gl_Position = ModelViewProjection * vec4(VertexPos, 1.0);
       PSVertexColor = VertexColor;
    }
    )_";

static const char* FragmentShaderGlsl = R"_(
    #version 410

    in vec3 PSVertexColor;
    out vec4 FragColor;

    void main() {
       FragColor = vec4(PSVertexColor, 1);
    }
    )_";

void CheckShader(GLuint shader) {
    GLint r = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &r);
    if (r == GL_FALSE) {
        GLchar msg[4096] = {};
        GLsizei length;
        glGetShaderInfoLog(shader, sizeof(msg), &length, msg);
        THROW(Fmt("Compile shader failed: %s", msg));
    }
}

void CheckProgram(GLuint prog) {
    GLint r = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &r);
    if (r == GL_FALSE) {
        GLchar msg[4096] = {};
        GLsizei length;
        glGetProgramInfoLog(prog, sizeof(msg), &length, msg);
        THROW(Fmt("Link program failed: %s", msg));
    }
}

static void OpenGLInitializeResources()
{
    glGenFramebuffers(1, &g_swapchainFramebuffer);

    /*GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VertexShaderGlsl, nullptr);
    glCompileShader(vertexShader);
    CheckShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FragmentShaderGlsl, nullptr);
    glCompileShader(fragmentShader);
    CheckShader(fragmentShader);

    g_program = glCreateProgram();
    glAttachShader(g_program, vertexShader);
    glAttachShader(g_program, fragmentShader);
    glLinkProgram(g_program);
    CheckProgram(g_program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    g_modelViewProjectionUniformLocation = glGetUniformLocation(g_program, "ModelViewProjection");

    g_vertexAttribCoords = glGetAttribLocation(g_program, "VertexPos");
    g_vertexAttribColor = glGetAttribLocation(g_program, "VertexColor");

    glGenBuffers(1, &g_cubeVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, g_cubeVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Geometry::c_cubeVertices), Geometry::c_cubeVertices, GL_STATIC_DRAW);

    glGenBuffers(1, &g_cubeIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_cubeIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Geometry::c_cubeIndices), Geometry::c_cubeIndices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &g_vao);
    glBindVertexArray(g_vao);
    glEnableVertexAttribArray(g_vertexAttribCoords);
    glEnableVertexAttribArray(g_vertexAttribColor);
    glBindBuffer(GL_ARRAY_BUFFER, g_cubeVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_cubeIndexBuffer);
    glVertexAttribPointer(g_vertexAttribCoords, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex), nullptr);
    glVertexAttribPointer(g_vertexAttribColor, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry::Vertex),
        reinterpret_cast<const void*>(sizeof(XrVector3f)));
    */
    //device = createDevice(video::EDT_OPENGL, irr::core::dimension2d<u32>(640, 480), 16, false, false, false, 0);
    //device->getSceneManager()->addCubeSceneNode();
    //device->getSceneManager()->addCameraSceneNode(0, irr::core::vector3df(0, 30, -40), irr::core::vector3df(0, 5, 0));
    //auto videoData = device->getVideoDriver()->getExposedVideoData().OpenGLWin32;
    

}
//GLFWwindow* glfwWindow; // (In the accompanying source code, this variable is global for simplicity)

static void OpenGLInitializeDevice(XrInstance instance, XrSystemId systemId)
{
    // Extension function must be loaded by name
    PFN_xrGetOpenGLGraphicsRequirementsKHR pfnGetOpenGLGraphicsRequirementsKHR = nullptr;
    CHECK_XRCMD(xrGetInstanceProcAddr(instance, "xrGetOpenGLGraphicsRequirementsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&pfnGetOpenGLGraphicsRequirementsKHR)));

    XrGraphicsRequirementsOpenGLKHR graphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_OPENGL_KHR };
    CHECK_XRCMD(pfnGetOpenGLGraphicsRequirementsKHR(instance, systemId, &graphicsRequirements));

    // Initialize the gl extensions. Note we have to open a window.
    ksDriverInstance driverInstance{};
    ksGpuQueueInfo queueInfo{};
    ksGpuSurfaceColorFormat colorFormat{ KS_GPU_SURFACE_COLOR_FORMAT_B8G8R8A8 };
    ksGpuSurfaceDepthFormat depthFormat{ KS_GPU_SURFACE_DEPTH_FORMAT_D24 };
    ksGpuSampleCount sampleCount{ KS_GPU_SAMPLE_COUNT_1 };

    //if (!ksGpuWindow_Create(&g_window, &driverInstance, &queueInfo, 0, colorFormat, depthFormat, sampleCount, 640, 480, false)) {
    //    THROW("Unable to create GL context");
   // }
    g_graphicsBinding.hDC = wglGetCurrentDC();
    g_graphicsBinding.hGLRC = wglGetCurrentContext();

    //g_graphicsBinding.hDC = reinterpret_cast<HDC>(videoData.HDc);//GetDC(glfwGetWin32Window(glfwWindow));//g_window.context.hDC;
    //g_graphicsBinding.hGLRC = reinterpret_cast<HGLRC>(videoData.HRc);//g_window.context.hGLRC;

    /*if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return;
    }
    glfwWindowHint(GLFW_SAMPLES, sampleCount);
    glfwWindowHint(GLFW_DEPTH_BITS, depthFormat);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_VERSION_MAJOR); // We want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_VERSION_MINOR);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); // We don't want the old OpenGL 
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Open a window and create its OpenGL context
    glfwWindow = glfwCreateWindow(640, 480, "Tutorial 01", NULL, NULL);
    if (glfwWindow == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(glfwWindow); // Initialize GLEW
    GlInitExtensions();
    glfwWindowHwnd = glfwGetWin32Window(glfwWindow);
    */
    
    GlInitExtensions();
    GLint major = 0;
    GLint minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    const XrVersion desiredApiVersion = XR_MAKE_VERSION(major, minor, 0);
    std::cout << "Min OpenGL version: " << graphicsRequirements.minApiVersionSupported << std::endl;
    std::cout << "OpenGL version: " << desiredApiVersion << std::endl;
    if (graphicsRequirements.minApiVersionSupported > desiredApiVersion) {
        //THROW("Runtime does not support desired Graphics API and/or version");
    }
#ifdef XR_USE_PLATFORM_WIN32
    //g_graphicsBinding.hDC = GetDC(glfwGetWin32Window(glfwWindow));//g_window.context.hDC;
    //g_graphicsBinding.hGLRC = wglGetCurrentContext();//g_window.context.hGLRC;
#elif defined(XR_USE_PLATFORM_XLIB)
    g_graphicsBinding.xDisplay = g_window.context.xDisplay;
    g_graphicsBinding.visualid = g_window.context.visualid;
    g_graphicsBinding.glxFBConfig = g_window.context.glxFBConfig;
    g_graphicsBinding.glxDrawable = g_window.context.glxDrawable;
    g_graphicsBinding.glxContext = g_window.context.glxContext;
#elif defined(XR_USE_PLATFORM_XCB)
    // TODO: Still missing the platform adapter, and some items to make this usable.
    g_graphicsBinding.connection = g_window.connection;
    // g_graphicsBinding.screenNumber = g_window.context.screenNumber;
    // g_graphicsBinding.fbconfigid = g_window.context.fbconfigid;
    g_graphicsBinding.visualid = g_window.context.visualid;
    g_graphicsBinding.glxDrawable = g_window.context.glxDrawable;
    // g_graphicsBinding.glxContext = g_window.context.glxContext;
#elif defined(XR_USE_PLATFORM_WAYLAND)
    // TODO: Just need something other than NULL here for now (for validation).  Eventually need
    //       to correctly put in a valid pointer to an wl_display
    g_graphicsBinding.display = reinterpret_cast<wl_display*>(0xFFFFFFFF);
#endif

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message,
            const void* userParam) {
                std::cout << "GL Debug: " << std::string(message, 0, length) << std::endl;
        },
        nullptr);

    OpenGLInitializeResources();
}

static int64_t OpenGLSelectColorSwapchainFormat(const std::vector<int64_t>& runtimeFormats)
{
    // List of supported color swapchain formats.
    constexpr int64_t SupportedColorSwapchainFormats[] = {
        GL_RGB10_A2,
        GL_RGBA16F,
        // The two below should only be used as a fallback, as they are linear color formats without enough bits for color
        // depth, thus leading to banding.
        GL_RGBA8,
        GL_RGBA8_SNORM,
    };

    auto swapchainFormatIt =
        std::find_first_of(runtimeFormats.begin(), runtimeFormats.end(), std::begin(SupportedColorSwapchainFormats),
            std::end(SupportedColorSwapchainFormats));
    if (swapchainFormatIt == runtimeFormats.end()) {
        THROW("No runtime swapchain format supported for color swapchain");
    }

    return *swapchainFormatIt;
}

static std::vector<XrSwapchainImageBaseHeader*> OpenGLAllocateSwapchainImageStructs(
    uint32_t capacity, const XrSwapchainCreateInfo& )
{
    // Allocate and initialize the buffer of image structs (must be sequential in memory for xrEnumerateSwapchainImages).
    // Return back an array of pointers to each swapchain image struct so the consumer doesn't need to know the type/size.
    std::vector<XrSwapchainImageOpenGLKHR> swapchainImageBuffer(capacity);
    std::vector<XrSwapchainImageBaseHeader*> swapchainImageBase;
    for (XrSwapchainImageOpenGLKHR& image : swapchainImageBuffer) {
        image.type = XR_TYPE_SWAPCHAIN_IMAGE_OPENGL_KHR;
        swapchainImageBase.push_back(reinterpret_cast<XrSwapchainImageBaseHeader*>(&image));
    }

    // Keep the buffer alive by moving it into the list of buffers.
    g_swapchainImageBuffers.push_back(std::move(swapchainImageBuffer));

    return swapchainImageBase;
}

static uint32_t OpenGLGetDepthTexture(uint32_t colorTexture)
{
    // If a depth-stencil view has already been created for this back-buffer, use it.
    auto depthBufferIt = g_colorToDepthMap.find(colorTexture);
    if (depthBufferIt != g_colorToDepthMap.end()) {
        return depthBufferIt->second;
    }

    // This back-buffer has no corresponding depth-stencil texture, so create one with matching dimensions.

    GLint width;
    GLint height;
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

    uint32_t depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    g_colorToDepthMap.insert(std::make_pair(colorTexture, depthTexture));

    return depthTexture;
}



XrCompositionLayerProjectionView currentLayerInfo;
static void OpenGLRenderView(const XrCompositionLayerProjectionView& layerView, const XrSwapchainImageBaseHeader* swapchainImage,
    int64_t swapchainFormat, const std::vector<Space>& spaces, int viewIndex)
{
    glBindFramebuffer(GL_FRAMEBUFFER, g_swapchainFramebuffer);
    const uint32_t colorTexture = reinterpret_cast<const XrSwapchainImageOpenGLKHR*>(swapchainImage)->image;

    glViewport(static_cast<GLint>(layerView.subImage.imageRect.offset.x),
        static_cast<GLint>(layerView.subImage.imageRect.offset.y),
        static_cast<GLsizei>(layerView.subImage.imageRect.extent.width),
        static_cast<GLsizei>(layerView.subImage.imageRect.extent.height));

    const uint32_t depthTexture = OpenGLGetDepthTexture(colorTexture);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    

    gayBeginScene();
    memcpy(&currentLayerInfo, &layerView, sizeof(XrCompositionLayerProjectionView));
    gayDrawAll();
    gayEndScene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (viewIndex == 1)
    {
        renderForPc = true;
        gayBeginScene();
        gayDrawAll();
        gayEndScene();
        renderForPc = false;
    }
    
}


static void OpenGLTearDown()
{
    if (g_swapchainFramebuffer != 0) {
        glDeleteFramebuffers(1, &g_swapchainFramebuffer);
    }
    if (g_program != 0) {
        glDeleteProgram(g_program);
    }
    if (g_vao != 0) {
        glDeleteVertexArrays(1, &g_vao);
    }
    if (g_cubeVertexBuffer != 0) {
        glDeleteBuffers(1, &g_cubeVertexBuffer);
    }
    if (g_cubeIndexBuffer != 0) {
        glDeleteBuffers(1, &g_cubeIndexBuffer);
    }

    for (auto& colorToDepth : g_colorToDepthMap) {
        if (colorToDepth.second != 0) {
            glDeleteTextures(1, &colorToDepth.second);
        }
    }
}


//============================================================================================
// OpenXR state and functions.

#if !defined(XR_USE_PLATFORM_WIN32)
#define strcpy_s(dest, source) strncpy((dest), (source), sizeof(dest))
#endif

namespace Side {
    const int LEFT = 0;
    const int RIGHT = 1;
    const int COUNT = 2;
}  // namespace Side

struct InputState {
    XrActionSet actionSet{ XR_NULL_HANDLE };
    XrAction triggerAction{ XR_NULL_HANDLE };
    XrAction joystickXAction{ XR_NULL_HANDLE };
    XrAction joystickYAction{ XR_NULL_HANDLE };
    XrAction grabAction{ XR_NULL_HANDLE };
    XrAction poseAction{ XR_NULL_HANDLE };
    XrAction vibrateAction{ XR_NULL_HANDLE };
    XrAction quitAction{ XR_NULL_HANDLE };
    std::array<XrPath, Side::COUNT> handSubactionPath;
    std::array<XrSpace, Side::COUNT> handSpace;
    std::array<float, Side::COUNT> handScale = { {1.0f, 1.0f} };
    std::array<XrBool32, Side::COUNT> handActive;
};

struct Swapchain {
    XrSwapchain handle;
    int32_t width;
    int32_t height;
};

XrInstance g_instance{ XR_NULL_HANDLE };
XrSession g_session{ XR_NULL_HANDLE };
XrSpace g_appSpace{ XR_NULL_HANDLE };
XrFormFactor g_formFactor{ XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY };
XrViewConfigurationType g_viewConfigType{ XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO };
XrEnvironmentBlendMode g_environmentBlendMode{ XR_ENVIRONMENT_BLEND_MODE_OPAQUE };
XrSystemId g_systemId{ XR_NULL_SYSTEM_ID };

std::vector<XrViewConfigurationView> g_configViews;
std::vector<Swapchain> g_swapchains;
std::map<XrSwapchain, std::vector<XrSwapchainImageBaseHeader*>> g_swapchainImages;
std::vector<XrView> g_views;
int64_t g_colorSwapchainFormat{ -1 };

std::vector<XrSpace> g_visualizedSpaces;

// Application's current lifecycle state according to the runtime
XrSessionState g_sessionState{ XR_SESSION_STATE_UNKNOWN };

XrEventDataBuffer g_eventDataBuffer;
InputState g_input;

void OpenXRCreateInstance()
{
    g_verbosity = 1000;
#ifdef XR_USE_PLATFORM_WIN32
    CHECK_HRCMD(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
#endif

    CHECK(g_instance == XR_NULL_HANDLE);

    // Create union of extensions required by OpenGL.
    std::vector<const char*> extensions = { XR_KHR_OPENGL_ENABLE_EXTENSION_NAME };

    XrInstanceCreateInfo createInfo{ XR_TYPE_INSTANCE_CREATE_INFO };
    createInfo.next = nullptr;  // Needs to be set on Android.
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.enabledExtensionNames = extensions.data();
    

    /// @todo Change the application name here.
    strcpy(createInfo.applicationInfo.applicationName, "OpenXR-OpenGL-Example");
    createInfo.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;

    CHECK_XRCMD(xrCreateInstance(&createInfo, &g_instance));
}

static XrReferenceSpaceCreateInfo GetXrReferenceSpaceCreateInfo(const std::string& referenceSpaceTypeStr) {
    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{ XR_TYPE_REFERENCE_SPACE_CREATE_INFO };
    referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::Identity();
    if (EqualsIgnoreCase(referenceSpaceTypeStr, "View")) {
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "ViewFront")) {
        // Render head-locked 2m in front of device.
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::Translation({ 0.f, 0.f, -2.f }),
            referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "Local")) {
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "Stage")) {
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageLeft")) {
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(0.f, { -2.f, 0.f, -2.f });
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageRight")) {
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(0.f, { 2.f, 0.f, -2.f });
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageLeftRotated")) {
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(3.14f / 3.f, { -2.f, 0.5f, -2.f });
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    }
    else if (EqualsIgnoreCase(referenceSpaceTypeStr, "StageRightRotated")) {
        referenceSpaceCreateInfo.poseInReferenceSpace = Math::Pose::RotateCCWAboutYAxis(-3.14f / 3.f, { 2.f, 0.5f, -2.f });
        referenceSpaceCreateInfo.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    }
    else {
        throw std::invalid_argument(Fmt("Unknown reference space type '%s'", referenceSpaceTypeStr.c_str()));
    }
    return referenceSpaceCreateInfo;
}

/// @todo Change these to match the desired behavior.
struct Options {
    std::string GraphicsPlugin;
    XrFormFactor FormFactor{ XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY };
    XrViewConfigurationType ViewConfiguration{ XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO };
    XrEnvironmentBlendMode EnvironmentBlendMode{ XR_ENVIRONMENT_BLEND_MODE_OPAQUE };
    std::string AppSpace{ "Local" };
} g_options;

void OpenXRInitializeSystem()
{
    CHECK(g_instance != XR_NULL_HANDLE);
    CHECK(g_systemId == XR_NULL_SYSTEM_ID);

    g_formFactor = g_options.FormFactor;
    g_viewConfigType = g_options.ViewConfiguration;
    g_environmentBlendMode = g_options.EnvironmentBlendMode;

    XrSystemGetInfo systemInfo{ XR_TYPE_SYSTEM_GET_INFO };
    systemInfo.formFactor = g_formFactor;
    CHECK_XRCMD(xrGetSystem(g_instance, &systemInfo, &g_systemId));

    if (g_verbosity >= 2) std::cout << "Using system " << g_systemId
        << " for form factor " << to_string(g_formFactor) << std::endl;
    CHECK(g_instance != XR_NULL_HANDLE);
    CHECK(g_systemId != XR_NULL_SYSTEM_ID);

    /// @todo Print information about the system here in verbose mode.

    // The graphics API can initialize the graphics device now that the systemId and instance
    // handle are available.
    OpenGLInitializeDevice(g_instance, g_systemId);//TODO
}

/// @todo Change the behaviors by modifying the action bindings.

void OpenXRInitializeActions() {
    // Create an action set.
    {
        XrActionSetCreateInfo actionSetInfo{ XR_TYPE_ACTION_SET_CREATE_INFO };
        strcpy_s(actionSetInfo.actionSetName, "gameplay");
        strcpy_s(actionSetInfo.localizedActionSetName, "Gameplay");
        actionSetInfo.priority = 0;
        CHECK_XRCMD(xrCreateActionSet(g_instance, &actionSetInfo, &g_input.actionSet));
    }

    // Get the XrPath for the left and right hands - we will use them as subaction paths.
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left", &g_input.handSubactionPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right", &g_input.handSubactionPath[Side::RIGHT]));

    // Create actions.
    {
        // Create an input action for grabbing objects with the left and right hands.
        XrActionCreateInfo actionInfo{ XR_TYPE_ACTION_CREATE_INFO };
        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "grab_object");
        strcpy_s(actionInfo.localizedActionName, "Grab Object");
        actionInfo.countSubactionPaths = uint32_t(g_input.handSubactionPath.size());
        actionInfo.subactionPaths = g_input.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(g_input.actionSet, &actionInfo, &g_input.grabAction));

        // Create an input action for using objects with the left and right hands.
        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "trigger_value");
        strcpy_s(actionInfo.localizedActionName, "Trigger Value");
        actionInfo.countSubactionPaths = uint32_t(g_input.handSubactionPath.size());
        actionInfo.subactionPaths = g_input.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(g_input.actionSet, &actionInfo, &g_input.triggerAction));

        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "thumbstick_x_value");
        strcpy_s(actionInfo.localizedActionName, "Thumbstick X Value");
        actionInfo.countSubactionPaths = uint32_t(g_input.handSubactionPath.size());
        actionInfo.subactionPaths = g_input.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(g_input.actionSet, &actionInfo, &g_input.joystickXAction));

        actionInfo.actionType = XR_ACTION_TYPE_FLOAT_INPUT;
        strcpy_s(actionInfo.actionName, "thumbstick_y_value");
        strcpy_s(actionInfo.localizedActionName, "Thumbstick Y Value");
        actionInfo.countSubactionPaths = uint32_t(g_input.handSubactionPath.size());
        actionInfo.subactionPaths = g_input.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(g_input.actionSet, &actionInfo, &g_input.joystickYAction));

        // Create an input action getting the left and right hand poses.
        actionInfo.actionType = XR_ACTION_TYPE_POSE_INPUT;
        strcpy_s(actionInfo.actionName, "hand_pose");
        strcpy_s(actionInfo.localizedActionName, "Hand Pose");
        actionInfo.countSubactionPaths = uint32_t(g_input.handSubactionPath.size());
        actionInfo.subactionPaths = g_input.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(g_input.actionSet, &actionInfo, &g_input.poseAction));

        // Create output actions for vibrating the left and right controller.
        actionInfo.actionType = XR_ACTION_TYPE_VIBRATION_OUTPUT;
        strcpy_s(actionInfo.actionName, "vibrate_hand");
        strcpy_s(actionInfo.localizedActionName, "Vibrate Hand");
        actionInfo.countSubactionPaths = uint32_t(g_input.handSubactionPath.size());
        actionInfo.subactionPaths = g_input.handSubactionPath.data();
        CHECK_XRCMD(xrCreateAction(g_input.actionSet, &actionInfo, &g_input.vibrateAction));

        // Create input actions for quitting the session using the left and right controller.
        // Since it doesn't matter which hand did this, we do not specify subaction paths for it.
        // We will just suggest bindings for both hands, where possible.
        actionInfo.actionType = XR_ACTION_TYPE_BOOLEAN_INPUT;
        strcpy_s(actionInfo.actionName, "quit_session");
        strcpy_s(actionInfo.localizedActionName, "Quit Session");
        actionInfo.countSubactionPaths = 0;
        actionInfo.subactionPaths = nullptr;
        CHECK_XRCMD(xrCreateAction(g_input.actionSet, &actionInfo, &g_input.quitAction));
    }

    std::array<XrPath, Side::COUNT> selectPath;
    std::array<XrPath, Side::COUNT> squeezeValuePath;
    std::array<XrPath, Side::COUNT> triggerValuePath;
    std::array<XrPath, Side::COUNT> joystickXValuePath;
    std::array<XrPath, Side::COUNT> joystickYValuePath;
    std::array<XrPath, Side::COUNT> squeezeForcePath;
    std::array<XrPath, Side::COUNT> squeezeClickPath;
    std::array<XrPath, Side::COUNT> posePath;
    std::array<XrPath, Side::COUNT> hapticPath;
    std::array<XrPath, Side::COUNT> menuClickPath;
    std::array<XrPath, Side::COUNT> bClickPath;
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/input/select/click", &selectPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/input/select/click", &selectPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/input/squeeze/value", &squeezeValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/input/squeeze/value", &squeezeValuePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/input/squeeze/force", &squeezeForcePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/input/squeeze/force", &squeezeForcePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/input/squeeze/click", &squeezeClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/input/squeeze/click", &squeezeClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/input/grip/pose", &posePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/input/grip/pose", &posePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/output/haptic", &hapticPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/output/haptic", &hapticPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/input/menu/click", &menuClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/input/menu/click", &menuClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/input/b/click", &bClickPath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/input/b/click", &bClickPath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/input/trigger/value", &triggerValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/input/trigger/value", &triggerValuePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/input/thumbstick/x", &joystickXValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/input/thumbstick/x", &joystickXValuePath[Side::RIGHT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/left/input/thumbstick/y", &joystickYValuePath[Side::LEFT]));
    CHECK_XRCMD(xrStringToPath(g_instance, "/user/hand/right/input/thumbstick/y", &joystickYValuePath[Side::RIGHT]));
    // Suggest bindings for KHR Simple.
    {
        XrPath khrSimpleInteractionProfilePath;
        CHECK_XRCMD(
            xrStringToPath(g_instance, "/interaction_profiles/khr/simple_controller", &khrSimpleInteractionProfilePath));
        std::vector<XrActionSuggestedBinding> bindings{ {// Fall back to a click input for the grab action.
                                                        {g_input.grabAction, selectPath[Side::LEFT]},
                                                        {g_input.grabAction, selectPath[Side::RIGHT]},
                                                        {g_input.poseAction, posePath[Side::LEFT]},
                                                        {g_input.poseAction, posePath[Side::RIGHT]},
                                                        {g_input.quitAction, menuClickPath[Side::LEFT]},
                                                        {g_input.quitAction, menuClickPath[Side::RIGHT]},
                                                        {g_input.vibrateAction, hapticPath[Side::LEFT]},
                                                        {g_input.vibrateAction, hapticPath[Side::RIGHT]}} };
        XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
        suggestedBindings.interactionProfile = khrSimpleInteractionProfilePath;
        suggestedBindings.suggestedBindings = bindings.data();
        suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
        CHECK_XRCMD(xrSuggestInteractionProfileBindings(g_instance, &suggestedBindings));
    }
    // Suggest bindings for the Oculus Touch.
    {
        XrPath oculusTouchInteractionProfilePath;
        CHECK_XRCMD(
            xrStringToPath(g_instance, "/interaction_profiles/oculus/touch_controller", &oculusTouchInteractionProfilePath));
        std::vector<XrActionSuggestedBinding> bindings{ {{g_input.grabAction, squeezeValuePath[Side::LEFT]},
                                                        {g_input.grabAction, squeezeValuePath[Side::RIGHT]},
                                                        {g_input.triggerAction, triggerValuePath[Side::LEFT]},
                                                        {g_input.triggerAction, triggerValuePath[Side::RIGHT]},
                                                        {g_input.joystickXAction, joystickXValuePath[Side::LEFT]},
                                                        {g_input.joystickXAction, joystickXValuePath[Side::RIGHT]},
                                                        {g_input.joystickYAction, joystickYValuePath[Side::LEFT]},
                                                        {g_input.joystickYAction, joystickYValuePath[Side::RIGHT]},
                                                        {g_input.poseAction, posePath[Side::LEFT]},
                                                        {g_input.poseAction, posePath[Side::RIGHT]},
                                                        {g_input.quitAction, menuClickPath[Side::LEFT]},
                                                        {g_input.vibrateAction, hapticPath[Side::LEFT]},
                                                        {g_input.vibrateAction, hapticPath[Side::RIGHT]}} };
        XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
        suggestedBindings.interactionProfile = oculusTouchInteractionProfilePath;
        suggestedBindings.suggestedBindings = bindings.data();
        suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
        CHECK_XRCMD(xrSuggestInteractionProfileBindings(g_instance, &suggestedBindings));
    }
    // Suggest bindings for the Vive Controller.
    {
        XrPath viveControllerInteractionProfilePath;
        CHECK_XRCMD(
            xrStringToPath(g_instance, "/interaction_profiles/htc/vive_controller", &viveControllerInteractionProfilePath));
        std::vector<XrActionSuggestedBinding> bindings{ {{g_input.grabAction, triggerValuePath[Side::LEFT]},
                                                        {g_input.grabAction, triggerValuePath[Side::RIGHT]},
                                                        {g_input.poseAction, posePath[Side::LEFT]},
                                                        {g_input.poseAction, posePath[Side::RIGHT]},
                                                        {g_input.quitAction, menuClickPath[Side::LEFT]},
                                                        {g_input.quitAction, menuClickPath[Side::RIGHT]},
                                                        {g_input.vibrateAction, hapticPath[Side::LEFT]},
                                                        {g_input.vibrateAction, hapticPath[Side::RIGHT]}} };
        XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
        suggestedBindings.interactionProfile = viveControllerInteractionProfilePath;
        suggestedBindings.suggestedBindings = bindings.data();
        suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
        CHECK_XRCMD(xrSuggestInteractionProfileBindings(g_instance, &suggestedBindings));
    }

    // Suggest bindings for the Valve Index Controller.
    {
        XrPath indexControllerInteractionProfilePath;
        CHECK_XRCMD(
            xrStringToPath(g_instance, "/interaction_profiles/valve/index_controller", &indexControllerInteractionProfilePath));
        std::vector<XrActionSuggestedBinding> bindings{ {{g_input.grabAction, squeezeForcePath[Side::LEFT]},
                                                        {g_input.grabAction, squeezeForcePath[Side::RIGHT]},
                                                        {g_input.poseAction, posePath[Side::LEFT]},
                                                        {g_input.poseAction, posePath[Side::RIGHT]},
                                                        {g_input.quitAction, bClickPath[Side::LEFT]},
                                                        {g_input.quitAction, bClickPath[Side::RIGHT]},
                                                        {g_input.vibrateAction, hapticPath[Side::LEFT]},
                                                        {g_input.vibrateAction, hapticPath[Side::RIGHT]}} };
        XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
        suggestedBindings.interactionProfile = indexControllerInteractionProfilePath;
        suggestedBindings.suggestedBindings = bindings.data();
        suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
        CHECK_XRCMD(xrSuggestInteractionProfileBindings(g_instance, &suggestedBindings));
    }

    // Suggest bindings for the Microsoft Mixed Reality Motion Controller.
    {
        XrPath microsoftMixedRealityInteractionProfilePath;
        CHECK_XRCMD(xrStringToPath(g_instance, "/interaction_profiles/microsoft/motion_controller",
            &microsoftMixedRealityInteractionProfilePath));
        std::vector<XrActionSuggestedBinding> bindings{ {{g_input.grabAction, squeezeClickPath[Side::LEFT]},
                                                        {g_input.grabAction, squeezeClickPath[Side::RIGHT]},
                                                        {g_input.poseAction, posePath[Side::LEFT]},
                                                        {g_input.poseAction, posePath[Side::RIGHT]},
                                                        {g_input.quitAction, menuClickPath[Side::LEFT]},
                                                        {g_input.quitAction, menuClickPath[Side::RIGHT]},
                                                        {g_input.vibrateAction, hapticPath[Side::LEFT]},
                                                        {g_input.vibrateAction, hapticPath[Side::RIGHT]}} };
        XrInteractionProfileSuggestedBinding suggestedBindings{ XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
        suggestedBindings.interactionProfile = microsoftMixedRealityInteractionProfilePath;
        suggestedBindings.suggestedBindings = bindings.data();
        suggestedBindings.countSuggestedBindings = (uint32_t)bindings.size();
        CHECK_XRCMD(xrSuggestInteractionProfileBindings(g_instance, &suggestedBindings));
    }
    XrActionSpaceCreateInfo actionSpaceInfo{ XR_TYPE_ACTION_SPACE_CREATE_INFO };
    actionSpaceInfo.action = g_input.poseAction;
    actionSpaceInfo.poseInActionSpace.orientation.w = 1.f;
    actionSpaceInfo.subactionPath = g_input.handSubactionPath[Side::LEFT];
    CHECK_XRCMD(xrCreateActionSpace(g_session, &actionSpaceInfo, &g_input.handSpace[Side::LEFT]));
    actionSpaceInfo.subactionPath = g_input.handSubactionPath[Side::RIGHT];
    CHECK_XRCMD(xrCreateActionSpace(g_session, &actionSpaceInfo, &g_input.handSpace[Side::RIGHT]));

    XrSessionActionSetsAttachInfo attachInfo{ XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
    attachInfo.countActionSets = 1;
    attachInfo.actionSets = &g_input.actionSet;
    CHECK_XRCMD(xrAttachSessionActionSets(g_session, &attachInfo));
}


void OpenXRCreateVisualizedSpaces() {
    CHECK(g_session != XR_NULL_HANDLE);

    /// @todo Change this to modify the spaces that have things drawn in them.  They can all be removed
    /// if you draw things in world space.  Removing these will not remove the cubes drawn for the hands.
    std::string visualizedSpaces[] = { "ViewFront", "Local", "Stage", "StageLeft", "StageRight", "StageLeftRotated",
                                      "StageRightRotated" };

    for (const auto& visualizedSpace : visualizedSpaces) {
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(visualizedSpace);
        XrSpace space;
        XrResult res = xrCreateReferenceSpace(g_session, &referenceSpaceCreateInfo, &space);
        if (XR_SUCCEEDED(res)) {
            g_visualizedSpaces.push_back(space);
            g_spaceNames[space] = visualizedSpace;
        }
        else {
            if (g_verbosity >= 0) {
                std::cerr << "Failed to create reference space " << visualizedSpace << " with error " << res
                    << std::endl;
            }
        }
    }
}


void OpenXRInitializeSession()
{
    CHECK(g_instance != XR_NULL_HANDLE);
    CHECK(g_session == XR_NULL_HANDLE);

    {
        if (g_verbosity >= 2) std::cout << Fmt("Creating session...") << std::endl;

        XrSessionCreateInfo createInfo{ XR_TYPE_SESSION_CREATE_INFO };
        createInfo.next = reinterpret_cast<const XrBaseInStructure*>(&g_graphicsBinding);
        createInfo.systemId = g_systemId;
        CHECK_XRCMD(xrCreateSession(g_instance, &createInfo, &g_session));
    }

    /// @todo Print the reference spaces.

    OpenXRInitializeActions();
    OpenXRCreateVisualizedSpaces();

    {
        XrReferenceSpaceCreateInfo referenceSpaceCreateInfo = GetXrReferenceSpaceCreateInfo(g_options.AppSpace);
        CHECK_XRCMD(xrCreateReferenceSpace(g_session, &referenceSpaceCreateInfo, &g_appSpace));
    }
}

void OpenXRCreateSwapchains()
{
    CHECK(g_session != XR_NULL_HANDLE);
    CHECK(g_swapchains.empty());
    CHECK(g_configViews.empty());

    // Read graphics properties for preferred swapchain length and logging.
    XrSystemProperties systemProperties{ XR_TYPE_SYSTEM_PROPERTIES };
    CHECK_XRCMD(xrGetSystemProperties(g_instance, g_systemId, &systemProperties));

    // Log system properties.
    if (g_verbosity >= 1) {
        std::cout <<
            Fmt("System Properties: Name=%s VendorId=%d", systemProperties.systemName, systemProperties.vendorId)
            << std::endl;
        std::cout << Fmt("System Graphics Properties: MaxWidth=%d MaxHeight=%d MaxLayers=%d",
            systemProperties.graphicsProperties.maxSwapchainImageWidth,
            systemProperties.graphicsProperties.maxSwapchainImageHeight,
            systemProperties.graphicsProperties.maxLayerCount)
            << std::endl;
        std::cout << Fmt("System Tracking Properties: OrientationTracking=%s PositionTracking=%s",
            systemProperties.trackingProperties.orientationTracking == XR_TRUE ? "True" : "False",
            systemProperties.trackingProperties.positionTracking == XR_TRUE ? "True" : "False")
            << std::endl;
    }

    // Note: No other view configurations exist at the time this code was written. If this
    // condition is not met, the project will need to be audited to see how support should be
    // added.
    CHECK_MSG(g_viewConfigType == XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO, "Unsupported view configuration type");

    // Query and cache view configuration views.
    uint32_t viewCount;
    CHECK_XRCMD(xrEnumerateViewConfigurationViews(g_instance, g_systemId, g_viewConfigType, 0, &viewCount, nullptr));
    g_configViews.resize(viewCount, { XR_TYPE_VIEW_CONFIGURATION_VIEW });
    CHECK_XRCMD(xrEnumerateViewConfigurationViews(g_instance, g_systemId, g_viewConfigType, viewCount, &viewCount,
        g_configViews.data()));

    // Create and cache view buffer for xrLocateViews later.
    g_views.resize(viewCount, { XR_TYPE_VIEW });

    // Create the swapchain and get the images.
    if (viewCount > 0) {
        // Select a swapchain format.
        uint32_t swapchainFormatCount;
        CHECK_XRCMD(xrEnumerateSwapchainFormats(g_session, 0, &swapchainFormatCount, nullptr));
        std::vector<int64_t> swapchainFormats(swapchainFormatCount);
        CHECK_XRCMD(xrEnumerateSwapchainFormats(g_session, (uint32_t)swapchainFormats.size(), &swapchainFormatCount,
            swapchainFormats.data()));
        CHECK(swapchainFormatCount == swapchainFormats.size());
        g_colorSwapchainFormat = OpenGLSelectColorSwapchainFormat(swapchainFormats);//TODO

        // Print swapchain formats and the selected one.
        {
            std::string swapchainFormatsString;
            for (int64_t format : swapchainFormats) {
                const bool selected = format == g_colorSwapchainFormat;
                swapchainFormatsString += " ";
                if (selected) {
                    swapchainFormatsString += "[";
                }
                swapchainFormatsString += std::to_string(format);
                if (selected) {
                    swapchainFormatsString += "]";
                }
            }
            if (g_verbosity >= 1) std::cout << Fmt("Swapchain Formats: %s", swapchainFormatsString.c_str()) << std::endl;
        }

        // Create a swapchain for each view.
        for (uint32_t i = 0; i < viewCount; i++) {
            const XrViewConfigurationView& vp = g_configViews[i];
            if (g_verbosity >= 1) {
                std::cout <<
                    Fmt("Creating swapchain for view %d with dimensions Width=%d Height=%d SampleCount=%d", i,
                        vp.recommendedImageRectWidth, vp.recommendedImageRectHeight, vp.recommendedSwapchainSampleCount)
                    << std::endl;
            }

            // Create the swapchain.
            XrSwapchainCreateInfo swapchainCreateInfo{ XR_TYPE_SWAPCHAIN_CREATE_INFO };
            swapchainCreateInfo.arraySize = 1;
            swapchainCreateInfo.format = g_colorSwapchainFormat;
            swapchainCreateInfo.width = vp.recommendedImageRectWidth;
            swapchainCreateInfo.height = vp.recommendedImageRectHeight;
            swapchainCreateInfo.mipCount = 1;
            swapchainCreateInfo.faceCount = 1;
            swapchainCreateInfo.sampleCount = vp.recommendedSwapchainSampleCount;
            swapchainCreateInfo.usageFlags = XR_SWAPCHAIN_USAGE_SAMPLED_BIT | XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
            Swapchain swapchain;
            swapchain.width = swapchainCreateInfo.width;
            swapchain.height = swapchainCreateInfo.height;
            CHECK_XRCMD(xrCreateSwapchain(g_session, &swapchainCreateInfo, &swapchain.handle));

            g_swapchains.push_back(swapchain);

            uint32_t imageCount;
            CHECK_XRCMD(xrEnumerateSwapchainImages(swapchain.handle, 0, &imageCount, nullptr));
            // XXX This should really just return XrSwapchainImageBaseHeader*
            std::vector<XrSwapchainImageBaseHeader*> swapchainImages = OpenGLAllocateSwapchainImageStructs(imageCount, swapchainCreateInfo);
            CHECK_XRCMD(xrEnumerateSwapchainImages(swapchain.handle, imageCount, &imageCount, swapchainImages[0]));

            g_swapchainImages.insert(std::make_pair(swapchain.handle, std::move(swapchainImages)));
        }
    }
}

// Return event if one is available, otherwise return null.
static XrEventDataBaseHeader* OpenXRTryReadNextEvent() {
    // It is sufficient to clear the just the XrEventDataBuffer header to
    // XR_TYPE_EVENT_DATA_BUFFER
    XrEventDataBaseHeader* baseHeader = reinterpret_cast<XrEventDataBaseHeader*>(&g_eventDataBuffer);
    *baseHeader = { XR_TYPE_EVENT_DATA_BUFFER };
    const XrResult xr = xrPollEvent(g_instance, &g_eventDataBuffer);
    if (xr == XR_SUCCESS) {
        if (baseHeader->type == XR_TYPE_EVENT_DATA_EVENTS_LOST) {
            const XrEventDataEventsLost* const eventsLost = reinterpret_cast<const XrEventDataEventsLost*>(baseHeader);
            if (g_verbosity > 0) std::cerr << Fmt("%d events lost", eventsLost) << std::endl;
        }

        return baseHeader;
    }
    if (xr == XR_EVENT_UNAVAILABLE) {
        return nullptr;
    }
    THROW_XR(xr, "xrPollEvent");
}

void OpenXRHandleSessionStateChangedEvent(const XrEventDataSessionStateChanged& stateChangedEvent, bool* exitRenderLoop,
    bool* requestRestart) {
    const XrSessionState oldState = g_sessionState;
    g_sessionState = stateChangedEvent.state;

    if (g_verbosity >= 1) {
        std::cout << Fmt("XrEventDataSessionStateChanged: state %s->%s session=%lld time=%lld", to_string(oldState),
            to_string(g_sessionState), stateChangedEvent.session, stateChangedEvent.time)
            << std::endl;
    }

    if ((stateChangedEvent.session != XR_NULL_HANDLE) && (stateChangedEvent.session != g_session)) {
        std::cerr << "XrEventDataSessionStateChanged for unknown session" << std::endl;
        return;
    }

    switch (g_sessionState) {
    case XR_SESSION_STATE_READY: {
        CHECK(g_session != XR_NULL_HANDLE);
        XrSessionBeginInfo sessionBeginInfo{ XR_TYPE_SESSION_BEGIN_INFO };
        sessionBeginInfo.primaryViewConfigurationType = g_viewConfigType;
        CHECK_XRCMD(xrBeginSession(g_session, &sessionBeginInfo));
        g_sessionRunning = true;
        break;
    }
    case XR_SESSION_STATE_STOPPING: {
        CHECK(g_session != XR_NULL_HANDLE);
        g_sessionRunning = false;
        CHECK_XRCMD(xrEndSession(g_session))
            break;
    }
    case XR_SESSION_STATE_EXITING: {
        *exitRenderLoop = true;
        // Do not attempt to restart because user closed this session.
        *requestRestart = false;
        break;
    }
    case XR_SESSION_STATE_LOSS_PENDING: {
        *exitRenderLoop = true;
        // Poll for a new instance.
        *requestRestart = true;
        break;
    }
    default:
        break;
    }
}


void OpenXRPollEvents(bool* exitRenderLoop, bool* requestRestart) {
    *exitRenderLoop = *requestRestart = false;

    // Process all pending messages.
    while (const XrEventDataBaseHeader* event = OpenXRTryReadNextEvent()) {
        switch (event->type) {
        case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING: {
            const auto& instanceLossPending = *reinterpret_cast<const XrEventDataInstanceLossPending*>(event);
            if (g_verbosity > 0) std::cerr << Fmt("XrEventDataInstanceLossPending by %lld", instanceLossPending.lossTime) << std::endl;
            *exitRenderLoop = true;
            *requestRestart = true;
            return;
        }
        case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED: {
            auto sessionStateChangedEvent = *reinterpret_cast<const XrEventDataSessionStateChanged*>(event);
            OpenXRHandleSessionStateChangedEvent(sessionStateChangedEvent, exitRenderLoop, requestRestart);
            break;
        }
        case XR_TYPE_EVENT_DATA_INTERACTION_PROFILE_CHANGED:
            break;
        case XR_TYPE_EVENT_DATA_REFERENCE_SPACE_CHANGE_PENDING:
        default: {
            if (g_verbosity >= 2) std::cout << Fmt("Ignoring event type %d", event->type) << std::endl;
            break;
        }
        }
    }
}


float OpenXRGetFloatActionValue(int hand, XrAction action)
{
    XrActionStateGetInfo getInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
    getInfo.action = action;
    getInfo.subactionPath = g_input.handSubactionPath[hand];

    XrActionStateFloat grabValue{ XR_TYPE_ACTION_STATE_FLOAT };
    CHECK_XRCMD(xrGetActionStateFloat(g_session, &getInfo, &grabValue));
    if (grabValue.isActive == XR_TRUE) {

        return grabValue.currentState;
    }
}
XrVector2f OpenXRGetVector2ActionValue(int hand, XrAction action)
{
    XrActionStateGetInfo getInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
    getInfo.action = action;
    getInfo.subactionPath = g_input.handSubactionPath[hand];

    XrActionStateVector2f grabValue{ XR_TYPE_ACTION_STATE_VECTOR2F };
    (xrGetActionStateVector2f(g_session, &getInfo, &grabValue));
    if (grabValue.isActive == XR_TRUE) {

        return grabValue.currentState;
    }
}
void OpenXrApplyHaptics()
{
}

void OpenXRPollActions() {
    g_input.handActive = { XR_FALSE, XR_FALSE };

    // Sync actions
    const XrActiveActionSet activeActionSet{ g_input.actionSet, XR_NULL_PATH };
    XrActionsSyncInfo syncInfo{ XR_TYPE_ACTIONS_SYNC_INFO };
    syncInfo.countActiveActionSets = 1;
    syncInfo.activeActionSets = &activeActionSet;
    CHECK_XRCMD(xrSyncActions(g_session, &syncInfo));

    // Get pose and grab action state and start haptic vibrate when hand is 90% squeezed.
    for (auto hand : { Side::LEFT, Side::RIGHT }) {
        
        FusionXRHandData* currentHandData = hand == 0 ? &fusionXrData.leftHand : &fusionXrData.rightHand;
        currentHandData->trigger = OpenXRGetFloatActionValue(hand, g_input.triggerAction);
        currentHandData->grip = OpenXRGetFloatActionValue(hand, g_input.grabAction);
        currentHandData->xAxis = OpenXRGetFloatActionValue(hand,g_input.joystickXAction);
        currentHandData->yAxis = OpenXRGetFloatActionValue(hand, g_input.joystickYAction);

        XrActionStateGetInfo getInfo{ XR_TYPE_ACTION_STATE_GET_INFO };
        getInfo.subactionPath = g_input.handSubactionPath[hand];
        getInfo.action = g_input.poseAction;
        XrActionStatePose poseState{ XR_TYPE_ACTION_STATE_POSE };
        CHECK_XRCMD(xrGetActionStatePose(g_session, &getInfo, &poseState));
        g_input.handActive[hand] = poseState.isActive;
    }

    // There were no subaction paths specified for the quit action, because we don't care which hand did it.
    XrActionStateGetInfo getInfo{ XR_TYPE_ACTION_STATE_GET_INFO, nullptr, g_input.quitAction, XR_NULL_PATH };
    XrActionStateBoolean quitValue{ XR_TYPE_ACTION_STATE_BOOLEAN };
    CHECK_XRCMD(xrGetActionStateBoolean(g_session, &getInfo, &quitValue));
    if ((quitValue.isActive == XR_TRUE) && (quitValue.changedSinceLastSync == XR_TRUE) && (quitValue.currentState == XR_TRUE)) {
        CHECK_XRCMD(xrRequestExitSession(g_session));
    }
}

bool OpenXRRenderLayer(XrTime predictedDisplayTime, std::vector<XrCompositionLayerProjectionView>& projectionLayerViews,
    XrCompositionLayerProjection& layer)
{

    XrResult res;

    XrViewState viewState{ XR_TYPE_VIEW_STATE };
    uint32_t viewCapacityInput = (uint32_t)g_views.size();
    uint32_t viewCountOutput;

    XrViewLocateInfo viewLocateInfo{ XR_TYPE_VIEW_LOCATE_INFO };
    viewLocateInfo.viewConfigurationType = g_viewConfigType;
    viewLocateInfo.displayTime = predictedDisplayTime;
    viewLocateInfo.space = g_appSpace;

    res = xrLocateViews(g_session, &viewLocateInfo, &viewState, viewCapacityInput, &viewCountOutput, g_views.data());
    CHECK_XRRESULT(res, "xrLocateViews");
    if ((viewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT) == 0 ||
        (viewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT) == 0) {
        return false;  // There is no valid tracking poses for the views.
    }

    CHECK(viewCountOutput == viewCapacityInput);
    CHECK(viewCountOutput == g_configViews.size());
    CHECK(viewCountOutput == g_swapchains.size());

    projectionLayerViews.resize(viewCountOutput);

    // For each locatable space that we want to visualize, render a 25cm cube.
    std::vector<Space> spaces;

    for (XrSpace visualizedSpace : g_visualizedSpaces) {
        XrSpaceLocation spaceLocation{ XR_TYPE_SPACE_LOCATION };
        res = xrLocateSpace(visualizedSpace, g_appSpace, predictedDisplayTime, &spaceLocation);
        CHECK_XRRESULT(res, "xrLocateSpace");
        if (XR_UNQUALIFIED_SUCCESS(res)) {
            if ((spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
                (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
                spaces.push_back(Space{ spaceLocation.pose, {0.25f, 0.25f, 0.25f},g_spaceNames[visualizedSpace] });;
            }
        }
        else {
            if (g_verbosity >= 2) std::cout << Fmt("Unable to locate a visualized reference space in app space: %d", res) << std::endl;
        }
    }

    // Render a 10cm cube scaled by grabAction for each hand. Note renderHand will only be
    // true when the application has focus.
    /// @todo Remove these if you do not want to draw things in hand space.
    
    const char* handName[] = { "left", "right" };
    for (auto hand : { Side::LEFT, Side::RIGHT }) {
        XrSpaceLocation spaceLocation{ XR_TYPE_SPACE_LOCATION };
        res = xrLocateSpace(g_input.handSpace[hand], g_appSpace, predictedDisplayTime, &spaceLocation);
        CHECK_XRRESULT(res, "xrLocateSpace");
        if (XR_UNQUALIFIED_SUCCESS(res)) {
            if ((spaceLocation.locationFlags & XR_SPACE_LOCATION_POSITION_VALID_BIT) != 0 &&
                (spaceLocation.locationFlags & XR_SPACE_LOCATION_ORIENTATION_VALID_BIT) != 0) {
                FusionXRHandData* currentData = hand == 0 ? &fusionXrData.leftHand : &fusionXrData.rightHand;
                const float RADTODEG = 180 / 3.14159265358979323846264338;
                currentData->xPos = spaceLocation.pose.position.x;
                currentData->yPos = spaceLocation.pose.position.y;
                currentData->zPos = spaceLocation.pose.position.z;
                currentData->xRot = spaceLocation.pose.orientation.x* RADTODEG;
                currentData->yRot = spaceLocation.pose.orientation.y* RADTODEG;
                currentData->zRot = spaceLocation.pose.orientation.z* RADTODEG;
            }
        }
        else {
            // Tracking loss is expected when the hand is not active so only log a message
            // if the hand is active.
            if (g_input.handActive[hand] == XR_TRUE) {
                if (g_verbosity >= 2) {
                    std::cout << Fmt("Unable to locate %s hand action space in app space: %d", handName[hand], res) << std::endl;
                }
            }
        }
    }

    // Render view to the appropriate part of the swapchain image.
    for (uint32_t i = 0; i < viewCountOutput; i++) {
        // Each view has a separate swapchain which is acquired, rendered to, and released.
        const Swapchain viewSwapchain = g_swapchains[i];

        XrSwapchainImageAcquireInfo acquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };

        uint32_t swapchainImageIndex;
        CHECK_XRCMD(xrAcquireSwapchainImage(viewSwapchain.handle, &acquireInfo, &swapchainImageIndex));

        XrSwapchainImageWaitInfo waitInfo{ XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
        waitInfo.timeout = XR_INFINITE_DURATION;
        CHECK_XRCMD(xrWaitSwapchainImage(viewSwapchain.handle, &waitInfo));

        projectionLayerViews[i] = { XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW };
        projectionLayerViews[i].pose = g_views[i].pose;
        projectionLayerViews[i].fov = g_views[i].fov;
        projectionLayerViews[i].subImage.swapchain = viewSwapchain.handle;
        projectionLayerViews[i].subImage.imageRect.offset = { 0, 0 };
        projectionLayerViews[i].subImage.imageRect.extent = { viewSwapchain.width, viewSwapchain.height };
           


        const XrSwapchainImageBaseHeader* const swapchainImage = g_swapchainImages[viewSwapchain.handle][swapchainImageIndex];
        OpenGLRenderView(projectionLayerViews[i], swapchainImage, g_colorSwapchainFormat, spaces,i);

        XrSwapchainImageReleaseInfo releaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
        CHECK_XRCMD(xrReleaseSwapchainImage(viewSwapchain.handle, &releaseInfo));
    }
    fusionXrData.headXPos = (g_views[0].pose.position.x + g_views[1].pose.position.x) / 2.0f;
    fusionXrData.headYPos = (g_views[0].pose.position.y + g_views[1].pose.position.y) / 2.0f;
    fusionXrData.headZPos = (g_views[0].pose.position.z + g_views[1].pose.position.z) / 2.0f;
    fusionXrData.headXRot = (g_views[0].pose.orientation.x + g_views[1].pose.orientation.x) / 2.0f;
    fusionXrData.headYRot = (g_views[0].pose.orientation.y + g_views[1].pose.orientation.y) / 2.0f;
    fusionXrData.headZRot = (g_views[0].pose.orientation.z + g_views[1].pose.orientation.z) / 2.0f;
    layer.space = g_appSpace;
    layer.viewCount = (uint32_t)projectionLayerViews.size();
    layer.views = projectionLayerViews.data();
    return true;
}

void OpenXRRenderFrame()
{

    CHECK(g_session != XR_NULL_HANDLE);

    XrFrameWaitInfo frameWaitInfo{ XR_TYPE_FRAME_WAIT_INFO };
    XrFrameState frameState{ XR_TYPE_FRAME_STATE };
    CHECK_XRCMD(xrWaitFrame(g_session, &frameWaitInfo, &frameState));

    XrFrameBeginInfo frameBeginInfo{ XR_TYPE_FRAME_BEGIN_INFO };
    CHECK_XRCMD(xrBeginFrame(g_session, &frameBeginInfo));

    std::vector<XrCompositionLayerBaseHeader*> layers;
    XrCompositionLayerProjection layer{ XR_TYPE_COMPOSITION_LAYER_PROJECTION };
    std::vector<XrCompositionLayerProjectionView> projectionLayerViews;
    if (frameState.shouldRender == XR_TRUE) {
        if (OpenXRRenderLayer(frameState.predictedDisplayTime, projectionLayerViews, layer)) {
            layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&layer));
        }
    }

    XrFrameEndInfo frameEndInfo{ XR_TYPE_FRAME_END_INFO };
    frameEndInfo.displayTime = frameState.predictedDisplayTime;
    frameEndInfo.environmentBlendMode = g_environmentBlendMode;
    frameEndInfo.layerCount = (uint32_t)layers.size();
    frameEndInfo.layers = layers.data();
    CHECK_XRCMD(xrEndFrame(g_session, &frameEndInfo));

}

void OpenXRTearDown()
{
    OpenGLTearDown();

    if (g_input.actionSet != XR_NULL_HANDLE) {
        for (auto hand : { Side::LEFT, Side::RIGHT }) {
            xrDestroySpace(g_input.handSpace[hand]);
        }
        xrDestroyActionSet(g_input.actionSet);
    }

    for (Swapchain swapchain : g_swapchains) {
        xrDestroySwapchain(swapchain.handle);
    }

    for (XrSpace visualizedSpace : g_visualizedSpaces) {
        xrDestroySpace(visualizedSpace);
    }

    if (g_appSpace != XR_NULL_HANDLE) {
        xrDestroySpace(g_appSpace);
    }

    if (g_session != XR_NULL_HANDLE) {
        xrDestroySession(g_session);
    }

    if (g_instance != XR_NULL_HANDLE) {
        xrDestroyInstance(g_instance);
    }

#ifdef XR_USE_PLATFORM_WIN32
    CoUninitialize();
#endif
}

//============================================================================================
int main(int argc, char* argv[])
{
    try {
        // Parse the command line.
        int realParams = 0;
        for (int i = 1; i < argc; i++) {
            if (std::string("--verbosity") == argv[i]) {
                if (++i >= argc) { Usage(argv[0]); return 1; }
                g_verbosity = atoi(argv[i]);
            }
            else if (argv[i][0] == '-') {
                Usage(argv[0]); return 1;
            }
            else switch (++realParams) {
            case 1:
            default:
                Usage(argv[0]);
                return 1;
            }
        }

        // Spawn a thread to wait for a keypress
        static bool quitKeyPressed = false;
        auto exitPollingThread = std::thread{ [] {
            if (g_verbosity > 0) { std::cout << "Press any key to shutdown..." << std::endl; }
            (void)getchar();
            quitKeyPressed = true;
        } };
        exitPollingThread.detach();

        bool requestRestart = false;
        do {

            // Initialize OpenXR.
            OpenXRCreateInstance();
            OpenXRInitializeSystem();
            OpenXRInitializeSession();
            OpenXRCreateSwapchains();

            while (!quitKeyPressed) {
                bool exitRenderLoop = false;
                OpenXRPollEvents(&exitRenderLoop, &requestRestart);
                if (exitRenderLoop) {
                    break;
                }

                if (g_sessionRunning) {
                    OpenXRPollActions();
                    OpenXRRenderFrame();
                }
                else {
                    // Throttle loop since xrWaitFrame won't be called.
                    std::this_thread::sleep_for(std::chrono::milliseconds(250));
                }
            }

            OpenXRTearDown();

        } while (!quitKeyPressed && requestRestart);

        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown Error" << std::endl;
        return 1;
    }
}
