#pragma once

#include "Constants.hpp"
#include <uuid/uuid.h>

struct myUuid
{
    uuid_t uuid;
};

struct Reply
{
    uuid_t user;
    char body[MAX_BODY_LENGTH];
};
