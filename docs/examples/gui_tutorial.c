#include <rat.h>

// Window procedure callback
RatResult RatMainWindowProc(RatWindowHandle window, RatMsgId message, RatWordParam wparam, RatLongParam lparam) {
    switch (message) {
        case RAT_MSG_PAINT: {
            RatSurfaceHandle surface = RatBeginFrame(window);

            // Clear background
            RatSurfaceClear(surface, RAT_COLOR_WHITE);

            // Draw text (high-level text API provided by RatAPI)
            RatSurfaceDrawText(surface, "Hello, World!", 10, 30, RAT_COLOR_BLACK);

            // Submit the frame for compositing
            RatEndFrame(window, surface);

            return RAT_OK;
        }
        case RAT_MSG_DESTROY:
            RatPostQuitMessage(0);
            return RAT_OK;
        default:
            return RatDefaultWindowProc(window, message, wparam, lparam);
    }
}

int main(void) {
    RatStatus status;

    // Initialize RatAPI
    status = RatInitialize();
    if (status != RAT_OK) {
        return (int)status;
    }

    // Define window class
    RatWindowClass windowClass = {0};
    windowClass.className = "MainWindowClass";
    windowClass.proc      = RatMainWindowProc;

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

    RatShowWindow(mainWindow, RAT_WINDOW_SHOW);
    RatUpdateWindow(mainWindow);

    // Standard message loop
    RatMsg msg;
    while (RatGetMessage(&msg, RAT_NULL, 0, 0)) {
        RatTranslateMessage(&msg);
        RatDispatchMessage(&msg);
    }

    RatShutdown();
    return (int)msg.wparam;
}