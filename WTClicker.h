#pragma once
#include <vector>

#define DELTA 100 // pixels
#define WTDEFAULT 1000 // default payload waiting time, ms
#define PLFREQ 10 // polling frequency, ms

UINT waitingTime = WTDEFAULT;
size_t counter = 0;
LONG dy = -DELTA;

UINT_PTR timerController = NULL;
UINT_PTR timerPayload = NULL;

IMMDeviceEnumerator* pEnumerator;

const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

void printText(const CHAR* text);
void printIntVariable(const CHAR* name, int value);

inline void printWT() { printIntVariable("WaitingTime", waitingTime); }
inline void printCounter() { printIntVariable("Counter", counter); }

inline void increaseWT(UINT i);
inline void decreaseWT(UINT i);

void callbackTinyIncrease() { increaseWT(1); }
void callbackSmallIncrease() { increaseWT(10); }
void callbackNormalIncrease() { increaseWT(100); }
void callbackTinyDecrease() { decreaseWT(1); }
void callbackSmallDecrease() { decreaseWT(10); }
void callbackNormalDecrease() { decreaseWT(100); }
void callbackTogglePayload();

void callbackExit();
void callbackHoldLeft();
void callbackHoldRight();
void callbackToggleMicrophone();

struct Instance { std::vector<BYTE> keys; void (*callback)(); bool pressed = false; };

std::vector<Instance> keys = {
    { { VK_NUMPAD7 }, callbackTinyDecrease},
    { { VK_NUMPAD8 }, callbackTinyIncrease},
    { { VK_NUMPAD9 }, callbackExit},

    { { VK_NUMPAD4 }, callbackSmallDecrease},
    { { VK_NUMPAD5 }, callbackSmallIncrease},

    { { VK_NUMPAD1 }, callbackNormalDecrease},
    { { VK_NUMPAD2 }, callbackNormalIncrease},
    { { VK_NUMPAD3 }, callbackTogglePayload},

    { { VK_MENU, VK_XBUTTON1 }, callbackHoldLeft},
    { { VK_MENU, VK_XBUTTON2 }, callbackHoldRight},

    { { VK_CONTROL, VK_XBUTTON1 }, callbackToggleMicrophone},
    { { VK_MENU, 'M' }, callbackToggleMicrophone}
};

inline void setTimer(UINT_PTR* timer, UINT uElapse, TIMERPROC callback);
inline void killTimer(UINT_PTR* timer);

inline void sendInput(DWORD flags);

void CALLBACK callbackPayload(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime);
void CALLBACK callbackController(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime);
