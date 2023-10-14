#pragma once

#include <stdnoreturn.h>

#define NORETURN _Noreturn

NORETURN void error_and_die(const char* fmt, ...);
