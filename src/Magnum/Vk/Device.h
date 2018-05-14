#ifndef Magnum_Vk_Device_h
#define Magnum_Vk_Device_h
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
 * @brief Class @ref Magnum::Vk::Device, @ref Magnum::Vk::DeviceCreateInfo
 */

#include "Magnum/Vk/Tags.h"
#include "Magnum/Vk/AbstractObject.h"
#include "Magnum/Vk/visibility.h"

namespace Magnum { namespace Vk {

/**
@brief Device creation info

Wraps a @type_vk_keyword{DeviceCreateInfo}.
@see @ref Device::Device(const DeviceCreateInfo&)
*/
class MAGNUM_VK_EXPORT DeviceCreateInfo {
    public:
        /**
         * @brief Default constructor
         *
         * See information about default values in particular member functions.
         */
        /*implicit*/ DeviceCreateInfo();

        /** @brief Construct without initializing the contents */
        explicit DeviceCreateInfo(NoInitT);

        /**
         * @brief Construct from existing data
         *
         * Copies the existing values verbatim, pointers are kept unchanged
         * without taking over the ownership. Modifying the newly create
         * instance will not modify the original data or the pointed-to data.
         */
        explicit DeviceCreateInfo(const VkDeviceCreateInfo& info);

        ~DeviceCreateInfo();

        /** @brief Underlying @type_vk{DeviceCreateInfo} structure */
        VkDeviceCreateInfo& info() { return _info; }
        const VkDeviceCreateInfo& info() const { return _info; } /**< @overload */

    private:
        VkDeviceCreateInfo _info;
};

/**
@brief Device

Wraps a @type_vk_keyword{Device} and stores all instance-specific function
pointers.
@see @ref vulkan-wrapping
*/
class MAGNUM_VK_EXPORT Device {
    public:
        /**
         * @brief Wrap existing Vulkan instance
         * @param handle        The @type_vk{Device} handle
         * @param flags         Object creation flags
         *
         * The @p handle is expected to be of an existing Vulkan instance.
         * Unlike an instance created using a constructor, the Vulkan instance
         * is by default not deleted on destruction, use @p flags for different
         * behavior.
         * @see @ref release()
         */
        Device wrap(VkDevice handle, ObjectFlags flags);

        /**
         * @brief Default constructor
         *
         * @see @fn_vk_keyword{CreateDevice}
         */
        explicit Device(const DeviceCreateInfo& info = {});

        /**
         * @brief Construct without creating the instance
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         */
        explicit Device(NoCreateT);

        /**
         * @brief Destructor
         *
         * Destroys associated @type_vk{Device} object, unless the instance
         * was created using @ref wrap() without @ref ObjectFlag::DeleteOnDestruction
         * specified.
         * @see @fn_vk_keyword{DestroyDevice}, @ref release()
         */
        ~Device();

        /** @brief Underlying @type_vk{Device} handle */
        VkDevice handle() { return _handle; }

        /**
         * @brief Release the underlying Vulkan device
         *
         * Releases ownership of the Vulkan device and returns its handle so
         * it is not deleted on destruction. The internal state is then
         * equivalent to moved-from state.
         * @see @ref wrap()
         */
        VkDevice release();

        /**
         * @brief Populate global device-level function pointers to be used with third-party code
         *
         * Populates device-level global function pointers so third-party
         * code is able to call global device-level `vk*` functions.
         *
         * @snippet MagnumVk-device.cpp global-device-function-pointers
         *
         * See also @ref Instance::populateGlobalFunctionPointers().
         * @attention This operation is changing global state. You need to
         *      ensure that this function is not called simultaenously from
         *      multiple threads and code using those function points is
         *      calling them with the same device as the one returned by
         *      @ref handle().
         */
        void populateGlobalFunctionPointers();

        #ifndef DOXYGEN_GENERATING_OUTPUT
        const FlextVkDevice& operator->() const { return _functionPointers; }
        #endif

    private:
        VkDevice _handle;
        ObjectFlags _flags;
        FlextVkDevice _functionPointers;
};

}}

#endif

