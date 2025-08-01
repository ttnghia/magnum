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

/* Included as first to check that we *really* don't need the StridedArrayView
   header for definition of pixels(). We actually need, but just for the
   arrayCast() template, which is forward-declared. */
#include "Magnum/Trade/ImageData.h"

#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>
#include <Corrade/TestSuite/Compare/String.h>
#include <Corrade/Utility/DebugStl.h> /** @todo remove once dataProperties() std::pair is gone */

#include "Magnum/ImageView.h"
#include "Magnum/PixelFormat.h"
#include "Magnum/Math/Color.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct ImageDataTest: TestSuite::Tester {
    explicit ImageDataTest();

    void constructGeneric();
    void constructImplementationSpecific();
    void constructCompressedGeneric();
    void constructCompressedImplementationSpecific();

    void constructGenericNotOwned();
    void constructImplementationSpecificNotOwned();
    void constructCompressedGenericNotOwned();
    void constructCompressedImplementationSpecificNotOwned();
    void constructGenericNotOwnedFlagOwned();
    void constructImplementationSpecificNotOwnedFlagOwned();
    void constructCompressedGenericNotOwnedFlagOwned();
    void constructCompressedImplementationSpecificNotOwnedFlagOwned();

    void constructUnknownImplementationSpecificPixelSize();
    void constructInvalidPixelSize();
    void constructInvalidSize();
    void constructInvalidCubeMap();
    void constructCompressedUnknownImplementationSpecificBlockSize();
    void constructCompressedInvalidBlockSize();
    void constructCompressedInvalidSize();
    void constructCompressedInvalidCubeMap();

    void constructCopy();

    void constructMoveGeneric();
    void constructMoveImplementationSpecific();
    void constructMoveCompressedGeneric();
    void constructMoveCompressedImplementationSpecific();

    void constructMoveAttachState();
    void constructMoveCompressedAttachState();

    void moveCompressedToUncompressed();
    void moveUncompressedToCompressed();

    void propertiesInvalid();

    template<class T> void toViewGeneric();
    template<class T> void toViewImplementationSpecific();
    template<class T> void toViewCompressedGeneric();
    template<class T> void toViewCompressedImplementationSpecific();

    void data();
    void dataRvalue();
    void mutableAccessNotAllowed();

    void dataProperties();
    void dataPropertiesCompressed();

    void release();
    void releaseCompressed();

    void pixels1D();
    void pixels2D();
    void pixels3D();
};

template<class> struct MutabilityTraits;
template<> struct MutabilityTraits<const char> {
    typedef const ImageData2D ImageType;

    static const char* name() { return "ImageView"; }
};
template<> struct MutabilityTraits<char> {
    typedef ImageData2D ImageType;

    static const char* name() { return "MutableImageView"; }
};

struct {
    const char* name;
    DataFlags dataFlags;
} NotOwnedData[] {
    {"", {}},
    {"mutable", DataFlag::Mutable},
};

ImageDataTest::ImageDataTest() {
    addTests({&ImageDataTest::constructGeneric,
              &ImageDataTest::constructImplementationSpecific,
              &ImageDataTest::constructCompressedGeneric,
              &ImageDataTest::constructCompressedImplementationSpecific});

    addInstancedTests({&ImageDataTest::constructGenericNotOwned,
                       &ImageDataTest::constructImplementationSpecificNotOwned,
                       &ImageDataTest::constructCompressedGenericNotOwned,
                       &ImageDataTest::constructCompressedImplementationSpecificNotOwned},
        Containers::arraySize(NotOwnedData));

    addTests({&ImageDataTest::constructGenericNotOwnedFlagOwned,
              &ImageDataTest::constructImplementationSpecificNotOwnedFlagOwned,
              &ImageDataTest::constructCompressedGenericNotOwnedFlagOwned,
              &ImageDataTest::constructCompressedImplementationSpecificNotOwnedFlagOwned,

              &ImageDataTest::constructUnknownImplementationSpecificPixelSize,
              &ImageDataTest::constructInvalidPixelSize,
              &ImageDataTest::constructInvalidSize,
              &ImageDataTest::constructInvalidCubeMap,
              &ImageDataTest::constructCompressedUnknownImplementationSpecificBlockSize,
              &ImageDataTest::constructCompressedInvalidBlockSize,
              &ImageDataTest::constructCompressedInvalidSize,
              &ImageDataTest::constructCompressedInvalidCubeMap,

              &ImageDataTest::constructCopy,

              &ImageDataTest::constructMoveGeneric,
              &ImageDataTest::constructMoveImplementationSpecific,
              &ImageDataTest::constructMoveCompressedGeneric,
              &ImageDataTest::constructMoveCompressedImplementationSpecific,

              &ImageDataTest::constructMoveAttachState,
              &ImageDataTest::constructMoveCompressedAttachState,

              &ImageDataTest::moveCompressedToUncompressed,
              &ImageDataTest::moveUncompressedToCompressed,

              &ImageDataTest::propertiesInvalid,

              &ImageDataTest::toViewGeneric<const char>,
              &ImageDataTest::toViewGeneric<char>,
              &ImageDataTest::toViewImplementationSpecific<const char>,
              &ImageDataTest::toViewImplementationSpecific<char>,
              &ImageDataTest::toViewCompressedGeneric<const char>,
              &ImageDataTest::toViewCompressedGeneric<char>,
              &ImageDataTest::toViewCompressedImplementationSpecific<const char>,
              &ImageDataTest::toViewCompressedImplementationSpecific<char>,

              &ImageDataTest::data,
              &ImageDataTest::dataRvalue,
              &ImageDataTest::mutableAccessNotAllowed,

              &ImageDataTest::dataProperties,
              &ImageDataTest::dataPropertiesCompressed,

              &ImageDataTest::release,
              &ImageDataTest::releaseCompressed,

              &ImageDataTest::pixels1D,
              &ImageDataTest::pixels2D,
              &ImageDataTest::pixels3D});
}

namespace GL {
    enum class PixelFormat { RGB = 666 };
    enum class PixelType { UnsignedShort = 1337 };
    /* Clang -Wmissing-prototypes warns otherwise, even though this is in an
       anonymous namespace */
    UnsignedInt pixelFormatSize(PixelFormat, PixelType);
    UnsignedInt pixelFormatSize(PixelFormat format, PixelType type) {
        CORRADE_INTERNAL_ASSERT(format == PixelFormat::RGB);
        CORRADE_INTERNAL_ASSERT(type == PixelType::UnsignedShort);
        #ifdef CORRADE_NO_ASSERT
        static_cast<void>(format);
        static_cast<void>(type);
        #endif
        return 6;
    }

