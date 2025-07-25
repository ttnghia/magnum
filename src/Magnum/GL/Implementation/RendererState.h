#ifndef Magnum_GL_Implementation_RendererState_h
#define Magnum_GL_Implementation_RendererState_h
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

#include "Magnum/GL/Renderer.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace GL { namespace Implementation {

struct ContextState;

struct RendererState {
    explicit RendererState(Context& context, ContextState& contextState, Containers::StaticArrayView<Implementation::ExtensionCount, const char*> extensions);

    Range1D(*lineWidthRangeImplementation)();
    /* These are direct pointers to the GL functions, so need a __stdcall on
       Windows to compile properly on 32 bits */
    #ifndef MAGNUM_TARGET_GLES
    void(APIENTRY *clearDepthImplementation)(GLdouble);
    void(APIENTRY *depthRangeImplementation)(GLdouble, GLdouble);
    #endif
    void(APIENTRY *clearDepthfImplementation)(GLfloat);
    void(APIENTRY *depthRangefImplementation)(GLfloat, GLfloat);
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(APIENTRY *minSampleShadingImplementation)(GLfloat);
    #endif
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    void(APIENTRY *patchParameteriImplementation)(GLenum, GLint);
    #endif
    #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
    void(APIENTRY *enableiImplementation)(GLenum, GLuint);
    void(APIENTRY *disableiImplementation)(GLenum, GLuint);
    void(APIENTRY *blendEquationiImplementation)(GLuint, GLenum);
    void(APIENTRY *blendEquationSeparateiImplementation)(GLuint, GLenum, GLenum);
    void(APIENTRY *blendFunciImplementation)(GLuint, GLenum, GLenum);
    void(APIENTRY *blendFuncSeparateiImplementation)(GLuint, GLenum, GLenum, GLenum, GLenum);
    void(APIENTRY *colorMaskiImplementation)(GLuint, GLboolean, GLboolean, GLboolean, GLboolean);
    #endif
    #ifdef MAGNUM_TARGET_GLES
    void(APIENTRY *polygonModeImplementation)(GLenum, GLenum);
    #endif
    #ifndef MAGNUM_TARGET_WEBGL
    GLenum(APIENTRY *graphicsResetStatusImplementation)();

    Renderer::ResetNotificationStrategy resetNotificationStrategy;
    #endif

    struct PixelStorage {
        enum: Int { DisengagedValue = -1 };

        explicit PixelStorage();

        void reset();

        Int alignment;
        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        Int rowLength;
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        Int imageHeight;
        Vector3i skip;
        #endif
        #ifndef MAGNUM_TARGET_GLES
        Vector3i compressedBlockSize;
        Int compressedBlockDataSize;
        Int disengagedBlockSize;
        #endif

        #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
        Int disengagedRowLength;
        #endif
    };

    PixelStorage packPixelStorage, unpackPixelStorage;
    Range1D lineWidthRange;
    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    GLint maxPatchVertexCount{};
    #endif
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    GLint maxClipDistances{};
    #endif
    #ifndef MAGNUM_TARGET_GLES2
    GLint maxCullDistances{}, maxCombinedClipAndCullDistances{};
    #endif

    /* Bool parameter is ugly, but this is implementation detail of internal
       API so who cares */
    void applyPixelStorageInternal(const Magnum::PixelStorage& storage, bool unpack);
    /* Used internally in *Texture::image(), *Texture::subImage(),
       *Texture::setImage(), *Texture::setSubImage() and *Framebuffer::read() */
    void applyPixelStoragePack(const Magnum::PixelStorage& storage) {
        applyPixelStorageInternal(storage, false);
    }
    void applyPixelStorageUnpack(const Magnum::PixelStorage& storage) {
        applyPixelStorageInternal(storage, true);
    }
    /* Deleted to avoid accidents -- use applyCompressedPixelStorage*() */
    void applyPixelStoragePack(const Magnum::CompressedPixelStorage&) = delete;
    void applyPixelStorageUnpack(const Magnum::CompressedPixelStorage&) = delete;

    /* Bool parameter is ugly, but this is implementation detail of internal
       API so who cares */
    void applyCompressedPixelStorageInternal(const CompressedPixelStorage& storage, const Vector3i& blockSize, Int blockDataSize, bool unpack);
    /* Used internally in *Texture::compressedImage(),
       *Texture::compressedSubImage(), *Texture::setCompressedImage() and
       *Texture::setCompressedSubImage(). The overload with explicit block
       properties is used in APIs that take an Image& and which replace it with
       am image of a new format along with its properties. */
    void applyCompressedPixelStoragePack(const CompressedPixelStorage& storage, const Vector3i& blockSize, Int blockDataSize) {
        applyCompressedPixelStorageInternal(storage, blockSize, blockDataSize, false);
    }
    template<class T> void applyCompressedPixelStoragePack(const T& image) {
        applyCompressedPixelStoragePack(image.storage(), image.blockSize(), image.blockDataSize());
    }
    template<class T> void applyCompressedPixelStorageUnpack(const T& image) {
        applyCompressedPixelStorageInternal(image.storage(), image.blockSize(), image.blockDataSize(), true);
    }
};

}}}

#endif
