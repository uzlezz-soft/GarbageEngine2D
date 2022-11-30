#include "Rendering/Window.h"
#include "Core/Log.h"
#pragma warning(push, 0)
#include <GLFW/glfw3.h>
#pragma warning(pop)

FORCEINLINE static bool ThreadIdsEquals(std::thread::id left, int32 right)
{
    return right == *(int32*)&left;
}

#define DO_SOME_MAGIC(x, ...) \
    if (ThreadIdsEquals(std::this_thread::get_id(), m_threadId)) { x; } \
    else m_pendingActions.AddAction([__VA_ARGS__]() { x; });



Monitor::Monitor(uint8 index)
{
    int count = 0;
    GLFWmonitor** monitors = glfwGetMonitors(&count);

    if (count > 0 && index < count)
    {
        m_handle = monitors[index];
    }
}

Monitor::~Monitor()
{
}

uint8 Monitor::GetCount()
{
    int count;
    glfwGetMonitors(&count);
    return uint8(count);
}

std::string Monitor::GetName() const
{
    return glfwGetMonitorName((GLFWmonitor*)m_handle);
}

Vector2 Monitor::GetPhysicalSize() const
{
    int physicalSizeX = 0;
    int physicalSizeY = 0;

    glfwGetMonitorPhysicalSize((GLFWmonitor*)m_handle, &physicalSizeX, &physicalSizeY);

    return Vector2((float)physicalSizeX, (float)physicalSizeY);
}

Monitor::VideoMode Monitor::GetVideoMode() const
{
    const GLFWvidmode* glfwVideoMode = glfwGetVideoMode((GLFWmonitor*)m_handle);

    return { Vector2((float)glfwVideoMode->width, (float)glfwVideoMode->height), (uint16)glfwVideoMode->refreshRate };
}

std::vector<Monitor::VideoMode> Monitor::GetAvailableVideoModes() const
{
    std::vector<VideoMode> videoModes;

    int count = 0;
    auto glfwVideoModes = glfwGetVideoModes((GLFWmonitor*)m_handle, &count);

    for (int i = count - 1; i >= 0; i--)
    {
        const GLFWvidmode& glfwVideoMode = glfwVideoModes[i];
        videoModes.push_back({ Vector2((float)glfwVideoMode.width, (float)glfwVideoMode.height), (uint16)glfwVideoMode.refreshRate });
    }

    return videoModes;
}

Rect Monitor::GetWorkArea() const
{
    int workAreaPositionX = 0;
    int workAreaPositionY = 0;
    int workAreaSizeX = 0;
    int workAreaSizeY = 0;

    glfwGetMonitorWorkarea((GLFWmonitor*)m_handle, &workAreaPositionX, &workAreaPositionY, &workAreaSizeX, &workAreaSizeY);

    return Rect((float)workAreaPositionX, (float)workAreaPositionY, (float)workAreaSizeX, (float)workAreaSizeY);
}

bool Monitor::IsValid() const
{
    return m_handle != nullptr;
}

void* Monitor::GetHandle() const
{
    return m_handle;
}





Window::Window(const Context& context)
{
    glfwWindowHint(GLFW_RESIZABLE, context.Resizable);
    glfwWindowHint(GLFW_VISIBLE, context.Visible);
    glfwWindowHint(GLFW_DECORATED, context.Decorated);
    glfwWindowHint(GLFW_FOCUSED, context.Focused);
    glfwWindowHint(GLFW_AUTO_ICONIFY, context.AutoIconify);
    glfwWindowHint(GLFW_FLOATING, context.Floating);
    glfwWindowHint(GLFW_MAXIMIZED, context.Maximized);
    glfwWindowHint(GLFW_CENTER_CURSOR, context.CenterCursor);
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, context.FocusOnShow);
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, context.ScaleToMonitor);

    glfwWindowHint(GLFW_RED_BITS, context.RedBits);
    glfwWindowHint(GLFW_GREEN_BITS, context.GreenBits);
    glfwWindowHint(GLFW_BLUE_BITS, context.BlueBits);
    glfwWindowHint(GLFW_ALPHA_BITS, context.AlphaBits);
    glfwWindowHint(GLFW_DEPTH_BITS, context.DepthBits);
    glfwWindowHint(GLFW_STENCIL_BITS, context.StencilBits);
    glfwWindowHint(GLFW_ACCUM_RED_BITS, context.AccumRedBits);
    glfwWindowHint(GLFW_ACCUM_GREEN_BITS, context.AccumGreenBits);
    glfwWindowHint(GLFW_ACCUM_BLUE_BITS, context.AccumBlueBits);
    glfwWindowHint(GLFW_ACCUM_ALPHA_BITS, context.AccumAlphaBits);

    glfwWindowHint(GLFW_AUX_BUFFERS, context.AuxBuffers);
    glfwWindowHint(GLFW_REFRESH_RATE, context.RefreshRate);
    glfwWindowHint(GLFW_STEREO, context.Stereo);
    glfwWindowHint(GLFW_SRGB_CAPABLE, context.SrgbCapable);
    glfwWindowHint(GLFW_DOUBLEBUFFER, context.DoubleBuffer);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, false);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, false);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 0);

    auto id = std::this_thread::get_id();
    m_threadId = *(int32*)&id;
}

