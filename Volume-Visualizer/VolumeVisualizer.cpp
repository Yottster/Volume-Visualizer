// VolumeVisualizer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

LPCGUID g_guidMyContext = NULL;
// GUID g_guidMyContext = GUID_NULL;

#define EXIT_ON_ERROR(hr)  \
              if (FAILED(hr)) { goto Exit; }
#define ERROR_CANCEL(hr)  \
              if (FAILED(hr)) {  \
                  MessageBox(hDlg, TEXT("The program will exit."),  \
                             TEXT("Fatal error"), MB_OK);  \
                  EndDialog(hDlg, TRUE); return TRUE; }


// TODO This should be inlined or so
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }


// Simulate a keystroke
// TODO: Look into SCAN-codes:
// http://www.computer-engineering.org/ps2keyboard/scancodes2.html
// https://stackoverflow.com/questions/18647053/sendinput-not-equal-to-pressing-key-manually-on-keyboard-in-c
// MapVirtualKey(code, MAPVK_VK_TO_VSC)
// Also remember to inspect return value of SendInput (on fail)
UINT SendKeyStroke(int vk_key) {
	INPUT inputs[2];
	ZeroMemory(inputs, sizeof(inputs));

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = vk_key;

	inputs[1].type = INPUT_KEYBOARD;
	inputs[1].ki.wVk = vk_key;
	inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

	UINT result = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));

	if (result != ARRAYSIZE(inputs)) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	return S_OK;
}


int main(int argc, char *argv[], char *envp[])
{
	HRESULT hr = S_OK;

	// Init COM
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	EXIT_ON_ERROR(hr);

	CAudioEndpointVolume *pAudioEndpointVolume = NULL;

	hr = CAudioEndpointVolume::CreateInstance(
		WM_APP + 4, 
		NULL, 
		&pAudioEndpointVolume);
	EXIT_ON_ERROR(hr);

	BOOL initialMute = FALSE;
	pAudioEndpointVolume->GetMute(&initialMute);

	float initialVolume = 0.0;
	pAudioEndpointVolume->GetVolume(&initialVolume);
	
	hr = VolumeBar::Initialize();
	EXIT_ON_ERROR(hr);
	hr = VolumeBar::Show(initialVolume, initialMute);
	EXIT_ON_ERROR(hr);

	int vk_key = NULL;

	if (argc >= 2 && strcmp(argv[1], "up") == 0) {
		vk_key = VK_VOLUME_UP;
	}
	else if (argc >= 2 && strcmp(argv[1], "down") == 0) {
		vk_key = VK_VOLUME_DOWN;
	}
	else if (argc >= 2 && strcmp(argv[1], "mute") == 0) {
		vk_key = VK_VOLUME_MUTE;
	}

	if (vk_key != NULL) {
		hr = SendKeyStroke(vk_key);
		EXIT_ON_ERROR(hr);

		BOOL mute = FALSE;
		pAudioEndpointVolume->GetMute(&mute);

		float currentVolume = 0.0;
		pAudioEndpointVolume->GetVolume(&currentVolume);
		hr = VolumeBar::Show(currentVolume, mute);
		EXIT_ON_ERROR(hr);
	}

	

Exit:
	if (FAILED(hr))
	{
		std::cout << "This program requires Windows Vista. " << hr;
	}
	CoUninitialize();
	SafeRelease(&pAudioEndpointVolume);

	return 0;
}

