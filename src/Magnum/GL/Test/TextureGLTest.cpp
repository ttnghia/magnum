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

#include <Corrade/TestSuite/Compare/Container.h>

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/BufferImage.h"
#endif
#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/ImageFormat.h"
#endif
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/Texture.h"
#include "Magnum/GL/TextureFormat.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#include "Magnum/GL/TextureArray.h"
#include "Magnum/GL/CubeMapTexture.h"
#include "Magnum/GL/CubeMapTextureArray.h"
#endif

namespace Magnum { namespace GL { namespace Test { namespace {

struct TextureGLTest: OpenGLTester {
    explicit TextureGLTest();

    #ifndef MAGNUM_TARGET_GLES
    void compressedBlockSize1D();
    void compressedBlockSize2D();
    void compressedBlockSize3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void construct1D();
    #endif
    void construct2D();
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void construct3D();
    #endif

    void constructMove();

    #ifndef MAGNUM_TARGET_GLES
    void wrap1D();
    #endif
    void wrap2D();
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void wrap3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void wrapCreateIfNotAlready1D();
    #endif
    void wrapCreateIfNotAlready2D();
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void wrapCreateIfNotAlready3D();
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    #ifndef MAGNUM_TARGET_GLES
    void label1D();
    #endif
    void label2D();
    void label3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void bind1D();
    #endif
    void bind2D();
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void bind3D();
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    void bindImage1D();
    #endif
    void bindImage2D();
    void bindImage3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    template<class T> void sampling1D();
    #endif
    template<class T> void sampling2D();
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    template<class T> void sampling3D();
    #endif

    #ifndef MAGNUM_TARGET_WEBGL
    #ifndef MAGNUM_TARGET_GLES
    void samplingSrgbDecode1D();
    #endif
    void samplingSrgbDecode2D();
    void samplingSrgbDecode3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_GLES
    void samplingSwizzle1D();
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    void samplingSwizzle2D();
    void samplingSwizzle3D();
    #endif
    #endif
    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingMaxLevel2D();
    void samplingMaxLevel3D();
    void samplingCompare2D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void samplingBorderInteger1D();
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingBorderInteger2D();
    void samplingBorderInteger3D();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void samplingDepthStencilMode1D();
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    void samplingDepthStencilMode2D();
    void samplingDepthStencilMode3D();
    #endif
    #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
    void samplingBorder2D();
    void samplingBorder3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void storage1D();
    #endif
    void storage2D();
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void storage3D();
    #endif

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifndef MAGNUM_TARGET_GLES
    void view1D();
    #endif
    void view2D();
    void view2DOnArray();
    void view2DOnCubeMap();
    void view2DOnCubeMapArray();
    void view3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void image1D();
    void image1DBuffer();
    void image1DQueryView();
    void subImage1D();
    void subImage1DBuffer();
    void subImage1DQuery();
    void subImage1DQueryView();
    void subImage1DQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */

    void compressedImage1D();
    void compressedImage1DBuffer();
    void compressedImage1DQueryView();
    void compressedSubImage1D();
    void compressedSubImage1DBuffer();
    void compressedSubImage1DQuery();
    void compressedSubImage1DQueryView();
    void compressedSubImage1DQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */
    #endif

    void image2D();
    #ifndef MAGNUM_TARGET_GLES2
    void image2DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void image2DQueryView();
    #endif
    void subImage2D();
    #ifndef MAGNUM_TARGET_GLES2
    void subImage2DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void subImage2DQuery();
    void subImage2DQueryView();
    void subImage2DQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */
    #endif

    void compressedImage2D();
    #ifndef MAGNUM_TARGET_GLES2
    void compressedImage2DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void compressedImage2DQueryView();
    #endif
    void compressedSubImage2D();
    #ifndef MAGNUM_TARGET_GLES2
    void compressedSubImage2DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void compressedSubImage2DQuery();
    void compressedSubImage2DQueryView();
    void compressedSubImage2DQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */
    #endif

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void image3D();
    #ifndef MAGNUM_TARGET_GLES2
    void image3DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void image3DQueryView();
    #endif
    void subImage3D();
    #ifndef MAGNUM_TARGET_GLES2
    void subImage3DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void subImage3DQuery();
    void subImage3DQueryView();
    void subImage3DQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */
    #endif

    void compressedImage3D();
    #ifndef MAGNUM_TARGET_GLES2
    void compressedImage3DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void compressedImage3DQueryView();
    #endif
    void compressedSubImage3D();
    #ifndef MAGNUM_TARGET_GLES2
    void compressedSubImage3DBuffer();
    #endif
    #ifndef MAGNUM_TARGET_GLES
    void compressedSubImage3DQuery();
    void compressedSubImage3DQueryView();
    void compressedSubImage3DQueryBuffer();
    /* View query assertions tested in AbstractTextureGLTest */
    #endif
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void generateMipmap1D();
    #endif
    void generateMipmap2D();
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void generateMipmap3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void invalidateImage1D();
    #endif
    void invalidateImage2D();
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void invalidateImage3D();
    #endif

    #ifndef MAGNUM_TARGET_GLES
    void invalidateSubImage1D();
    #endif
    void invalidateSubImage2D();
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void invalidateSubImage3D();
    #endif

