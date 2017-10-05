#pragma once

#include <Shlwapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <Audioclient.h>
#include <Audiopolicy.h>
#pragma comment(lib, "shlwapi.lib")

#include <new>

template <class T> void SafeRelease(T **ppT)
{
	if (*ppT)
	{
		(*ppT)->Release();
		*ppT = NULL;
	}
}

class CAudioEndpointVolume : public IAudioEndpointVolumeCallback
{
public:
	// Static method to create an instance of the object.
	static HRESULT CreateInstance(
		UINT uNotificationMessage,
		HWND hwndNotification,
		CAudioEndpointVolume **ppAudioSessionVolume
	);

	// IUnknown methods.
	STDMETHODIMP QueryInterface(REFIID iid, void** ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// The remaining audio session events do not require any action.
	STDMETHODIMP OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);

	// Other methods
	HRESULT GetVolume(float *pflVolume);
	HRESULT SetVolume(float flVolume);
	HRESULT GetMute(BOOL *pbMute);
	HRESULT SetMute(BOOL bMute);

protected:
	CAudioEndpointVolume(UINT uNotificationMessage, HWND hwndNotification);
	~CAudioEndpointVolume();

	HRESULT Initialize();

protected:
	LONG m_cRef;                        // Reference count.
	UINT m_uNotificationMessage;        // Window message to send when an audio event occurs.
	HWND m_hwndNotification;            // Window to receives messages.
	BOOL m_bNotificationsEnabled;       // Are audio notifications enabled?

	IAudioEndpointVolume *m_pAudioVolumeEndpoint;
};