    enum class CompressedPixelFormat { RGBS3tcDxt1 = 21 };
    /* Clang -Wmissing-prototypes warns otherwise, even though this is in an
       anonymous namespace */
    Vector3i compressedPixelFormatBlockSize(CompressedPixelFormat);
    Vector3i compressedPixelFormatBlockSize(CompressedPixelFormat format) {
        CORRADE_INTERNAL_ASSERT(format == CompressedPixelFormat::RGBS3tcDxt1);
        #ifdef CORRADE_NO_ASSERT
        static_cast<void>(format);
        #endif
        return {4, 4, 1};
    }
    UnsignedInt compressedPixelFormatBlockDataSize(CompressedPixelFormat);
    UnsignedInt compressedPixelFormatBlockDataSize(CompressedPixelFormat format) {
        CORRADE_INTERNAL_ASSERT(format == CompressedPixelFormat::RGBS3tcDxt1);
        #ifdef CORRADE_NO_ASSERT
        static_cast<void>(format);
        #endif
        return 8;
    }
}

namespace Vk {
    enum class PixelFormat { R32G32B32F = 42 };
    /* Clang -Wmissing-prototypes warns otherwise, even though this is in an
       anonymous namespace */
    UnsignedInt pixelFormatSize(PixelFormat);
    UnsignedInt pixelFormatSize(PixelFormat format) {
        CORRADE_INTERNAL_ASSERT(format == PixelFormat::R32G32B32F);
        #ifdef CORRADE_NO_ASSERT
        static_cast<void>(format);
        #endif
        return 12;
    }

    enum class CompressedPixelFormat { Astc5x5x4RGBAF = 111 };
    /* Clang -Wmissing-prototypes warns otherwise, even though this is in an
       anonymous namespace */
    Vector3i compressedPixelFormatBlockSize(CompressedPixelFormat);
    Vector3i compressedPixelFormatBlockSize(CompressedPixelFormat format) {
        #ifdef CORRADE_NO_ASSERT
        static_cast<void>(format);
        #endif
        CORRADE_INTERNAL_ASSERT(format == CompressedPixelFormat::Astc5x5x4RGBAF);
        return {5, 5, 4};
    }
    UnsignedInt compressedPixelFormatBlockDataSize(CompressedPixelFormat);
    UnsignedInt compressedPixelFormatBlockDataSize(CompressedPixelFormat format) {
        #ifdef CORRADE_NO_ASSERT
        static_cast<void>(format);
        #endif
        CORRADE_INTERNAL_ASSERT(format == CompressedPixelFormat::Astc5x5x4RGBAF);
        return 16;
    }
}

