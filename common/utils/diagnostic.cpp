#include "common/utils/diagnostic.h"

#include <string>

GUNGNIR_NAMESPACE_OPEN_SCOPE

namespace utils {

const char*
verifyStringFormat(const char *msg, ...)
{
    return msg;
}

bool
verifyFailedHelper(const char *cond, const char *msg)
{
    std::string errorMsg =
        std::string("Failed verification: ' ") + cond + " '";

    if(msg) {
        errorMsg += " -- ";
        errorMsg += msg;
    }

    std::cout << errorMsg << std::endl;

    return false;
}

}

GUNGNIR_NAMESPACE_CLOSE_SCOPE
