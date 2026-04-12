#pragma once

#define MAX_NAME_LENGTH 32
#define MAX_DESCRIPTION_LENGTH 255
#define MAX_BODY_LENGTH 512

enum Context {
    NONE,
    TEAM,
    CHANNEL,
    THREAD
};
