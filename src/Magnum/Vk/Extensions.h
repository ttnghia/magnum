#ifndef Magnum_Vk_Extensions_h
#define Magnum_Vk_Extensions_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Namespace @ref Magnum::Vk::Extensions
 */

#include "Magnum/Vk/Version.h"

namespace Magnum { namespace Vk {

/**
@brief Compile-time information about Vulkan extensions

Each extension is a `struct` named hierarchically by prefix, vendor and
extension name taken from list at @ref vulkan-support, for example
`Extensions::EXT::debug_report`.

Each struct has the same public methods as @ref Extension class
(@ref Extension::requiredVersion() "requiredVersion()",
@ref Extension::coreVersion() "coreVersion()" and @ref Extension::string() "string()"),
but these structs are better suited for compile-time decisions rather than
@ref Extension instances. See @ref Device::isExtensionSupported() for
example usage.

This library is built if `WITH_VK` is enabled when building Magnum. To use this
library with CMake, you need to request the `Vk` component of the `Magnum`
package and link to the `Magnum::Vk` target:

@code{.cmake}
find_package(Magnum REQUIRED Vk)

# ...
target_link_libraries(your-app Magnum::Vk)
@endcode

See @ref building, @ref cmake and @ref vulkan for more information.
@see @ref MAGNUM_ASSERT_EXTENSION_SUPPORTED()
*/
namespace Extensions {

#ifndef DOXYGEN_GENERATING_OUTPUT
#define _extension(index, prefix, vendor, extension, _requiredVersion, _coreVersion) \
    struct extension {                                                      \
        enum: std::size_t { Index = index };                                \
        constexpr static Version requiredVersion() { return Version::_requiredVersion; } \
        constexpr static Version coreVersion() { return Version::_coreVersion; } \
        constexpr static const char* string() { return #prefix "_" #vendor "_" #extension; } \
    };

/* IMPORTANT: don't forget to add new extensions also in Context.cpp */

namespace VK {

namespace EXT {
    _extension(0,VK,EXT,debug_report,                   VK100, None) // #12
    _extension(1,VK,EXT,debug_marker,                   VK100, None) // #23
} namespace KHR {
    _extension(2,VK,KHR,get_physical_device_properties2,VK100, None) // #60
    _extension(3,VK,KHR,maintenance1,                   VK100, None) // #70
    _extension(4,VK,KHR,maintenance2,                   VK100, None) // #118
    _extension(5,VK,KHR,get_memory_requirements2,       VK100, None) // #147
} namespace NV {
    _extension(6,VK,NV,glsl_shader,                     VK100, None) // #13
}

}
#undef _extension
#endif

}

}

#endif

