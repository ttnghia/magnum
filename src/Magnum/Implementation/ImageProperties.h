#ifndef Magnum_Implementation_ImageProperties_h
#define Magnum_Implementation_ImageProperties_h
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

#include <utility> /* std::pair */
#include <Corrade/Containers/StridedArrayView.h>

#include "Magnum/Magnum.h"
#ifndef CORRADE_NO_ASSERT
#include "Magnum/ImageFlags.h"
#endif
#include "Magnum/DimensionTraits.h"

namespace Magnum { namespace Implementation {

/* Used in *Image and Compressed*Image constructors */
#ifndef CORRADE_NO_ASSERT
inline void checkPixelSize(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const prefix
    #endif
    , const UnsignedInt pixelSize)
{
    CORRADE_ASSERT(pixelSize && pixelSize < 256,
        prefix << "expected pixel size to be non-zero and less than 256 but got" << pixelSize, );
}

inline bool checkBlockProperties(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const prefix
    #endif
    , const Vector3i& blockSize, const UnsignedInt blockDataSize)
{
    CORRADE_ASSERT((blockSize > Vector3i{}).all() &&
                   (blockSize < Vector3i{256}).all(),
        prefix << "expected block size to be greater than zero and less than 256 but got" << Debug::packed << blockSize, {});
    CORRADE_ASSERT(blockDataSize && blockDataSize < 256,
        prefix << "expected block data size to be non-zero and less than 256 but got" << blockDataSize, {});
    return true;
}
/* GL::BufferImage has block size statically defined for all known formats so
   it doesn't need the above, only this */
inline void checkBlockPropertiesForStorage(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const prefix
    #endif
    , const Vector3i& blockSize, const UnsignedInt blockDataSize, const CompressedPixelStorage& storage)
{
    CORRADE_ASSERT(storage.compressedBlockSize() == Vector3i{} || storage.compressedBlockSize() == blockSize,
        prefix << "expected pixel storage block size to be either not set at all or equal to" << Debug::packed << blockSize << "but got" << Debug::packed << storage.compressedBlockSize(), );
    CORRADE_ASSERT(!storage.compressedBlockDataSize() || UnsignedInt(storage.compressedBlockDataSize()) == blockDataSize,
        prefix << "expected pixel storage block data size to be either not set at all or equal to" << blockDataSize << "but got" << storage.compressedBlockDataSize(), );
}

inline void checkImageFlagsForSize(const char*, const ImageFlags1D, const Math::Vector<1, Int>&) {}
inline void checkImageFlagsForSize(const char*, const ImageFlags2D, const Vector2i&) {}
inline void checkImageFlagsForSize(const char*
    #ifndef CORRADE_STANDARD_ASSERT
    const prefix
    #endif
    , const ImageFlags3D flags, const Vector3i& size)
{
    CORRADE_ASSERT(!(flags & ImageFlag3D::CubeMap) || size.x() == size.y(),
        prefix << "expected square faces for a cube map, got" << Debug::packed << size.xy(), );
    CORRADE_ASSERT(!(flags & ImageFlag3D::CubeMap) || (flags & ImageFlag3D::Array) || size.z() == 6,
        prefix << "expected exactly 6 faces for a cube map, got" << size.z(), );
    CORRADE_ASSERT(!(flags >= (ImageFlag3D::CubeMap|ImageFlag3D::Array)) || size.z() % 6 == 0,
        prefix << "expected a multiple of 6 faces for a cube map array, got" << size.z(), );
}
#endif

/* Used in *Image::dataProperties() */
template<std::size_t dimensions, class T> std::pair<Math::Vector<dimensions, std::size_t>, Math::Vector<dimensions, std::size_t>> imageDataProperties(const T& image) {
    std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties = image.storage().dataProperties(image.pixelSize(), Vector3i::pad(image.size(), 1));
    return std::make_pair(Math::Vector<dimensions, std::size_t>::pad(dataProperties.first), Math::Vector<dimensions, std::size_t>::pad(dataProperties.second));
}

/* Used in CompressedPixelStorage::dataProperties(), where it passes the
   storage-supplied block size, and in compressedImageDataSizeFor() below where
   it passes the block size from the image */
inline std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> compressedDataProperties(const CompressedPixelStorage& storage, const Vector3i& blockSize, const UnsignedInt blockDataSize, const Vector3i& size) {
    const Vector3i blockCount = (size + blockSize - Vector3i{1})/blockSize;
    const Math::Vector3<std::size_t> dataSize{
        std::size_t(storage.rowLength() ? (storage.rowLength() + blockSize.x() - 1)/blockSize.x() : blockCount.x()),
        std::size_t(storage.imageHeight() ? (storage.imageHeight() + blockSize.y() - 1)/blockSize.y() : blockCount.y()),
        std::size_t(blockCount.z())};

    const Vector3i skipBlockCount = (storage.skip() + blockSize - Vector3i{1})/blockSize;
    const Math::Vector3<std::size_t> offset = (Math::Vector3<std::size_t>{1, dataSize.x(), dataSize.xy().product()}*Math::Vector3<std::size_t>{skipBlockCount})*blockDataSize;

    return std::make_pair(offset, size.product() ? dataSize : Math::Vector3<std::size_t>{});
}

template<class, class = void> struct CompressedImageTraits;
template<class T> struct CompressedImageTraits<T, typename std::enable_if<std::is_same<decltype(std::declval<T>().storage()), CompressedPixelStorage>::value>::type> {
    static CompressedPixelStorage storage(const T& image) { return image.storage(); }
};
template<class T> struct CompressedImageTraits<T, typename std::enable_if<std::is_same<decltype(std::declval<T>().storage()), PixelStorage>::value>::type> {
    static CompressedPixelStorage storage(const T& image) { return image.compressedStorage(); }
};

/* Used in Compressed*Image::dataProperties() */
template<std::size_t dimensions, class T> std::pair<Math::Vector<dimensions, std::size_t>, Math::Vector<dimensions, std::size_t>> compressedImageDataProperties(const T& image) {
    std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties = compressedDataProperties(CompressedImageTraits<T>::storage(image), image.blockSize(), image.blockDataSize(), Vector3i::pad(image.size(), 1));
    return std::make_pair(Math::Vector<dimensions, std::size_t>::pad(dataProperties.first), Math::Vector<dimensions, std::size_t>::pad(dataProperties.second));
}

/* Used in image query functions */
template<std::size_t dimensions, class T> std::size_t imageDataSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
    std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties = image.storage().dataProperties(image.pixelSize(), Vector3i::pad(size, 1));

