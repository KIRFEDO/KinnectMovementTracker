#pragma once

enum RECORD_DATA_STATE
{
    DO_NOT_RECORD,
    INITIATE_FILE_HANDLES,
    RECORDING,
    FINISH_RECORDING
};

enum VIEW_MODE {
    DEPTH,
    SKELETON
};

enum class APP_MODE
{
	LIVE,
	READING
};

inline const wchar_t* ToWString(const APP_MODE& appState)
{
	switch (appState)
	{
	case APP_MODE::LIVE:
		return L"LIVE";
	case APP_MODE::READING:
		return L"READING";
	}
}

inline const wchar_t* ToWString(const RECORD_DATA_STATE& recordState)
{
	switch (recordState)
	{
		case RECORD_DATA_STATE::DO_NOT_RECORD:
			return L"RECORDING STOPPED";
		case RECORD_DATA_STATE::INITIATE_FILE_HANDLES:
			return L"RECORDING IS INITIATING";
		case RECORD_DATA_STATE::RECORDING:
			return L"RECORDING";
		case RECORD_DATA_STATE::FINISH_RECORDING:
			return L"RECORDING IS FINISHING";
	}
}