    void srgbStorage();
    void srgbAlphaStorage();
};

struct GenericSampler {
    typedef Magnum::SamplerFilter Filter;
    typedef Magnum::SamplerMipmap Mipmap;
    typedef Magnum::SamplerWrapping Wrapping;
};
struct GLSampler {
    typedef GL::SamplerFilter Filter;
    typedef GL::SamplerMipmap Mipmap;
    typedef GL::SamplerWrapping Wrapping;
};

#ifndef MAGNUM_TARGET_GLES
constexpr UnsignedByte Data1D[]{
    0, 0, 0, 0,
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    PixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} PixelStorage1DData[]{
    {"default pixel storage",
        Containers::arrayView(Data1D).exceptPrefix(4), {},
        Containers::arrayView(Data1D).exceptPrefix(4), 0},
    {"skip X",
        Containers::arrayView(Data1D).exceptPrefix(4), PixelStorage{}.setSkip({1, 0, 0}),
        Containers::arrayView(Data1D), 4}};
#endif

constexpr UnsignedByte Data2D[]{
    0, 0, 0, 0, 0, 0, 0, 0,
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    PixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} PixelStorage2DData[]{
    {"default pixel storage",
        Containers::arrayView(Data2D).exceptPrefix(8), {},
        Containers::arrayView(Data2D).exceptPrefix(8), 0},
    #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
    {"skip Y",
        Containers::arrayView(Data2D).exceptPrefix(8), PixelStorage{}.setSkip({0, 1, 0}),
        Containers::arrayView(Data2D), 8}
    #endif
};

/* Just 4x8 0x00 - 0x3f compressed using RGBA DXT3 by the driver */
constexpr UnsignedByte CompressedData2D[]{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
      0,  17, 17,  34,  34,  51,  51,  67,
    232,  57,  0,   0, 213, 255, 170,   2
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    CompressedPixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} CompressedPixelStorage2DData[]{
    {"default pixel storage",
        Containers::arrayView(CompressedData2D).exceptPrefix(16),
        {},
        Containers::arrayView(CompressedData2D).exceptPrefix(16), 0},
    #ifndef MAGNUM_TARGET_GLES
    {"skip Y",
        Containers::arrayView(CompressedData2D).exceptPrefix(16),
        CompressedPixelStorage{}
            .setSkip({0, 4, 0}),
        Containers::arrayView(CompressedData2D), 16}
    #endif
};

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
constexpr UnsignedByte Data3D[]{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,

    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,

    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    PixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} PixelStorage3DData[]{
    {"default pixel storage",
        Containers::arrayView(Data3D).exceptPrefix(16), {},
        Containers::arrayView(Data3D).exceptPrefix(16), 0},
    #if !defined(MAGNUM_TARGET_GLES2) || !defined(MAGNUM_TARGET_WEBGL)
    {"skip Z",
        Containers::arrayView(Data3D).exceptPrefix(16), PixelStorage{}.setSkip({0, 0, 1}),
        Containers::arrayView(Data3D), 16}
    #endif
};
#endif

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
/* Just 4x4x8 0x00 - 0xff compressed using RGBA BPTC Unorm by the driver */
constexpr UnsignedByte CompressedData3D[]{
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    144, 224, 128,   3,  80,   0, 129, 170,
     84, 253,  73,  36, 109, 100, 107, 255,
    144, 232, 161, 135,  94, 244, 129, 170,
     84, 253,  65,  34, 109, 100, 107, 255,

    144, 240, 194,  11,  47, 248, 130, 170,
     84, 253,  65,  34, 109, 100, 107, 251,
    144, 247, 223, 143,  63, 252, 131, 170,
     84, 253,  73,  34, 109, 100,  91, 251
};

const struct {
    const char* name;
    Containers::ArrayView<const UnsignedByte> data;
    CompressedPixelStorage storage;
    Containers::ArrayView<const UnsignedByte> dataSparse;
    std::size_t offset;
} CompressedPixelStorage3DData[]{
    {"default pixel storage",
        Containers::arrayView(CompressedData3D).exceptPrefix(16*4),
        {},
        Containers::arrayView(CompressedData3D).exceptPrefix(16*4), 0},
    #ifndef MAGNUM_TARGET_GLES
    {"skip Z",
        Containers::arrayView(CompressedData3D).exceptPrefix(16*4),
        CompressedPixelStorage{}
            .setSkip({0, 0, 4}),
        Containers::arrayView(CompressedData3D), 16*4}
    #endif
};

/* Combination of CompressedZero3D (defined below) and CompressedData3D */
constexpr UnsignedByte CompressedSubData3DComplete[]{
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    144, 224, 128,   3,  80,   0, 129, 170,
     84, 253,  73,  36, 109, 100, 107, 255,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    144, 232, 161, 135,  94, 244, 129, 170,
     84, 253,  65,  34, 109, 100, 107, 255,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    144, 240, 194,  11,  47, 248, 130, 170,
     84, 253,  65,  34, 109, 100, 107, 251,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    144, 247, 223, 143,  63, 252, 131, 170,
     84, 253,  73,  34, 109, 100,  91, 251,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

const struct {
    const char* name;
    CompressedPixelStorage storage;
    Vector3i size, offset;
    Containers::ArrayView<const UnsignedByte> dataSparse;
} CompressedSubImage3DData[]{
    {"upload full image at zero offset, default pixel storage",
        {}, {12, 4, 4}, {},
        Containers::arrayView(CompressedSubData3DComplete)},
    #ifndef MAGNUM_TARGET_GLES
    {"upload full image at zero offset, redundant row length & image height",
        CompressedPixelStorage{}
            .setRowLength(12)
            .setImageHeight(4), {12, 4, 4}, {},
        Containers::arrayView(CompressedSubData3DComplete)},
    {"upload a slice of full image at zero offset, row length & image height",
        CompressedPixelStorage{}
            .setRowLength(12)
            .setImageHeight(4), {8, 4, 4}, {},
        Containers::arrayView(CompressedSubData3DComplete)},
    #endif
    {"upload a smaller image at offset, default pixel storage",
        {}, {4, 4, 4}, {4, 0, 0},
        Containers::arrayView(CompressedData3D).exceptPrefix(16*4)},
    #ifndef MAGNUM_TARGET_GLES
    {"upload a smaller image at offset, skip Z",
        CompressedPixelStorage{}
            .setSkip({0, 0, 4}), {4, 4, 4}, {4, 0, 0},
        Containers::arrayView(CompressedData3D)}
    #endif
};
#endif

TextureGLTest::TextureGLTest() {
    addTests({
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::compressedBlockSize1D,
        &TextureGLTest::compressedBlockSize2D,
        &TextureGLTest::compressedBlockSize3D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::construct1D,
        #endif
        &TextureGLTest::construct2D,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        &TextureGLTest::construct3D,
        #endif

        &TextureGLTest::constructMove,

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::wrap1D,
        #endif
        &TextureGLTest::wrap2D,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        &TextureGLTest::wrap3D,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::wrapCreateIfNotAlready1D,
        #endif
        &TextureGLTest::wrapCreateIfNotAlready2D,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        &TextureGLTest::wrapCreateIfNotAlready3D,
        #endif

        #ifndef MAGNUM_TARGET_WEBGL
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::label1D,
        #endif
        &TextureGLTest::label2D,
        &TextureGLTest::label3D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::bind1D,
        #endif
        &TextureGLTest::bind2D,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        &TextureGLTest::bind3D,
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::bindImage1D,
        #endif
        &TextureGLTest::bindImage2D,
        &TextureGLTest::bindImage3D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::sampling1D<GenericSampler>,
        &TextureGLTest::sampling1D<GLSampler>,
        #endif
        &TextureGLTest::sampling2D<GenericSampler>,
        &TextureGLTest::sampling2D<GLSampler>,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        &TextureGLTest::sampling3D<GenericSampler>,
        &TextureGLTest::sampling3D<GLSampler>,
        #endif

        #ifndef MAGNUM_TARGET_WEBGL
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingSrgbDecode1D,
        #endif
        &TextureGLTest::samplingSrgbDecode2D,
        &TextureGLTest::samplingSrgbDecode3D,
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingSwizzle1D,
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        &TextureGLTest::samplingSwizzle2D,
        &TextureGLTest::samplingSwizzle3D,
        #endif
        #endif
        #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        &TextureGLTest::samplingMaxLevel2D,
        &TextureGLTest::samplingMaxLevel3D,
        &TextureGLTest::samplingCompare2D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingBorderInteger1D,
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        &TextureGLTest::samplingBorderInteger2D,
        &TextureGLTest::samplingBorderInteger3D,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::samplingDepthStencilMode1D,
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::samplingDepthStencilMode2D,
        &TextureGLTest::samplingDepthStencilMode3D,
        #endif
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        &TextureGLTest::samplingBorder2D,
        &TextureGLTest::samplingBorder3D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::storage1D,
        #endif
        &TextureGLTest::storage2D,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        &TextureGLTest::storage3D,
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::view1D,
        #endif
        &TextureGLTest::view2D,
        &TextureGLTest::view2DOnArray,
        &TextureGLTest::view2DOnCubeMap,
        &TextureGLTest::view2DOnCubeMapArray,
        &TextureGLTest::view3D
        #endif
        });

    #ifndef MAGNUM_TARGET_GLES
    addInstancedTests({
        &TextureGLTest::image1D,
        &TextureGLTest::image1DBuffer,
        &TextureGLTest::image1DQueryView,
        &TextureGLTest::subImage1D,
        &TextureGLTest::subImage1DBuffer,
        &TextureGLTest::subImage1DQuery,
        &TextureGLTest::subImage1DQueryView,
        &TextureGLTest::subImage1DQueryBuffer},
        Containers::arraySize(PixelStorage1DData));

    addTests({&TextureGLTest::compressedImage1D,
              &TextureGLTest::compressedImage1DBuffer,
              &TextureGLTest::compressedImage1DQueryView,
              &TextureGLTest::compressedSubImage1D,
              &TextureGLTest::compressedSubImage1DBuffer,
              &TextureGLTest::compressedSubImage1DQuery,
              &TextureGLTest::compressedSubImage1DQueryView,
              &TextureGLTest::compressedSubImage1DQueryBuffer});
    #endif

    addInstancedTests({
        &TextureGLTest::image2D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::image2DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::image2DQueryView,
        #endif
        &TextureGLTest::subImage2D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::subImage2DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::subImage2DQuery,
        &TextureGLTest::subImage2DQueryView,
        &TextureGLTest::subImage2DQueryBuffer,
        #endif
        }, Containers::arraySize(PixelStorage2DData));

    addInstancedTests({
        &TextureGLTest::compressedImage2D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::compressedImage2DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::compressedImage2DQueryView,
        #endif
        &TextureGLTest::compressedSubImage2D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::compressedSubImage2DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::compressedSubImage2DQuery,
        &TextureGLTest::compressedSubImage2DQueryView,
        &TextureGLTest::compressedSubImage2DQueryBuffer
        #endif
        }, Containers::arraySize(CompressedPixelStorage2DData));

    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    addInstancedTests({
        &TextureGLTest::image3D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::image3DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::image3DQueryView,
        #endif
        &TextureGLTest::subImage3D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::subImage3DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::subImage3DQuery,
        &TextureGLTest::subImage3DQueryView,
        &TextureGLTest::subImage3DQueryBuffer,
        #endif
        }, Containers::arraySize(PixelStorage3DData));

    addInstancedTests({
        &TextureGLTest::compressedImage3D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::compressedImage3DBuffer,
        #endif
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::compressedImage3DQueryView,
        #endif
        }, Containers::arraySize(CompressedPixelStorage3DData));

    addInstancedTests({
        &TextureGLTest::compressedSubImage3D,
        #ifndef MAGNUM_TARGET_GLES2
        &TextureGLTest::compressedSubImage3DBuffer,
        #endif
        }, Containers::arraySize(CompressedSubImage3DData));

    #ifndef MAGNUM_TARGET_GLES
    addInstancedTests({
        &TextureGLTest::compressedSubImage3DQuery,
        &TextureGLTest::compressedSubImage3DQueryView,
        &TextureGLTest::compressedSubImage3DQueryBuffer
        }, Containers::arraySize(CompressedPixelStorage3DData));
    #endif
    #endif

    addTests({
        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::generateMipmap1D,
        #endif
        &TextureGLTest::generateMipmap2D,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        &TextureGLTest::generateMipmap3D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::invalidateImage1D,
        #endif
        &TextureGLTest::invalidateImage2D,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        &TextureGLTest::invalidateImage3D,
        #endif

        #ifndef MAGNUM_TARGET_GLES
        &TextureGLTest::invalidateSubImage1D,
        #endif
        &TextureGLTest::invalidateSubImage2D,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        &TextureGLTest::invalidateSubImage3D,
        #endif

        &TextureGLTest::srgbStorage,
        &TextureGLTest::srgbAlphaStorage});
}

#ifndef MAGNUM_TARGET_WEBGL
using namespace Containers::Literals;
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::compressedBlockSize1D() {
    /* For uncompressed formats returns zero */
    CORRADE_COMPARE(Texture1D::compressedBlockSize(TextureFormat::RGBA8), 0);
    CORRADE_COMPARE(Texture1D::compressedBlockDataSize(TextureFormat::RGBA8), 0);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::compressedBlockSize2D() {
    /* For uncompressed formats returns zero */
    CORRADE_COMPARE(Texture2D::compressedBlockSize(TextureFormat::RGBA8), Vector2i{});
    CORRADE_COMPARE(Texture2D::compressedBlockDataSize(TextureFormat::RGBA8), 0);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    {
        /* Same happens with e.g. ASTC 10x10, where it reports 1 (?!) */
        CORRADE_EXPECT_FAIL_IF(Context::current().detectedDriver() & Context::DetectedDriver::Mesa,
            "Mesa misreports compressed block size for certain formats.");
        CORRADE_COMPARE(Texture2D::compressedBlockSize(TextureFormat::CompressedRGBAS3tcDxt1), Vector2i{4});
    }
    CORRADE_COMPARE(Texture2D::compressedBlockSize(TextureFormat::CompressedRGBAS3tcDxt3), Vector2i{4});
    CORRADE_COMPARE(Texture2D::compressedBlockDataSize(TextureFormat::CompressedRGBAS3tcDxt1), 8);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureGLTest::compressedBlockSize3D() {
    /* For uncompressed formats returns zero */
    CORRADE_COMPARE(Texture3D::compressedBlockSize(TextureFormat::RGBA8), Vector3i{});
    CORRADE_COMPARE(Texture3D::compressedBlockDataSize(TextureFormat::RGBA8), 0);

    MAGNUM_VERIFY_NO_GL_ERROR();

    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::ARB::texture_compression_bptc::string() << "is not supported.");

    CORRADE_COMPARE(Texture3D::compressedBlockSize(TextureFormat::CompressedRGBABptcUnorm), (Vector3i{4, 4, 1}));
    CORRADE_COMPARE(Texture3D::compressedBlockDataSize(TextureFormat::CompressedRGBABptcUnorm), 16);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::construct1D() {
    {
        Texture1D texture;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureGLTest::construct2D() {
    {
        Texture2D texture;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void TextureGLTest::construct3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    #endif

    {
        Texture3D texture;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(texture.id() > 0);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureGLTest::constructMove() {
    /* Move constructor tested in AbstractTexture, here we just verify there
       are no extra members that would need to be taken care of */
    CORRADE_COMPARE(sizeof(Texture2D), sizeof(AbstractTexture));

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Texture2D>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Texture2D>::value);
}

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::wrap1D() {
    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = Texture1D::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    Texture1D::wrap(id);
    glDeleteTextures(1, &id);
}
#endif

void TextureGLTest::wrap2D() {
    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = Texture2D::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    Texture2D::wrap(id);
    glDeleteTextures(1, &id);
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void TextureGLTest::wrap3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    #endif

    GLuint id;
    glGenTextures(1, &id);

    /* Releasing won't delete anything */
    {
        auto texture = Texture3D::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(texture.release(), id);
    }

    /* ...so we can wrap it again */
    Texture3D::wrap(id);
    glDeleteTextures(1, &id);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::wrapCreateIfNotAlready1D() {
    /* Make an object and ensure it's created */
    Texture1D texture;
    texture.bind(0);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(texture.flags(), ObjectFlag::Created|ObjectFlag::DeleteOnDestruction);

    /* Wrap into another object without ObjectFlag::Created being set, which is
       a common usage pattern to make non-owning references. Then calling an
       API that internally does createIfNotAlready() shouldn't assert just
       because Created isn't set but the object is bound, instead it should
       just mark it as such when it discovers it. */
    Texture1D wrapped = Texture1D::wrap(texture.id());
    CORRADE_COMPARE(wrapped.flags(), ObjectFlags{});

    wrapped.label();
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(wrapped.flags(), ObjectFlag::Created);
}
#endif

void TextureGLTest::wrapCreateIfNotAlready2D() {
    /* Make an object and ensure it's created */
    Texture2D texture;
    texture.bind(0);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(texture.flags(), ObjectFlag::Created|ObjectFlag::DeleteOnDestruction);

    /* Wrap into another object without ObjectFlag::Created being set, which is
       a common usage pattern to make non-owning references. Then calling an
       API that internally does createIfNotAlready() shouldn't assert just
       because Created isn't set but the object is bound, instead it should
       just mark it as such when it discovers it. */
    Texture2D wrapped = Texture2D::wrap(texture.id());
    CORRADE_COMPARE(wrapped.flags(), ObjectFlags{});

    #ifndef MAGNUM_TARGET_WEBGL
    wrapped.label();
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(wrapped.flags(), ObjectFlag::Created);
    #else
    CORRADE_SKIP("No API that would call createIfNotAlready() on WebGL, can't test.");
    #endif
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void TextureGLTest::wrapCreateIfNotAlready3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    #endif

    /* Make an object and ensure it's created */
    Texture3D texture;
    texture.bind(0);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(texture.flags(), ObjectFlag::Created|ObjectFlag::DeleteOnDestruction);

    /* Wrap into another object without ObjectFlag::Created being set, which is
       a common usage pattern to make non-owning references. Then calling an
       API that internally does createIfNotAlready() shouldn't assert just
       because Created isn't set but the object is bound, instead it should
       just mark it as such when it discovers it. */
    Texture3D wrapped = Texture3D::wrap(texture.id());
    CORRADE_COMPARE(wrapped.flags(), ObjectFlags{});

    #ifndef MAGNUM_TARGET_WEBGL
    wrapped.label();
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(wrapped.flags(), ObjectFlag::Created);
    #else
    CORRADE_SKIP("No API that would call createIfNotAlready() on WebGL, can't test.");
    #endif
}
#endif

#ifndef MAGNUM_TARGET_WEBGL
#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::label1D() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Texture1D texture;
    CORRADE_COMPARE(texture.label(), "");
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Test the string size gets correctly used, instead of relying on null
       termination */
    texture.setLabel("MyTexture!"_s.exceptSuffix(1));
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(texture.label(), "MyTexture");
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureGLTest::label2D() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Texture2D texture;
    CORRADE_COMPARE(texture.label(), "");
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Test the string size gets correctly used, instead of relying on null
       termination */
    texture.setLabel("MyTexture!"_s.exceptSuffix(1));
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(texture.label(), "MyTexture");
    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureGLTest::label3D() {
    /* No-Op version is tested in AbstractObjectGLTest */
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Texture3D texture;
    CORRADE_COMPARE(texture.label(), "");
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Test the string size gets correctly used, instead of relying on null
       termination */
    texture.setLabel("MyTexture!"_s.exceptSuffix(1));
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(texture.label(), "MyTexture");
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::bind1D() {
    Texture1D texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureGLTest::bind2D() {
    Texture2D texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void TextureGLTest::bind3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.bind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(15);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::bind(7, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbind(7, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::bindImage1D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::ARB::shader_image_load_store::string() << "is not supported.");

    Texture1D texture;
    texture.setStorage(1, TextureFormat::RGBA8, 32)
        .bindImage(2, 0, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbindImage(2);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureGLTest::bindImage2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::ARB::shader_image_load_store::string() << "is not supported.");
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{32})
        .bindImage(2, 0, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbindImage(2);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}

void TextureGLTest::bindImage3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::shader_image_load_store>())
        CORRADE_SKIP(Extensions::ARB::shader_image_load_store::string() << "is not supported.");
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 is not supported.");
    #endif

    Texture3D texture;
    texture.setStorage(1, TextureFormat::RGBA8, {32, 32, 4})
        .bindImage(2, 0, 1, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    MAGNUM_VERIFY_NO_GL_ERROR();

    texture.bindImageLayered(3, 0, ImageAccess::ReadWrite, ImageFormat::RGBA8);

    AbstractTexture::unbindImage(2);
    AbstractTexture::unbindImage(3);

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    AbstractTexture::bindImages(1, {&texture, nullptr, &texture});

    MAGNUM_VERIFY_NO_GL_ERROR();

    AbstractTexture::unbindImages(1, 3);

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
template<class T> void TextureGLTest::sampling1D() {
    setTestCaseTemplateName(std::is_same<T, GenericSampler>::value ?
        "GenericSampler" : "GLSampler");

    Texture1D texture;
    texture.setMinificationFilter(T::Filter::Linear, T::Mipmap::Linear)
           .setMagnificationFilter(T::Filter::Linear)
           .setMinLod(-750.0f)
           .setMaxLod(750.0f)
           .setLodBias(0.5f)
           .setBaseLevel(1)
           .setMaxLevel(750)
           .setWrapping(T::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
           .setCompareMode(SamplerCompareMode::CompareRefToTexture)
           .setCompareFunction(SamplerCompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureGLTest::samplingSrgbDecode1D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::EXT::texture_sRGB_decode::string() << "is not supported.");

    Texture1D texture;
    texture.setSrgbDecode(false);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureGLTest::samplingSwizzle1D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::ARB::texture_swizzle::string() << "is not supported.");

    Texture1D texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureGLTest::samplingBorderInteger1D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::EXT::texture_integer::string() << "is not supported.");

    Texture1D a;
    a.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture1D b;
    b.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureGLTest::samplingDepthStencilMode1D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::ARB::stencil_texturing::string() << "is not supported.");

    Texture1D texture;
    texture.setDepthStencilMode(SamplerDepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

template<class T> void TextureGLTest::sampling2D() {
    setTestCaseTemplateName(std::is_same<T, GenericSampler>::value ?
        "GenericSampler" : "GLSampler");

    Texture2D texture;
    texture.setMinificationFilter(T::Filter::Linear, T::Mipmap::Linear)
           .setMagnificationFilter(T::Filter::Linear)
           #ifndef MAGNUM_TARGET_GLES2
           .setMinLod(-750.0f)
           .setMaxLod(750.0f)
           #ifndef MAGNUM_TARGET_GLES
           .setLodBias(0.5f)
           #endif
           .setBaseLevel(1)
           .setMaxLevel(750)
           #endif
           #ifndef MAGNUM_TARGET_GLES
           .setWrapping(T::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           #else
           .setWrapping(T::Wrapping::ClampToEdge)
           #endif
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy())
           #ifndef MAGNUM_TARGET_GLES2
           .setCompareMode(SamplerCompareMode::CompareRefToTexture)
           .setCompareFunction(SamplerCompareFunction::GreaterOrEqual)
            #endif
           ;

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_WEBGL
void TextureGLTest::samplingSrgbDecode2D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::EXT::sRGB>())
        CORRADE_SKIP(Extensions::EXT::sRGB::string() << "is not supported.");
    #endif
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::EXT::texture_sRGB_decode::string() << "is not supported.");

    Texture2D texture;
    texture.setSrgbDecode(false);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureGLTest::samplingSwizzle2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::ARB::texture_swizzle::string() << "is not supported.");
    #endif

    Texture2D texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureGLTest::samplingMaxLevel2D() {
    if(!Context::current().isExtensionSupported<Extensions::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::APPLE::texture_max_level::string() << "is not supported.");

    Texture2D texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void TextureGLTest::samplingCompare2D() {
    if(!Context::current().isExtensionSupported<Extensions::EXT::shadow_samplers>())
        CORRADE_SKIP(Extensions::EXT::shadow_samplers::string() << "is not supported.");

    Texture2D texture;
    texture.setCompareMode(SamplerCompareMode::CompareRefToTexture)
           .setCompareFunction(SamplerCompareFunction::GreaterOrEqual);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureGLTest::samplingBorderInteger2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::EXT::texture_integer::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::EXT::texture_border_clamp::string() << "is not supported.");
    #endif

    Texture2D a;
    a.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture2D b;
    b.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::samplingDepthStencilMode2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::ARB::stencil_texturing::string() << "is not supported.");
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isVersionSupported(Version::GLES310) && !Context::current().isExtensionSupported<Extensions::ANGLE::stencil_texturing>())
        CORRADE_SKIP("Neither OpenGL ES 3.1 nor" << Extensions::ANGLE::stencil_texturing::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::stencil_texturing>())
        CORRADE_SKIP(Extensions::WEBGL::stencil_texturing::string() << "is not supported.");
    #endif

    Texture2D texture;
    texture.setDepthStencilMode(SamplerDepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
void TextureGLTest::samplingBorder2D() {
    if(!Context::current().isExtensionSupported<Extensions::NV::texture_border_clamp>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP("No required extension is supported.");

    Texture2D texture;
    texture.setWrapping(SamplerWrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
template<class T> void TextureGLTest::sampling3D() {
    setTestCaseTemplateName(std::is_same<T, GenericSampler>::value ?
        "GenericSampler" : "GLSampler");

    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setMinificationFilter(T::Filter::Linear, T::Mipmap::Linear)
           .setMagnificationFilter(T::Filter::Linear)
           #ifndef MAGNUM_TARGET_GLES2
           .setMinLod(-750.0f)
           .setMaxLod(750.0f)
           #ifndef MAGNUM_TARGET_GLES
           .setLodBias(0.5f)
           #endif
           .setBaseLevel(1)
           .setMaxLevel(750)
           #endif
           #ifndef MAGNUM_TARGET_GLES
           .setWrapping(T::Wrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f))
           #else
           .setWrapping(T::Wrapping::ClampToEdge)
           #endif
           .setMaxAnisotropy(Sampler::maxMaxAnisotropy());

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#ifndef MAGNUM_TARGET_WEBGL
void TextureGLTest::samplingSrgbDecode3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::sRGB>())
        CORRADE_SKIP(Extensions::EXT::sRGB::string() << "is not supported.");
    #endif
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_sRGB_decode>())
        CORRADE_SKIP(Extensions::EXT::texture_sRGB_decode::string() << "is not supported.");

    Texture3D texture;
    texture.setSrgbDecode(false);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureGLTest::samplingSwizzle3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_swizzle>())
        CORRADE_SKIP(Extensions::ARB::texture_swizzle::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setSwizzle<'b', 'g', 'r', '0'>();

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureGLTest::samplingMaxLevel3D() {
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::APPLE::texture_max_level>())
        CORRADE_SKIP(Extensions::APPLE::texture_max_level::string() << "is not supported.");

    Texture3D texture;
    texture.setMaxLevel(750);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void TextureGLTest::samplingBorderInteger3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_integer>())
        CORRADE_SKIP(Extensions::EXT::texture_integer::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP(Extensions::EXT::texture_border_clamp::string() << "is not supported.");
    #endif

    Texture3D a;
    a.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4i(1, 56, 78, -2));
    Texture3D b;
    b.setWrapping(SamplerWrapping::ClampToBorder)
     .setBorderColor(Vector4ui(35, 56, 78, 15));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::samplingDepthStencilMode3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::stencil_texturing>())
        CORRADE_SKIP(Extensions::ARB::stencil_texturing::string() << "is not supported.");
    #elif !defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isVersionSupported(Version::GLES310) && !Context::current().isExtensionSupported<Extensions::ANGLE::stencil_texturing>())
        CORRADE_SKIP("Neither OpenGL ES 3.1 nor" << Extensions::ANGLE::stencil_texturing::string() << "is supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::stencil_texturing>())
        CORRADE_SKIP(Extensions::WEBGL::stencil_texturing::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setDepthStencilMode(SamplerDepthStencilMode::StencilIndex);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
void TextureGLTest::samplingBorder3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    #endif

    if(!Context::current().isExtensionSupported<Extensions::NV::texture_border_clamp>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::texture_border_clamp>())
        CORRADE_SKIP("No required extension is supported.");

    Texture3D texture;
    texture.setWrapping(SamplerWrapping::ClampToBorder)
           .setBorderColor(Color3(0.5f));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::storage1D() {
    Texture1D texture;
    texture.setStorage(5, TextureFormat::RGBA8, 32);

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), 32);
    CORRADE_COMPARE(texture.imageSize(1), 16);
    CORRADE_COMPARE(texture.imageSize(2),  8);
    CORRADE_COMPARE(texture.imageSize(3),  4);
    CORRADE_COMPARE(texture.imageSize(4),  2);
    CORRADE_COMPARE(texture.imageSize(5),  0); /* not available */

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureGLTest::storage2D() {
    Texture2D texture;
    texture.setStorage(5,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::RGBA8,
        #else
        TextureFormat::RGBA,
        #endif
        Vector2i(32));

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported, skipping image size testing.");
    #endif

    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 0)); /* not available */

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void TextureGLTest::storage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setStorage(5, TextureFormat::RGBA8, Vector3i(32));

    MAGNUM_VERIFY_NO_GL_ERROR();

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    #ifdef MAGNUM_TARGET_GLES
    if(!Context::current().isVersionSupported(Version::GLES310))
        CORRADE_SKIP("OpenGL ES 3.1 not supported, skipping image size testing.");
    #endif

    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 0)); /* not available */

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::view1D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_storage>())
        CORRADE_SKIP(Extensions::ARB::texture_storage::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_view>())
        CORRADE_SKIP(Extensions::ARB::texture_view::string() << "is not supported.");

    Texture1D texture;
    texture.setStorage(5, TextureFormat::RGBA8, 128);

    auto view = Texture1D::view(texture, TextureFormat::RGBA8, 2, 3);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(view.imageSize(0), 32);
    CORRADE_COMPARE(view.imageSize(1), 16);
    CORRADE_COMPARE(view.imageSize(2), 8);
}
#endif

void TextureGLTest::view2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_storage>())
        CORRADE_SKIP(Extensions::ARB::texture_storage::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_view>())
        CORRADE_SKIP(Extensions::ARB::texture_view::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_view>() &&
       !Context::current().isExtensionSupported<Extensions::OES::texture_view>())
        CORRADE_SKIP("Neither" << Extensions::EXT::texture_view::string() << "nor" << Extensions::OES::texture_view::string() << "is supported.");
    #endif

    Texture2D texture;
    texture.setStorage(5, TextureFormat::RGBA8, {128, 32});

    auto view = Texture2D::view(texture, TextureFormat::RGBA8, 2, 3);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(view.imageSize(0), (Vector2i{32, 8}));
    CORRADE_COMPARE(view.imageSize(1), (Vector2i{16, 4}));
    CORRADE_COMPARE(view.imageSize(2), (Vector2i{8, 2}));
}

void TextureGLTest::view2DOnArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_storage>())
        CORRADE_SKIP(Extensions::ARB::texture_storage::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_view>())
        CORRADE_SKIP(Extensions::ARB::texture_view::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_view>() &&
       !Context::current().isExtensionSupported<Extensions::OES::texture_view>())
        CORRADE_SKIP("Neither" << Extensions::EXT::texture_view::string() << "nor" << Extensions::OES::texture_view::string() << "is supported.");
    #endif

    Texture2DArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, {128, 32, 7});

