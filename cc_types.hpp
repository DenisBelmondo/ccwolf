#ifndef CCWOLF_TYPES_HPP
#define CCWOLF_TYPES_HPP

#include <stdint.h>

namespace CcWolf::Types
{
    using byte     = uint8_t;
    using word     = uint16_t;
    using fixed    = int32_t;
    using longword = uint32_t;
    using boolean  = int8_t;
    using memptr   = void*;
}

#endif // CCWOLF_TYPES_HPP
