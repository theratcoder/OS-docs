#include "rat.h"

// Callback for when a window needs to repaint
RatResult OnFrame(RatWindowHandle window, void* userdata) {
    RatSurfaceHandle surface = RatBeginFrame(window);

    // Clear background
    RatSurfaceClear(surface, RAT_COLOR_WHITE);

    // Draw text (high-level text API provided by RatAPI)
    RatSurfaceDrawText(surface, "Hello, World!", 10, 30, RAT_COLOR_BLACK);

    // Submit the frame for compositing
    RatEndFrame(window, surface);

    return RAT_OK;
}

RatResult OnClose(RatWindowHandle window, void* userdata) {
    RatPostQuitMessage(0);
    return RAT_OK;
}

int main(void) {
    RatStatus status = RatInitialize();
    if (status != RAT_OK) {
        return (int)status;
    }

    // Define window class
    RatWindowClass windowClass = {0};
    windowClass.className = "MainWindowClass";

    status = RatRegisterClass(&windowClass);
    if (status != RAT_OK) {
        RatShutdown();
        return (int)status;
    }

    // Create main window
    RatWindowHandle mainWindow = RatCreateWindow(
        "MainWindowClass",
        "Hello, World!",
        RAT_WINDOW_STYLE_OVERLAPPED,
        100, 100, 400, 300,
        RAT_NULL, RAT_NULL
    );

    if (!mainWindow) {
        RatShutdown();
        return -1;
    }

    // Register frame + close events
    RatRegisterEvent(mainWindow, RAT_MSG_FRAME, OnFrame, RAT_NULL);
    RatRegisterEvent(mainWindow, RAT_MSG_CLOSE, OnClose, RAT_NULL);

    RatShowWindow(mainWindow, RAT_WINDOW_SHOW);

    // Main event loop
    while (RatPumpEvents() == RAT_OK) {
        // Idle work (timers, async I/O, etc.) could run here
    }

    RatShutdown();
    return 0;
}