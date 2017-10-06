#include "stdafx.h"
#include "VolumeBar.h"


VolumeBar::VolumeBar()
{
}


VolumeBar::~VolumeBar()
{
}

HRESULT VolumeBar::Initialize()
{
	CorsairProtocolDetails protocol = CorsairPerformProtocolHandshake();
	if (const auto error = CorsairGetLastError()) {
		std::cout << "Handshake failed: " << toString(error) << "\nPress any key to quit." << std::endl;
		getchar();
		return -1;
	}
	
	int deviceCount = CorsairGetDeviceCount();
	if (deviceCount < 1) return 1;

	CorsairLedPositions* positions = NULL;
	for (auto deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
		if (CorsairDeviceInfo* deviceInfo = CorsairGetDeviceInfo(deviceIndex)) {
			if (deviceInfo->type != CDT_Keyboard) {
				continue;
			}
			positions = CorsairGetLedPositionsByDeviceIndex(deviceIndex);
		}
	}
	if (!positions || positions->numberOfLed < 0) {
		return 1;
	}

	return S_OK;
}

HRESULT VolumeBar::Show(float volume) {
	const uint16_t normalised = (uint16_t)std::round(volume*10);
	CorsairLedColor* volumeLeds = new CorsairLedColor[normalised];

	for (int i = 0; i < normalised; i++) {
		CorsairLedColor led = CorsairLedColor();
		led.ledId = (CorsairLedId) (startLed + i);
		led.r = 255;
		volumeLeds[i] = led;
	}

	CorsairSetLedsColors(normalised, volumeLeds);
	Sleep(500);

	delete[] volumeLeds;
	return S_OK;
}

//std::vector<CorsairLedColor> getAvailableKeys()
//{
//	auto colorsVector = std::vector<CorsairLedColor>();
//	for (auto deviceIndex = 0; deviceIndex < CorsairGetDeviceCount(); deviceIndex++) {
//		if (auto deviceInfo = CorsairGetDeviceInfo(deviceIndex)) {
//			switch (deviceInfo->type) {
//			case CDT_Mouse: {
//				auto numberOfKeys = deviceInfo->physicalLayout - CPL_Zones1 + 1;
//				for (auto i = 0; i < numberOfKeys; i++) {
//					auto ledId = static_cast<CorsairLedId>(CLM_1 + i);
//					colorsVector.push_back(CorsairLedColor{ ledId, 0, 0, 0 });
//				}
//			} break;
//			case CDT_Keyboard: {
//				auto ledPositions = CorsairGetLedPositions();
//				if (ledPositions) {
//					for (auto i = 0; i < ledPositions->numberOfLed; i++) {
//						auto ledId = ledPositions->pLedPosition[i].ledId;
//						colorsVector.push_back(CorsairLedColor{ ledId, 0, 0, 0 });
//					}
//				}
//			} break;
//			case CDT_Headset: {
//				colorsVector.push_back(CorsairLedColor{ CLH_LeftLogo, 0, 0, 0 });
//				colorsVector.push_back(CorsairLedColor{ CLH_RightLogo, 0, 0, 0 });
//			} break;
//			default:
//				break;
//			}
//		}
//	}
//	return colorsVector;
//}
