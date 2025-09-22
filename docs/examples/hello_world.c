#include <rat.h>

int main(void) {
    RatStatus status;

    // Initialize RatAPI
    status = RatInitialize();
    if (status != RAT_OK) {
        // If initialization fails, exit with error code
        return (int)status;
    }

    // Write "Hello, World!" to the console
    status = RatConsoleWrite("Hello, World!\n");
    if (status != RAT_OK) {
        RatShutdown();
        return (int)status;
    }

    // Shut down RatAPI
    RatShutdown();
    return 0;
}