Window::~Window()
{
    if (m_handle)
    {
        glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(m_handle));
    }

    m_handle = nullptr;
}

bool Window::IsValid() const noexcept
{
    return m_handle != nullptr;
}

bool Window::IsOpened() const noexcept
{
    return !glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_handle));
}

void Window::Close() noexcept
{
    DO_SOME_MAGIC(glfwSetWindowShouldClose(reinterpret_cast<GLFWwindow*>(m_handle), true), this);
}

std::string_view Window::GetTitle() const noexcept
{
    return m_title;
}

void Window::SetTitle(std::string_view title) noexcept
{
    DO_SOME_MAGIC(glfwSetWindowTitle(reinterpret_cast<GLFWwindow*>(m_handle), title.data()); m_title = title, this, title);
}

void Window::Maximize() noexcept
{
    DO_SOME_MAGIC(glfwMaximizeWindow(reinterpret_cast<GLFWwindow*>(m_handle)), this);
}

void Window::Show() noexcept
{
    DO_SOME_MAGIC(glfwShowWindow(reinterpret_cast<GLFWwindow*>(m_handle)), this);
}

void Window::Hide() noexcept
{
    DO_SOME_MAGIC(glfwHideWindow(reinterpret_cast<GLFWwindow*>(m_handle)), this);
}

void Window::Focus() noexcept
{
    DO_SOME_MAGIC(glfwFocusWindow(reinterpret_cast<GLFWwindow*>(m_handle)), this);
}

void Window::RequestAttention() noexcept
{
    DO_SOME_MAGIC(glfwRequestWindowAttention(reinterpret_cast<GLFWwindow*>(m_handle)), this);
}

void Window::Center() noexcept
{
    auto videoMode = m_monitor.GetVideoMode();
    auto size = GetSize();

    DO_SOME_MAGIC(glfwSetWindowPos(reinterpret_cast<GLFWwindow*>(m_handle), (videoMode.Size.X - size.X) / 2, (videoMode.Size.Y - size.Y) / 2), this, videoMode, size);
}

void Window::AllowResize() noexcept
{
    DO_SOME_MAGIC(glfwSetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_handle), GLFW_RESIZABLE, true), this);
}

void Window::DeclineResize() noexcept
{
    DO_SOME_MAGIC(glfwSetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_handle), GLFW_RESIZABLE, false), this);
}

void Window::Resize(Vector2 newSize) noexcept
{
    DO_SOME_MAGIC(glfwSetWindowSize(reinterpret_cast<GLFWwindow*>(m_handle), newSize.X, newSize.Y), this, newSize);
}

bool Window::IsVisible() const noexcept
{
    return glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_handle), GLFW_VISIBLE);
}

bool Window::IsResizable() const noexcept
{
    return glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_handle), GLFW_RESIZABLE);
}

bool Window::IsMaximized() const noexcept
{
    return glfwGetWindowAttrib(reinterpret_cast<GLFWwindow*>(m_handle), GLFW_MAXIMIZED);
}

Vector2 Window::GetSize() const noexcept
{
    int width = 0;
    int height = 0;

    glfwGetWindowSize(reinterpret_cast<GLFWwindow*>(m_handle), &width, &height);

    return Vector2(width, height);
}

Vector2 Window::GetFramebufferSize() const noexcept
{
    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(m_handle), &width, &height);

    return Vector2((float)width, (float)height);
}

