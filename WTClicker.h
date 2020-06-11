#pragma once
#include <vector>

#define DELTA 100 // pixels
#define WTDEFAULT 1000 // default payload waiting time, ms
#define PLFREQ 10 // polling frequency, ms

void callbackExit();
void callbackToggle();
void callbackTinyIncrease();
void callbackTinyDecrease();
void callbackSmallIncrease();
void callbackSmallDecrease();
void callbackNormalIncrease();
void callbackNormalDecrease();

typedef struct { std::vector<BYTE> keys; bool pressed; void (*callback)(); } Instance;

std::vector<Instance> keys = {
    { { VK_NUMPAD7 }, false, callbackTinyDecrease},
    { { VK_NUMPAD8 }, false, callbackTinyIncrease},
    { { VK_NUMPAD9 }, false, callbackExit},

    { { VK_NUMPAD4 }, false, callbackSmallDecrease},
    { { VK_NUMPAD5 }, false, callbackSmallIncrease},

    { { VK_NUMPAD1 }, false, callbackNormalDecrease},
    { { VK_NUMPAD2 }, false, callbackNormalIncrease},
    { { VK_NUMPAD3 }, false, callbackToggle}
};

inline void setTimer(UINT_PTR* timer, UINT uElapse, TIMERPROC callback);
inline void killTimer(UINT_PTR* timer);

inline void exit(const char* message);

inline void sendInput(DWORD flags);

void CALLBACK callbackPayload(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime);
void CALLBACK callbackController(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime);