#ifndef ENGINE_V29090909_IWINDOW_HPP
#define ENGINE_V29090909_IWINDOW_HPP

enum Backend : int;

class IWindow {
public:
    Backend rendering_backend {};
    virtual ~IWindow() = default;
    virtual void PollEvents() = 0;
    virtual void SwapBuffers() = 0;
    virtual void *GetNativeHandle() = 0;
    virtual bool ShouldClose() = 0;
    virtual void SetUserPointer(void* t) = 0;
    virtual void SetMouseGrab(bool flag) = 0;
};

#endif //ENGINE_V29090909_IWINDOW_HPP
