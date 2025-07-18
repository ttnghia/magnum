#ifndef Magnum_GL_BufferImage_h
#define Magnum_GL_BufferImage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
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

#ifndef MAGNUM_TARGET_GLES2
/** @file
 * @brief Class @ref Magnum::GL::BufferImage, @ref Magnum::GL::CompressedBufferImage, typedef @ref Magnum::GL::BufferImage1D, @ref Magnum::GL::BufferImage2D, @ref Magnum::GL::BufferImage3D, @ref Magnum::GL::CompressedBufferImage1D, @ref Magnum::GL::CompressedBufferImage2D, @ref Magnum::GL::CompressedBufferImage3D
 */
#endif

#include "Magnum/DimensionTraits.h"
#include "Magnum/PixelStorage.h"
#include "Magnum/GL/Buffer.h"
#include "Magnum/Math/Vector4.h"

#ifndef MAGNUM_TARGET_GLES2
namespace Magnum { namespace GL {

/**
@brief Buffer image

Stores multi-dimensional image data in GPU memory together with layout and
pixel format description. See @ref Image for the client memory counterpart.

This class can act as a drop-in replacement for @ref Image, @ref ImageView and
@ref Trade::ImageData APIs. See also @ref CompressedBufferImage for equivalent
functionality targeted on compressed image formats.

@section GL-BufferImage-usage Basic usage

The image creates a @ref Buffer instance and fills it with passed data, storing
corresponding image size and pixel format properties. Because this is a
GL-centric class, it's common to specify the format using @ref GL::PixelFormat
and @link GL::PixelType @endlink:

@snippet GL.cpp BufferImage-usage

It's also possible to pass the generic @ref Magnum::PixelFormat to it, however
the @ref format() and @ref type() queries will always return the GL-specific
value. On construction, the image internally calculates pixel size
corresponding to given pixel format using either @ref GL::pixelFormatSize() or
@ref Magnum::pixelFormatSize(). This value is needed to check that the passed
data are large enough and also required by most of image manipulation
operations.

Besides creating and owning the buffer, you can also pass existing buffer to
it, for example to use buffer storage and other advanced functionality. The
image will take an ownership of the buffer, you can use @ref Buffer::wrap() to
make a non-owning copy.

@snippet GL.cpp BufferImage-usage-wrap

It's also possible to create just an image placeholder, storing only the image
properties without data or size. That is useful for example to specify desired
format of image queries in graphics APIs:

@snippet GL.cpp BufferImage-usage-query

Similarly to @ref ImageView, this class supports extra storage parameters.
See @ref ImageView-usage for more information.

Unlike @ref Image, @ref ImageView and @ref Trade::ImageData, this class doesn't
support setting @ref ImageFlags. In those, image flags are used mainly to
distinguish between various image layouts when interfacing with file formats
and external conversion libraries. The buffer image, however, usually acts as a
temporary storage for data transfer following OpenGL constraints, and as such
the annotations aren't as essential.

@see @ref BufferImage1D, @ref BufferImage2D, @ref BufferImage3D
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
@requires_webgl20 Pixel buffer objects are not available in WebGL 1.0.
*/
template<UnsignedInt dimensions> class BufferImage {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Constructor
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         */
        explicit BufferImage(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Equivalent to calling @ref BufferImage(PixelStorage, PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         * with default-constructed @ref PixelStorage.
         */
        explicit BufferImage(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage): BufferImage{{}, format, type, size, data, usage} {}

        /**
         * @brief Constructor
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Converts @ref Magnum::PixelFormat to GL-specific values using
         * @ref pixelFormat() and @ref pixelType() and then calls
         * @ref BufferImage(PixelStorage, PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage).
         */
        explicit BufferImage(PixelStorage storage, Magnum::PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /**
         * @brief Constructor
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Equivalent to calling @ref BufferImage(PixelStorage, Magnum::PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         * with default-constructed @ref PixelStorage.
         */
        explicit BufferImage(Magnum::PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage): BufferImage{{}, format, size, data, usage} {}

        /**
         * @brief Construct from existing buffer
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param buffer            Buffer
         * @param dataSize          Buffer data size
         *
         * If @p dataSize is @cpp 0 @ce, the buffer is unconditionally
         * reallocated on the first call to @ref setData().
         */
        explicit BufferImage(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept;

        /**
         * @brief Construct from existing buffer
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param buffer            Buffer
         * @param dataSize          Buffer data size
         *
         * Equivalent to calling @ref BufferImage(PixelStorage, PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Buffer&&, std::size_t)
         * with default-constructed @ref PixelStorage.
         */
        explicit BufferImage(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept: BufferImage{{}, format, type, size, Utility::move(buffer), dataSize} {}

        /**
         * @brief Construct from existing buffer
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param buffer            Buffer
         * @param dataSize          Buffer data size
         *
         * Converts @ref Magnum::PixelFormat to GL-specific values using
         * @ref pixelFormat() and @ref pixelType() and then calls
         * @ref BufferImage(PixelStorage, PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Buffer&&, std::size_t).
         */
        explicit BufferImage(PixelStorage storage, Magnum::PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept;

        /**
         * @brief Construct from existing buffer
         * @param format            Format of pixel data
         * @param size              Image size
         * @param buffer            Buffer
         * @param dataSize          Buffer data size
         *
         * Equivalent to calling @ref BufferImage(PixelStorage, Magnum::PixelFormat, const VectorTypeFor<dimensions, Int>&, Buffer&&, std::size_t)
         * with default-constructed @ref PixelStorage.
         */
        explicit BufferImage(Magnum::PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept: BufferImage{{}, format, size, Utility::move(buffer), dataSize} {}

        /**
         * @brief Construct an image placeholder
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Size is zero and buffer are empty, call @ref setData() to fill the
         * image with data.
         */
        /*implicit*/ BufferImage(PixelStorage storage, PixelFormat format, PixelType type);

        /**
         * @brief Construct an image placeholder
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         *
         * Equivalent to calling @ref BufferImage(PixelStorage, PixelFormat, PixelType)
         * with default-constructed @ref PixelStorage.
         */
        /*implicit*/ BufferImage(PixelFormat format, PixelType type): BufferImage{{}, format, type} {}

        /**
         * @brief Construct an image placeholder
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         *
         * Converts @ref Magnum::PixelFormat to GL-specific values using
         * @ref pixelFormat() and @ref pixelType() and then calls
         * @ref BufferImage(PixelStorage, PixelFormat, PixelType).
         */
        /*implicit*/ BufferImage(PixelStorage storage, Magnum::PixelFormat format);

        /**
         * @brief Construct an image placeholder
         * @param format            Format of pixel data
         *
         * Equivalent to calling @ref BufferImage(PixelStorage, Magnum::PixelFormat)
         * with default-constructed @ref PixelStorage.
         */
        /*implicit*/ BufferImage(Magnum::PixelFormat format): BufferImage{{}, format} {}

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state with
         * @ref PixelFormat::RGBA and @ref PixelType::UnsignedByte. Useful in
         * cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         * @see @ref BufferImage(), @ref Buffer::wrap()
         */
        explicit BufferImage(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        BufferImage(const BufferImage<dimensions>&) = delete;

        /** @brief Move constructor */
        BufferImage(BufferImage<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        BufferImage<dimensions>& operator=(const BufferImage<dimensions>&) = delete;

        /** @brief Move assignment */
        BufferImage<dimensions>& operator=(BufferImage<dimensions>&& other) noexcept;

        /** @brief Currently allocated data size */
        std::size_t dataSize() const { return _dataSize; }

        /**
         * @brief Image buffer
         *
         * @see @ref release(), @ref size(), @ref pixelSize()
         */
        Buffer& buffer() { return _buffer; }

        /**
         * @brief Set image data
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param type              Data type of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Updates the image buffer with given data. Passing @cpp nullptr @ce
         * zero-sized @p data will not reallocate current storage, but expects
         * that current data size is large enough for the new parameters.
         * @see @ref Buffer::setData()
         */
        void setData(PixelStorage storage, PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /** @overload
         * Similar to the above, but uses default @ref PixelStorage parameters.
         */
        void setData(PixelFormat format, PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage) {
            setData({}, format, type, size, data, usage);
        }

        /**
         * @brief Set image data
         * @param storage           Storage of pixel data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Converts @ref Magnum::PixelFormat to GL-specific values using
         * @ref pixelFormat() and @ref pixelType() and then calls
         * @ref setData(PixelStorage, PixelFormat, PixelType, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage).
         */
        void setData(PixelStorage storage, Magnum::PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /**
         * @brief Set image data
         * @param format            Format of pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Equivalent to calling @ref setData(PixelStorage, Magnum::PixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         * with default-constructed @ref PixelStorage.
         */
        void setData(Magnum::PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage) {
            setData({}, format, size, data, usage);
        }

        /** @brief Storage of pixel data */
        PixelStorage storage() const { return _storage; }

        /** @brief Format of pixel data */
        PixelFormat format() const { return _format; }

        /** @brief Data type of pixel data */
        PixelType type() const { return _type; }

        /**
         * @brief Size of a pixel in bytes
         *
         * @see @ref size(), @ref Magnum::pixelFormatSize(),
         *      @ref GL::pixelFormatSize()
         */
        UnsignedInt pixelSize() const { return _pixelSize; }

        /**
         * @brief Image size in pixels
         *
         * @see @ref pixelSize()
         */
        VectorTypeFor<Dimensions, Int> size() const { return _size; }

        /**
         * @brief Image data properties
         *
         * See @ref PixelStorage::dataProperties() for more information.
         */
        std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> dataProperties() const;

        /**
         * @brief Release the image buffer
         *
         * Releases the ownership of the data array and resets @ref size() to
         * zero. The state afterwards is equivalent to moved-from state.
         * @see @ref buffer()
         */
        Buffer release();

    private:
        PixelStorage _storage;
        PixelFormat _format;
        PixelType _type;
        Math::Vector<Dimensions, Int> _size;
        Buffer _buffer;
        UnsignedInt _pixelSize;
        std::size_t _dataSize;
};

/** @brief One-dimensional buffer image */
typedef BufferImage<1> BufferImage1D;

/** @brief Two-dimensional buffer image */
typedef BufferImage<2> BufferImage2D;

/** @brief Three-dimensional buffer image */
typedef BufferImage<3> BufferImage3D;

/**
@brief Compressed buffer image

Stores multi-dimensional compressed image data in GPU memory together with
layout and compressed block description. See @ref CompressedImage for the
client memory counterpart.

This class can act as a drop-in replacement for @ref CompressedImage,
@ref CompressedImageView and @ref Trade::ImageData APIs. See also
@ref BufferImage for equivalent functionality targeted on non-compressed image
formats.

@section GL-CompressedBufferImage-usage Basic usage

The image creates a @ref Buffer instance and fills it with passed data, storing
corresponding image size and compression format properties. Because this is a
GL-centric class, it's common to specify the format using
@link GL::CompressedPixelFormat @endlink:

@snippet GL.cpp CompressedBufferImage-usage

It's also possible to pass the generic @ref Magnum::CompressedPixelFormat to
it, however the @ref format() query will always return the GL-specific value.

Besides creating and owning the buffer, you can also pass existing buffer to
it, for example to use buffer storage and other advanced functionality. The
image will take an ownership of the buffer, you can use @ref Buffer::wrap() to
make a non-owning copy.

@snippet GL.cpp CompressedBufferImage-usage-wrap

It's also possible to create just an image placeholder, storing only the image
properties without data or size. That is useful for example to specify desired
format of image queries in graphics APIs:

@snippet GL.cpp CompressedBufferImage-usage-query

Similarly to @ref CompressedImageView, this class supports extra storage
parameters. See @ref CompressedImageView-usage for more information.

Unlike @ref CompressedImage, @ref CompressedImageView and
@ref Trade::ImageData, this class doesn't support setting @ref ImageFlags. In
those, image flags are used mainly to distinguish between various image
layouts when interfacing with file formats and external conversion libraries.
The buffer image, however, usually acts as a temporary storage for data
transfer following OpenGL constraints, and as such the annotations aren't as
essential.

@see @ref CompressedBufferImage1D, @ref CompressedBufferImage2D,
    @ref CompressedBufferImage3D
@requires_gles30 Pixel buffer objects are not available in OpenGL ES 2.0.
@requires_webgl20 Pixel buffer objects are not available in WebGL 1.0.
*/
template<UnsignedInt dimensions> class CompressedBufferImage {
    public:
        enum: UnsignedInt {
            Dimensions = dimensions /**< Image dimension count */
        };

        /**
         * @brief Constructor
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * The @p data array is expected to be of proper size for given
         * parameters. @ref CompressedPixelStorage::compressedBlockSize() and
         * @relativeref{CompressedPixelStorage,compressedBlockDataSize()} in
         * @p storage are expected to be either both zero or exactly matching
         * properties of given @p format.
         *
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         */
        explicit CompressedBufferImage(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /**
         * @brief Constructor
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Equivalent to calling @ref CompressedBufferImage(CompressedPixelStorage, Magnum::CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit CompressedBufferImage(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage): CompressedBufferImage{{}, format, size, data, usage} {}

        /**
         * @brief Constructor
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Converts @ref Magnum::CompressedPixelFormat to a GL-specific value
         * using @ref compressedPixelFormat() and and then calls
         * @ref CompressedBufferImage(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage).
         */
        explicit CompressedBufferImage(CompressedPixelStorage storage, Magnum::CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /**
         * @brief Constructor
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Equivalent to calling @ref CompressedBufferImage(CompressedPixelStorage, Magnum::CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit CompressedBufferImage(Magnum::CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage): CompressedBufferImage{{}, format, size, data, usage} {}

        /**
         * @brief Constructor
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param buffer            Buffer
         * @param dataSize          Buffer data size
         *
         * The @p dataSize is expected to be of proper size for given
         * parameters. @ref CompressedPixelStorage::compressedBlockSize() and
         * @relativeref{CompressedPixelStorage,compressedBlockDataSize()} in
         * @p storage are expected to be either both zero or exactly matching
         * properties of given @p format.
         *
         * If @p dataSize is @cpp 0 @ce, the buffer is unconditionally
         * reallocated on the first call to @ref setData().
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         */
        explicit CompressedBufferImage(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept;

        /**
         * @brief Construct from existing buffer
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param buffer            Buffer
         * @param dataSize          Buffer data size
         *
         * Equivalent to calling @ref CompressedBufferImage(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Buffer&&, std::size_t)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit CompressedBufferImage(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept: CompressedBufferImage{{}, format, size, Utility::move(buffer), dataSize} {}

        /**
         * @brief Construct from existing buffer
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param buffer            Buffer
         * @param dataSize          Buffer data size
         *
         * Converts @ref Magnum::CompressedPixelFormat to a GL-specific value
         * using @ref compressedPixelFormat() and and then calls
         * @ref CompressedBufferImage(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Buffer&&, std::size_t).
         */
        explicit CompressedBufferImage(CompressedPixelStorage storage, Magnum::CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept;

        /**
         * @brief Construct from existing buffer
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param buffer            Buffer
         * @param dataSize          Buffer data size
         *
         * Equivalent to calling @ref CompressedBufferImage(CompressedPixelStorage, Magnum::CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Buffer&&, std::size_t)
         * with default-constructed @ref CompressedPixelStorage.
         */
        explicit CompressedBufferImage(Magnum::CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, std::size_t dataSize) noexcept: CompressedBufferImage{{}, format, size, Utility::move(buffer), dataSize} {}

        /**
         * @brief Construct an image placeholder
         * @param storage           Storage of compressed pixel data
         *
         * Format and block properties are undefined, size is zero and buffer
         * is empty. Call @ref setData() to fill the image with data.
         *
         * @ref CompressedPixelStorage::compressedBlockSize() and
         * @relativeref{CompressedPixelStorage,compressedBlockDataSize()} in
         * @p storage are expected to be both zero.
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         */
        /*implicit*/ CompressedBufferImage(CompressedPixelStorage storage);

        /**
         * @brief Construct an image placeholder
         *
         * Equivalent to calling @ref CompressedBufferImage(CompressedPixelStorage)
         * with default-constructed @ref CompressedPixelStorage.
         */
        /*implicit*/ CompressedBufferImage(): CompressedBufferImage{CompressedPixelStorage{}} {}

        /**
         * @brief Construct without creating the underlying OpenGL object
         *
         * The constructed instance is equivalent to moved-from state. Useful
         * in cases where you will overwrite the instance later anyway. Move
         * another object over it to make it useful.
         *
         * This function can be safely used for constructing (and later
         * destructing) objects even without any OpenGL context being active.
         * However note that this is a low-level and a potentially dangerous
         * API, see the documentation of @ref NoCreate for alternatives.
         * @see @ref CompressedBufferImage(), @ref Buffer::wrap()
         */
        explicit CompressedBufferImage(NoCreateT) noexcept;

        /** @brief Copying is not allowed */
        CompressedBufferImage(const CompressedBufferImage<dimensions>&) = delete;

        /** @brief Move constructor */
        CompressedBufferImage(CompressedBufferImage<dimensions>&& other) noexcept;

        /** @brief Copying is not allowed */
        CompressedBufferImage<dimensions>& operator=(const CompressedBufferImage<dimensions>&) = delete;

        /** @brief Move assignment */
        CompressedBufferImage<dimensions>& operator=(CompressedBufferImage<dimensions>&& other) noexcept;

        /**
         * @brief Image buffer
         *
         * @see @ref release(), @ref size(), @ref blockSize(),
         *      @ref blockDataSize()
         */
        Buffer& buffer() { return _buffer; }

        /** @brief Raw data size */
        std::size_t dataSize() const { return _dataSize; }

        /**
         * @brief Set image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Updates the image buffer with given data. Passing @cpp nullptr @ce
         * zero-sized @p data will not reallocate current storage, but expects
         * that current data size is large enough for the new parameters.
         * @see @ref Buffer::setData()
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         */
        void setData(CompressedPixelStorage storage, CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /**
         * @brief Set image data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Equivalent to calling @ref setData(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         * with default-constructed @ref CompressedPixelStorage.
         */
        void setData(CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage) {
            setData({}, format, size, data, usage);
        }

        /**
         * @brief Set image data
         * @param storage           Storage of compressed pixel data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Converts @ref Magnum::CompressedPixelFormat to a GL-specific value
         * using @ref compressedPixelFormat() and and then calls
         * @ref setData(CompressedPixelStorage, CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage).
         */
        void setData(CompressedPixelStorage storage, Magnum::CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage);

        /**
         * @brief Set image data
         * @param format            Format of compressed pixel data
         * @param size              Image size
         * @param data              Image data
         * @param usage             Image buffer usage
         *
         * Equivalent to calling @ref setData(CompressedPixelStorage, Magnum::CompressedPixelFormat, const VectorTypeFor<dimensions, Int>&, Containers::ArrayView<const void>, BufferUsage)
         * with default-constructed @ref CompressedPixelStorage.
         */
        void setData(Magnum::CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> data, BufferUsage usage) {
            setData({}, format, size, data, usage);
        }

        /** @brief Storage of compressed pixel data */
        CompressedPixelStorage storage() const { return _storage; }

        /** @brief Format of compressed pixel data */
        CompressedPixelFormat format() const { return _format; }

        /**
         * @brief Size of a compressed block in pixels
         * @m_since_latest
         *
         * Note that the blocks can be 3D even for 2D images and 2D or 3D even
         * for 1D images, in which case only the first slice in the extra
         * dimensions is used.
         * @see @ref blockDataSize(), @ref size(),
         *      @ref compressedPixelFormatBlockSize()
         */
        Vector3i blockSize() const { return Vector3i{_blockSize}; }

        /**
         * @brief Size of a compressed block in bytes
         * @m_since_latest
         *
         * @see @ref blockSize(), @ref size(),
         *      @ref compressedPixelFormatBlockDataSize()
         */
        UnsignedInt blockDataSize() const { return _blockDataSize; }

        /**
         * @brief Image size in pixels
         *
         * If the size isn't divisible by @ref blockSize(), the edge blocks are
         * still present in full but used only partially.
         * @see @ref blockDataSize()
         */
        VectorTypeFor<Dimensions, Int> size() const { return _size; }

        /**
         * @brief Compressed image data properties
         *
         * See @ref CompressedPixelStorage::dataProperties() for more
         * information.
         * @requires_gl42 Extension @gl_extension{ARB,compressed_texture_pixel_storage}
         *      for non-default @ref CompressedPixelStorage
         * @requires_gl Non-default @ref CompressedPixelStorage is not
         *      available in OpenGL ES and WebGL.
         */
        std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> dataProperties() const;

        /**
         * @brief Release the image buffer
         *
         * Releases the ownership of the data array and resets @ref size() to
         * zero. The state afterwards is equivalent to moved-from state.
         * @see @ref buffer()
         */
        Buffer release();

    private:
        CompressedPixelStorage _storage;
        CompressedPixelFormat _format;
        /* Largest blocks are 12x12 in ASTC and at most 32 bytes, so an 8-bit
           type should be more than enough. As even 1D images can have 3D
           blocks, the member isn't dependent on dimension count. */
        Vector3ub _blockSize;
        UnsignedByte _blockDataSize;
        Math::Vector<Dimensions, Int> _size;
        Buffer _buffer;
        std::size_t _dataSize;
};

/** @brief One-dimensional compressed buffer image */
typedef CompressedBufferImage<1> CompressedBufferImage1D;

/** @brief Two-dimensional compressed buffer image */
typedef CompressedBufferImage<2> CompressedBufferImage2D;

/** @brief Three-dimensional compressed buffer image */
typedef CompressedBufferImage<3> CompressedBufferImage3D;

}}
#else
#error this header is not available in OpenGL ES 2.0 build
#endif

#endif