    auto view = Texture2D::view(texture, TextureFormat::RGBA8, 2, 3, 4);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(view.imageSize(0), (Vector2i{32, 8}));
    CORRADE_COMPARE(view.imageSize(1), (Vector2i{16, 4}));
    CORRADE_COMPARE(view.imageSize(2), (Vector2i{8, 2}));
}

void TextureGLTest::view2DOnCubeMap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_storage>())
        CORRADE_SKIP(Extensions::ARB::texture_storage::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_view>())
        CORRADE_SKIP(Extensions::ARB::texture_view::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_view>() &&
       !Context::current().isExtensionSupported<Extensions::OES::texture_view>())
        CORRADE_SKIP("Neither" << Extensions::EXT::texture_view::string() << "nor" << Extensions::OES::texture_view::string() << "is supported.");
    #endif

    CubeMapTexture texture;
    texture.setStorage(5, TextureFormat::RGBA8, {32, 32});

    auto view = Texture2D::view(texture, TextureFormat::RGBA8, 2, 3, 4);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(view.imageSize(0), (Vector2i{8, 8}));
    CORRADE_COMPARE(view.imageSize(1), (Vector2i{4, 4}));
    CORRADE_COMPARE(view.imageSize(2), (Vector2i{2, 2}));
}

void TextureGLTest::view2DOnCubeMapArray() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::ARB::texture_cube_map_array::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_storage>())
        CORRADE_SKIP(Extensions::ARB::texture_storage::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_view>())
        CORRADE_SKIP(Extensions::ARB::texture_view::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_cube_map_array>())
        CORRADE_SKIP(Extensions::EXT::texture_cube_map_array::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_view>() &&
       !Context::current().isExtensionSupported<Extensions::OES::texture_view>())
        CORRADE_SKIP("Neither" << Extensions::EXT::texture_view::string() << "nor" << Extensions::OES::texture_view::string() << "is supported.");
    #endif

    CubeMapTextureArray texture;
    texture.setStorage(5, TextureFormat::RGBA8, {32, 32, 12});

    auto view = Texture2D::view(texture, TextureFormat::RGBA8, 2, 3, 11);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(view.imageSize(0), (Vector2i{8, 8}));
    CORRADE_COMPARE(view.imageSize(1), (Vector2i{4, 4}));
    CORRADE_COMPARE(view.imageSize(2), (Vector2i{2, 2}));
}