void Window::Open(uint16 width, uint16 height, const std::string_view& title, bool fullscreen, const Monitor& monitor)
{
    m_monitor = monitor;
    m_title = title;

    m_handle = glfwCreateWindow(width, height, title.data(), fullscreen ? (GLFWmonitor*)monitor.GetHandle() : nullptr, nullptr);
    MakeContextCurrent();

    glfwSetWindowUserPointer(reinterpret_cast<GLFWwindow*>(m_handle), this);

    glfwSetKeyCallback(reinterpret_cast<GLFWwindow*>(m_handle), [](GLFWwindow* handle, int32 key, int32 scanCode, int32 action, int32 mods)
        {
            Window* window = (Window*)glfwGetWindowUserPointer(handle);

            switch (action)
            {
            case GLFW_PRESS:
                window->OnKeyDown((KeyCode)key);
                break;

            case GLFW_RELEASE:
                window->OnKeyUp((KeyCode)key);
                break;
            }
        });

    glfwSetMouseButtonCallback(reinterpret_cast<GLFWwindow*>(m_handle), [](GLFWwindow* handle, int32 button, int32 action, int32 mode)
        {
            Window* window = (Window*)glfwGetWindowUserPointer(handle);

            switch (action)
            {
            case GLFW_PRESS:
                window->OnMouseButtonDown((MouseButton)button);
                break;

            case GLFW_RELEASE:
                window->OnMouseButtonUp((MouseButton)button);
                break;
            }
        });

    glfwSetCursorPosCallback(reinterpret_cast<GLFWwindow*>(m_handle), [](GLFWwindow* handle, double xPosition, double yPosition)
        {
            Window* window = (Window*)glfwGetWindowUserPointer(handle);
            window->OnMouseMove({ (float)xPosition, (float)yPosition });
        });

    glfwSetScrollCallback(reinterpret_cast<GLFWwindow*>(m_handle), [](GLFWwindow* handle, double xOffset, double yOffset)
        {
            Window* window = (Window*)glfwGetWindowUserPointer(handle);
            window->OnMouseScroll({ (float)xOffset, (float)yOffset });
        });

    glfwSetFramebufferSizeCallback(reinterpret_cast<GLFWwindow*>(m_handle), [](GLFWwindow* handle, int width, int height)
        {
            //CoreEvents::InvokeOnMainFramebufferSizeChanged({ (float)width, (float)height });
            Window* window = (Window*)glfwGetWindowUserPointer(handle);

            //WindowResizeEvent event(width, height);
            //window->m_eventCallback(event);
        });

    glfwSetWindowCloseCallback(reinterpret_cast<GLFWwindow*>(m_handle), [](GLFWwindow* handle)
        {
            Window* window = (Window*)glfwGetWindowUserPointer(handle);

            //WindowCloseEvent event;
            //window->m_eventCallback(event);
        });

    glfwSetCharCallback(reinterpret_cast<GLFWwindow*>(m_handle), [](GLFWwindow* handle, uint32 decimal)
        {
            //Window* window = (Window*)glfwGetWindowUserPointer(handle);
            //window->m_enteredCharacters += (uint8)decimal;
        });

    Center();
}

void Window::PollEvents() noexcept
{
    ResetJustPressedKeys();

    MakeContextCurrent();

    m_pendingActions.Execute();

    glfwPollEvents();
}

void Window::SwapBuffers() const noexcept
{
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(m_handle));
}

void Window::EnableVSync() noexcept
{
    m_isVSyncEnabled = true;
    glfwSwapInterval(1);
}

void Window::DisableVSync() noexcept
{
    m_isVSyncEnabled = false;
    glfwSwapInterval(0);
}

void Window::LockCursor() noexcept
{
    DO_SOME_MAGIC(glfwSetInputMode(reinterpret_cast<GLFWwindow*>(m_handle), GLFW_CURSOR, GLFW_CURSOR_DISABLED), this);
}

void Window::UnlockCursor() noexcept
{
    DO_SOME_MAGIC(glfwSetInputMode(reinterpret_cast<GLFWwindow*>(m_handle), GLFW_CURSOR, GLFW_CURSOR_NORMAL), this);
}

void Window::ShowCursor() noexcept
{
    DO_SOME_MAGIC(glfwSetInputMode(reinterpret_cast<GLFWwindow*>(m_handle), GLFW_CURSOR, GLFW_CURSOR_NORMAL), this);
}

void Window::HideCursor() noexcept
{
    DO_SOME_MAGIC(glfwSetInputMode(reinterpret_cast<GLFWwindow*>(m_handle), GLFW_CURSOR, GLFW_CURSOR_HIDDEN), this);
}

