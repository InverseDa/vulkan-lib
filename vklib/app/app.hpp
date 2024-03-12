#ifndef VULKAN_LIB_APP_HPP
#define VULKAN_LIB_APP_HPP

#include "SDL2/SDL.h"
#include "SDL2/SDL_vulkan.h"

class SDLWindow {
  public:
    SDLWindow(int w, int h);
    ~SDLWindow();

    void SetTitle(const char* title);
    void SetSize(int w, int h);
    void SetPosition(int x, int y);
    void SetFullscreen(bool fullscreen);
    void SetResizable(bool resizable);
    void SetBorderless(bool borderless);
    void SetMinimized(bool minimized);
    void SetMaximized(bool maximized);
    void SetInputFocus();
    void SetMouseFocus();
    void SetGrab(bool grabbed);
    void SetCursorVisible(bool visible);
    void SetIcon(const char* path);
    void SetOpacity(float opacity);
    void SetBrightness(float brightness);
    void SetGamma(float red, float green, float blue);
    void SetCursor(const char* path);
    void SetCursor(SDL_Cursor* cursor);
    void SetRelativeMouseMode(bool enabled);
    void SetWindowedMode(int w, int h);
    void SetFullscreenMode(int w, int h);
    void SetDisplayMode(int w, int h);
    void SetDisplayMode(int displayIndex, int w, int h);
    void SetDisplayMode(int displayIndex, int w, int h, int refreshRate);
    void SetDisplayMode(int displayIndex, int w, int h, int refreshRate, int pixelFormat);
    void SetDisplayMode(int displayIndex, int w, int h, int refreshRate, int pixelFormat, int flags);
    void SetDisplayMode(int displayIndex, int w, int h, int refreshRate, int pixelFormat, int flags, int gammaRampSize);
    void SetDisplayMode(int displayIndex, int w, int h, int refreshRate, int pixelFormat, int flags, int gammaRampSize, const Uint16* red, const Uint16* green, const Uint16* blue);
    void SetDisplayMode(int displayIndex, int w, int h, int refreshRate, int pixelFormat, int flags, int gammaRampSize, const Uint16* red, const Uint16* green, const Uint16* blue, const Uint16* alpha);
    void SetDisplayMode(int displayIndex, int w, int h, int refreshRate, int pixelFormat, int flags, int gammaRampSize, const Uint16* red, const Uint16* green, const Uint16* blue, const Uint16* alpha, const Uint16* gammaRamp);

  private:
    SDL_Window* window_;
    int width_;
    int height_;

};

#endif // VULKAN_LIB_APP_HPP
