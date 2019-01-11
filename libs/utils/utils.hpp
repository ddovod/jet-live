
#pragma once

#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wsign-conversion"
#elif defined(__GNUC__)
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wsign-conversion"
#endif

#define CHOBO_SMALL_VECTOR_ERROR_HANDLING 0 // CHOBO_SMALL_VECTOR_ERROR_HANDLING_NONE
#include "small_vector/small_vector.hpp"

#if defined(__clang__)
#   pragma clang diagnostic pop
#elif defined(__GNUC__)
#   pragma GCC diagnostic pop
#endif

namespace jet
{
    using namespace chobo;
}
