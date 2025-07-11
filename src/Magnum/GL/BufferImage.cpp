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

#include "BufferImage.h"

#include "Magnum/GL/PixelFormat.h"
#include "Magnum/Implementation/ImageProperties.h"

namespace Magnum { namespace GL {

#ifndef MAGNUM_TARGET_GLES2
template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(const PixelStorage storage, const PixelFormat format, const PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> const data, const BufferUsage usage): BufferImage{storage, format, type, size, Buffer{Buffer::TargetHint::PixelPack}, data.size()} {
    _buffer.setData(data, usage);
}

template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(const PixelStorage storage, const Magnum::PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> const data, const BufferUsage usage): BufferImage{storage, GL::pixelFormat(format), GL::pixelType(format), size, data, usage} {}

template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(const PixelStorage storage, const PixelFormat format, const PixelType type, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, const std::size_t dataSize) noexcept: _storage{storage}, _format{format}, _type{type}, _size{size}, _buffer{Utility::move(buffer)}, _pixelSize{pixelFormatSize(format, type)}, _dataSize{dataSize} {
    CORRADE_ASSERT(Magnum::Implementation::imageDataSize(*this) <= dataSize, "GL::BufferImage: data too small, got" << dataSize << "but expected at least" << Magnum::Implementation::imageDataSize(*this) << "bytes", );
}

template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(const PixelStorage storage, const Magnum::PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, const std::size_t dataSize) noexcept: BufferImage{storage, GL::pixelFormat(format), GL::pixelType(format), size, Utility::move(buffer), dataSize} {}

template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(const PixelStorage storage, const PixelFormat format, const PixelType type): _storage{storage}, _format{format}, _type{type}, _buffer{Buffer::TargetHint::PixelPack}, _pixelSize{pixelFormatSize(format, type)}, _dataSize{} {
    /* Not delegating to the (buffer&&, dataSize) constructor to avoid a size
       assertion that'd happen with certain storage parameters */
}

template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(const PixelStorage storage, const Magnum::PixelFormat format): BufferImage{storage, GL::pixelFormat(format), GL::pixelType(format)} {}

template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(NoCreateT) noexcept: _format{PixelFormat::RGBA}, _type{PixelType::UnsignedByte}, _buffer{NoCreate}, _pixelSize{4}, _dataSize{} {}

template<UnsignedInt dimensions> BufferImage<dimensions>::BufferImage(BufferImage<dimensions>&& other) noexcept: _storage{Utility::move(other._storage)}, _format{Utility::move(other._format)}, _type{Utility::move(other._type)}, _size{Utility::move(other._size)}, _buffer{Utility::move(other._buffer)}, _pixelSize{Utility::move(other._pixelSize)}, _dataSize{Utility::move(other._dataSize)} {
    other._size = {};
}

template<UnsignedInt dimensions> BufferImage<dimensions>& BufferImage<dimensions>::operator=(BufferImage<dimensions>&& other) noexcept {
    using Utility::swap;
    swap(_storage, other._storage);
    swap(_format, other._format);
    swap(_type, other._type);
    swap(_size, other._size);
    swap(_buffer, other._buffer);
    swap(_pixelSize, other._pixelSize);
    swap(_dataSize, other._dataSize);
    return *this;
}

template<UnsignedInt dimensions> std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> BufferImage<dimensions>::dataProperties() const {
    return Magnum::Implementation::imageDataProperties<dimensions>(*this);
}

template<UnsignedInt dimensions> void BufferImage<dimensions>::setData(const PixelStorage storage, const PixelFormat format, const PixelType type, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> const data, const BufferUsage usage) {
    _storage = storage;
    _format = format;
    _type = type;
    _size = size;
    _pixelSize = pixelFormatSize(format, type);

    /* Keep the old storage if zero-sized nullptr buffer was passed */
    if(data.data() == nullptr && data.size() == 0)
        CORRADE_ASSERT(Magnum::Implementation::imageDataSize(*this) <= _dataSize, "GL::BufferImage::setData(): current storage too small, got" << _dataSize << "but expected at least" << Magnum::Implementation::imageDataSize(*this) << "bytes", );
    else {
        CORRADE_ASSERT(Magnum::Implementation::imageDataSize(*this) <= data.size(), "GL::BufferImage::setData(): data too small, got" << data.size() << "but expected at least" << Magnum::Implementation::imageDataSize(*this) << "bytes", );
        _buffer.setData(data, usage);
        _dataSize = data.size();
    }
}

template<UnsignedInt dimensions> void BufferImage<dimensions>::setData(const PixelStorage storage, const Magnum::PixelFormat format, const VectorTypeFor<dimensions, Int>& size, Containers::ArrayView<const void> const data, const BufferUsage usage) {
    setData(storage, GL::pixelFormat(format), GL::pixelType(format), size, data, usage);
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data, const BufferUsage usage): CompressedBufferImage{storage, format, size, Buffer{Buffer::TargetHint::PixelPack}, data.size()} {
    /* Size and block properties checks done in the delegated-to constructor
       already */
    _buffer.setData(data, usage);
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(const CompressedPixelStorage storage, const Magnum::CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data, const BufferUsage usage): CompressedBufferImage{storage, compressedPixelFormat(format), size, data, usage} {}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, const std::size_t dataSize) noexcept: _storage{storage}, _format{format}, _blockSize{Vector3ub(compressedPixelFormatBlockSize(format))}, _blockDataSize{UnsignedByte(compressedPixelFormatBlockDataSize(format))}, _size{size}, _buffer{Utility::move(buffer)}, _dataSize{dataSize} {
    #ifndef CORRADE_NO_ASSERT
    Magnum::Implementation::checkBlockPropertiesForStorage("GL::CompressedBufferImage:", Vector3i{_blockSize}, _blockDataSize, storage);
    CORRADE_ASSERT(Magnum::Implementation::compressedImageDataSize(*this) <= dataSize, "GL::CompressedBufferImage: data too small, got" << dataSize << "but expected at least" << Magnum::Implementation::compressedImageDataSize(*this) << "bytes", );
    #endif
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(const CompressedPixelStorage storage, const Magnum::CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, Buffer&& buffer, const std::size_t dataSize) noexcept: CompressedBufferImage{storage, compressedPixelFormat(format), size, Utility::move(buffer), dataSize} {}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(const CompressedPixelStorage storage): _storage{storage}, _format{}, _blockDataSize{}, _buffer{Buffer::TargetHint::PixelPack}, _dataSize{} {
    CORRADE_ASSERT(storage.compressedBlockSize() == Vector3i{},
        "GL::CompressedBufferImage: expected pixel storage block size to not be set at all but got" << Debug::packed << storage.compressedBlockSize(), );
    CORRADE_ASSERT(!storage.compressedBlockDataSize(),
        "GL::CompressedBufferImage: expected pixel storage block data size to not be set at all but got" << storage.compressedBlockDataSize(), );
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(NoCreateT) noexcept: _format{}, _blockDataSize{}, _buffer{NoCreate}, _dataSize{} {}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>::CompressedBufferImage(CompressedBufferImage<dimensions>&& other) noexcept: _storage{other._storage}, _format{other._format}, _blockSize{other._blockSize}, _blockDataSize{other._blockDataSize}, _size{other._size}, _buffer{Utility::move(other._buffer)}, _dataSize{other._dataSize} {
    other._size = {};
    other._dataSize = {};
}

template<UnsignedInt dimensions> CompressedBufferImage<dimensions>& CompressedBufferImage<dimensions>::operator=(CompressedBufferImage<dimensions>&& other) noexcept {
    using Utility::swap;
    swap(_storage, other._storage);
    swap(_format, other._format);
    swap(_blockSize, other._blockSize);
    swap(_blockDataSize, other._blockDataSize);
    swap(_size, other._size);
    swap(_buffer, other._buffer);
    swap(_dataSize, other._dataSize);
    return *this;
}

template<UnsignedInt dimensions> std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> CompressedBufferImage<dimensions>::dataProperties() const {
    return Magnum::Implementation::compressedImageDataProperties<dimensions>(*this);
}

template<UnsignedInt dimensions> void CompressedBufferImage<dimensions>::setData(const CompressedPixelStorage storage, const CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data, const BufferUsage usage) {
    _storage = storage;
    _format = format;
    _blockSize = Vector3ub(compressedPixelFormatBlockSize(format));
    _blockDataSize = UnsignedByte(compressedPixelFormatBlockDataSize(format));
    _size = size;
    #ifndef CORRADE_NO_ASSERT
    Magnum::Implementation::checkBlockPropertiesForStorage("GL::CompressedBufferImage::setData():", Vector3i{_blockSize}, _blockDataSize, storage);
    #endif

    /* Keep the old storage if zero-sized nullptr buffer was passed */
    if(data.data() == nullptr && data.size() == 0) {
        CORRADE_ASSERT(Magnum::Implementation::compressedImageDataSize(*this) <= _dataSize, "GL::CompressedBufferImage::setData(): current storage too small, got" << _dataSize << "but expected at least" << Magnum::Implementation::compressedImageDataSize(*this) << "bytes", );
    } else {
        CORRADE_ASSERT(Magnum::Implementation::compressedImageDataSize(*this) <= data.size(), "GL::CompressedBufferImage::setData(): data too small, got" << data.size() << "but expected at least" << Magnum::Implementation::compressedImageDataSize(*this) << "bytes", );
        _buffer.setData(data, usage);
        _dataSize = data.size();
    }
}

template<UnsignedInt dimensions> void CompressedBufferImage<dimensions>::setData(const CompressedPixelStorage storage, const Magnum::CompressedPixelFormat format, const VectorTypeFor<dimensions, Int>& size, const Containers::ArrayView<const void> data, const BufferUsage usage) {
    setData(storage, compressedPixelFormat(format), size, data, usage);
}

template<UnsignedInt dimensions> Buffer BufferImage<dimensions>::release() {
    _size = {};
    _dataSize = {};
    return Utility::move(_buffer);
}

template<UnsignedInt dimensions> Buffer CompressedBufferImage<dimensions>::release() {
    _size = {};
    _dataSize = {};
    return Utility::move(_buffer);
}

template class MAGNUM_GL_EXPORT BufferImage<1>;
template class MAGNUM_GL_EXPORT BufferImage<2>;
template class MAGNUM_GL_EXPORT BufferImage<3>;

template class MAGNUM_GL_EXPORT CompressedBufferImage<1>;
template class MAGNUM_GL_EXPORT CompressedBufferImage<2>;
template class MAGNUM_GL_EXPORT CompressedBufferImage<3>;
#endif

}}
