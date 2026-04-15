#pragma once

#define MAX_NAME_LENGTH 32
#define MAX_DESCRIPTION_LENGTH 255
#define MAX_BODY_LENGTH 512
#define UNPARSED_UUUID 36

#define SAVE_FILE "db.team"

enum Context {
    NONE,
    TEAM,
    CHANNEL,
    THREAD,
    COMMENT
};
