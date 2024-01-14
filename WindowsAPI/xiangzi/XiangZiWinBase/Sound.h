
bool* _SoundPlay;
char* _SoundName;
void _PlaySound() {
	bool* play = _SoundPlay;
	char* name = _SoundName;
	PlaySoundA(name, NULL, SND_ASYNC|SND_NOSTOP);
	while((*play)){
		Sleep(1);
	}
	PlaySoundA(NULL, NULL, SND_FILENAME);
}

class XSound {
private:
	bool Play;
	char* Name;
public:
	void set(const char* filename) {
		Name = _char(filename);
	}
	void play() {
		Play = true;
		_SoundPlay = &Play;
		_SoundName = Name;
		thread sound(_PlaySound);
		sound.detach();
	}
	void stop() {
		Play = false;
	}
};

/*
enum State {
	_play,
	_pause,
	_stop,
	_close
};

class XSound {
private:
	MCIDEVICEID nDeviceID;
public:
	State state = _close;
	bool open(const char* strSongPath) noexcept
	{
		MCI_OPEN_PARMS mciOP;
		mciOP.lpstrDeviceType = nullptr;
		mciOP.lpstrElementName = _char(strSongPath);

		if (0 == mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_WAIT | MCI_OPEN_SHAREABLE, (DWORD_PTR)(static_cast<LPVOID>(&mciOP))))
		{
			state = _stop;
			nDeviceID = mciOP.wDeviceID;
			return true;
		}
		else
		{
			nDeviceID = -1;
			return false;
		}
	}
	bool play() noexcept
	{
		MCI_PLAY_PARMS mciPP{};
		state = _play;
		return (0 == mciSendCommand(nDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD_PTR)(static_cast<LPVOID>(&mciPP))));
	}
	bool pause() noexcept
	{
		MCI_GENERIC_PARMS mciGP{};
		state = _pause;
		return (0 == mciSendCommand(nDeviceID, MCI_PAUSE, MCI_NOTIFY | MCI_WAIT, (DWORD_PTR)(static_cast<LPVOID>(&mciGP))));
	}
	bool stop() noexcept
	{
		MCI_SEEK_PARMS mciSP{};
		state = _stop;
		return (0 == mciSendCommand(nDeviceID, MCI_SEEK, MCI_WAIT | MCI_NOTIFY | MCI_SEEK_TO_START, (DWORD_PTR)(static_cast<LPVOID>(&mciSP))));
	}
	bool close() noexcept
	{
		MCI_GENERIC_PARMS mciGP{};
		
		if (0 == mciSendCommand(nDeviceID, MCI_CLOSE, MCI_NOTIFY | MCI_WAIT, (DWORD_PTR)(static_cast<LPVOID>(&mciGP))))
		{
			state = _close;
			nDeviceID = -1;
			return true;
		}
		else
		{
			return false;
		}
	}
	bool getCurrentTime(DWORD& pos) noexcept
	{
		MCI_STATUS_PARMS mciSP{};
		mciSP.dwItem = MCI_STATUS_POSITION;

		if (0 == mciSendCommand(nDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD_PTR)(static_cast<LPVOID>(&mciSP))))
		{
			pos = static_cast<DWORD>(mciSP.dwReturn);
			return true;
		}
		else
		{
			pos = 0;
			return false;
		}
	}
	bool getTotalTime(DWORD& time) noexcept
	{
		MCI_STATUS_PARMS mciSP{};
		mciSP.dwItem = MCI_STATUS_LENGTH;

		if (0 == mciSendCommand(nDeviceID, MCI_STATUS, MCI_WAIT | MCI_STATUS_ITEM, (DWORD_PTR)(static_cast<LPVOID>(&mciSP))))
		{
			time = static_cast<DWORD>(mciSP.dwReturn);
			return true;
		}
		else
		{
			time = 0;
			return false;
		}
	}
	bool setVolume(size_t nVolumeValue) noexcept
	{
		if (nVolumeValue > 1000)
		{
			nVolumeValue = 1000;
		}
		else if (nVolumeValue < 0)
		{
			nVolumeValue = 0;
		}

		MCI_DGV_SETAUDIO_PARMS mciDSP;
		mciDSP.dwItem = MCI_DGV_SETAUDIO_VOLUME;
		mciDSP.dwValue = static_cast<DWORD>(nVolumeValue);

		return (0 == mciSendCommand(nDeviceID, MCI_SETAUDIO, MCI_DGV_SETAUDIO_VALUE | MCI_DGV_SETAUDIO_ITEM, (DWORD_PTR)(static_cast<LPVOID>(&mciDSP))));
	}
	bool setStartTime(size_t start_time) noexcept
	{
		DWORD end_time = 0;
		this->getTotalTime(end_time);

		if (start_time > end_time)
		{
			return false;
		}

		MCI_PLAY_PARMS mciPlay{};
		mciPlay.dwFrom = static_cast<DWORD>(start_time);
		mciPlay.dwTo = static_cast<DWORD>(end_time);

		if (0 == mciSendCommand(nDeviceID, MCI_PLAY, MCI_TO | MCI_FROM, (DWORD_PTR)(static_cast<LPVOID>(&mciPlay))))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

};
*/


#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")

class XMusic {
private:
public:
};
