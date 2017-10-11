#pragma once
class VolumeBar
{
private:
	static const CorsairLedId startLed = CLK_1;

public:
	VolumeBar();
	~VolumeBar();

	static HRESULT Initialize();

	static HRESULT Show(float, bool=false);

	static const char* toString(CorsairError error)
	{
		switch (error) {
		case CE_Success:
			return "CE_Success";
		case CE_ServerNotFound:
			return "CE_ServerNotFound";
		case CE_NoControl:
			return "CE_NoControl";
		case CE_ProtocolHandshakeMissing:
			return "CE_ProtocolHandshakeMissing";
		case CE_IncompatibleProtocol:
			return "CE_IncompatibleProtocol";
		case CE_InvalidArguments:
			return "CE_InvalidArguments";
		default:
			return "unknown error";
		}
	}
};