void Window::SetMousePosition(Vector2 position) const noexcept
{
    glfwSetCursorPos(reinterpret_cast<GLFWwindow*>(m_handle), position.X, position.Y);
}

float Window::GetAspectRatio() const noexcept
{
    const Vector2 size = GetFramebufferSize();
    return size.X / size.Y;
}

void Window::OnKeyDown(KeyCode key)
{
    m_keys[(uint64)key] = true;
    m_justPressedKeys[(uint64)key] = true;
    m_justReleasedKeys[(uint64)key] = false;

    int32 modifierKeyBit = MK_None;

    if (IsKeyPressed(KeyCode::LeftControl) || IsKeyPressed(KeyCode::RightControl)) modifierKeyBit |= MK_Control;
    if (IsKeyPressed(KeyCode::LeftShift) || IsKeyPressed(KeyCode::RightShift)) modifierKeyBit |= MK_Shift;
    if (IsKeyPressed(KeyCode::LeftAlt) || IsKeyPressed(KeyCode::RightAlt)) modifierKeyBit |= MK_Alt;
    if (IsKeyPressed(KeyCode::LeftSuper) || IsKeyPressed(KeyCode::RightShift)) modifierKeyBit |= MK_Super;
    if (IsKeyPressed(KeyCode::CapsLock)) modifierKeyBit |= MK_CapsLock;
    if (IsKeyPressed(KeyCode::NumLock)) modifierKeyBit |= MK_NumLock;

    //KeyPressedEvent event((int32)key, (ModifierKeyBit)modifierKeyBit);
    //m_eventCallback(event);
}

void Window::OnKeyUp(KeyCode key)
{
    m_keys[(uint64)key] = false;
    m_justPressedKeys[(uint64)key] = false;
    m_justReleasedKeys[(uint64)key] = true;

    //KeyReleasedEvent event((int32)key);
    //m_eventCallback(event);
}

void Window::OnMouseButtonDown(MouseButton mouseButton)
{
    m_mouseButtons[(uint64)mouseButton] = true;
    m_justPressedMouseButtons[(uint64)mouseButton] = true;
    m_justReleasedMouseButtons[(uint64)mouseButton] = false;

    //MouseButtonPressedEvent event((int32)mouseButton);
    //m_eventCallback(event);
}

void Window::OnMouseButtonUp(MouseButton mouseButton)
{
    m_mouseButtons[(uint64)mouseButton] = false;
    m_justPressedMouseButtons[(uint64)mouseButton] = false;
    m_justReleasedMouseButtons[(uint64)mouseButton] = true;

    //MouseButtonReleasedEvent event((int32)mouseButton);
    //m_eventCallback(event);
}

void Window::OnMouseMove(Vector2 mousePosition)
{
    m_mouseMoveDelta = mousePosition - m_mousePosition;
    m_normalizedMouseMoveDelta = m_mouseMoveDelta / (GetFramebufferSize().Y / 2.0f);
    m_mousePosition = mousePosition;

    //MouseMovedEvent event(m_mouseMoveDelta);
    //m_eventCallback(event);
}

void Window::OnMouseScroll(Vector2 scrollDelta)
{
    m_lastMouseScrollDelta = scrollDelta;
    m_mouseScrollDelta = scrollDelta;

    //MouseScrolledEvent event(scrollDelta);
    //m_eventCallback(event);
}

void Window::ResetJustPressedKeys()
{
    for (uint16 i = 0; i < (uint16)KeyCode::MaxValue; i++)
    {
        m_justPressedKeys[i] = false;
        m_justReleasedKeys[i] = false;
    }

    for (uint8 i = 0; i < (int8)MouseButton::MaxValue; i++)
    {
        m_justPressedMouseButtons[i] = false;
        m_justReleasedMouseButtons[i] = false;
    }

    m_mouseScrollDelta = Vector2(0.0f);
    m_mouseMoveDelta = Vector2(0.0f);
    m_normalizedMouseMoveDelta = Vector2(0.0f);
}

void Window::MakeContextCurrent() const noexcept
{
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(m_handle));
}

void Window::InitSubsystem() noexcept
{
    glfwInit();
    GARBAGE_CORE_TRACE("Window subsystem initialized.");
}

void Window::ShutdownSubsystem() noexcept
{
    glfwTerminate();
    GARBAGE_CORE_TRACE("Window subsystem shutdowned.");
}
