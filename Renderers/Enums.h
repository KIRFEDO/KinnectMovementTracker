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