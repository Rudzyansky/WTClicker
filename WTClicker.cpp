#include <Windows.h>
#include <strsafe.h>
#include <Mmdeviceapi.h>
#include <endpointvolume.h>
#include "WTClicker.h"

#define EXIT_ON_ERROR_WO_GOTO(hr, message) if (FAILED(hr)) { printText(message); PostQuitMessage(hr); }
#define EXIT_ON_ERROR(hr, message) if (FAILED(hr)) { printText(message); PostQuitMessage(hr); goto Exit; }
#define SAFE_RELEASE(instance) if ((instance) != NULL) { (instance)->Release(); (instance) = NULL; }

void callbackExit() {
    killTimer(&timerController);
    printText("Exiting");
    PostQuitMessage(0);
}

void callbackTerminal() {
    printText("Open Terminal");
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    HRESULT hr = CreateProcessA(NULL, (LPSTR)"wt.exe", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    EXIT_ON_ERROR_WO_GOTO(hr, "CreateProcess");
}

void callbackCloseActive() {
    printText("Close Window");
    LRESULT lr = SendMessage(GetForegroundWindow(), WM_SYSCOMMAND, SC_CLOSE, 0);
    EXIT_ON_ERROR_WO_GOTO(lr, "CloseWindow");
}

void callbackHoldLeft() {
    printText("Left Button Holded");
    sendInput(MOUSEEVENTF_LEFTDOWN);
}

void callbackHoldRight() {
    printText("Right Button Holded");
    sendInput(MOUSEEVENTF_RIGHTDOWN);
}

void callbackToggleMicrophone() {
    HRESULT hr;
    IMMDevice* pDevice = NULL;
    IAudioEndpointVolume* paeVolume = NULL;
    BOOL bMute;

    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &pDevice);
    EXIT_ON_ERROR(hr, "GetDefaultAudioEndpoint failed");

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&paeVolume);
    EXIT_ON_ERROR(hr, "pDevice->Activate failed");

    hr = paeVolume->GetMute(&bMute);
    EXIT_ON_ERROR(hr, "paeVolume->GetMute failed");

    bMute = !bMute;
    hr = paeVolume->SetMute(bMute, NULL);
    EXIT_ON_ERROR(hr, "paeVolume->SetMute failed");

    printText(bMute ? "Default Audio Device Muted" : "Default Audio Device Unmuted");
Exit:
    SAFE_RELEASE(paeVolume);
    SAFE_RELEASE(pDevice);
}

void printMicrophoneState() {
    HRESULT hr;
    IMMDevice* pDevice = NULL;
    IAudioEndpointVolume* paeVolume = NULL;
    BOOL bMute;

    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &pDevice);
    EXIT_ON_ERROR(hr, "GetDefaultAudioEndpoint failed");

    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)&paeVolume);
    EXIT_ON_ERROR(hr, "pDevice->Activate failed");

    hr = paeVolume->GetMute(&bMute);
    EXIT_ON_ERROR(hr, "paeVolume->GetMute failed");

    printText(bMute ? "Default Audio Device Muted" : "Default Audio Device Unmuted");
Exit:
    SAFE_RELEASE(paeVolume);
    SAFE_RELEASE(pDevice);
}

inline void setTimer(UINT_PTR *timer, UINT uElapse, TIMERPROC callback) {
    *timer = SetTimer(NULL, NULL, uElapse, callback);
    if (timer == NULL) {
        printText("SetTimer failed");
        PostQuitMessage(1);
    }
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

void printText(PCSTR text) {
    const size_t cchDest = 32;
    CHAR pszDest[cchDest];
    CHAR* ppszDestEnd;
    StringCchPrintfExA(pszDest, cchDest, &ppszDestEnd, NULL, NULL, "%s\n", text);
    WriteConsoleA(hStdOut, pszDest, ppszDestEnd - pszDest, NULL, NULL);
}

int main() {
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);
    if (FAILED(hr)) {
        printText("CoCreateInstance failed");
        ExitProcess(1);
    }

    printMicrophoneState();
    setTimer(&timerController, PLFREQ, callbackController);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    SAFE_RELEASE(pEnumerator);
    CoUninitialize();
    ExitProcess(0);
}
