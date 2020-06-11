#include <Windows.h>
#include <shellapi.h>
#include <Shlwapi.h>
#include <stdio.h>
#include "WTClicker.h"

UINT waitingTime = WTDEFAULT;

LONG dy = -DELTA;

UINT_PTR timerController = NULL;
UINT_PTR timerPayload = NULL;

size_t counter = 0;

void callbackExit() {
    killTimer(&timerController);
    if (timerPayload != NULL) {
        sendInput(MOUSEEVENTF_LEFTUP);
        killTimer(&timerPayload);
        printf("Counter = %d\n", counter);
    }
    ExitProcess(0);
}

void callbackToggle() {
    if (timerPayload == NULL) {
        sendInput(MOUSEEVENTF_LEFTDOWN);
        setTimer(&timerPayload, waitingTime, callbackPayload);
    } else {
        sendInput(MOUSEEVENTF_LEFTUP);
        killTimer(&timerPayload);
        printf("Counter = %d\n", counter);
        counter = 0;
        dy = -DELTA;
    }
}

void callbackTinyIncrease() {
    waitingTime += 1;
    printf("WaitingTime = %d\n", waitingTime);
}

void callbackTinyDecrease() {
    waitingTime -= 1;
    if (waitingTime < 1) waitingTime += 1;
    printf("WaitingTime = %d\n", waitingTime);
}

void callbackSmallIncrease() {
    waitingTime += 10;
    printf("WaitingTime = %d\n", waitingTime);
}

void callbackSmallDecrease() {
    waitingTime -= 10;
    if (waitingTime < 1) waitingTime += 10;
    printf("WaitingTime = %d\n", waitingTime);
}

void callbackNormalIncrease() {
    waitingTime += 100;
    printf("WaitingTime = %d\n", waitingTime);
}

void callbackNormalDecrease() {
    waitingTime -= 100;
    if (waitingTime < 1) waitingTime += 100;
    printf("WaitingTime = %d\n", waitingTime);
}

inline void exit(const char* message) {
    printf(message);
    ExitProcess(1);
}

inline void setTimer(UINT_PTR *timer, UINT uElapse, TIMERPROC callback) {
    *timer = SetTimer(NULL, NULL, uElapse, callback);
    if (timerController == NULL) exit("SetTimer failed\n");
}

inline void killTimer(UINT_PTR* timer) {
    KillTimer(NULL, *timer);
    *timer = NULL;
}

inline void sendInput(DWORD flags) {
    INPUT Inputs[1] = { 0 };
    Inputs[0].type = INPUT_MOUSE;
    Inputs[0].mi.dwFlags = flags;
    SendInput(1, Inputs, sizeof(INPUT));
}

void CALLBACK callbackPayload(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime) {
    ++counter;
    INPUT Inputs[1] = { 0 };
    Inputs[0].type = INPUT_MOUSE;
    Inputs[0].mi.dy = dy = -dy;
    Inputs[0].mi.dwFlags = MOUSEEVENTF_MOVE;
    SendInput(1, Inputs, sizeof(INPUT));
    killTimer(&timerPayload);
    setTimer(&timerPayload, waitingTime, callbackPayload);
}

void CALLBACK callbackController(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime) {
    for (auto it = keys.begin(); it != keys.end(); ++it) {
        bool pressed = true;
        for (auto itk = it->keys.begin(); itk != it->keys.end(); ++itk) {
            pressed &= (GetAsyncKeyState(*itk) & 0x8000) == 0x8000;
        }
        if (pressed) {
            if (it->pressed) return;
            it->callback();
        }
        it->pressed = pressed;
    }
}

int main() {
    LPWSTR* szArglist;
    int nArgs;

    szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (szArglist == NULL) exit("CommandLineToArgvW failed\n");
    else if (nArgs == 2) waitingTime = _wtoi(szArglist[1]);

    LocalFree(szArglist);

    printf("WaitingTime = %d\n", waitingTime);

    setTimer(&timerController, PLFREQ, callbackController);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