void ImageDataTest::constructGeneric() {
    {
        auto data = new char[4*4];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{PixelFormat::RGBA8Unorm, {1, 3}, Containers::Array<char>{data, 4*4}, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RGBA8Unorm);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 4);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 4*4);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Color4ub>()[0][0]), data);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 4*4);
        CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Color4ub>()[0][0]), data);
        CORRADE_COMPARE(a.importerState(), &state);
    } {
        auto data = new char[3*2];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{PixelStorage{}.setAlignment(1),
            PixelFormat::R16UI, {1, 3}, Containers::Array<char>{data, 3*2}, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::R16UI);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 2);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*2);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<UnsignedShort>()[0][0]), data);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 3*2);
        CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<UnsignedShort>()[0][0]), data);
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructImplementationSpecific() {
    /* Single format */
    {
        auto data = new char[3*12];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{PixelStorage{}.setAlignment(1),
            Vk::PixelFormat::R32G32B32F, {1, 3}, Containers::Array<char>{data, 3*12}, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*12);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Color3>()[0][0]), data);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 3*12);
        CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Color3>()[0][0]), data);
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Format + extra */
    {
        auto data = new char[3*6];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Manual pixel size */
    {
        auto data = new char[3*6];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {1, 3}, Containers::Array<char>{data, 3*6}, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructCompressedGeneric() {
    {
        auto data = new char[7*8];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{
            CompressedPixelFormat::Bc1RGBAUnorm, {12, 8},
            Containers::Array<char>{data, 7*8}, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.compressedStorage().rowLength(), 0);
        CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.blockSize(), (Vector3i{4, 4, 1}));
        CORRADE_COMPARE(a.blockDataSize(), 8);
        CORRADE_COMPARE(a.size(), (Vector2i{12, 8}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 7*8);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 7*8);
        CORRADE_COMPARE(a.importerState(), &state);
    } {
        auto data = new char[8*16];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{
            CompressedPixelStorage{}.setRowLength(20),
            CompressedPixelFormat::Astc5x5x4RGBAF, {15, 10},
            Containers::Array<char>{data, 8*16}, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.compressedStorage().rowLength(), 20);
        CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Astc5x5x4RGBAF);
        CORRADE_COMPARE(a.blockSize(), (Vector3i{5, 5, 4}));
        CORRADE_COMPARE(a.blockDataSize(), 16);
        CORRADE_COMPARE(a.size(), (Vector2i{15, 10}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 8*16);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 8*16);
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructCompressedImplementationSpecific() {
    /* Format with autodetection */
    {
        auto data = new char[8*8];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{
            CompressedPixelStorage{}.setRowLength(16),
            GL::CompressedPixelFormat::RGBS3tcDxt1, {12, 8},
            Containers::Array<char>{data, 8*8}, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.compressedStorage().rowLength(), 16);
        CORRADE_COMPARE(a.compressedFormat(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.blockSize(), (Vector3i{4, 4, 1}));
        CORRADE_COMPARE(a.blockDataSize(), 8);
        CORRADE_COMPARE(a.size(), (Vector2i{12, 8}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 8*8);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 8*8);
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Manual block properties */
    {
        auto data = new char[6*12];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{
            CompressedPixelStorage{}.setRowLength(6),
            111, {3, 4, 5}, 12, {3, 8}, Containers::Array<char>{data, 6*12},
            ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.compressedStorage().rowLength(), 6);
        CORRADE_COMPARE(a.compressedFormat(), compressedPixelFormatWrap(Vk::CompressedPixelFormat::Astc5x5x4RGBAF));
        CORRADE_COMPARE(a.blockSize(), (Vector3i{3, 4, 5}));
        CORRADE_COMPARE(a.blockDataSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{3, 8}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 6*12);
        CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
        CORRADE_COMPARE(a.mutableData().size(), 6*12);
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructGenericNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    {
        char data[4*4];
        int state;
        ImageData2D a{PixelFormat::RGBA8Unorm, {1, 3}, instanceData.dataFlags, data, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.storage().alignment(), 4);
        CORRADE_COMPARE(a.format(), PixelFormat::RGBA8Unorm);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 4);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 4*4);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Color4ub>()[0][0]), data);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 4*4);
            CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Color4ub>()[0][0]), data);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    } {
        char data[3*2];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            PixelFormat::R16UI, {1, 3}, instanceData.dataFlags, data, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), PixelFormat::R16UI);
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 2);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*2);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<UnsignedShort>()[0][0]), data);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 3*2);
            CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<UnsignedShort>()[0][0]), data);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructImplementationSpecificNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    /* Single format */
    {
        char data[3*12];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            Vk::PixelFormat::R32G32B32F, {1, 3}, instanceData.dataFlags, data, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(Vk::PixelFormat::R32G32B32F));
        CORRADE_COMPARE(a.formatExtra(), 0);
        CORRADE_COMPARE(a.pixelSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*12);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Color3>()[0][0]), data);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 3*12);
            CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Color3>()[0][0]), data);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Format + extra */
    {
        char data[3*6];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1),
            GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, instanceData.dataFlags, data, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 3*6);
            CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Manual pixel size */
    {
        char data[3*6];
        int state;
        ImageData2D a{PixelStorage{}.setAlignment(1), 666, 1337, 6, {1, 3}, instanceData.dataFlags, data, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(!a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.storage().alignment(), 1);
        CORRADE_COMPARE(a.format(), pixelFormatWrap(GL::PixelFormat::RGB));
        CORRADE_COMPARE(a.formatExtra(), UnsignedInt(GL::PixelType::UnsignedShort));
        CORRADE_COMPARE(a.pixelSize(), 6);
        CORRADE_COMPARE(a.size(), (Vector2i{1, 3}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 3*6);
        CORRADE_COMPARE(static_cast<const void*>(&a.pixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 3*6);
            CORRADE_COMPARE(static_cast<const void*>(&a.mutablePixels<Math::Vector3<UnsignedShort>>()[0][0]), data);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructCompressedGenericNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    {
        char data[6*8];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{
            CompressedPixelFormat::Bc1RGBAUnorm, {12, 8},
            instanceData.dataFlags, data, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.compressedStorage().rowLength(), 0);
        CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Bc1RGBAUnorm);
        CORRADE_COMPARE(a.blockSize(), (Vector3i{4, 4, 1}));
        CORRADE_COMPARE(a.blockDataSize(), 8);
        CORRADE_COMPARE(a.size(), (Vector2i{12, 8}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 6*8);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 6*8);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    } {
        char data[8*16];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{
            CompressedPixelStorage{}.setRowLength(20),
            CompressedPixelFormat::Astc5x5x4RGBAF, {15, 10},
            instanceData.dataFlags, data, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.compressedStorage().rowLength(), 20);
        CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Astc5x5x4RGBAF);
        CORRADE_COMPARE(a.blockSize(), (Vector3i{5, 5, 4}));
        CORRADE_COMPARE(a.blockDataSize(), 16);
        CORRADE_COMPARE(a.size(), (Vector2i{15, 10}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 8*16);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 8*16);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructCompressedImplementationSpecificNotOwned() {
    auto&& instanceData = NotOwnedData[testCaseInstanceId()];
    setTestCaseDescription(instanceData.name);

    /* Format with autodetection */
    {
        char data[8*8];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{
            CompressedPixelStorage{}.setRowLength(16),
            GL::CompressedPixelFormat::RGBS3tcDxt1, {12, 8},
            instanceData.dataFlags, data, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.compressedStorage().rowLength(), 16);
        CORRADE_COMPARE(a.compressedFormat(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
        CORRADE_COMPARE(a.blockSize(), (Vector3i{4, 4, 1}));
        CORRADE_COMPARE(a.blockDataSize(), 8);
        CORRADE_COMPARE(a.size(), (Vector2i{12, 8}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 8*8);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 8*8);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }

    /* Manual block properties */
    {
        char data[6*12];
        int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
        ImageData2D a{
            CompressedPixelStorage{}.setRowLength(6),
            111, {3, 4, 5}, 12, {3, 8},
            instanceData.dataFlags, data, ImageFlag2D::Array, &state};

        CORRADE_COMPARE(a.dataFlags(), instanceData.dataFlags);
        CORRADE_VERIFY(a.isCompressed());
        CORRADE_COMPARE(a.flags(), ImageFlag2D::Array);
        CORRADE_COMPARE(a.compressedStorage().rowLength(), 6);
        CORRADE_COMPARE(a.compressedFormat(), compressedPixelFormatWrap(Vk::CompressedPixelFormat::Astc5x5x4RGBAF));
        CORRADE_COMPARE(a.blockSize(), (Vector3i{3, 4, 5}));
        CORRADE_COMPARE(a.blockDataSize(), 12);
        CORRADE_COMPARE(a.size(), (Vector2i{3, 8}));
        CORRADE_COMPARE(static_cast<const void*>(a.data().data()), data);
        CORRADE_COMPARE(a.data().size(), 6*12);
        if(instanceData.dataFlags & DataFlag::Mutable) {
            CORRADE_COMPARE(static_cast<const void*>(a.mutableData().data()), data);
            CORRADE_COMPARE(a.mutableData().size(), 6*12);
        }
        CORRADE_COMPARE(a.importerState(), &state);
    }
}

void ImageDataTest::constructGenericNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char data[4*4];

    Containers::String out;
    Error redirectError{&out};
    ImageData2D{PixelFormat::RGBA8Unorm, {1, 3}, DataFlag::Owned, data};
    ImageData2D{PixelStorage{}.setAlignment(1), PixelFormat::R16UI, {1, 3}, DataFlag::Owned, data};
    CORRADE_COMPARE(out,
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n"
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void ImageDataTest::constructImplementationSpecificNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char data[3*12];

    Containers::String out;
    Error redirectError{&out};
    ImageData2D{PixelStorage{}.setAlignment(1), Vk::PixelFormat::R32G32B32F, {1, 3}, DataFlag::Owned, data};
    ImageData2D{PixelStorage{}.setAlignment(1), GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, DataFlag::Owned, data};
    CORRADE_COMPARE(out,
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n"
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void ImageDataTest::constructCompressedGenericNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char data[8];

    Containers::String out;
    Error redirectError{&out};
    ImageData2D{
        CompressedPixelFormat::Bc1RGBAUnorm, {4, 4},
        DataFlag::Owned, data};
    ImageData2D{
        CompressedPixelStorage{}.setRowLength(4),
        CompressedPixelFormat::Bc1RGBAUnorm, {4, 4},
        DataFlag::Owned, data};
    CORRADE_COMPARE(out,
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n"
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void ImageDataTest::constructCompressedImplementationSpecificNotOwnedFlagOwned() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char data[8];

    Containers::String out;
    Error redirectError{&out};
    ImageData2D a{
        CompressedPixelStorage{}.setRowLength(4),
        GL::CompressedPixelFormat::RGBS3tcDxt1, {4, 4},
        DataFlag::Owned, data};
    CORRADE_COMPARE(out,
        "Trade::ImageData: can't construct a non-owned instance with Trade::DataFlag::Owned\n");
}

void ImageDataTest::constructUnknownImplementationSpecificPixelSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char data[1];

    Containers::String out;
    Error redirectError{&out};
    ImageData2D{pixelFormatWrap(0x666), {1, 1}, Containers::Array<char>{NoInit, 1}};
    ImageData2D{pixelFormatWrap(0x777), {1, 1}, DataFlags{}, data};
    CORRADE_COMPARE_AS(out,
        "Trade::ImageData: can't determine size of an implementation-specific pixel format 0x666, pass it explicitly\n"
        /* The next messages are printed because it cannot exit the
           construction from the middle of the member initializer list. It does
           so with non-graceful asserts tho and just one message is printed. */
        "Trade::ImageData: expected pixel size to be non-zero and less than 256 but got 0\n"

        "Trade::ImageData: can't determine size of an implementation-specific pixel format 0x777, pass it explicitly\n"
        "Trade::ImageData: expected pixel size to be non-zero and less than 256 but got 0\n",
        TestSuite::Compare::String);
}

void ImageDataTest::constructInvalidPixelSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    ImageData2D{PixelStorage{}, 666, 0, 0, {}, nullptr};
    ImageData2D{PixelStorage{}, 666, 0, 256, {}, nullptr};
    ImageData2D{PixelStorage{}, 666, 0, 0, {}, DataFlags{}, nullptr};
    ImageData2D{PixelStorage{}, 666, 0, 256, {}, DataFlags{}, nullptr};
    CORRADE_COMPARE_AS(out,
        "Trade::ImageData: expected pixel size to be non-zero and less than 256 but got 0\n"
        "Trade::ImageData: expected pixel size to be non-zero and less than 256 but got 256\n"
        "Trade::ImageData: expected pixel size to be non-zero and less than 256 but got 0\n"
        "Trade::ImageData: expected pixel size to be non-zero and less than 256 but got 256\n",
        TestSuite::Compare::String);
}

void ImageDataTest::constructInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};

    /* Doesn't consider alignment */
    ImageData2D{PixelFormat::RGB8Unorm, {1, 3}, Containers::Array<char>{3*3}};
    CORRADE_COMPARE(out, "Trade::ImageData: data too small, got 9 but expected at least 12 bytes\n");
}

void ImageDataTest::constructInvalidCubeMap() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    ImageData3D{PixelFormat::RGBA8Unorm, {3, 3, 5}, Containers::Array<char>{3*3*5*4}, ImageFlag3D::CubeMap};
    ImageData3D{PixelFormat::RGBA8Unorm, {3, 4, 6}, Containers::Array<char>{3*4*6*4}, ImageFlag3D::CubeMap};
    ImageData3D{PixelFormat::RGBA8Unorm, {3, 3, 17}, Containers::Array<char>{3*3*17*4}, ImageFlag3D::CubeMap |ImageFlag3D::Array};
    ImageData3D{PixelFormat::RGBA8Unorm, {4, 3, 18}, Containers::Array<char>{4*3*18*4}, ImageFlag3D::CubeMap |ImageFlag3D::Array};
    CORRADE_COMPARE(out,
        "Trade::ImageData: expected exactly 6 faces for a cube map, got 5\n"
        "Trade::ImageData: expected square faces for a cube map, got {3, 4}\n"
        "Trade::ImageData: expected a multiple of 6 faces for a cube map array, got 17\n"
        "Trade::ImageData: expected square faces for a cube map, got {4, 3}\n");
}

void ImageDataTest::constructCompressedUnknownImplementationSpecificBlockSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const char data[1]{};

    Containers::String out;
    Error redirectError{&out};
    ImageData2D{compressedPixelFormatWrap(0x666), {1, 1}, Containers::Array<char>{NoInit, 1}};
    ImageData2D{compressedPixelFormatWrap(0x777), {1, 1}, DataFlags{}, data};
    CORRADE_COMPARE_AS(out,
        "Trade::ImageData: can't determine block size of an implementation-specific pixel format 0x666, pass it explicitly\n"
        /* The next messages are printed because it cannot exit the
           construction from the middle of the member initializer list. It does
           so with non-graceful asserts tho and just one message is printed. */
        "Trade::ImageData: expected block size to be greater than zero and less than 256 but got {0, 0, 0}\n"

        "Trade::ImageData: can't determine block size of an implementation-specific pixel format 0x777, pass it explicitly\n"
        "Trade::ImageData: expected block size to be greater than zero and less than 256 but got {0, 0, 0}\n",
        TestSuite::Compare::String);
}

void ImageDataTest::constructCompressedInvalidBlockSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* This is all okay. In particular, it's also completely fine that the Y
       and Z size is more than 1 for 1D and 2D. */
    ImageData1D{CompressedPixelStorage{}, 666, {4, 5, 6}, 8, 1, Containers::Array<char>{NoInit, 8}};
    ImageData2D{CompressedPixelStorage{}, 666, {4, 5, 6}, 8, {1, 1}, Containers::Array<char>{NoInit, 8}};
    ImageData2D{CompressedPixelStorage{}
        .setCompressedBlockSize({4, 5, 6})
        .setCompressedBlockDataSize(8),
        666, {4, 5, 6}, 8, {1, 1}, Containers::Array<char>{NoInit, 8}};

    const char data[8]{};

    /* Tested mainly in ImageViewTest, here is just a subset to verify the same
       helper is used internally and a proper prefix is printed */
    Containers::String out;
    Error redirectError{&out};
    ImageData2D{CompressedPixelStorage{}, 666, {0, 0, 0}, 4, {1, 1}, nullptr};
    ImageData2D{CompressedPixelStorage{}, 666, {0, 0, 0}, 4, {1, 1}, {}, nullptr};
    ImageData2D{CompressedPixelStorage{}, 666, {4, 4, 4}, 0, {1, 1}, nullptr};
    ImageData2D{CompressedPixelStorage{}, 666, {4, 4, 4}, 0, {1, 1}, {}, nullptr};
    ImageData2D{CompressedPixelStorage{}
        .setCompressedBlockSize({5, 5, 5})
        .setCompressedBlockDataSize(8),
        666, {4, 4, 1}, 8, {1, 1}, Containers::Array<char>{NoInit, 8}};
    ImageData2D{CompressedPixelStorage{}
        .setCompressedBlockSize({5, 5, 5})
        .setCompressedBlockDataSize(8),
        666, {4, 4, 1}, 8, {1, 1}, {}, data};
    ImageData2D{CompressedPixelStorage{}
        .setCompressedBlockSize({4, 4, 1})
        .setCompressedBlockDataSize(4),
        666, {4, 4, 1}, 8, {1, 1}, Containers::Array<char>{NoInit, 8}};
    ImageData2D{CompressedPixelStorage{}
        .setCompressedBlockSize({4, 4, 1})
        .setCompressedBlockDataSize(4),
        666, {4, 4, 1}, 8, {1, 1}, {}, data};
    CORRADE_COMPARE_AS(out,
        "Trade::ImageData: expected block size to be greater than zero and less than 256 but got {0, 0, 0}\n"
        "Trade::ImageData: expected block size to be greater than zero and less than 256 but got {0, 0, 0}\n"
        "Trade::ImageData: expected block data size to be non-zero and less than 256 but got 0\n"
        "Trade::ImageData: expected block data size to be non-zero and less than 256 but got 0\n"
        "Trade::ImageData: expected pixel storage block size to be either not set at all or equal to {4, 4, 1} but got {5, 5, 5}\n"
        "Trade::ImageData: expected pixel storage block size to be either not set at all or equal to {4, 4, 1} but got {5, 5, 5}\n"
        "Trade::ImageData: expected pixel storage block data size to be either not set at all or equal to 8 but got 4\n"
        "Trade::ImageData: expected pixel storage block data size to be either not set at all or equal to 8 but got 4\n",
        TestSuite::Compare::String);
}

