#ifndef YAGA_DI_EXCEPTION
#define YAGA_DI_EXCEPTION

#include <format>
#include <stdexcept>

#define THROW(fmt, ...) throw std::runtime_error(std::format(fmt "; file: {}; line: {}.", __VA_ARGS__, __FILE__, __LINE__))

#endif // !YAGA_DI_EXCEPTION