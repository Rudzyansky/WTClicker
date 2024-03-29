#pragma once
#include <vector>

constexpr auto PLFREQ = 10; // polling frequency, ms

UINT_PTR timerController = NULL;

IMMDeviceEnumerator* pEnumerator;

const HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

void printText(PCSTR text);

void callbackExit();
void callbackTerminal();
void callbackCloseActive();
void callbackHoldLeft();
void callbackHoldRight();
void callbackToggleMicrophone();

struct Instance { std::vector<BYTE> keys; void (*callback)(); bool pressed = false; };

std::vector<Instance> keys = {
	{ { VK_CONTROL, VK_MENU, 'E' }, callbackExit},
	{ { VK_LWIN, VK_RETURN }, callbackTerminal},
	{ { VK_LWIN, VK_SHIFT, 'Q' }, callbackCloseActive},
	{ { VK_LMENU, 'Q' }, callbackCloseActive},

	{ { VK_MENU, VK_XBUTTON1 }, callbackHoldLeft},
	{ { VK_MENU, VK_XBUTTON2 }, callbackHoldRight},

	{ { VK_CONTROL, VK_XBUTTON1 }, callbackToggleMicrophone},
	{ { VK_CONTROL, VK_XBUTTON2 }, callbackToggleMicrophone},
	{ { VK_MENU, 'M' }, callbackToggleMicrophone}
};

inline void setTimer(UINT_PTR* timer, UINT uElapse, TIMERPROC callback);
inline void killTimer(UINT_PTR* timer);

inline void sendInput(DWORD flags);

void CALLBACK callbackController(HWND hwnd, UINT uMsg, UINT timerId, DWORD dwTime);
