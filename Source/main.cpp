#include "StdAfx.h"

#ifdef ENGINE
std::unique_ptr<ChironLog> logContext = std::make_unique<ChironLog>();
#endif // ENGINE


int main(int argc, char** argv)
{
    LOG_TRACE("aaaa {}", 1);
    LOG_INFO("bbbb");
    LOG_ERROR("cccc");
    LOG_WARNING("dddd");
    LOG_FATAL("eeee");

    return 1;
}