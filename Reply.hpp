#include "Constants.hpp"
#include <uuid/uuid.h>

struct Reply
{
    uuid_t user;
    char body[MAX_BODY_LENGTH];
};