void ImageDataTest::constructCompressedInvalidSize() {
    CORRADE_SKIP_IF_NO_ASSERT();

    /* Too small for given format */
    {
        Containers::String out;
        Error redirectError{&out};
        ImageData2D{CompressedPixelFormat::Bc2RGBAUnorm, {4, 4}, Containers::Array<char>{15}};
        CORRADE_COMPARE(out, "Trade::ImageData: data too small, got 15 but expected at least 16 bytes\n");

    /* Size should be rounded up even if the image size is not full block */
    } {
        Containers::String out;
        Error redirectError{&out};
        ImageData2D{CompressedPixelFormat::Bc2RGBAUnorm, {2, 2}, Containers::Array<char>{15}};
        CORRADE_COMPARE(out, "Trade::ImageData: data too small, got 15 but expected at least 16 bytes\n");
    }
}

void ImageDataTest::constructCompressedInvalidCubeMap() {
    CORRADE_SKIP_IF_NO_ASSERT();

    Containers::String out;
    Error redirectError{&out};
    ImageData3D{CompressedPixelFormat::Bc1RGBAUnorm, {3, 3, 5}, Containers::Array<char>{8*5}, ImageFlag3D::CubeMap};
    ImageData3D{CompressedPixelFormat::Bc1RGBAUnorm, {3, 4, 6}, Containers::Array<char>{8*6}, ImageFlag3D::CubeMap};
    ImageData3D{CompressedPixelFormat::Bc1RGBAUnorm, {3, 3, 17}, Containers::Array<char>{8*17}, ImageFlag3D::CubeMap |ImageFlag3D::Array};
    ImageData3D{CompressedPixelFormat::Bc1RGBAUnorm, {4, 3, 18}, Containers::Array<char>{8*18}, ImageFlag3D::CubeMap |ImageFlag3D::Array};
    CORRADE_COMPARE(out,
        "Trade::ImageData: expected exactly 6 faces for a cube map, got 5\n"
        "Trade::ImageData: expected square faces for a cube map, got {3, 4}\n"
        "Trade::ImageData: expected a multiple of 6 faces for a cube map array, got 17\n"
        "Trade::ImageData: expected square faces for a cube map, got {4, 3}\n");
}

void ImageDataTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<ImageData2D>{});
    CORRADE_VERIFY(!std::is_copy_assignable<ImageData2D>{});
}

void ImageDataTest::constructMoveGeneric() {
    auto data = new char[3*16];
    int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    ImageData2D a{PixelStorage{}.setAlignment(1),
        PixelFormat::RGBA32F, {1, 3}, Containers::Array<char>{data, 3*16}, ImageFlag2D::Array, &state};
    ImageData2D b(Utility::move(a));

    CORRADE_COMPARE(a.data(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!b.isCompressed());
    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), PixelFormat::RGBA32F);
    CORRADE_COMPARE(b.formatExtra(), 0);
    CORRADE_COMPARE(b.pixelSize(), 16);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(b.data().size(), 3*16);
    CORRADE_COMPARE(b.importerState(), &state);

    auto data2 = new char[24];
    ImageData2D c{PixelFormat::R8I, {2, 6}, Containers::Array<char>{data2, 24}};
    c = Utility::move(b);

    CORRADE_COMPARE(b.data(), static_cast<const void*>(data2));
    CORRADE_COMPARE(b.size(), (Vector2i{2, 6}));

    CORRADE_COMPARE(c.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!c.isCompressed());
    CORRADE_COMPARE(c.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(c.storage().alignment(), 1);
    CORRADE_COMPARE(c.format(), PixelFormat::RGBA32F);
    CORRADE_COMPARE(c.formatExtra(), 0);
    CORRADE_COMPARE(c.pixelSize(), 16);
    CORRADE_COMPARE(c.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(c.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(c.data().size(), 3*16);
    CORRADE_COMPARE(c.importerState(), &state);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<ImageData2D>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<ImageData2D>::value);
}

void ImageDataTest::constructMoveImplementationSpecific() {
    auto data = new char[3*6];
    int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    ImageData2D a{PixelStorage{}.setAlignment(1),
        GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}, ImageFlag2D::Array, &state};
    ImageData2D b(Utility::move(a));

    CORRADE_COMPARE(a.data(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!b.isCompressed());
    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(b.formatExtra(), 1337);
    CORRADE_COMPARE(b.pixelSize(), 6);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(b.data().size(), 3*6);
    CORRADE_COMPARE(b.importerState(), &state);

    auto data2 = new char[12*4*2];
    ImageData2D c{PixelStorage{},
        1, 2, 8, {2, 6}, Containers::Array<char>{data2, 12*4*2}};
    c = Utility::move(b);

    CORRADE_COMPARE(b.data(), static_cast<const void*>(data2));
    CORRADE_COMPARE(b.size(), Vector2i(2, 6));

    CORRADE_COMPARE(c.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!c.isCompressed());
    CORRADE_COMPARE(c.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(c.storage().alignment(), 1);
    CORRADE_COMPARE(c.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(c.formatExtra(), 1337);
    CORRADE_COMPARE(c.pixelSize(), 6);
    CORRADE_COMPARE(c.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(c.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(c.data().size(), 3*6);
    CORRADE_COMPARE(c.importerState(), &state);
}

void ImageDataTest::constructMoveCompressedGeneric() {
    auto data = new char[8*16];
    int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    ImageData2D a{
        CompressedPixelStorage{}.setRowLength(20),
        CompressedPixelFormat::Astc5x5x4RGBAF, {15, 10},
        Containers::Array<char>{data, 8*16}, ImageFlag2D::Array, &state};
    ImageData2D b{Utility::move(a)};

    CORRADE_COMPARE(a.data(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(b.isCompressed());
    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.compressedStorage().rowLength(), 20);
    CORRADE_COMPARE(b.compressedFormat(), CompressedPixelFormat::Astc5x5x4RGBAF);
    CORRADE_COMPARE(b.blockSize(), (Vector3i{5, 5, 4}));
    CORRADE_COMPARE(b.blockDataSize(), 16);
    CORRADE_COMPARE(b.size(), (Vector2i{15, 10}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(b.data().size(), 8*16);
    CORRADE_COMPARE(b.importerState(), &state);

    auto data2 = new char[16];
    ImageData2D c{CompressedPixelFormat::Bc1RGBAUnorm, {8, 4}, Containers::Array<char>{data2, 16}};
    c = Utility::move(b);

    CORRADE_COMPARE(b.data(), static_cast<const void*>(data2));
    CORRADE_COMPARE(b.size(), (Vector2i{8, 4}));

    CORRADE_COMPARE(c.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(c.isCompressed());
    CORRADE_COMPARE(c.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(c.compressedStorage().rowLength(), 20);
    CORRADE_COMPARE(c.compressedFormat(), CompressedPixelFormat::Astc5x5x4RGBAF);
    CORRADE_COMPARE(c.blockSize(), (Vector3i{5, 5, 4}));
    CORRADE_COMPARE(c.blockDataSize(), 16);
    CORRADE_COMPARE(c.size(), (Vector2i{15, 10}));
    CORRADE_COMPARE(c.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(c.data().size(), 8*16);
    CORRADE_COMPARE(c.importerState(), &state);
}

void ImageDataTest::constructMoveCompressedImplementationSpecific() {
    auto data = new char[8*16];
    int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    ImageData2D a{
        CompressedPixelStorage{}.setRowLength(20),
        Vk::CompressedPixelFormat::Astc5x5x4RGBAF, {15, 10},
        Containers::Array<char>{data, 8*16}, ImageFlag2D::Array, &state};
    ImageData2D b{Utility::move(a)};

    CORRADE_COMPARE(a.data(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(b.isCompressed());
    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.compressedStorage().rowLength(), 20);
    CORRADE_COMPARE(b.compressedFormat(), compressedPixelFormatWrap(Vk::CompressedPixelFormat::Astc5x5x4RGBAF));
    CORRADE_COMPARE(b.blockSize(), (Vector3i{5, 5, 4}));
    CORRADE_COMPARE(b.blockDataSize(), 16);
    CORRADE_COMPARE(b.size(), (Vector2i{15, 10}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(b.data().size(), 8*16);
    CORRADE_COMPARE(b.importerState(), &state);

    auto data2 = new char[16];
    ImageData2D c{CompressedPixelFormat::Bc1RGBAUnorm, {8, 4}, Containers::Array<char>{data2, 16}};
    c = Utility::move(b);

    CORRADE_COMPARE(b.data(), static_cast<const void*>(data2));
    CORRADE_COMPARE(b.size(), (Vector2i{8, 4}));

    CORRADE_COMPARE(c.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(c.isCompressed());
    CORRADE_COMPARE(c.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(c.compressedStorage().rowLength(), 20);
    CORRADE_COMPARE(c.compressedFormat(), compressedPixelFormatWrap(Vk::CompressedPixelFormat::Astc5x5x4RGBAF));
    CORRADE_COMPARE(c.blockSize(), (Vector3i{5, 5, 4}));
    CORRADE_COMPARE(c.blockDataSize(), 16);
    CORRADE_COMPARE(c.size(), (Vector2i{15, 10}));
    CORRADE_COMPARE(c.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(c.data().size(), 8*16);
    CORRADE_COMPARE(c.importerState(), &state);
}

void ImageDataTest::constructMoveAttachState() {
    auto data = new char[3*6];
    /* GCC 11 pointlessly complains that "maybe uninitialized" w/o the {} */
    int stateOld{}, stateNew{};
    ImageData2D a{PixelStorage{}.setAlignment(1),
        GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}, ImageFlag2D::Array, &stateOld};
    ImageData2D b{Utility::move(a), &stateNew};

    CORRADE_COMPARE(a.data(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!b.isCompressed());
    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(b.formatExtra(), 1337);
    CORRADE_COMPARE(b.pixelSize(), 6);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(b.data().size(), 3*6);
    CORRADE_COMPARE(b.importerState(), &stateNew);
}

void ImageDataTest::constructMoveCompressedAttachState() {
    auto data = new char[8*8];
    /* GCC 11 pointlessly complains that "maybe uninitialized" w/o the {} */
    int stateOld{}, stateNew{};
    ImageData2D a{
        CompressedPixelStorage{}.setRowLength(16),
        GL::CompressedPixelFormat::RGBS3tcDxt1, {12, 8},
        Containers::Array<char>{data, 8*8}, ImageFlag2D::Array, &stateOld};
    ImageData2D b{Utility::move(a), &stateNew};

    CORRADE_COMPARE(a.data(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i{});

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(b.isCompressed());
    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.compressedStorage().rowLength(), 16);
    CORRADE_COMPARE(b.compressedFormat(), compressedPixelFormatWrap(GL::CompressedPixelFormat::RGBS3tcDxt1));
    CORRADE_COMPARE(b.blockSize(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE(b.blockDataSize(), 8);
    CORRADE_COMPARE(b.size(), (Vector2i{12, 8}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(b.data().size(), 8*8);
    CORRADE_COMPARE(b.importerState(), &stateNew);
}

void ImageDataTest::moveCompressedToUncompressed() {
    auto data = new char[8*16];
    int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    ImageData2D a{
        CompressedPixelStorage{}
            .setSkip({4, 4, 1})
            .setRowLength(8)
            .setImageHeight(8)
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16),
        CompressedPixelFormat::Bc3RGBAUnorm, {4, 4}, Containers::Array<char>{data, 8*16}, ImageFlag2D::Array, &state};

    auto data2 = new char[24];
    int state2{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    ImageData2D b{
        PixelStorage{}
            .setAlignment(2)
            .setRowLength(2)
            .setImageHeight(6)
            .setSkip({0, 1, 0}),
        PixelFormat::R8I, 0x12345678, 1, {2, 5}, Containers::Array<char>{data2, 24}, ImageFlag2D(0x80), &state2};

    /* The operation should swap the contents completely, not just partially
       because one is compressed and the other not */
    b = Utility::move(a);

    CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!a.isCompressed());
    CORRADE_COMPARE(a.flags(), ImageFlag2D(0x80));
    CORRADE_COMPARE(a.storage().alignment(), 2);
    CORRADE_COMPARE(a.storage().rowLength(), 2);
    CORRADE_COMPARE(a.storage().imageHeight(), 6);
    CORRADE_COMPARE(a.storage().skip(), (Vector3i{0, 1, 0}));
    CORRADE_COMPARE(a.format(), PixelFormat::R8I);
    CORRADE_COMPARE(a.formatExtra(), 0x12345678);
    CORRADE_COMPARE(a.pixelSize(), 1);
    CORRADE_COMPARE(a.size(), (Vector2i{2, 5}));
    CORRADE_COMPARE(a.data(), static_cast<const void*>(data2));
    CORRADE_COMPARE(a.data().size(), 24);
    CORRADE_COMPARE(a.importerState(), &state2);

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(b.isCompressed());
    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.compressedStorage().rowLength(), 8);
    CORRADE_COMPARE(b.compressedStorage().imageHeight(), 8);
    CORRADE_COMPARE(b.compressedStorage().skip(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE(b.compressedStorage().compressedBlockSize(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE(b.compressedStorage().compressedBlockDataSize(), 16);
    CORRADE_COMPARE(b.compressedFormat(), CompressedPixelFormat::Bc3RGBAUnorm);
    CORRADE_COMPARE(b.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(b.data().size(), 8*16);
    CORRADE_COMPARE(b.importerState(), &state);
}

void ImageDataTest::moveUncompressedToCompressed() {
    auto data = new char[24];
    int state{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    ImageData2D a{
        PixelStorage{}
            .setAlignment(2)
            .setRowLength(2)
            .setImageHeight(6)
            .setSkip({0, 1, 0}),
        PixelFormat::R8I, 0x12345678, 1, {2, 5}, Containers::Array<char>{data, 24}, ImageFlag2D::Array, &state};

    auto data2 = new char[8*16];
    int state2{}; /* GCC 11 complains that "maybe uninitialized" w/o the {} */
    ImageData2D b{
        CompressedPixelStorage{}
            .setSkip({4, 4, 1})
            .setRowLength(8)
            .setImageHeight(8)
            .setCompressedBlockSize({4, 4, 1})
            .setCompressedBlockDataSize(16),
        CompressedPixelFormat::Bc3RGBAUnorm, {4, 4}, Containers::Array<char>{data2, 8*16}, ImageFlag2D(0x80), &state2};

    /* The operation should swap the contents completely, not just partially
       because one is compressed and the other not */
    b = Utility::move(a);

    CORRADE_COMPARE(a.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(a.isCompressed());
    CORRADE_COMPARE(a.flags(), ImageFlag2D(0x80));
    CORRADE_COMPARE(a.compressedStorage().rowLength(), 8);
    CORRADE_COMPARE(a.compressedStorage().imageHeight(), 8);
    CORRADE_COMPARE(a.compressedStorage().skip(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE(a.compressedStorage().compressedBlockSize(), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE(a.compressedStorage().compressedBlockDataSize(), 16);
    CORRADE_COMPARE(a.compressedFormat(), CompressedPixelFormat::Bc3RGBAUnorm);
    CORRADE_COMPARE(a.size(), (Vector2i{4, 4}));
    CORRADE_COMPARE(a.data(), static_cast<const void*>(data2));
    CORRADE_COMPARE(a.data().size(), 8*16);
    CORRADE_COMPARE(a.importerState(), &state2);

    CORRADE_COMPARE(b.dataFlags(), DataFlag::Owned|DataFlag::Mutable);
    CORRADE_VERIFY(!b.isCompressed());
    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.storage().alignment(), 2);
    CORRADE_COMPARE(b.storage().rowLength(), 2);
    CORRADE_COMPARE(b.storage().imageHeight(), 6);
    CORRADE_COMPARE(b.storage().skip(), (Vector3i{0, 1, 0}));
    CORRADE_COMPARE(b.format(), PixelFormat::R8I);
    CORRADE_COMPARE(b.formatExtra(), 0x12345678);
    CORRADE_COMPARE(b.pixelSize(), 1);
    CORRADE_COMPARE(b.size(), (Vector2i{2, 5}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(b.data().size(), 24);
    CORRADE_COMPARE(b.importerState(), &state);
}

void ImageDataTest::propertiesInvalid() {
    CORRADE_SKIP_IF_NO_ASSERT();

    char data[8]{};
    ImageData2D uncompressed{PixelFormat::RGBA8Unorm, {1, 1}, DataFlag::Mutable, data};
    ImageData2D compressed{CompressedPixelFormat::Bc1RGBUnorm, {1, 1}, DataFlag::Mutable, data};

    Containers::String out;
    Error redirectError{&out};
    uncompressed.compressedStorage();
    uncompressed.compressedFormat();
    uncompressed.blockSize();
    uncompressed.blockDataSize();
    uncompressed.compressedDataProperties();
    compressed.storage();
    compressed.format();
    compressed.formatExtra();
    compressed.pixelSize();
    compressed.dataProperties();
    compressed.pixels();
    /* pixels<T>() calls non-templated pixels(), so assume there it will blow
       up correctly as well (can't test because it asserts inside arrayCast()
       due to zero stride) */
    compressed.mutablePixels();
    /* Same for mutablePixels<T>() */
    CORRADE_COMPARE_AS(out,
        "Trade::ImageData::compressedStorage(): the image is not compressed\n"
        "Trade::ImageData::compressedFormat(): the image is not compressed\n"
        "Trade::ImageData::blockSize(): the image is not compressed\n"
        "Trade::ImageData::blockDataSize(): the image is not compressed\n"
        "Trade::ImageData::compressedDataProperties(): the image is not compressed\n"
        "Trade::ImageData::storage(): the image is compressed\n"
        "Trade::ImageData::format(): the image is compressed\n"
        "Trade::ImageData::formatExtra(): the image is compressed\n"
        "Trade::ImageData::pixelSize(): the image is compressed\n"
        "Trade::ImageData::dataProperties(): the image is compressed\n"
        "Trade::ImageData::pixels(): the image is compressed\n"
        "Trade::ImageData::mutablePixels(): the image is compressed\n",
        TestSuite::Compare::String);
}

template<class T> void ImageDataTest::toViewGeneric() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    auto data = new char[3*4];
    typename MutabilityTraits<T>::ImageType a{PixelStorage{}.setAlignment(1),
        PixelFormat::RG16I, {1, 3}, Containers::Array<char>{data, 3*4}, ImageFlag2D::Array};
    ImageView<2, T> b = a;

    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), PixelFormat::RG16I);
    CORRADE_COMPARE(b.formatExtra(), 0);
    CORRADE_COMPARE(b.pixelSize(), 4);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
}

template<class T> void ImageDataTest::toViewImplementationSpecific() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    auto data = new char[3*6];
    typename MutabilityTraits<T>::ImageType a{PixelStorage{}.setAlignment(1),
        GL::PixelFormat::RGB, GL::PixelType::UnsignedShort, {1, 3}, Containers::Array<char>{data, 3*6}, ImageFlag2D::Array};
    ImageView<2, T> b = a;

    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.storage().alignment(), 1);
    CORRADE_COMPARE(b.format(), pixelFormatWrap(GL::PixelFormat::RGB));
    CORRADE_COMPARE(b.formatExtra(), 1337);
    CORRADE_COMPARE(b.pixelSize(), 6);
    CORRADE_COMPARE(b.size(), (Vector2i{1, 3}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
}

template<class T> void ImageDataTest::toViewCompressedGeneric() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    auto data = new char[8*16];
    typename MutabilityTraits<T>::ImageType a{
        CompressedPixelStorage{}.setRowLength(20),
        CompressedPixelFormat::Astc5x5x4RGBAF, {15, 10},
        Containers::Array<char>{data, 8*16}, ImageFlag2D::Array};
    CompressedImageView<2, T> b = a;

    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.storage().rowLength(), 20);
    CORRADE_COMPARE(b.format(), CompressedPixelFormat::Astc5x5x4RGBAF);
    CORRADE_COMPARE(b.blockSize(), (Vector3i{5, 5, 4}));
    CORRADE_COMPARE(b.blockDataSize(), 16);
    CORRADE_COMPARE(b.size(), (Vector2i{15, 10}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(b.data().size(), 8*16);
}

template<class T> void ImageDataTest::toViewCompressedImplementationSpecific() {
    setTestCaseTemplateName(MutabilityTraits<T>::name());

    auto data = new char[8*16];
    typename MutabilityTraits<T>::ImageType a{
        CompressedPixelStorage{}.setRowLength(20),
        Vk::CompressedPixelFormat::Astc5x5x4RGBAF, {15, 10},
        Containers::Array<char>{data, 8*16}, ImageFlag2D::Array};
    CompressedImageView<2, T> b = a;

    CORRADE_COMPARE(b.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(b.storage().rowLength(), 20);
    CORRADE_COMPARE(b.format(), compressedPixelFormatWrap(Vk::CompressedPixelFormat::Astc5x5x4RGBAF));
    CORRADE_COMPARE(b.blockSize(), (Vector3i{5, 5, 4}));
    CORRADE_COMPARE(b.blockDataSize(), 16);
    CORRADE_COMPARE(b.size(), (Vector2i{15, 10}));
    CORRADE_COMPARE(b.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(b.data().size(), 8*16);
}

void ImageDataTest::data() {
    auto data = new char[4*4];
    ImageData2D a{PixelFormat::RGBA8Unorm, {1, 3}, Containers::Array<char>{data, 4*4}};
    const ImageData2D& ca = a;
    CORRADE_COMPARE(a.data(), static_cast<const void*>(data));
    CORRADE_COMPARE(ca.data(), static_cast<const void*>(data));
}

void ImageDataTest::dataRvalue() {
    auto data = new char[4*4];
    Containers::Array<char> released = ImageData2D{PixelFormat::RGBA8Unorm,
        {1, 3}, Containers::Array<char>{data, 4*4}}.data();
    CORRADE_COMPARE(released.data(), static_cast<const void*>(data));
}

void ImageDataTest::mutableAccessNotAllowed() {
    CORRADE_SKIP_IF_NO_ASSERT();

    const char data[4*4]{};
    ImageData2D uncompressed{PixelFormat::RGBA8Unorm, {2, 2}, DataFlags{}, data};
    ImageData2D compressed{CompressedPixelFormat::Bc1RGBAUnorm, {2, 2}, DataFlags{}, data};

    Containers::String out;
    Error redirectError{&out};
    uncompressed.mutableData();
    uncompressed.mutablePixels();
    /* Can't do just MutableImageView2D(a) because the compiler then treats it
       as a function. Can't do MutableImageView2D{a} because that doesn't work
       on old Clang. So it's this mess, then. Sigh. */
    auto b = MutableImageView2D(uncompressed);
    static_cast<void>(b);
    auto c = MutableCompressedImageView2D(compressed);
    static_cast<void>(c);
    /* a.mutablePixels<T>() calls non-templated mutablePixels(), so assume
       there it will blow up correctly as well (can't test because it asserts
       inside arrayCast() due to zero stride) */
    CORRADE_COMPARE(out,
        "Trade::ImageData::mutableData(): the image is not mutable\n"
        "Trade::ImageData::mutablePixels(): the image is not mutable\n"
        "Trade::ImageData: the image is not mutable\n"
        "Trade::ImageData: the image is not mutable\n");
}

void ImageDataTest::dataProperties() {
    ImageData3D image{
        PixelStorage{}
            .setAlignment(8)
            .setSkip({3, 2, 1}),
        PixelFormat::R8Unorm, {2, 4, 6},
        Containers::Array<char>{224}};
    CORRADE_COMPARE(image.dataProperties(),
        (std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>>{{3, 16, 32}, {8, 4, 6}}));
}

void ImageDataTest::dataPropertiesCompressed() {
    ImageData3D image{
        CompressedPixelStorage{}
            .setRowLength(12)
            .setImageHeight(8)
            .setSkip({8, 4, 4}),
        CompressedPixelFormat::Bc1RGBAUnorm, {2, 3, 3},
        Containers::Array<char>{NoInit, 336}};
    CORRADE_COMPARE(image.compressedDataProperties(),
        (std::pair<Math::Vector3<std::size_t>, Math::Vector3<std::size_t>>{{16, 24, 192}, {3, 2, 3}}));
}

void ImageDataTest::release() {
    char data[] = {'b', 'e', 'e', 'r'};
    Trade::ImageData2D a{PixelFormat::RGBA8Unorm, {1, 1}, Containers::Array<char>{data, 4}};
    const char* const pointer = a.release().release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i());
}

void ImageDataTest::releaseCompressed() {
    char data[8];
    Trade::ImageData2D a{CompressedPixelFormat::Bc1RGBAUnorm, {4, 4}, Containers::Array<char>{data, 8}};
    const char* const pointer = a.release().release();

    CORRADE_COMPARE(pointer, data);
    CORRADE_COMPARE(a.data(), static_cast<const void*>(nullptr));
    CORRADE_COMPARE(a.size(), Vector2i());
}

void ImageDataTest::pixels1D() {
    ImageData1D image{
        PixelStorage{}
            .setAlignment(1) /** @todo alignment 4 expects 17 bytes. what */
            .setSkip({3, 0, 0}),
        PixelFormat::RGB8Unorm, 2,
        Containers::Array<char>{15}};
    const ImageData1D& cimage = image;

    /* Full test is in ImageTest, this is just a sanity check */

    {
        Containers::StridedArrayView1D<Color3ub> pixels = image.mutablePixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), 2);
        CORRADE_COMPARE(pixels.stride(), 3);
        CORRADE_COMPARE(pixels.data(), image.data() + 3*3);
    } {
        Containers::StridedArrayView1D<const Color3ub> pixels = cimage.pixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), 2);
        CORRADE_COMPARE(pixels.stride(), 3);
        CORRADE_COMPARE(pixels.data(), cimage.data() + 3*3);
    }
}

void ImageDataTest::pixels2D() {
    ImageData2D image{
        PixelStorage{}
            .setAlignment(4)
            .setSkip({3, 2, 0})
            .setRowLength(6),
        PixelFormat::RGB8Unorm, {2, 4},
        Containers::Array<char>{120}};
    const ImageData2D& cimage = image;

    /* Full test is in ImageTest, this is just a sanity check */

    {
        Containers::StridedArrayView2D<Color3ub> pixels = image.mutablePixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), (Containers::Size2D{4, 2}));
        CORRADE_COMPARE(pixels.stride(), (Containers::Stride2D{20, 3}));
        CORRADE_COMPARE(pixels.data(), image.data() + 2*20 + 3*3);
    } {
        Containers::StridedArrayView2D<const Color3ub> pixels = cimage.pixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), (Containers::Size2D{4, 2}));
        CORRADE_COMPARE(pixels.stride(), (Containers::Stride2D{20, 3}));
        CORRADE_COMPARE(pixels.data(), cimage.data() + 2*20 + 3*3);
    }
}

void ImageDataTest::pixels3D() {
    ImageData3D image{
        PixelStorage{}
            .setAlignment(4)
            .setSkip({3, 2, 1})
            .setRowLength(6)
            .setImageHeight(7),
        PixelFormat::RGB8Unorm, {2, 4, 3},
        Containers::Array<char>{560}};
    const ImageData3D& cimage = image;

    /* Full test is in ImageTest, this is just a sanity check */

    {
        Containers::StridedArrayView3D<Color3ub> pixels = image.mutablePixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), (Containers::Size3D{3, 4, 2}));
        CORRADE_COMPARE(pixels.stride(), (Containers::Stride3D{140, 20, 3}));
        CORRADE_COMPARE(pixels.data(), image.data() + 140 + 2*20 + 3*3);
    } {
        Containers::StridedArrayView3D<const Color3ub> pixels = cimage.pixels<Color3ub>();
        CORRADE_COMPARE(pixels.size(), (Containers::Size3D{3, 4, 2}));
        CORRADE_COMPARE(pixels.stride(), (Containers::Stride3D{140, 20, 3}));
        CORRADE_COMPARE(pixels.data(), cimage.data() + 140 + 2*20 + 3*3);
    }
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::ImageDataTest)