void TextureGLTest::view3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_storage>())
        CORRADE_SKIP(Extensions::ARB::texture_storage::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_view>())
        CORRADE_SKIP(Extensions::ARB::texture_view::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_view>() &&
       !Context::current().isExtensionSupported<Extensions::OES::texture_view>())
        CORRADE_SKIP("Neither" << Extensions::EXT::texture_view::string() << "nor" << Extensions::OES::texture_view::string() << "is supported.");
    #endif

    Texture3D texture;
    texture.setStorage(5, TextureFormat::RGBA8, {128, 32, 64});

    auto view = Texture3D::view(texture, TextureFormat::RGBA8, 2, 3);
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(view.imageSize(0), (Vector3i{32, 8, 16}));
    CORRADE_COMPARE(view.imageSize(1), (Vector3i{16, 4, 8}));
    CORRADE_COMPARE(view.imageSize(2), (Vector3i{8, 2, 4}));
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::image1D() {
    auto&& data = PixelStorage1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView1D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2,
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image1D image = texture.image(0, {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags1D{});
    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::image1DBuffer() {
    auto&& data = PixelStorage1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage1D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2,
        data.dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage1D image = texture.image(0,
        {data.storage, PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticDraw);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::image1DQueryView() {
    auto&& data = PixelStorage1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView1D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2,
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 2*4};
    MutableImageView1D image{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2, imageData};
    texture.image(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlags1D{});
    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

constexpr UnsignedByte Zero1D[4*4] = {};
constexpr UnsignedByte SubData1DComplete[]{
    0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0
};

void TextureGLTest::subImage1D() {
    auto&& data = PixelStorage1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView1D(PixelFormat::RGBA, PixelType::UnsignedByte, 4, Zero1D));
    texture.setSubImage(0, 1, ImageView1D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2,
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image1D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubData1DComplete),
        TestSuite::Compare::Container);
}

void TextureGLTest::subImage1DBuffer() {
    auto&& data = PixelStorage1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView1D(PixelFormat::RGBA, PixelType::UnsignedByte, 4, Zero1D));
    texture.setSubImage(0, 1, BufferImage1D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2,
        data.dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage1D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), 4);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData), Containers::arrayView(SubData1DComplete),
        TestSuite::Compare::Container);
}

