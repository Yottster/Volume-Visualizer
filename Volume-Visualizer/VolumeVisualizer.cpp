// VolumeVisualizer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


// Guid for AudioSession
LPCGUID g_guidMyContext = NULL;
// GUID g_guidMyContext = GUID_NULL;

static IAudioEndpointVolume *g_pEndptVol = NULL;
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd370892(v=vs.85).aspx

static IAudioClient *g_AudioClient = NULL;
// https://msdn.microsoft.com/ko-kr/library/windows/desktop/dd316531(v=vs.85).aspx isimpleaudiovolume

static IAudioSessionManager *g_AudioSessionManager = NULL;

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
	int nInputs = 1;
	INPUT input;
	input.type = INPUT_KEYBOARD;
	KEYBDINPUT kb;
	kb.wVk = vk_key;
	input.ki = kb;

	Sleep(25);
	SendInput(1, &input, sizeof(input));
	Sleep(50);
	input.ki.dwFlags = KEYEVENTF_KEYUP;
	return SendInput(1, &input, sizeof(input));
}


int main(int argc, char *argv[], char *envp[])
{
	HRESULT hr = S_OK;

	// Init COM
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

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
	hr = VolumeBar::Show(initialVolume);
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
		SendKeyStroke(vk_key);
		BOOL mute = FALSE;
		pAudioEndpointVolume->GetMute(&mute);

		float currentVolume = 0.0;
		pAudioEndpointVolume->GetVolume(&currentVolume);
		hr = VolumeBar::Show(currentVolume);
		EXIT_ON_ERROR(hr);
	}

	

Exit:
	if (FAILED(hr))
	{
		std::cout << "This program requires Windows Vista. " << hr;
	}
	//if (pEnumerator != NULL)
	//{
	//	/*g_pEndptVol->UnregisterControlChangeNotify(
	//		(IAudioEndpointVolumeCallback*)&EPVolEvents);*/
	//}
	CoUninitialize();
	SafeRelease(&pAudioEndpointVolume);

	return 0;
}