    /* Smallest line/rectangle/cube that covers the area. In other words, make
       it so that it matches what can be practically achieved by slicing a
       larger image. For example, if an image of 100x100 is sliced to a 50x50
       rectangle at offset (25, 25), the data size is 100x75. I.e., including
       the extra 25 padding pixels until the end of the last row, because the
       original image would have them anyway. */
    std::size_t dataOffset = 0;
    if(dataProperties.first.z())
        dataOffset += dataProperties.first.z();
    else if(dataProperties.first.y()) {
        if(!image.storage().imageHeight())
            dataOffset += dataProperties.first.y();
    } else if(dataProperties.first.x()) {
        if(!image.storage().rowLength())
            dataOffset += dataProperties.first.x();
    }
    return dataOffset + dataProperties.second.product();
}

/* Used in data size assertions */
template<class T> inline std::size_t imageDataSize(const T& image) {
    return imageDataSizeFor(image, image.size());
}

/* Unlike imageDataSizeFor() this produces separate offset and size because
   because compressedImageDataSizeFor() it's also used in GL image queries,
   where the nv-cubemap-broken-full-compressed-image-query workaround needs to
   go slice by slice, taking offset and incrementing it by size divided by the
   Z dimension. */
template<std::size_t dimensions> std::pair<std::size_t, std::size_t> compressedImageDataOffsetSizeFor(const CompressedPixelStorage& storage, const Vector3i& blockSize, const UnsignedInt blockDataSize, const Math::Vector<dimensions, Int>& size) {
    std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>> dataProperties = compressedDataProperties(storage, blockSize, blockDataSize, Vector3i::pad(size, 1));

    /* Smallest line/rectangle/cube that covers the area. Same logic as in
       imageDataSizeFor() above. */
    std::size_t dataOffset = 0;
    if(dataProperties.first.z())
        dataOffset += dataProperties.first.z();
    else if(dataProperties.first.y()) {
        if(!storage.imageHeight())
            dataOffset += dataProperties.first.y();
    } else if(dataProperties.first.x()) {
        if(!storage.rowLength())
            dataOffset += dataProperties.first.x();
    }
    return {dataOffset, dataProperties.second.product()*blockDataSize};
}

template<std::size_t dimensions, class T> std::pair<std::size_t, std::size_t> compressedImageDataOffsetSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
    return compressedImageDataOffsetSizeFor(CompressedImageTraits<T>::storage(image), image.blockSize(), image.blockDataSize(), size);
}