void TextureGLTest::subImage1DQuery() {
    auto&& data = PixelStorage1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    Texture1D texture;
    texture.setStorage(1, TextureFormat::RGBA8, 4)
           .setSubImage(0, {}, ImageView1D{PixelFormat::RGBA, PixelType::UnsignedByte, 4, SubData1DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image1D image = texture.subImage(0, Range1Di::fromSize(1, 2),
        {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags1D{});
    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::subImage1DQueryView() {
    auto&& data = PixelStorage1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    Texture1D texture;
    texture.setStorage(1, TextureFormat::RGBA8, 4)
           .setSubImage(0, {}, ImageView1D{PixelFormat::RGBA, PixelType::UnsignedByte, 4, SubData1DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 2*4};
    MutableImageView1D image{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, 2, imageData};
    texture.subImage(0, Range1Di::fromSize(1, 2), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlags1D{});
    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::subImage1DQueryBuffer() {
    auto&& data = PixelStorage1DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    Texture1D texture;
    texture.setStorage(1, TextureFormat::RGBA8, 4)
           .setSubImage(0, {}, ImageView1D{PixelFormat::RGBA, PixelType::UnsignedByte, 4, SubData1DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage1D image = texture.subImage(0, Range1Di::fromSize(1, 2),
        {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), 2);
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::compressedImage1D() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::compressedImage1DBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::compressedImage1DQueryView() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::compressedSubImage1D() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::compressedSubImage1DBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::compressedSubImage1DQuery() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::compressedSubImage1DQueryView() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}

void TextureGLTest::compressedSubImage1DQueryBuffer() {
    CORRADE_SKIP("No 1D texture compression format exists.");
}
#endif

void TextureGLTest::image2D() {
    auto&& data = PixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(data.storage != PixelStorage{} && !Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() << "is not supported.");
    #else
    if(data.storage != PixelStorage{})
        CORRADE_SKIP("Image unpack is not supported in WebGL 1.");
    #endif
    #endif

    Texture2D texture;
    texture.setImage(0,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::RGBA8,
        #else
        TextureFormat::RGBA,
        #endif
        ImageView2D{data.storage,
            PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
            data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = texture.image(0, {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags2D{});
    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::image2DBuffer() {
    auto&& data = PixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage2D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image = texture.image(0,
        {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::image2DQueryView() {
    auto&& data = PixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView2D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 2*2*4};
    MutableImageView2D image{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, imageData, ImageFlag2D::Array};
    texture.image(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(image.size(), Vector2i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}
#endif

constexpr UnsignedByte Zero2D[4*4*4]{};

#ifndef MAGNUM_TARGET_GLES
constexpr UnsignedByte SubData2DComplete[]{
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
    0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
};
#endif

void TextureGLTest::subImage2D() {
    auto&& data = PixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES2
    #ifndef MAGNUM_TARGET_WEBGL
    if(data.storage != PixelStorage{} && !Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() << "is not supported.");
    #else
    if(data.storage != PixelStorage{})
        CORRADE_SKIP("Image unpack is not supported in WebGL 1.");
    #endif
    #endif

    Texture2D texture;
    texture.setImage(0,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::RGBA8,
        #else
        TextureFormat::RGBA,
        #endif
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero2D));
    texture.setSubImage(0, Vector2i(1), ImageView2D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::subImage2DBuffer() {
    auto&& data = PixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture2D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(4), Zero2D));
    texture.setSubImage(0, Vector2i(1), BufferImage2D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(2),
        data.dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage2D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData), Containers::arrayView(SubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::subImage2DQuery() {
    auto&& data = PixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image2D image = texture.subImage(0, Range2Di::fromSize(Vector2i{1}, Vector2i{2}),
        {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags2D{});
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::subImage2DQueryView() {
    auto&& data = PixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 2*2*4};
    MutableImageView2D image{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{2}, imageData, ImageFlag2D::Array};
    texture.subImage(0, Range2Di::fromSize(Vector2i{1}, Vector2i{2}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::subImage2DQueryBuffer() {
    auto&& data = PixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector2i{4})
           .setSubImage(0, {}, ImageView2D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i{4}, SubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage2D image = texture.subImage(0, Range2Di::fromSize(Vector2i{1}, Vector2i{2}),
        {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}
#endif

void TextureGLTest::compressedImage2D() {
    auto&& data = CompressedPixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #endif

    Texture2D texture;
    texture.setCompressedImage(0, CompressedImageView2D{
        data.storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(0, {data.storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags2D{});
    CORRADE_COMPARE(image.size(), Vector2i{4});

    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::compressedImage2DBuffer() {
    auto&& data = CompressedPixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #endif

    Texture2D texture;
    texture.setCompressedImage(0, CompressedBufferImage2D{
        data.storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        data.dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage2D image = texture.compressedImage(0, {data.storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::compressedImage2DQueryView() {
    auto&& data = CompressedPixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");

    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    Texture2D texture;
    texture.setCompressedImage(0, CompressedImageView2D{
        data.storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 1*16};
    MutableCompressedImageView2D image{data.storage, CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, imageData, ImageFlag2D::Array};
    texture.compressedImage(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(image.size(), Vector2i{4});

    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}
#endif

/* Just 12x4 zeros compressed using RGBA DXT3 by the driver */
constexpr UnsignedByte CompressedZero2D[3*16]{};

#ifndef MAGNUM_TARGET_GLES
/* Combination of CompressedZero2D and CompressedData2D */
constexpr UnsignedByte CompressedSubData2DComplete[]{
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,  17,  17,  34,  34,  51,  51,  67,
    232,  57,   0,   0, 213, 255, 170,   2,
      0,   0,   0,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0
};
#endif

void TextureGLTest::compressedSubImage2D() {
    auto&& data = CompressedPixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #endif

    Texture2D texture;
    texture.setCompressedImage(0, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3,
        {12, 4}, CompressedZero2D});
    texture.setCompressedSubImage(0, {4, 0}, CompressedImageView2D{
        data.storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedImage2D image = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector2i{12, 4}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(CompressedSubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::compressedSubImage2DBuffer() {
    auto&& data = CompressedPixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_WEBGL)
    if(!Context::current().isExtensionSupported<Extensions::WEBGL::compressed_texture_s3tc>())
        CORRADE_SKIP(Extensions::WEBGL::compressed_texture_s3tc::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::texture_compression_dxt3>())
        CORRADE_SKIP(Extensions::ANGLE::texture_compression_dxt3::string() << "is not supported.");
    #endif

    #ifndef MAGNUM_TARGET_GLES
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #endif

    Texture2D texture;
    texture.setCompressedImage(0, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3,
        {12, 4}, CompressedZero2D});
    texture.setCompressedSubImage(0, {4, 0}, CompressedBufferImage2D{
        data.storage,
        CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4},
        data.dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage2D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector2i{12, 4}));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData),
        Containers::arrayView(CompressedSubData2DComplete),
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::compressedSubImage2DQuery() {
    auto&& data = CompressedPixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 4})
           .setCompressedSubImage(0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 4}, CompressedSubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedImage2D image = texture.compressedSubImage(0, Range2Di::fromSize({4, 0}, Vector2i{4}),
        {data.storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags2D{});
    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::compressedSubImage2DQueryView() {
    auto&& data = CompressedPixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 4})
           .setCompressedSubImage(0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 4}, CompressedSubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 1*16};
    MutableCompressedImageView2D image{data.storage, CompressedPixelFormat::RGBAS3tcDxt3, Vector2i{4}, imageData, ImageFlag2D::Array};
    texture.compressedSubImage(0, Range2Di::fromSize({4, 0}, Vector2i{4}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag2D::Array);
    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::compressedSubImage2DQueryBuffer() {
    auto&& data = CompressedPixelStorage2DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_s3tc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_s3tc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    Texture2D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBAS3tcDxt3, {12, 4})
           .setCompressedSubImage(0, {}, CompressedImageView2D{CompressedPixelFormat::RGBAS3tcDxt3, {12, 4}, CompressedSubData2DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedBufferImage2D image = texture.compressedSubImage(0, Range2Di::fromSize({4, 0}, Vector2i{4}),
        {data.storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector2i{4});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}
#endif

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void TextureGLTest::image3D() {
    auto&& data = PixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    if(data.storage != PixelStorage{} && !Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView3D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2),
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags3D{});
    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::image3DBuffer() {
    auto&& data = PixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8, BufferImage3D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2),
        data.dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0,
        {data.storage, PixelFormat::RGBA, PixelType::UnsignedByte},
        BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::image3DQueryView() {
    auto&& data = PixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8, ImageView3D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2),
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 2*2*2*4};
    MutableImageView3D image{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{2}, imageData, ImageFlag3D::Array};
    texture.image(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
    CORRADE_COMPARE(image.size(), Vector3i(2));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}
#endif

constexpr UnsignedByte Zero3D[4*4*4*4]{};

#ifndef MAGNUM_TARGET_GLES
constexpr UnsignedByte SubData3DComplete[]{
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0, 0, 0, 0,
    0, 0, 0, 0, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0, 0, 0, 0,
    0, 0, 0, 0, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,

    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0,
    0, 0, 0, 0,    0,    0,    0,    0,    0,    0,    0,    0, 0, 0, 0, 0
};
#endif

void TextureGLTest::subImage3D() {
    auto&& data = PixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    if(data.storage != PixelStorage{} && !Context::current().isExtensionSupported<Extensions::EXT::unpack_subimage>())
        CORRADE_SKIP(Extensions::EXT::unpack_subimage::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(4), Zero3D));
    texture.setSubImage(0, Vector3i(1), ImageView3D{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2),
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    Image3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
        Containers::arrayView(SubData3DComplete),
        TestSuite::Compare::Container);
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::subImage3DBuffer() {
    auto&& data = PixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(4), Zero3D));
    texture.setSubImage(0, Vector3i(1), BufferImage3D{
        data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(2),
        data.dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    BufferImage3D image = texture.image(0, {PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i(4));
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData),
        Containers::arrayView(SubData3DComplete),
        TestSuite::Compare::Container);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::subImage3DQuery() {
    auto&& data = PixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    Texture3D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector3i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{4}, SubData3DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Image3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, Vector3i{2}),
        {data.storage, PixelFormat::RGBA, PixelType::UnsignedByte});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags3D{});
    CORRADE_COMPARE(image.size(), Vector3i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::subImage3DQueryView() {
    auto&& data = PixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    Texture3D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector3i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{4}, SubData3DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 2*2*2*4};
    MutableImageView3D image{data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{2}, imageData, ImageFlag3D::Array};
    texture.subImage(0, Range3Di::fromSize(Vector3i{1}, Vector3i{2}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
    CORRADE_COMPARE(image.size(), Vector3i{2});
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}

void TextureGLTest::subImage3DQueryBuffer() {
    auto&& data = PixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");

    Texture3D texture;
    texture.setStorage(1, TextureFormat::RGBA8, Vector3i{4})
           .setSubImage(0, {}, ImageView3D{PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i{4}, SubData3DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    BufferImage3D image = texture.subImage(0, Range3Di::fromSize(Vector3i{1}, Vector3i{2}),
        {data.storage,
        PixelFormat::RGBA, PixelType::UnsignedByte}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{2});
    /* Works fine with llvmpipe. No idea about Mesa AMD and such. */
    CORRADE_EXPECT_FAIL_IF(
        Context::current().rendererString().contains("Mesa Intel(R)"),
        "Mesa Intel has this broken since 21.2, worked fine with 21.1.");
    CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
        data.data,
        TestSuite::Compare::Container);
}
#endif

void TextureGLTest::compressedImage3D() {
    auto&& data = CompressedPixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_SKIP("No 3D texture compression format available on OpenGL ES 2.0.");
    #else
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::ARB::texture_compression_bptc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_bptc::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setCompressedImage(0, CompressedImageView3D{
        data.storage,
        CompressedPixelFormat::RGBABptcUnorm, Vector3i{4},
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CompressedImage3D image = texture.compressedImage(0, {data.storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags3D{});
    CORRADE_COMPARE(image.size(), Vector3i{4});
    {
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
    #endif
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::compressedImage3DBuffer() {
    auto&& data = CompressedPixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::ARB::texture_compression_bptc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_bptc::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setCompressedImage(0, CompressedBufferImage3D{
        data.storage,
        CompressedPixelFormat::RGBABptcUnorm, Vector3i{4},
        data.dataSparse,
        BufferUsage::StaticDraw});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage3D image = texture.compressedImage(0, {data.storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{4});
    {
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::compressedImage3DQueryView() {
    auto&& data = CompressedPixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::ARB::texture_compression_bptc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    Texture3D texture;
    texture.setCompressedImage(0, CompressedImageView3D{
        data.storage,
        CompressedPixelFormat::RGBABptcUnorm, Vector3i{4},
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 64};
    MutableCompressedImageView3D image{data.storage, CompressedPixelFormat::RGBABptcUnorm, Vector3i{4}, imageData, ImageFlag3D::Array};
    texture.compressedImage(0, image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
    CORRADE_COMPARE(image.size(), Vector3i{4});
    {
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
/* Just 12x4x4 zeros compressed using RGBA BPTC Unorm by the driver */
constexpr UnsignedByte CompressedZero3D[3*4*16]{
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
#endif

void TextureGLTest::compressedSubImage3D() {
    auto&& data = CompressedSubImage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #if defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    CORRADE_SKIP("No 3D texture compression format available on OpenGL ES 2.0.");
    #else
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::ARB::texture_compression_bptc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_bptc::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBABptcUnorm,
        {12, 4, 4}, CompressedZero3D});
    texture.setCompressedSubImage(0, data.offset, CompressedImageView3D{data.storage,
        CompressedPixelFormat::RGBABptcUnorm, data.size,
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CompressedImage3D image = texture.compressedImage(0, {});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 4, 4}));

    {
        /* This "works" if running the test cases that have a default pixel
           storage as the first ever, i.e. with no test before setting
           GL_UNPACK_COMPRESSED_BLOCK_WIDTH etc. Which used to be the common
           case back when compressed block properties were meant to be
           specified manually in CompressedPixelStorage, and so this particular
           test case seemed to pass on NVidia and seemed to only behave weird
           with the Z skip.

           But the block properties are now taken implicitly from the format
           and set internally almost always, thus resulting in this test being
           broken in almost all cases. I tried many different things, including
           uploading slice-by-slice (and thus avoiding the need to set Z skip),
           but as soon as the block size state is non-zero, only the very first
           slice uploaded, no other. Using setStorage() instead of setImage()
           didn't make any difference, although such a change is known to work
           around certain bugs in cube maps.

           One option I refuse to try out is reverting the change in
           https://github.com/mosra/magnum/commit/214dd5dbadf4bba8884e2cafed1eced838cda977
           -- i.e., it seems that NV treats 3D BPTC blocks as cubes, being
           4x4x4 instead of 4x4x1, and then uploads them as such, which might
           *partially* explain what's going on in here. But even if reordering
           data in such a way would make some more tests pass, it'd still limit
           the upload to be only possible with multiples of four slices, which
           isn't really any better.

           Thus, I fear, it's unfixable. Fortunately it's only the case of 3D
           textures, which only support BPTC / BC7. 2D arrays or cubemaps don't
           seem to exhibit any similar bug. 3D ASTC formats would theoretically
           be another format that works with 3D textures, but even 2D ASTC
           still isn't exposed by NVidia even in 2025, so that's out of
           question. The workaround is shown in the passing tests, i.e.
           uploading whole slices. */
        Int pixelStoreBlockPropertiesSet = 0;
        glGetIntegerv(GL_UNPACK_COMPRESSED_BLOCK_WIDTH, &pixelStoreBlockPropertiesSet);
        CORRADE_EXPECT_FAIL_IF((pixelStoreBlockPropertiesSet || data.storage != CompressedPixelStorage{}) && data.size != (Vector3i{12, 4, 4}) && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Compressed 3D texture upload behaves weirdly on NVidia if non-default pixel storage is used and not uploading the whole image.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()),
            Containers::arrayView(CompressedSubData3DComplete),
            TestSuite::Compare::Container);
    }
    #endif
    #endif
}

#ifndef MAGNUM_TARGET_GLES2
void TextureGLTest::compressedSubImage3DBuffer() {
    auto&& data = CompressedSubImage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::ARB::texture_compression_bptc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");
    #else
    if(!Context::current().isExtensionSupported<Extensions::EXT::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::EXT::texture_compression_bptc::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setCompressedImage(0, CompressedImageView3D{CompressedPixelFormat::RGBABptcUnorm,
        {12, 4, 4}, CompressedZero3D});
    texture.setCompressedSubImage(0, data.offset, CompressedImageView3D{data.storage,
        CompressedPixelFormat::RGBABptcUnorm, data.size,
        data.dataSparse});

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CompressedBufferImage3D image = texture.compressedImage(0, {}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), (Vector3i{12, 4, 4}));

    {
        /* Same as in compressedSubImage3D(), see comment there for details */
        Int pixelStoreBlockPropertiesSet = 0;
        glGetIntegerv(GL_UNPACK_COMPRESSED_BLOCK_WIDTH, &pixelStoreBlockPropertiesSet);
        CORRADE_EXPECT_FAIL_IF((pixelStoreBlockPropertiesSet || data.storage != CompressedPixelStorage{}) && data.size != (Vector3i{12, 4, 4}) && (Context::current().detectedDriver() & Context::DetectedDriver::NVidia),
            "Compressed 3D texture upload behaves weirdly on NVidia if non-default pixel storage is used and not uploading the whole image.");
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData),
            Containers::arrayView(CompressedSubData3DComplete),
            TestSuite::Compare::Container);
    }
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::compressedSubImage3DQuery() {
    auto&& data = CompressedPixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::ARB::texture_compression_bptc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    Texture3D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBABptcUnorm, {12, 4, 4})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBABptcUnorm, {12, 4, 4}, CompressedSubData3DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 0}, Vector3i{4}),
        {data.storage});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.flags(), ImageFlags3D{});
    CORRADE_COMPARE(image.size(), (Vector3i{4}));

    {
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
}

void TextureGLTest::compressedSubImage3DQueryView() {
    auto&& data = CompressedPixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::ARB::texture_compression_bptc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    Texture3D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBABptcUnorm, {12, 4, 4})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBABptcUnorm, {12, 4, 4}, CompressedSubData3DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    Containers::Array<char> imageData{data.offset + 64};
    MutableCompressedImageView3D image{data.storage, CompressedPixelFormat::RGBABptcUnorm, Vector3i{4}, imageData, ImageFlag3D::Array};
    texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 0}, Vector3i{4}), image);

    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Doesn't matter what flags are set, they stay untouched */
    CORRADE_COMPARE(image.flags(), ImageFlag3D::Array);
    CORRADE_COMPARE(image.size(), (Vector3i{4}));

    {
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(image.data()).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
}

void TextureGLTest::compressedSubImage3DQueryBuffer() {
    auto&& data = CompressedPixelStorage3DData[testCaseInstanceId()];
    setTestCaseDescription(data.name);

    if(!Context::current().isExtensionSupported<Extensions::ARB::get_texture_sub_image>())
        CORRADE_SKIP(Extensions::ARB::get_texture_sub_image::string() << "is not supported.");
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_compression_bptc>())
        CORRADE_SKIP(Extensions::ARB::texture_compression_bptc::string() << "is not supported.");
    if(data.storage != CompressedPixelStorage{} && !Context::current().isExtensionSupported<Extensions::ARB::compressed_texture_pixel_storage>())
        CORRADE_SKIP(Extensions::ARB::compressed_texture_pixel_storage::string() << "is not supported.");

    Texture3D texture;
    texture.setStorage(1, TextureFormat::CompressedRGBABptcUnorm, {12, 4, 4})
           .setCompressedSubImage(0, {}, CompressedImageView3D{CompressedPixelFormat::RGBABptcUnorm, {12, 4, 4}, CompressedSubData3DComplete});

    MAGNUM_VERIFY_NO_GL_ERROR();

    CompressedBufferImage3D image = texture.compressedSubImage(0, Range3Di::fromSize({4, 0, 0}, Vector3i{4}),
        {data.storage}, BufferUsage::StaticRead);
    const auto imageData = image.buffer().data();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(image.size(), Vector3i{4});

    {
        CORRADE_COMPARE_AS(Containers::arrayCast<UnsignedByte>(imageData).exceptPrefix(data.offset),
            data.data,
            TestSuite::Compare::Container);
    }
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::generateMipmap1D() {
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() << "is not supported.");

    Texture1D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView1D(PixelFormat::RGBA, PixelType::UnsignedByte, 32));

    CORRADE_COMPARE(texture.imageSize(0), 32);
    CORRADE_COMPARE(texture.imageSize(1),  0);

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(texture.imageSize(0), 32);
    CORRADE_COMPARE(texture.imageSize(1), 16);
    CORRADE_COMPARE(texture.imageSize(2),  8);
    CORRADE_COMPARE(texture.imageSize(3),  4);
    CORRADE_COMPARE(texture.imageSize(4),  2);
    CORRADE_COMPARE(texture.imageSize(5),  1);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureGLTest::generateMipmap2D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() << "is not supported.");
    #endif

    Texture2D texture;
    texture.setImage(0,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::RGBA8,
        #else
        TextureFormat::RGBA,
        #endif
        ImageView2D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector2i(32)));

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i( 0));
    #endif

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector2i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector2i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector2i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector2i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector2i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector2i( 1));

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void TextureGLTest::generateMipmap3D() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setImage(0, TextureFormat::RGBA8,
        ImageView3D(PixelFormat::RGBA, PixelType::UnsignedByte, Vector3i(32)));

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i( 0));
    #endif

    texture.generateMipmap();

    MAGNUM_VERIFY_NO_GL_ERROR();

    /** @todo How to test this on ES? */
    #ifndef MAGNUM_TARGET_GLES
    CORRADE_COMPARE(texture.imageSize(0), Vector3i(32));
    CORRADE_COMPARE(texture.imageSize(1), Vector3i(16));
    CORRADE_COMPARE(texture.imageSize(2), Vector3i( 8));
    CORRADE_COMPARE(texture.imageSize(3), Vector3i( 4));
    CORRADE_COMPARE(texture.imageSize(4), Vector3i( 2));
    CORRADE_COMPARE(texture.imageSize(5), Vector3i( 1));

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::invalidateImage1D() {
    Texture1D texture;
    texture.setStorage(2, TextureFormat::RGBA8, 32);
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureGLTest::invalidateImage2D() {
    Texture2D texture;
    texture.setStorage(2,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::RGBA8,
        #else
        TextureFormat::RGBA,
        #endif
        Vector2i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void TextureGLTest::invalidateImage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateImage(1);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

#ifndef MAGNUM_TARGET_GLES
void TextureGLTest::invalidateSubImage1D() {
    Texture1D texture;
    texture.setStorage(2, TextureFormat::RGBA8, 32);
    texture.invalidateSubImage(1, 2, 8);

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureGLTest::invalidateSubImage2D() {
    Texture2D texture;
    texture.setStorage(2,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::RGBA8,
        #else
        TextureFormat::RGBA,
        #endif
        Vector2i(32));
    texture.invalidateSubImage(1, Vector2i(2), Vector2i(8));

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void TextureGLTest::invalidateSubImage3D() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::OES::texture_3D>())
        CORRADE_SKIP(Extensions::OES::texture_3D::string() << "is not supported.");
    #endif

    Texture3D texture;
    texture.setStorage(2, TextureFormat::RGBA8, Vector3i(32));
    texture.invalidateSubImage(1, Vector3i(2), Vector3i(8));

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void TextureGLTest::srgbStorage() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::EXT::sRGB>())
        CORRADE_SKIP(Extensions::EXT::sRGB::string() << "is not supported.");
    #endif

    Texture2D texture;
    texture.setImage(0,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::SRGB8
        #else
        TextureFormat::SRGB
        #endif
        , ImageView2D{
        #ifndef MAGNUM_TARGET_GLES2
        PixelFormat::RGB
        #else
        PixelFormat::SRGB
        #endif
        , PixelType::UnsignedByte, Vector2i{32}, {nullptr, 32*32*3}});

    MAGNUM_VERIFY_NO_GL_ERROR();

    #ifndef MAGNUM_TARGET_GLES2
    texture.setStorage(1, TextureFormat::SRGB8, Vector2i{32});

    MAGNUM_VERIFY_NO_GL_ERROR();
    #endif
}

void TextureGLTest::srgbAlphaStorage() {
    #ifdef MAGNUM_TARGET_GLES2
    if(!Context::current().isExtensionSupported<Extensions::EXT::sRGB>())
        CORRADE_SKIP(Extensions::EXT::sRGB::string() << "is not supported.");
    #endif

    Texture2D texture;
    texture.setImage(0,
        #ifndef MAGNUM_TARGET_GLES2
        TextureFormat::SRGB8Alpha8
        #else
        TextureFormat::SRGBAlpha
        #endif
        , ImageView2D{
        #ifndef MAGNUM_TARGET_GLES2
        PixelFormat::RGBA
        #else
        PixelFormat::SRGBAlpha
        #endif
        , PixelType::UnsignedByte, Vector2i{32}, {nullptr, 32*32*4}});

    MAGNUM_VERIFY_NO_GL_ERROR();

    texture.setStorage(1,
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        TextureFormat::SRGB8Alpha8,
        #else
        TextureFormat::SRGBAlpha,
        #endif
        Vector2i{32});

    MAGNUM_VERIFY_NO_GL_ERROR();
}

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::TextureGLTest)
