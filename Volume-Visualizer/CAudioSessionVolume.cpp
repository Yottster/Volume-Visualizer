#include "stdafx.h"
#include "CAudioSessionVolume.h"

static const GUID AudioSessionVolumeCtx =
{ 0x2715279f, 0x4139, 0x4ba0,{ 0x9c, 0xb1, 0xb3, 0x51, 0xf1, 0xb5, 0x8a, 0x4a } };


CAudioEndpointVolume::CAudioEndpointVolume(
	UINT uNotificationMessage,
	HWND hwndNotification
)
	: m_cRef(1),
	m_uNotificationMessage(uNotificationMessage),
	m_hwndNotification(hwndNotification),
	m_bNotificationsEnabled(FALSE),
	m_pAudioVolumeEndpoint(NULL)
{
}

CAudioEndpointVolume::~CAudioEndpointVolume()
{
	// TODO Unregister notifications
	// EnableNotifications(FALSE);

	SafeRelease(&m_pAudioVolumeEndpoint);
};


//  Creates an instance of the CAudioSessionVolume object.

/* static */
HRESULT CAudioEndpointVolume::CreateInstance(
	UINT uNotificationMessage,
	HWND hwndNotification,
	CAudioEndpointVolume **ppAudioEndpointVolume
)
{
	CAudioEndpointVolume *pAudioEndpointVolume = new (std::nothrow) 
		CAudioEndpointVolume(uNotificationMessage, hwndNotification);

	if (pAudioEndpointVolume == NULL)
	{
		return E_OUTOFMEMORY;
	}

	HRESULT hr = pAudioEndpointVolume->Initialize();
	if (SUCCEEDED(hr))
	{
		*ppAudioEndpointVolume = pAudioEndpointVolume;
	}
	else
	{
		pAudioEndpointVolume->Release();
	}

	return hr;
}


//  Initializes the CAudioSessionVolume object.

HRESULT CAudioEndpointVolume::Initialize()
{
	HRESULT hr = S_OK;

	IMMDeviceEnumerator *pDeviceEnumerator = NULL;
	IMMDevice *pDevice = NULL;

	// Get the enumerator for the audio endpoint devices.
	hr = CoCreateInstance(
		__uuidof(MMDeviceEnumerator),
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&pDeviceEnumerator)
	);

	if (FAILED(hr))
	{
		goto done;
	}

	// Get the default audio endpoint that the SAR will use.
	hr = pDeviceEnumerator->GetDefaultAudioEndpoint(
		eRender,
		eConsole,   // The SAR uses 'eConsole' by default.
		&pDevice
	);

	if (FAILED(hr))
	{
		goto done;
	}

	// Get the session manager for this device.
	hr = pDevice->Activate(
		__uuidof(IAudioEndpointVolume),
		CLSCTX_INPROC_SERVER,
		NULL,
		(void**)&m_pAudioVolumeEndpoint
	);

	if (FAILED(hr))
	{
		goto done;
	}


done:
	SafeRelease(&pDeviceEnumerator);
	SafeRelease(&pDevice);
	return hr;
}

STDMETHODIMP CAudioEndpointVolume::QueryInterface(REFIID riid, void **ppvInterface)
{
	if (IID_IUnknown == riid)
	{
		AddRef();
		*ppvInterface = (IUnknown*)this;
	}
	else if (__uuidof(IAudioEndpointVolumeCallback) == riid)
	{
		AddRef();
		*ppvInterface = (IAudioEndpointVolumeCallback*)this;
	}
	else
	{
		*ppvInterface = NULL;
		return E_NOINTERFACE;
	}
	return S_OK;
}

STDMETHODIMP_(ULONG) CAudioEndpointVolume::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CAudioEndpointVolume::Release()
{
	LONG cRef = InterlockedDecrement(&m_cRef);
	if (cRef == 0)
	{
		delete this;
	}
	return cRef;
}


// Gets the session volume level.

HRESULT CAudioEndpointVolume::GetVolume(float *pflVolume)
{
	if (m_pAudioVolumeEndpoint == NULL)
	{
		return E_FAIL;
	}
	else
	{
		return m_pAudioVolumeEndpoint->GetMasterVolumeLevelScalar(pflVolume);
	}
}

//  Sets the session volume level.
//
//  flVolume: Ranges from 0 (silent) to 1 (full volume)

HRESULT CAudioEndpointVolume::SetVolume(float flVolume)
{
	if (m_pAudioVolumeEndpoint == NULL)
	{
		return E_FAIL;
	}
	else
	{
		return m_pAudioVolumeEndpoint->SetMasterVolumeLevelScalar(
			flVolume,
			&AudioSessionVolumeCtx  // Event context.
		);
	}
}


//  Gets the muting state of the session.

HRESULT CAudioEndpointVolume::GetMute(BOOL *pbMute)
{
	if (m_pAudioVolumeEndpoint == NULL)
	{
		return E_FAIL;
	}
	else
	{
		return m_pAudioVolumeEndpoint->GetMute(pbMute);
	}
}

//  Mutes or unmutes the session audio.

HRESULT CAudioEndpointVolume::SetMute(BOOL bMute)
{
	if (m_pAudioVolumeEndpoint == NULL)
	{
		return E_FAIL;
	}
	else
	{
		return m_pAudioVolumeEndpoint->SetMute(
			bMute,
			&AudioSessionVolumeCtx  // Event context.
		);
	}
}


HRESULT CAudioEndpointVolume::OnNotify(
	PAUDIO_VOLUME_NOTIFICATION_DATA pNotify
)
{
	// Check if we should post a message to the application.
	//if (m_bNotificationsEnabled &&
	//	(*EventContext != AudioSessionVolumeCtx) &&
	//	(m_hwndNotification != NULL)
	//	)
	//{
	//	// Notifications are enabled, AND
	//	// We did not trigger the event ourselves, AND
	//	// We have a valid window handle.

	//	// Post the message.
	//	::PostMessage(
	//		m_hwndNotification,
	//		m_uNotificationMessage,
	//		*((WPARAM*)(&NewVolume)),  // Coerce the float.
	//		(LPARAM)NewMute
	//	);
	//}
	return S_OK;
}