/* Used in image query functions */
template<std::size_t dimensions, class T> std::size_t compressedImageDataSizeFor(const T& image, const Math::Vector<dimensions, Int>& size) {
    auto r = compressedImageDataOffsetSizeFor(image, size);
    return r.first + r.second;
}

/* Used in image query functions */
template<std::size_t dimensions> std::size_t compressedImageDataSizeFor(const CompressedPixelStorage& storage, const Vector3i& blockSize, const UnsignedInt blockDataSize, const Math::Vector<dimensions, Int>& size) {
    auto r = compressedImageDataOffsetSizeFor(storage, blockSize, blockDataSize, size);
    return r.first + r.second;
}

/* Used in data size assertions */
template<class T> inline std::size_t compressedImageDataSize(const T& image) {
    auto r = compressedImageDataOffsetSizeFor(image, image.size());
    return r.first + r.second;
}

template<std::size_t dimensions, class T> std::ptrdiff_t pixelStorageSkipOffsetFor(const T& image, const Math::Vector<dimensions, Int>& size) {
    return image.storage().dataProperties(image.pixelSize(), Vector3i::pad(size, 1)).first.sum();
}
template<class T> std::ptrdiff_t pixelStorageSkipOffset(const T& image) {
    return pixelStorageSkipOffsetFor(image, image.size());
}

template<UnsignedInt dimensions, class T, class Image, class Data> Containers::StridedArrayView<dimensions + 1, T> imagePixelView(Image& image, const Data data) {
    const std::pair<VectorTypeFor<dimensions, std::size_t>, VectorTypeFor<dimensions, std::size_t>> properties = image.dataProperties();

    /* Size in the last dimension is byte size of the pixel, the remaining
       dimensions are reverted (first images, then rows, then pixels, last
       pixel bytes) */
    Containers::Size<dimensions + 1> size{NoInit};
    size[dimensions] = image.pixelSize();
    for(UnsignedInt i = dimensions; i != 0; --i)
        size[i - 1] = image.size()[dimensions - i];

    /* Stride in the last dimension is 1, stride in the second-to-last
       dimension ix pixel byte size. The remaining imensions are reverted
       (first image size, then row size, then pixel size, last 1). The
       data properties include pixel size in row size, so we have to take it
       out from the cumulative product. */
    Containers::Stride<dimensions + 1> stride{NoInit};
    stride[dimensions] = 1;
    stride[dimensions - 1] = 1;
    for(UnsignedInt i = dimensions - 1; i != 0; --i)
        stride[i - 1] = stride[i]*properties.second[dimensions - i - 1];
    stride[dimensions - 1] = image.pixelSize();

    static_assert(sizeof(decltype(image.data().front())) == 1,
        "pointer arithmetic expects image data type to have 1 byte");
    return {data.exceptPrefix(properties.first[dimensions - 1]), data + properties.first.sum(), size, stride};
}

}}

#endif
