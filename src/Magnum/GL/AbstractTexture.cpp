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

#include "AbstractTexture.h"

#include <Corrade/Containers/Array.h>
#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif

#include "Magnum/Image.h"
#include "Magnum/ImageView.h"
#include "Magnum/Implementation/ImageProperties.h"
#ifndef MAGNUM_TARGET_GLES2
#include "Magnum/GL/BufferImage.h"
#endif
#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/PixelFormat.h"
#include "Magnum/GL/TextureFormat.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif
#include "Magnum/GL/Implementation/imageProperties.h"
#include "Magnum/GL/Implementation/RendererState.h"
#include "Magnum/GL/Implementation/State.h"
#include "Magnum/GL/Implementation/TextureState.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Math/Range.h"

namespace Magnum { namespace GL {

#ifndef MAGNUM_TARGET_GLES2
Float AbstractTexture::maxLodBias() {
    GLfloat& value = Context::current().state().texture.maxLodBias;

    /* Get the value, if not already cached */
    if(value == 0.0f)
        glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &value);

    return value;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Int AbstractTexture::maxColorSamples() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_multisample>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().texture.maxColorSamples;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &value);

    return value;
}

Int AbstractTexture::maxDepthSamples() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_multisample>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().texture.maxDepthSamples;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &value);

    return value;
}

Int AbstractTexture::maxIntegerSamples() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::texture_multisample>())
    #else
    if(!Context::current().isVersionSupported(Version::GLES310))
    #endif
        return 0;

    GLint& value = Context::current().state().texture.maxIntegerSamples;

    /* Get the value, if not already cached */
    if(value == 0)
        glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &value);

    return value;
}
#endif

void AbstractTexture::unbind(const Int textureUnit) {
    Implementation::TextureState& textureState = Context::current().state().texture;

    /* If given texture unit is already unbound, nothing to do */
    if(textureState.bindings[textureUnit].second() == 0)
        return;

    /* Unbind the texture, reset state tracker */
    Context::current().state().texture.unbindImplementation(textureUnit);
    textureState.bindings[textureUnit] = {};
}

void AbstractTexture::unbindImplementationDefault(const GLint textureUnit) {
    Implementation::TextureState& textureState = Context::current().state().texture;

    /* Activate given texture unit if not already active, update state tracker */
    if(textureState.currentTextureUnit != textureUnit)
        glActiveTexture(GL_TEXTURE0 + (textureState.currentTextureUnit = textureUnit));

    CORRADE_INTERNAL_ASSERT(textureState.bindings[textureUnit].first() != 0);
    glBindTexture(textureState.bindings[textureUnit].first(), 0);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::unbindImplementationMulti(const GLint textureUnit) {
    constexpr static GLuint zero = 0;
    glBindTextures(textureUnit, 1, &zero);
}

void AbstractTexture::unbindImplementationDSA(const GLint textureUnit) {
    CORRADE_INTERNAL_ASSERT(Context::current().state().texture.bindings[textureUnit].first() != 0);
    glBindTextureUnit(textureUnit, 0);
}
#endif

void AbstractTexture::unbind(const Int firstTextureUnit, const std::size_t count) {
    /* State tracker is updated in the implementations */
    Context::current().state().texture.bindMultiImplementation(firstTextureUnit, {nullptr, count});
}

void AbstractTexture::bind(const Int firstTextureUnit, Containers::ArrayView<AbstractTexture* const> textures) {
    /* State tracker is updated in the implementations */
    Context::current().state().texture.bindMultiImplementation(firstTextureUnit, {textures.begin(), textures.size()});
}

void AbstractTexture::bind(const Int firstTextureUnit, const std::initializer_list<AbstractTexture*> textures) {
    bind(firstTextureUnit, Containers::arrayView(textures));
}

void AbstractTexture::bindImplementationFallback(const GLint firstTextureUnit, const Containers::ArrayView<AbstractTexture* const> textures) {
    for(std::size_t i = 0; i != textures.size(); ++i)
        textures && textures[i] ? textures[i]->bind(firstTextureUnit + i) : unbind(firstTextureUnit + i);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::bindImplementationMulti(const GLint firstTextureUnit, const Containers::ArrayView<AbstractTexture* const> textures) {
    Implementation::TextureState& textureState = Context::current().state().texture;

    /* Create array of IDs and also update bindings in state tracker */
    /** @todo VLAs */
    Containers::Array<GLuint> ids{textures ? textures.size() : 0};
    bool different = false;
    for(std::size_t i = 0; i != textures.size(); ++i) {
        const GLuint id = textures && textures[i] ? textures[i]->_id : 0;

        if(textures) {
            if(textures[i]) textures[i]->createIfNotAlready();
            ids[i] = id;
        }

        if(textureState.bindings[firstTextureUnit + i].second() != id) {
            different = true;
            textureState.bindings[firstTextureUnit + i].second() = id;
        }
    }

    /* Avoid doing the binding if there is nothing different */
    if(different) glBindTextures(firstTextureUnit, textures.size(), ids);
}
#endif

#ifndef MAGNUM_TARGET_GLES
Int AbstractTexture::compressedBlockDataSize(const GLenum target, const TextureFormat format) {
    return (Context::current().state().texture.compressedBlockDataSizeImplementation)(target, format);
}

Int AbstractTexture::compressedBlockDataSizeImplementationDefault(const GLenum target, const TextureFormat format) {
    GLint value;
    glGetInternalformativ(target, GLenum(format), GL_TEXTURE_COMPRESSED_BLOCK_SIZE, 1, &value);
    return value;
}

Int AbstractTexture::compressedBlockDataSizeImplementationBitsWorkaround(const GLenum target, const TextureFormat format) {
    /* NVidia (358.16) reports the value in bits instead of bytes */
    return compressedBlockDataSizeImplementationDefault(target, format)/8;
}
#endif

AbstractTexture::AbstractTexture(GLenum target): _target{target}, _flags{ObjectFlag::DeleteOnDestruction} {
    Context::current().state().texture.createImplementation(*this);
    CORRADE_INTERNAL_ASSERT(_id != Implementation::State::DisengagedBinding);
}

void AbstractTexture::createImplementationDefault(AbstractTexture& self) {
    glGenTextures(1, &self._id);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::createImplementationDSA(AbstractTexture& self) {
    glCreateTextures(self._target, 1, &self._id);
    self._flags |= ObjectFlag::Created;
}
#endif

AbstractTexture::~AbstractTexture() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction))
        return;

    /* Remove all bindings */
    for(auto& binding: Context::current().state().texture.bindings) {
        /* MSVC 2015 needs the parentheses around */
        if(binding.second() == _id) binding = {};
    }

    #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
    /* Remove all image bindings */
    for(auto& binding: Context::current().state().texture.imageBindings) {
        /* MSVC 2015 needs the parentheses around */
        if(binding.id == _id) binding = {};
    }
    #endif

    glDeleteTextures(1, &_id);
}

#ifndef MAGNUM_TARGET_WEBGL
void AbstractTexture::createIfNotAlready() {
    if(_flags & ObjectFlag::Created)
        return;

    /* glGen*() does not create the object, just reserves the name. Some
       commands (such as glBindTextures() or glObjectLabel()) operate with IDs
       directly and they require the object to be created. Binding the texture
       to desired target finally creates it. */
    bindInternal();

    /* In some cases, such as when this function is called on a object created
       using wrap(), ObjectFlag::Created might not be set but bindInternal()
       above was a no-op as the object was already bound somewhere. In that
       case assume that, since it's bound, it's already created, and we just
       didn't know. See the wrapCreateIfNotAlready() test for a repro case.

       Note that the branch is done this way instead of an unconditional |= to
       make code coverage report that this codepath is indeed tested. */
    if(!(_flags >= ObjectFlag::Created))
        _flags |= ObjectFlag::Created;
}
#endif

#ifndef MAGNUM_TARGET_WEBGL
Containers::String AbstractTexture::label() {
    createIfNotAlready();
    return Context::current().state().debug.getLabelImplementation(GL_TEXTURE, _id);
}

AbstractTexture& AbstractTexture::setLabel(const Containers::StringView label) {
    createIfNotAlready();
    Context::current().state().debug.labelImplementation(GL_TEXTURE, _id, label);
    return *this;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::unbindImage(const Int imageUnit) {
    Implementation::TextureState& textureState = Context::current().state().texture;

    /* If already unbound in given image unit, nothing to do */
    if(textureState.imageBindings[imageUnit].id == 0)
        return;

    /* Update state tracker, bind the texture to the unit */
    textureState.imageBindings[imageUnit].id = 0;
    glBindImageTexture(imageUnit, 0, 0, false, 0, GL_READ_ONLY, GL_RGBA8);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::unbindImages(const Int firstImageUnit, const std::size_t count) {
    bindImages(firstImageUnit, {nullptr, count});
}

void AbstractTexture::bindImages(const Int firstImageUnit, const Containers::ArrayView<AbstractTexture* const> textures) {
    Implementation::TextureState& textureState = Context::current().state().texture;

    /* Create array of IDs and also update bindings in state tracker */
    Containers::Array<GLuint> ids{textures ? textures.size() : 0};
    bool different = false;
    for(std::size_t i = 0; i != textures.size(); ++i) {
        const Implementation::TextureState::ImageBinding state = textures && textures[i] ?
            Implementation::TextureState::ImageBinding{textures[i]->_id, 0, true, 0, GL_READ_WRITE} :
            Implementation::TextureState::ImageBinding{0, 0, false, 0, GL_READ_ONLY};

        if(textures) {
            if(textures[i]) {
                textures[i]->createIfNotAlready();
            }
            ids[i] = state.id;
        }

        if(textureState.imageBindings[firstImageUnit + i] != state) {
            different = true;
            textureState.imageBindings[firstImageUnit + i] = state;
        }
    }

    /* Avoid doing the binding if there is nothing different */
    if(different) glBindImageTextures(firstImageUnit, textures.size(), ids);
}

void AbstractTexture::bindImages(const Int firstImageUnit, const std::initializer_list<AbstractTexture*> textures) {
    bindImages(firstImageUnit, Containers::arrayView(textures));
}
#endif

void AbstractTexture::bindImageInternal(const Int imageUnit, const Int level, const bool layered, const Int layer, const ImageAccess access, const ImageFormat format) {
    Implementation::TextureState& textureState = Context::current().state().texture;
    const Implementation::TextureState::ImageBinding state{_id, level, layered, layer, GLenum(access)};

    /* If already bound in given texture unit, nothing to do */
    if(textureState.imageBindings[imageUnit] == state)
        return;

    /* Update state tracker, bind the texture to the unit */
    textureState.imageBindings[imageUnit] = state;
    glBindImageTexture(imageUnit, _id, level, layered, layer, GLenum(access), GLenum(format));
}
#endif

void AbstractTexture::bind(Int textureUnit) {
    Implementation::TextureState& textureState = Context::current().state().texture;

    /* If already bound in given texture unit, nothing to do */
    if(textureState.bindings[textureUnit].second() == _id)
        return;

    /* Bind the texture to the unit, *then* update the state tracker. The order
       is important, as if bindImplementationMulti() is used, it calls into
       createIfNotAlready() which then, if the state tracker would be already
       updated, would see that the texture is bound already, and thus wouldn't
       bind it at all, never actually creating it. */
    textureState.bindImplementation(*this, textureUnit);
    textureState.bindings[textureUnit] = {_target, _id};
}

void AbstractTexture::bindImplementationDefault(AbstractTexture& self, GLint textureUnit) {
    Implementation::TextureState& textureState = Context::current().state().texture;

    /* Activate given texture unit if not already active, update state tracker */
    if(textureState.currentTextureUnit != textureUnit)
        glActiveTexture(GL_TEXTURE0 + (textureState.currentTextureUnit = textureUnit));

    /* Binding the texture finally creates it */
    self._flags |= ObjectFlag::Created;
    glBindTexture(self._target, self._id);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::bindImplementationMulti(AbstractTexture& self, GLint textureUnit) {
    /* ARB_multi_bind doesn't create the texture inside glBindImageTextures()
       because it doesn't know what for what target it should be created (a 2D
       texture, a cubemap, ...). So we need to ensure it's created first, which
       is either implicitly if using ARB_DSA or by calling glBindTexture(). */
    self.createIfNotAlready();
    glBindTextures(textureUnit, 1, &self._id);
}

void AbstractTexture::bindImplementationDSA(AbstractTexture& self, const GLint textureUnit) {
    glBindTextureUnit(textureUnit, self._id);
}

#ifdef CORRADE_TARGET_WINDOWS
void AbstractTexture::bindImplementationDSAIntelWindows(AbstractTexture& self, const GLint textureUnit) {
    /* See the "intel-windows-half-baked-dsa-texture-bind" workaround */
    if(self._target == GL_TEXTURE_CUBE_MAP) bindImplementationDefault(self, textureUnit);
    else bindImplementationDSA(self, textureUnit);
}
#endif

#ifdef CORRADE_TARGET_APPLE
void AbstractTexture::bindImplementationAppleBufferTextureWorkaround(AbstractTexture& self, const GLint textureUnit) {
    bindImplementationDefault(self, textureUnit);
    if(self._target == GL_TEXTURE_BUFFER)
        Context::current().state().texture.bufferTextureBound.set(textureUnit, true);
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::setBaseLevel(Int level) {
    Context::current().state().texture.parameteriImplementation(*this, GL_TEXTURE_BASE_LEVEL, level);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::setMaxLevel(Int level) {
    Context::current().state().texture.parameteriImplementation(*this,
        #ifndef MAGNUM_TARGET_GLES2
        GL_TEXTURE_MAX_LEVEL
        #else
        GL_TEXTURE_MAX_LEVEL_APPLE
        #endif
    , level);
}
#endif

void AbstractTexture::setMinificationFilter(SamplerFilter filter, SamplerMipmap mipmap) {
    Context::current().state().texture.parameteriImplementation(*this, GL_TEXTURE_MIN_FILTER, GLint(filter)|GLint(mipmap));
}

void AbstractTexture::setMagnificationFilter(const SamplerFilter filter) {
    Context::current().state().texture.parameteriImplementation(*this, GL_TEXTURE_MAG_FILTER, GLint(filter));
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::setMinLod(const Float lod) {
    Context::current().state().texture.parameterfImplementation(*this, GL_TEXTURE_MIN_LOD, lod);
}

void AbstractTexture::setMaxLod(const Float lod) {
    Context::current().state().texture.parameterfImplementation(*this, GL_TEXTURE_MAX_LOD, lod);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::setLodBias(const Float bias) {
    Context::current().state().texture.parameterfImplementation(*this, GL_TEXTURE_LOD_BIAS, bias);
}
#endif

#ifndef MAGNUM_TARGET_WEBGL
void AbstractTexture::setBorderColor(const Color4& color) {
    Context::current().state().texture.parameterfvImplementation(*this,
        #ifndef MAGNUM_TARGET_GLES2
        GL_TEXTURE_BORDER_COLOR,
        #else
        GL_TEXTURE_BORDER_COLOR_EXT,
        #endif
        color.data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::setBorderColor(const Vector4ui& color) {
    Context::current().state().texture.parameterIuivImplementation(*this, GL_TEXTURE_BORDER_COLOR, color.data());
}

void AbstractTexture::setBorderColor(const Vector4i& color) {
    Context::current().state().texture.parameterIivImplementation(*this, GL_TEXTURE_BORDER_COLOR, color.data());
}
#endif
#endif

void AbstractTexture::setMaxAnisotropy(const Float anisotropy) {
    Context::current().state().texture.setMaxAnisotropyImplementation(*this, anisotropy);
}

#ifndef MAGNUM_TARGET_WEBGL
void AbstractTexture::setSrgbDecode(bool decode) {
    Context::current().state().texture.parameteriImplementation(*this, GL_TEXTURE_SRGB_DECODE_EXT,
        decode ? GL_DECODE_EXT : GL_SKIP_DECODE_EXT);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::setSwizzleInternal(const GLint r, const GLint g, const GLint b, const GLint a) {
    #ifndef MAGNUM_TARGET_GLES
    const GLint rgba[] = {r, g, b, a};
    Context::current().state().texture.parameterivImplementation(*this, GL_TEXTURE_SWIZZLE_RGBA, rgba);
    #else
    Context::current().state().texture.parameteriImplementation(*this, GL_TEXTURE_SWIZZLE_R, r);
    Context::current().state().texture.parameteriImplementation(*this, GL_TEXTURE_SWIZZLE_G, g);
    Context::current().state().texture.parameteriImplementation(*this, GL_TEXTURE_SWIZZLE_B, b);
    Context::current().state().texture.parameteriImplementation(*this, GL_TEXTURE_SWIZZLE_A, a);
    #endif
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::setCompareMode(const SamplerCompareMode mode) {
    Context::current().state().texture.parameteriImplementation(*this,
        #ifndef MAGNUM_TARGET_GLES2
        GL_TEXTURE_COMPARE_MODE
        #else
        GL_TEXTURE_COMPARE_MODE_EXT
        #endif
        , GLenum(mode));
}

void AbstractTexture::setCompareFunction(const SamplerCompareFunction function) {
    Context::current().state().texture.parameteriImplementation(*this,
        #ifndef MAGNUM_TARGET_GLES2
        GL_TEXTURE_COMPARE_FUNC
        #else
        GL_TEXTURE_COMPARE_FUNC_EXT
        #endif
        , GLenum(function));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::setDepthStencilMode(const SamplerDepthStencilMode mode) {
    Context::current().state().texture.parameteriImplementation(*this,
        #ifndef MAGNUM_TARGET_WEBGL
        GL_DEPTH_STENCIL_TEXTURE_MODE
        #else
        GL_DEPTH_STENCIL_TEXTURE_MODE_ANGLE
        #endif
        , GLenum(mode));
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::viewInternal(AbstractTexture& original, const TextureFormat internalFormat, const Int levelOffset, const Int levelCount, const Int layerOffset, const Int layerCount) {
    Context::current().state().texture.viewImplementation(_id, _target, original._id, GLenum(internalFormat), levelOffset, levelCount, layerOffset, layerCount);
}
#endif

void AbstractTexture::invalidateImage(const Int level) {
    Context::current().state().texture.invalidateImageImplementation(*this, level);
}

void AbstractTexture::generateMipmap() {
    Context::current().state().texture.mipmapImplementation(*this);
}

void AbstractTexture::mipmapImplementationDefault(AbstractTexture& self) {
    self.bindInternal();
    glGenerateMipmap(self._target);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::mipmapImplementationDSA(AbstractTexture& self) {
    glGenerateTextureMipmap(self._id);
}
#endif

void AbstractTexture::bindInternal() {
    /* Using glBindTextures() here is meaningless, because the non-DSA
       functions need to have the texture bound in *currently active* unit,
       so we would need to call glActiveTexture() afterwards anyway. */

    Implementation::TextureState& textureState = Context::current().state().texture;

    /* If the texture is already bound in current unit, nothing to do */
    if(textureState.bindings[textureState.currentTextureUnit].second() == _id)
        return;

    /* Set internal unit as active if not already, update state tracker */
    CORRADE_INTERNAL_ASSERT(textureState.maxTextureUnits > 1);
    const GLint internalTextureUnit = textureState.maxTextureUnits-1;
    if(textureState.currentTextureUnit != internalTextureUnit)
        glActiveTexture(GL_TEXTURE0 + (textureState.currentTextureUnit = internalTextureUnit));

    /* If already bound in given texture unit, nothing to do */
    if(textureState.bindings[internalTextureUnit].second() == _id)
        return;

    /* Update state tracker, bind the texture to the unit. Not directly calling
       glBindTexture() here because we may need to include various
       platform-specific workarounds (Apple, Intel Windows), also can't just
       reuse textureState.bindImplementation as we *need* to call
       glBindTexture() in order to create it and have ObjectFlag::Created set
       (which is then asserted in createIfNotAlready()) */
    textureState.bindings[internalTextureUnit] = {_target, _id};
    textureState.bindInternalImplementation(*this, internalTextureUnit);
}

#if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
namespace {

PixelFormat pixelFormatForInternalFormat(const TextureFormat internalFormat) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(internalFormat) {
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::Red:
        case TextureFormat::R8:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8Snorm:
        #endif
        #if !defined(MAGNUM_TARGET_WEBGL) && !defined(MAGNUM_TARGET_GLES2)
        case TextureFormat::SR8:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16:
        case TextureFormat::R16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16F:
        case TextureFormat::R32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRed:
        #endif
        /* for WebGL 1 these two are in the Luminance case instead */
        case TextureFormat::CompressedR11Eac:
        case TextureFormat::CompressedSignedR11Eac:
            return PixelFormat::Red;
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::CompressedRedRgtc1:
        case TextureFormat::CompressedSignedRedRgtc1:
            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            return PixelFormat::Red;
            #else
            /* RGTC is on WebGL 1 but there's no Red pixel format (which is
               okay because WebGL doesn't allow compression by upload anyway).
               Assert here to have the enum value handled. */
            CORRADE_ASSERT_UNREACHABLE("No single-component pixel format in WebGL 1 for RGTC compression", {});
            #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8UI:
        case TextureFormat::R8I:
        case TextureFormat::R16UI:
        case TextureFormat::R16I:
        case TextureFormat::R32UI:
        case TextureFormat::R32I:
            return PixelFormat::RedInteger;
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::RG:
        case TextureFormat::RG8:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RG8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::SRG8:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RG16:
        case TextureFormat::RG16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RG16F:
        case TextureFormat::RG32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRG:
        #endif
        /* for WebGL 1 these two are in the LuminanceAlpha case instead */
        case TextureFormat::CompressedRG11Eac:
        case TextureFormat::CompressedSignedRG11Eac:
            return PixelFormat::RG;
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::CompressedRGRgtc2:
        case TextureFormat::CompressedSignedRGRgtc2:
            #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
            return PixelFormat::RG;
            #else
            /* RGTC is on WebGL 1 but there's no RG pixel format (which is okay
               because WebGL doesn't allow compression by upload anyway).
               Assert here to have the enum value handled. */
            CORRADE_ASSERT_UNREACHABLE("No two-component pixel format in WebGL 1 for RGTC compression", {});
            #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RG8UI:
        case TextureFormat::RG8I:
        case TextureFormat::RG16UI:
        case TextureFormat::RG16I:
        case TextureFormat::RG32UI:
        case TextureFormat::RG32I:
            return PixelFormat::RGInteger;
        #endif

        case TextureFormat::RGB:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::RGB8:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB16:
        case TextureFormat::RGB16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB16F:
        case TextureFormat::RGB32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R3G3B2:
        case TextureFormat::RGB4:
        case TextureFormat::RGB5:
        #endif
        case TextureFormat::RGB565:
        #if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
        case TextureFormat::RGB10:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB12:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R11FG11FB10F:
        case TextureFormat::RGB9E5:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGB:
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::CompressedRGBBptcUnsignedFloat:
        case TextureFormat::CompressedRGBBptcSignedFloat:
        #endif
        case TextureFormat::CompressedRGB8Etc2:
        case TextureFormat::CompressedRGBS3tcDxt1:
        #ifdef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGBPvrtc2bppV1:
        case TextureFormat::CompressedRGBPvrtc4bppV1:
        #endif
            return PixelFormat::RGB;

        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        case TextureFormat::SRGB:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::SRGB8:
        #endif
        case TextureFormat::CompressedSRGB8Etc2:
        case TextureFormat::CompressedSRGBS3tcDxt1:
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::CompressedSRGBPvrtc2bppV1:
        case TextureFormat::CompressedSRGBPvrtc4bppV1:
        #endif
            #ifndef MAGNUM_TARGET_GLES2
            return PixelFormat::RGB;
            #else
            return PixelFormat::SRGB;
            #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB8UI:
        case TextureFormat::RGB8I:
        case TextureFormat::RGB16UI:
        case TextureFormat::RGB16I:
        case TextureFormat::RGB32UI:
        case TextureFormat::RGB32I:
            return PixelFormat::RGBInteger;
        #endif

        case TextureFormat::RGBA:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::RGBA8:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGBA8Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGBA16:
        case TextureFormat::RGBA16Snorm:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGBA16F:
        case TextureFormat::RGBA32F:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA2:
        #endif
        case TextureFormat::RGBA4:
        case TextureFormat::RGB5A1:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::RGB10A2:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA12:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGBA:
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::CompressedRGBABptcUnorm:
        #endif
        case TextureFormat::CompressedRGB8PunchthroughAlpha1Etc2:
        case TextureFormat::CompressedRGBA8Etc2Eac:
        case TextureFormat::CompressedRGBAS3tcDxt1:
        case TextureFormat::CompressedRGBAS3tcDxt3:
        case TextureFormat::CompressedRGBAS3tcDxt5:
        case TextureFormat::CompressedRGBAAstc4x4:
        case TextureFormat::CompressedRGBAAstc5x4:
        case TextureFormat::CompressedRGBAAstc5x5:
        case TextureFormat::CompressedRGBAAstc6x5:
        case TextureFormat::CompressedRGBAAstc6x6:
        case TextureFormat::CompressedRGBAAstc8x5:
        case TextureFormat::CompressedRGBAAstc8x6:
        case TextureFormat::CompressedRGBAAstc8x8:
        case TextureFormat::CompressedRGBAAstc10x5:
        case TextureFormat::CompressedRGBAAstc10x6:
        case TextureFormat::CompressedRGBAAstc10x8:
        case TextureFormat::CompressedRGBAAstc10x10:
        case TextureFormat::CompressedRGBAAstc12x10:
        case TextureFormat::CompressedRGBAAstc12x12:
        #ifdef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGBAPvrtc2bppV1:
        case TextureFormat::CompressedRGBAPvrtc4bppV1:
        #endif
            return PixelFormat::RGBA;

        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        case TextureFormat::SRGBAlpha:
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::SRGB8Alpha8:
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::CompressedSRGBAlphaBptcUnorm:
        #endif
        case TextureFormat::CompressedSRGB8PunchthroughAlpha1Etc2:
        case TextureFormat::CompressedSRGB8Alpha8Etc2Eac:
        case TextureFormat::CompressedSRGBAlphaS3tcDxt1:
        case TextureFormat::CompressedSRGBAlphaS3tcDxt3:
        case TextureFormat::CompressedSRGBAlphaS3tcDxt5:
        case TextureFormat::CompressedSRGB8Alpha8Astc4x4:
        case TextureFormat::CompressedSRGB8Alpha8Astc5x4:
        case TextureFormat::CompressedSRGB8Alpha8Astc5x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc6x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc6x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x8:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x8:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x10:
        case TextureFormat::CompressedSRGB8Alpha8Astc12x10:
        case TextureFormat::CompressedSRGB8Alpha8Astc12x12:
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::CompressedSRGBAlphaPvrtc2bppV1:
        case TextureFormat::CompressedSRGBAlphaPvrtc4bppV1:
        #endif
            #ifndef MAGNUM_TARGET_GLES2
            return PixelFormat::RGBA;
            #else
            return PixelFormat::SRGBAlpha;
            #endif

        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::BGRA:
        case TextureFormat::BGRA8:
            return PixelFormat::BGRA;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGBA8UI:
        case TextureFormat::RGBA8I:
        case TextureFormat::RGBA16UI:
        case TextureFormat::RGBA16I:
        case TextureFormat::RGBA32UI:
        case TextureFormat::RGBA32I:
        case TextureFormat::RGB10A2UI:
            return PixelFormat::RGBAInteger;
        #endif

        #ifdef MAGNUM_TARGET_GLES2
        case TextureFormat::Luminance:
        #ifdef MAGNUM_TARGET_WEBGL
        /* Not sure if this is the right place, as neither of the
           {ANGLE,WEBGL}_compressed_texture_etc specifications says what the corresponding pixel format is meant to be. For desktop, ES and
           WebGL 2 these two are in the Red case instead. */
        case TextureFormat::CompressedR11Eac:
        case TextureFormat::CompressedSignedR11Eac:
        #endif
            return PixelFormat::Luminance;
        case TextureFormat::LuminanceAlpha:
        #ifdef MAGNUM_TARGET_WEBGL
        /* Not sure if this is the right place, as neither of the
           {ANGLE,WEBGL}_compressed_texture_etc specifications says what the corresponding pixel format is meant to be. For desktop, ES and
           WebGL 2 these two are in the RG case instead. */
        case TextureFormat::CompressedRG11Eac:
        case TextureFormat::CompressedSignedRG11Eac:
        #endif
            return PixelFormat::LuminanceAlpha;
        #endif

        case TextureFormat::DepthComponent:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::DepthComponent16:
        case TextureFormat::DepthComponent24:
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::DepthComponent32:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::DepthComponent32F:
        #endif
            return PixelFormat::DepthComponent;

        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::StencilIndex8:
            return PixelFormat::StencilIndex;
        #endif

        case TextureFormat::DepthStencil:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::Depth24Stencil8:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::Depth32FStencil8:
        #endif
            return PixelFormat::DepthStencil;
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

PixelType pixelTypeForInternalFormat(const TextureFormat internalFormat) {
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic error "-Wswitch"
    #endif
    switch(internalFormat) {
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::Red:
        case TextureFormat::RG:
        #endif
        case TextureFormat::RGB:
        case TextureFormat::RGBA:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::R8:
        case TextureFormat::RG8:
        case TextureFormat::RGB8:
        case TextureFormat::RGBA8:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8UI:
        case TextureFormat::RG8UI:
        case TextureFormat::RGB8UI:
        case TextureFormat::RGBA8UI:
        #endif
        #ifdef MAGNUM_TARGET_GLES2
        case TextureFormat::Luminance:
        case TextureFormat::LuminanceAlpha:
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::SR8:
        case TextureFormat::SRG8:
        #endif
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        case TextureFormat::SRGB:
        case TextureFormat::SRGBAlpha:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::SRGB8:
        #endif
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::SRGB8Alpha8:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA2: /**< @todo really? */
        #endif
        #if defined(MAGNUM_TARGET_GLES) && !defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::BGRA:
        case TextureFormat::BGRA8:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRed:
        case TextureFormat::CompressedRG:
        case TextureFormat::CompressedRGB:
        case TextureFormat::CompressedRGBA:
        #endif
        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::CompressedRedRgtc1:
        case TextureFormat::CompressedRGRgtc2:
        case TextureFormat::CompressedRGBABptcUnorm:
        case TextureFormat::CompressedSRGBAlphaBptcUnorm:
        #endif
        case TextureFormat::CompressedRGB8Etc2:
        case TextureFormat::CompressedSRGB8Etc2:
        case TextureFormat::CompressedRGB8PunchthroughAlpha1Etc2:
        case TextureFormat::CompressedSRGB8PunchthroughAlpha1Etc2:
        case TextureFormat::CompressedRGBA8Etc2Eac:
        case TextureFormat::CompressedSRGB8Alpha8Etc2Eac:
        case TextureFormat::CompressedR11Eac:
        case TextureFormat::CompressedSignedR11Eac:
        case TextureFormat::CompressedRG11Eac:
        case TextureFormat::CompressedSignedRG11Eac:
        case TextureFormat::CompressedRGBS3tcDxt1:
        case TextureFormat::CompressedSRGBS3tcDxt1:
        case TextureFormat::CompressedRGBAS3tcDxt1:
        case TextureFormat::CompressedSRGBAlphaS3tcDxt1:
        case TextureFormat::CompressedRGBAS3tcDxt3:
        case TextureFormat::CompressedSRGBAlphaS3tcDxt3:
        case TextureFormat::CompressedRGBAS3tcDxt5:
        case TextureFormat::CompressedSRGBAlphaS3tcDxt5:
        case TextureFormat::CompressedRGBAAstc4x4:
        case TextureFormat::CompressedSRGB8Alpha8Astc4x4:
        case TextureFormat::CompressedRGBAAstc5x4:
        case TextureFormat::CompressedSRGB8Alpha8Astc5x4:
        case TextureFormat::CompressedRGBAAstc5x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc5x5:
        case TextureFormat::CompressedRGBAAstc6x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc6x5:
        case TextureFormat::CompressedRGBAAstc6x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc6x6:
        case TextureFormat::CompressedRGBAAstc8x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x5:
        case TextureFormat::CompressedRGBAAstc8x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x6:
        case TextureFormat::CompressedRGBAAstc8x8:
        case TextureFormat::CompressedSRGB8Alpha8Astc8x8:
        case TextureFormat::CompressedRGBAAstc10x5:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x5:
        case TextureFormat::CompressedRGBAAstc10x6:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x6:
        case TextureFormat::CompressedRGBAAstc10x8:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x8:
        case TextureFormat::CompressedRGBAAstc10x10:
        case TextureFormat::CompressedSRGB8Alpha8Astc10x10:
        case TextureFormat::CompressedRGBAAstc12x10:
        case TextureFormat::CompressedSRGB8Alpha8Astc12x10:
        case TextureFormat::CompressedRGBAAstc12x12:
        case TextureFormat::CompressedSRGB8Alpha8Astc12x12:
        #ifdef MAGNUM_TARGET_GLES
        case TextureFormat::CompressedRGBPvrtc2bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::CompressedSRGBPvrtc2bppV1:
        #endif
        case TextureFormat::CompressedRGBAPvrtc2bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::CompressedSRGBAlphaPvrtc2bppV1:
        #endif
        case TextureFormat::CompressedRGBPvrtc4bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::CompressedSRGBPvrtc4bppV1:
        #endif
        case TextureFormat::CompressedRGBAPvrtc4bppV1:
        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::CompressedSRGBAlphaPvrtc4bppV1:
        #endif
        #endif
            return PixelType::UnsignedByte;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R8Snorm:
        case TextureFormat::RG8Snorm:
        case TextureFormat::RGB8Snorm:
        case TextureFormat::RGBA8Snorm:
        case TextureFormat::R8I:
        case TextureFormat::RG8I:
        case TextureFormat::RGB8I:
        case TextureFormat::RGBA8I:
            return PixelType::Byte;
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        case TextureFormat::CompressedSignedRedRgtc1:
        case TextureFormat::CompressedSignedRGRgtc2:
            #ifndef MAGNUM_TARGET_GLES2
            return PixelType::Byte;
            #else
            /* RGTC is on WebGL 1 but there's no RG pixel format (which is okay
               because WebGL doesn't allow compression by upload anyway).
               Assert here to have the enum value handled. */
            CORRADE_ASSERT_UNREACHABLE("No signed pixel type in OpenGL ES 2.0 for RGTC compression", {});
            #endif
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16:
        case TextureFormat::RG16:
        case TextureFormat::RGB16:
        case TextureFormat::RGBA16:
        #endif
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16UI:
        case TextureFormat::RG16UI:
        case TextureFormat::RGB16UI:
        case TextureFormat::RGBA16UI:
        #endif
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB12: /**< @todo really? */
        #endif
        case TextureFormat::RGBA4: /**< @todo really? */
        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGBA12: /**< @todo really? */
        #endif
            return PixelType::UnsignedShort;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16Snorm:
        case TextureFormat::RG16Snorm:
        case TextureFormat::RGB16Snorm:
        case TextureFormat::RGBA16Snorm:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16I:
        case TextureFormat::RG16I:
        case TextureFormat::RGB16I:
        case TextureFormat::RGBA16I:
        #endif
            return PixelType::Short;
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R16F:
        case TextureFormat::RG16F:
        case TextureFormat::RGB16F:
        case TextureFormat::RGBA16F:
            return PixelType::Half;

        case TextureFormat::R32UI:
        case TextureFormat::RG32UI:
        case TextureFormat::RGB32UI:
        case TextureFormat::RGBA32UI:
            return PixelType::UnsignedInt;

        case TextureFormat::R32I:
        case TextureFormat::RG32I:
        case TextureFormat::RGB32I:
        case TextureFormat::RGBA32I:
            return PixelType::Int;
        #endif

        #if !defined(MAGNUM_TARGET_GLES2) || defined(MAGNUM_TARGET_WEBGL)
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R32F:
        case TextureFormat::RG32F:
        case TextureFormat::RGB32F:
        case TextureFormat::RGBA32F:
        #endif
        case TextureFormat::CompressedRGBBptcUnsignedFloat:
        case TextureFormat::CompressedRGBBptcSignedFloat:
            return PixelType::Float;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::R3G3B2:
            return PixelType::UnsignedByte332;
        case TextureFormat::RGB4:
            return PixelType::UnsignedShort4444;
        #endif

        #ifndef MAGNUM_TARGET_GLES
        case TextureFormat::RGB5:
        #endif
        case TextureFormat::RGB5A1:
            return PixelType::UnsignedShort5551;

        case TextureFormat::RGB565:
            return PixelType::UnsignedShort565;

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        #if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
        case TextureFormat::RGB10:
        #endif
        case TextureFormat::RGB10A2:
        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::RGB10A2UI:
        #endif
            return PixelType::UnsignedInt2101010Rev; /**< @todo Rev for all? */
        #endif

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::R11FG11FB10F:
            return PixelType::UnsignedInt10F11F11FRev;
        case TextureFormat::RGB9E5:
            return PixelType::UnsignedInt5999Rev;
        #endif

        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::DepthComponent16:
            return PixelType::UnsignedShort;
        #endif

        case TextureFormat::DepthComponent:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::DepthComponent24:
        #endif
        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::DepthComponent32:
        #endif
            return PixelType::UnsignedInt;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::DepthComponent32F:
            return PixelType::Float;
        #endif

        #ifndef MAGNUM_TARGET_WEBGL
        case TextureFormat::StencilIndex8:
            return PixelType::UnsignedByte;
        #endif

        case TextureFormat::DepthStencil:
        #if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
        case TextureFormat::Depth24Stencil8:
        #endif
            return PixelType::UnsignedInt248;

        #ifndef MAGNUM_TARGET_GLES2
        case TextureFormat::Depth32FStencil8:
            return PixelType::Float32UnsignedInt248Rev;
        #endif
    }
    #ifdef CORRADE_TARGET_GCC
    #pragma GCC diagnostic pop
    #endif

    CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

}
#endif

void AbstractTexture::parameterImplementationDefault(AbstractTexture& self, GLenum parameter, GLint value) {
    self.bindInternal();
    glTexParameteri(self._target, parameter, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(AbstractTexture& self, const GLenum parameter, const GLint value) {
    glTextureParameteri(self._id, parameter, value);
}
#endif

void AbstractTexture::parameterImplementationDefault(AbstractTexture& self, GLenum parameter, GLfloat value) {
    self.bindInternal();
    glTexParameterf(self._target, parameter, value);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(AbstractTexture& self, const GLenum parameter, const GLfloat value) {
    glTextureParameterf(self._id, parameter, value);
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::parameterImplementationDefault(AbstractTexture& self, GLenum parameter, const GLint* values) {
    self.bindInternal();
    glTexParameteriv(self._target, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(AbstractTexture& self, const GLenum parameter, const GLint* const values) {
    glTextureParameteriv(self._id, parameter, values);
}
#endif
#endif

void AbstractTexture::parameterImplementationDefault(AbstractTexture& self, GLenum parameter, const GLfloat* values) {
    self.bindInternal();
    glTexParameterfv(self._target, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterImplementationDSA(AbstractTexture& self, const GLenum parameter, const GLfloat* const values) {
    glTextureParameterfv(self._id, parameter, values);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::parameterIImplementationDefault(AbstractTexture& self, GLenum parameter, const GLuint* values) {
    self.bindInternal();
    glTexParameterIuiv(self._target, parameter, values);
}

#ifdef MAGNUM_TARGET_GLES
void AbstractTexture::parameterIImplementationEXT(AbstractTexture& self, GLenum parameter, const GLuint* values) {
    self.bindInternal();
    glTexParameterIuivEXT(self._target, parameter, values);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterIImplementationDSA(AbstractTexture& self, const GLenum parameter, const GLuint* const values) {
    glTextureParameterIuiv(self._id, parameter, values);
}
#endif

void AbstractTexture::parameterIImplementationDefault(AbstractTexture& self, GLenum parameter, const GLint* values) {
    self.bindInternal();
    glTexParameterIiv(self._target, parameter, values);
}

#ifdef MAGNUM_TARGET_GLES
void AbstractTexture::parameterIImplementationEXT(AbstractTexture& self, GLenum parameter, const GLint* values) {
    self.bindInternal();
    glTexParameterIivEXT(self._target, parameter, values);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::parameterIImplementationDSA(AbstractTexture& self, const GLenum parameter, const GLint* const values) {
    glTextureParameterIiv(self._id, parameter, values);
}
#endif
#endif

void AbstractTexture::setMaxAnisotropyImplementationNoOp(AbstractTexture&, GLfloat) {}

void AbstractTexture::setMaxAnisotropyImplementationArbOrExt(AbstractTexture& self, GLfloat anisotropy) {
    Context::current().state().texture.parameterfImplementation(self,
        #ifndef MAGNUM_TARGET_GLES
        GL_TEXTURE_MAX_ANISOTROPY
        #else
        GL_TEXTURE_MAX_ANISOTROPY_EXT
        #endif
    , anisotropy);
}

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::getLevelParameterImplementationDefault(AbstractTexture& self, const GLint level, const GLenum parameter, GLint* const values) {
    self.bindInternal();
    glGetTexLevelParameteriv(self._target, level, parameter, values);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::getLevelParameterImplementationDSA(AbstractTexture& self, const GLint level, const GLenum parameter, GLint* const values) {
    glGetTextureLevelParameteriv(self._id, level, parameter, values);
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationFallback(AbstractTexture& self, const GLsizei levels, const TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
    const PixelFormat format = pixelFormatForInternalFormat(internalFormat);
    const PixelType type = pixelTypeForInternalFormat(internalFormat);

    for(GLsizei level = 0; level != levels; ++level)
        DataHelper<1>::setImage(self, level, internalFormat,
            ImageView1D{format, type, Math::max(Math::Vector<1, GLsizei>(1), size >> level)});
}

void AbstractTexture::storageImplementationDefault(AbstractTexture& self, GLsizei levels, TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
    self.bindInternal();
    glTexStorage1D(self._target, levels, GLenum(internalFormat), size[0]);
}

void AbstractTexture::storageImplementationDSA(AbstractTexture& self, const GLsizei levels, const TextureFormat internalFormat, const Math::Vector<1, GLsizei>& size) {
    glTextureStorage1D(self._id, levels, GLenum(internalFormat), size[0]);
}
#endif

#if !defined(MAGNUM_TARGET_GLES) || defined(MAGNUM_TARGET_GLES2)
void AbstractTexture::storageImplementationFallback(AbstractTexture& self, const GLsizei levels, const TextureFormat internalFormat, const Vector2i& size) {
    const PixelFormat format = pixelFormatForInternalFormat(internalFormat);
    const PixelType type = pixelTypeForInternalFormat(internalFormat);

    /* If EXT_texture_storage is not available on ES2, passing e.g.
       TextureFormat::RGBA8 would cause an error. On ES2 it's required to have
       internalFormat equal to format, so we do exactly that. */
    #ifndef MAGNUM_TARGET_GLES2
    const TextureFormat finalInternalFormat = internalFormat;
    #else
    const TextureFormat finalInternalFormat = TextureFormat(GLenum(format));
    #endif

    /* Common code for classic types */
    #ifndef MAGNUM_TARGET_GLES
    if(self._target == GL_TEXTURE_2D || self._target == GL_TEXTURE_RECTANGLE)
    #else
    if(self._target == GL_TEXTURE_2D)
    #endif
    {
        for(GLsizei level = 0; level != levels; ++level)
            DataHelper<2>::setImage(self, level, finalInternalFormat,
                ImageView2D{format, type, Math::max(Vector2i(1), size >> level)});

    /* Cube map additionally needs to specify all faces */
    } else if(self._target == GL_TEXTURE_CUBE_MAP) {
        for(GLsizei level = 0; level != levels; ++level) {
            for(GLenum face: {GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                              GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                              GL_TEXTURE_CUBE_MAP_NEGATIVE_Z})
                DataHelper<2>::setImage(self, face, level, finalInternalFormat,
                    ImageView2D{format, type, Math::max(Vector2i(1), size >> level)});
        }

    #ifndef MAGNUM_TARGET_GLES
    /* Array texture is not scaled in "layer" dimension */
    } else if(self._target == GL_TEXTURE_1D_ARRAY) {
        for(GLsizei level = 0; level != levels; ++level)
            DataHelper<2>::setImage(self, level, internalFormat,
                ImageView2D{format, type, Vector2i{Math::max(1, size.x() >> level), size.y()}});
    #endif

    /* No other targets are available */
    } else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::storageImplementationDefault(AbstractTexture& self, GLsizei levels, TextureFormat internalFormat, const Vector2i& size) {
    self.bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glTexStorage2D(self._target, levels, GLenum(internalFormat), size.x(), size.y());
    #else
    glTexStorage2DEXT(self._target, levels, GLenum(internalFormat), size.x(), size.y());
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationDSA(AbstractTexture& self, const GLsizei levels, const TextureFormat internalFormat, const Vector2i& size) {
    glTextureStorage2D(self._id, levels, GLenum(internalFormat), size.x(), size.y());
}
#endif

#if !defined(MAGNUM_TARGET_GLES) || (defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL))
void AbstractTexture::storageImplementationFallback(AbstractTexture& self, GLsizei levels, TextureFormat internalFormat, const Vector3i& size) {
    const PixelFormat format = pixelFormatForInternalFormat(internalFormat);
    const PixelType type = pixelTypeForInternalFormat(internalFormat);

    /* If EXT_texture_storage is not available on ES2, passing e.g.
       TextureFormat::RGBA8 would cause an error. On ES2 it's required to have
       internalFormat equal to format, so we do exactly that. */
    #ifndef MAGNUM_TARGET_GLES2
    const TextureFormat finalInternalFormat = internalFormat;
    #else
    const TextureFormat finalInternalFormat = TextureFormat(GLenum(format));
    #endif

    /* Common code for classic type */
    #ifndef MAGNUM_TARGET_GLES2
    if(self._target == GL_TEXTURE_3D)
    #else
    if(self._target == GL_TEXTURE_3D_OES)
    #endif
        for(GLsizei level = 0; level != levels; ++level)
            DataHelper<3>::setImage(self, level, finalInternalFormat,
                ImageView3D{format, type, Math::max(Vector3i(1), size >> level)});

    #ifndef MAGNUM_TARGET_GLES2
    /* Array texture is not scaled in "layer" dimension */
    else if(self._target == GL_TEXTURE_2D_ARRAY || self._target ==
        #ifndef MAGNUM_TARGET_GLES
        GL_TEXTURE_CUBE_MAP_ARRAY
        #else
        GL_TEXTURE_CUBE_MAP_ARRAY_EXT
        #endif
    ) {
        for(GLsizei level = 0; level != levels; ++level)
            DataHelper<3>::setImage(self, level, internalFormat,
                ImageView3D{format, type, Vector3i{Math::max(Vector2i{1}, size.xy() >> level), size.z()}});
    }
    #endif

    /* No other targets are available */
    else CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::storageImplementationDefault(AbstractTexture& self, GLsizei levels, TextureFormat internalFormat, const Vector3i& size) {
    self.bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glTexStorage3D(self._target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
    #else
    glTexStorage3DEXT(self._target, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageImplementationDSA(AbstractTexture& self, const GLsizei levels, const TextureFormat internalFormat, const Vector3i& size) {
    glTextureStorage3D(self._id, levels, GLenum(internalFormat), size.x(), size.y(), size.z());
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageMultisampleImplementationFallback(AbstractTexture& self, const GLsizei samples, const TextureFormat internalFormat, const Vector2i& size, const GLboolean fixedSampleLocations) {
    self.bindInternal();
    glTexImage2DMultisample(self._target, samples, GLenum(internalFormat), size.x(), size.y(), fixedSampleLocations);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::storageMultisampleImplementationDefault(AbstractTexture& self, const GLsizei samples, const TextureFormat internalFormat, const Vector2i& size, const GLboolean fixedSampleLocations) {
    self.bindInternal();
    glTexStorage2DMultisample(self._target, samples, GLenum(internalFormat), size.x(), size.y(), fixedSampleLocations);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageMultisampleImplementationDSA(AbstractTexture& self, const GLsizei samples, const TextureFormat internalFormat, const Vector2i& size, const GLboolean fixedSampleLocations) {
    glTextureStorage2DMultisample(self._id, samples, GLenum(internalFormat), size.x(), size.y(), fixedSampleLocations);
}

void AbstractTexture::storageMultisampleImplementationFallback(AbstractTexture& self, const GLsizei samples, const TextureFormat internalFormat, const Vector3i& size, const GLboolean fixedSampleLocations) {
    self.bindInternal();
    glTexImage3DMultisample(self._target, samples, GLenum(internalFormat), size.x(), size.y(), size.z(), fixedSampleLocations);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::storageMultisampleImplementationDefault(AbstractTexture& self, const GLsizei samples, const TextureFormat internalFormat, const Vector3i& size, const GLboolean fixedSampleLocations) {
    self.bindInternal();
    glTexStorage3DMultisample(self._target, samples, GLenum(internalFormat), size.x(), size.y(), size.z(), fixedSampleLocations);
}

#ifdef MAGNUM_TARGET_GLES
void AbstractTexture::storageMultisampleImplementationOES(AbstractTexture& self, const GLsizei samples, const TextureFormat internalFormat, const Vector3i& size, const GLboolean fixedSampleLocations) {
    self.bindInternal();
    glTexStorage3DMultisampleOES(self._target, samples, GLenum(internalFormat), size.x(), size.y(), size.z(), fixedSampleLocations);
}
#endif
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::storageMultisampleImplementationDSA(AbstractTexture& self, const GLsizei samples, const TextureFormat internalFormat, const Vector3i& size, const GLboolean fixedSampleLocations) {
    glTextureStorage3DMultisample(self._id, samples, GLenum(internalFormat), size.x(), size.y(), size.z(), fixedSampleLocations);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::getImageImplementationDefault(AbstractTexture& self, const GLint level, const PixelFormat format, const PixelType type, const std::size_t, GLvoid* const data) {
    self.bindInternal();
    glGetTexImage(self._target, level, GLenum(format), GLenum(type), data);
}

void AbstractTexture::getCompressedImageImplementationDefault(AbstractTexture& self, const GLint level, std::size_t, GLvoid* const data) {
    self.bindInternal();
    glGetCompressedTexImage(self._target, level, data);
}

void AbstractTexture::getImageImplementationDSA(AbstractTexture& self, const GLint level, const PixelFormat format, const PixelType type, const std::size_t dataSize, GLvoid* const data) {
    glGetTextureImage(self._id, level, GLenum(format), GLenum(type), dataSize, data);
}

void AbstractTexture::getCompressedImageImplementationDSA(AbstractTexture& self, const GLint level, const std::size_t dataSize, GLvoid* const data) {
    glGetCompressedTextureImage(self._id, level, dataSize, data);
}

void AbstractTexture::getImageImplementationRobustness(AbstractTexture& self, const GLint level, const PixelFormat format, const PixelType type, const std::size_t dataSize, GLvoid* const data) {
    self.bindInternal();
    glGetnTexImageARB(self._target, level, GLenum(format), GLenum(type), dataSize, data);
}

void AbstractTexture::getCompressedImageImplementationRobustness(AbstractTexture& self, const GLint level, const std::size_t dataSize, GLvoid* const data) {
    self.bindInternal();
    glGetnCompressedTexImageARB(self._target, level, dataSize, data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImageImplementationDefault(AbstractTexture& self, GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, PixelFormat format, PixelType type, const GLvoid* data) {
    self.bindInternal();
    glTexSubImage1D(self._target, level, offset[0], size[0], GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDefault(AbstractTexture& self, const GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    self.bindInternal();
    glCompressedTexSubImage1D(self._target, level, offset[0], size[0], GLenum(format), dataSize, data);
}

void AbstractTexture::subImageImplementationDSA(AbstractTexture& self, const GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, const PixelFormat format, const PixelType type, const GLvoid* const data) {
    glTextureSubImage1D(self._id, level, offset[0], size[0], GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDSA(AbstractTexture& self, const GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLsizei>& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    glCompressedTextureSubImage1D(self._id, level, offset[0], size[0], GLenum(format), dataSize, data);
}
#endif

void AbstractTexture::imageImplementationDefault(AbstractTexture& self, const GLenum target, const GLint level, const TextureFormat internalFormat, const Vector2i& size, const PixelFormat format, const PixelType type, const GLvoid* const data, const PixelStorage&) {
    self.bindInternal();
    glTexImage2D(target, level, GLint(internalFormat), size.x(), size.y(), 0, GLenum(format), GLenum(type), data);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::imageImplementationSvga3DSliceBySlice(AbstractTexture& self, const GLenum target, const GLint level, const TextureFormat internalFormat, const Vector2i& size, const PixelFormat format, const PixelType type, const GLvoid* const data, const PixelStorage& storage) {
    /* Allocate and upload the first slice */
    imageImplementationDefault(self, target, level, internalFormat, size, format, type, data, storage);

    /* Upload the next slices slice by slice only if this is an array texture
       with more than one slice and we are copying from user memory (not from a
       buffer). The hard work is done by the subImage() implementation.
       Moreover, I am simply calling the default implementation and not the DSA
       one as just using glTexImage() pollutes the state already anyway so the
       DSA cleanness is not worth it. */
    /** @todo this will break when we support uploading from buffer offset (i.e. data != nullptr) */
    if(target == GL_TEXTURE_1D_ARRAY && data && size.y() > 1)
        subImageImplementationSvga3DSliceBySlice<&AbstractTexture::subImage2DImplementationDefault>(self, level, {0, 1}, {size.x(), size.y() - 1}, format, type, static_cast<const char*>(data) + std::get<1>(storage.dataProperties(pixelFormatSize(format, type), {size, 1})).x(), storage);
}
#endif

void AbstractTexture::subImage2DImplementationDefault(AbstractTexture& self, GLint level, const Vector2i& offset, const Vector2i& size, PixelFormat format, PixelType type, const GLvoid* data, const PixelStorage&) {
    self.bindInternal();
    glTexSubImage2D(self._target, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}

#ifndef MAGNUM_TARGET_GLES
template<void(*original)(AbstractTexture&, GLint, const Vector2i&, const Vector2i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&)> void AbstractTexture::subImageImplementationSvga3DSliceBySlice(AbstractTexture& self, GLint level, const Vector2i& offset, const Vector2i& size, PixelFormat format, PixelType type, const GLvoid* const data, const PixelStorage& storage) {
    /* Upload the data slice by slice only if this is an array texture and we
       are copying from user memory (not from a buffer) */
    if(self._target == GL_TEXTURE_1D_ARRAY && data) {
        const std::size_t stride = std::get<1>(storage.dataProperties(pixelFormatSize(format, type), {size, 1})).x();
        for(Int i = 0; i != size.y(); ++i)
            original(self, level, {offset.x(), offset.y() + i}, {size.x(), 1}, format, type, static_cast<const char*>(data) + stride*i, storage);

    /* Otherwise just pass-though to the default implementation */
    } else original(self, level, offset, size, format, type, data, storage);
}

template void AbstractTexture::subImageImplementationSvga3DSliceBySlice<&AbstractTexture::subImage2DImplementationDefault>(AbstractTexture&, GLint, const Vector2i&, const Vector2i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
template void AbstractTexture::subImageImplementationSvga3DSliceBySlice<&AbstractTexture::subImage2DImplementationDSA>(AbstractTexture&, GLint, const Vector2i&, const Vector2i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
#endif

void AbstractTexture::compressedSubImageImplementationDefault(AbstractTexture& self, const GLint level, const Vector2i& offset, const Vector2i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    self.bindInternal();
    glCompressedTexSubImage2D(self._target, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), dataSize, data);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImage2DImplementationDSA(AbstractTexture& self, const GLint level, const Vector2i& offset, const Vector2i& size, const PixelFormat format, const PixelType type, const GLvoid* const data, const PixelStorage&) {
    glTextureSubImage2D(self._id, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDSA(AbstractTexture& self, const GLint level, const Vector2i& offset, const Vector2i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    glCompressedTextureSubImage2D(self._id, level, offset.x(), offset.y(), size.x(), size.y(), GLenum(format), dataSize, data);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::imageImplementationDefault(AbstractTexture& self, GLint level, TextureFormat internalFormat, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* data, const PixelStorage&) {
    self.bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glTexImage3D
    #else
    glTexImage3DOES
    #endif
        (self._target, level, GLint(internalFormat), size.x(), size.y(), size.z(), 0, GLenum(format), GLenum(type), data);
}

#ifndef MAGNUM_TARGET_WEBGL
void AbstractTexture::imageImplementationSvga3DSliceBySlice(AbstractTexture& self, GLint level, TextureFormat internalFormat, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* data, const PixelStorage& storage) {
    /* Allocate and upload the first slice */
    imageImplementationDefault(self, level, internalFormat, size, format, type, data, storage);

    /* Upload the next slices slice by slice only if this is an array texture
       with more than one slice or a 3D texture and we are copying from user
       memory (not from a buffer). The hard work is done by the subImage()
       implementation. Moreover, I am simply calling the default implementation
       and not the DSA one as just using glTexImage() pollutes the state
       already anyway so the DSA cleanness is not worth it. */
    /** @todo this will break when we support uploading from buffer offset (i.e. data != nullptr) */
    if((
        #ifndef MAGNUM_TARGET_GLES2
        self._target == GL_TEXTURE_2D_ARRAY || self._target == GL_TEXTURE_3D
        #else
        self._target == GL_TEXTURE_3D_OES
        #endif
        ) && data && size.z() > 1)
    {
        subImageImplementationSvga3DSliceBySlice<&AbstractTexture::subImage3DImplementationDefault>(self, level, {0, 0, 1}, {size.xy(), size.z() - 1}, format, type, static_cast<const char*>(data) + std::get<1>(storage.dataProperties(pixelFormatSize(format, type), size)).xy().product(), storage);
    }
}
#endif

void AbstractTexture::subImage3DImplementationDefault(AbstractTexture& self, GLint level, const Vector3i& offset, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* data, const PixelStorage&) {
    self.bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glTexSubImage3D(self._target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), GLenum(type), data);
    #else
    glTexSubImage3DOES(self._target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), GLenum(type), data);
    #endif
}

#ifndef MAGNUM_TARGET_WEBGL
template<void(*original)(AbstractTexture&, GLint, const Vector3i&, const Vector3i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&)> void AbstractTexture::subImageImplementationSvga3DSliceBySlice(AbstractTexture& self, GLint level, const Vector3i& offset, const Vector3i& size, PixelFormat format, PixelType type, const GLvoid* const data, const PixelStorage& storage) {
    /* Upload the data slice by slice only if this is an array texture and we
       are copying from user memory (not from a buffer) */
    if(
        #ifndef MAGNUM_TARGET_GLES2
        self._target == GL_TEXTURE_2D_ARRAY || self._target == GL_TEXTURE_3D
        #else
        self._target == GL_TEXTURE_3D_OES
        #endif
        )
    {
        const std::size_t stride = std::get<1>(storage.dataProperties(pixelFormatSize(format, type), size)).xy().product();
        for(Int i = 0; i != size.z(); ++i)
            original(self, level, {offset.xy(), offset.z() + i}, {size.xy(), 1}, format, type, static_cast<const char*>(data) + stride*i, storage);

    /* Otherwise just pass-though to the default implementation */
    } else original(self, level, offset, size, format, type, data, storage);
}

template void  AbstractTexture::subImageImplementationSvga3DSliceBySlice<&AbstractTexture::subImage3DImplementationDefault>(AbstractTexture&, GLint, const Vector3i&, const Vector3i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
#ifndef MAGNUM_TARGET_GLES
template void AbstractTexture::subImageImplementationSvga3DSliceBySlice<&AbstractTexture::subImage3DImplementationDSA>(AbstractTexture&, GLint, const Vector3i&, const Vector3i&, PixelFormat, PixelType, const GLvoid*, const PixelStorage&);
#endif
#endif

void AbstractTexture::compressedSubImageImplementationDefault(AbstractTexture& self, const GLint level, const Vector3i& offset, const Vector3i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    self.bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glCompressedTexSubImage3D(self._target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), dataSize, data);
    #else
    glCompressedTexSubImage3DOES(self._target, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), dataSize, data);
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::subImage3DImplementationDSA(AbstractTexture& self, const GLint level, const Vector3i& offset, const Vector3i& size, const PixelFormat format, const PixelType type, const GLvoid* const data, const PixelStorage&) {
    glTextureSubImage3D(self._id, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), GLenum(type), data);
}

void AbstractTexture::compressedSubImageImplementationDSA(AbstractTexture& self, const GLint level, const Vector3i& offset, const Vector3i& size, const CompressedPixelFormat format, const GLvoid* const data, const GLsizei dataSize) {
    glCompressedTextureSubImage3D(self._id, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z(), GLenum(format), dataSize, data);
}
#endif

void AbstractTexture::invalidateImageImplementationNoOp(AbstractTexture&, GLint) {}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::invalidateImageImplementationARB(AbstractTexture& self, GLint level) {
    self.createIfNotAlready();
    glInvalidateTexImage(self._id, level);
}
#endif

void AbstractTexture::invalidateSubImageImplementationNoOp(AbstractTexture&, GLint, const Vector3i&, const Vector3i&) {}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::invalidateSubImageImplementationARB(AbstractTexture& self, GLint level, const Vector3i& offset, const Vector3i& size) {
    self.createIfNotAlready();
    glInvalidateTexSubImage(self._id, level, offset.x(), offset.y(), offset.z(), size.x(), size.y(), size.z());
}
#endif

#ifndef MAGNUM_TARGET_GLES
template<UnsignedInt dimensions> void AbstractTexture::image(const GLint level, Image<dimensions>& image, const ImageFlags<dimensions> flags) {
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(*this, level);
    const std::size_t dataSize = Magnum::Implementation::imageDataSizeFor(image, size);

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyPixelStoragePack(image.storage());
    Context::current().state().texture.getImageImplementation(*this, level, pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), data.size(), data);
    image = Image<dimensions>{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), size, Utility::move(data), flags};
}

template void MAGNUM_GL_EXPORT AbstractTexture::image<1>(GLint, Image<1>&, ImageFlags1D);
template void MAGNUM_GL_EXPORT AbstractTexture::image<2>(GLint, Image<2>&, ImageFlags2D);
template void MAGNUM_GL_EXPORT AbstractTexture::image<3>(GLint, Image<3>&, ImageFlags3D);

template<UnsignedInt dimensions> void AbstractTexture::image(GLint level, const BasicMutableImageView<dimensions>& image) {
    #ifndef CORRADE_NO_ASSERT
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(*this, level);
    CORRADE_ASSERT(image.data().data() != nullptr || !size.product(),
        "GL::AbstractTexture::image(): image view is nullptr", );
    CORRADE_ASSERT(image.size() == size,
        "GL::AbstractTexture::image(): expected image view size" << size << "but got" << image.size(), );
    #endif

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyPixelStoragePack(image.storage());
    Context::current().state().texture.getImageImplementation(*this, level, pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data().size(), image.data());
}

template void MAGNUM_GL_EXPORT AbstractTexture::image<1>(GLint, const BasicMutableImageView<1>&);
template void MAGNUM_GL_EXPORT AbstractTexture::image<2>(GLint, const BasicMutableImageView<2>&);
template void MAGNUM_GL_EXPORT AbstractTexture::image<3>(GLint, const BasicMutableImageView<3>&);

template<UnsignedInt dimensions> void AbstractTexture::image(GLint level, BufferImage<dimensions>& image, BufferUsage usage) {
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(*this, level);
    const std::size_t dataSize = Magnum::Implementation::imageDataSizeFor(image, size);

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), image.format(), image.type(), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), image.format(), image.type(), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyPixelStoragePack(image.storage());
    Context::current().state().texture.getImageImplementation(*this, level, image.format(), image.type(), dataSize, nullptr);
}

template void MAGNUM_GL_EXPORT AbstractTexture::image<1>(GLint, BufferImage<1>&, BufferUsage);
template void MAGNUM_GL_EXPORT AbstractTexture::image<2>(GLint, BufferImage<2>&, BufferUsage);
template void MAGNUM_GL_EXPORT AbstractTexture::image<3>(GLint, BufferImage<3>&, BufferUsage);

template<UnsignedInt dimensions> void AbstractTexture::compressedImage(const GLint level, CompressedImage<dimensions>& image, const ImageFlags<dimensions> flags) {
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(*this, level);

    /* Get internal texture format, determine its properties (compressed
       GL::TextureFormat values are same as GL::CompressedPixelFormat) and
       calculate data size for those. Yes, if the format is unknown to Magnum,
       this will blow up. But that's likely a very rare scenario that isn't
       worth implementing (and is rather impossible to test), and the user can
       always query into a view with block properties specified in that case.

       The format is zero-init to have it deterministically assert inside
       compressedPixelFormatBlockSize() if the drivers are extra shitty and
       don't implement this query (Intel Windows drivers, I'm talking about
       you), otherwise it could give back a value that could randomly work, or
       cause OOMs, crashes and such. */
    GLint format{};
    Context::current().state().texture.getLevelParameterivImplementation(*this, level, GL_TEXTURE_INTERNAL_FORMAT, &format);
    const Vector3i blockSize = compressedPixelFormatBlockSize(CompressedPixelFormat(format));
    const UnsignedInt blockDataSize = compressedPixelFormatBlockDataSize(CompressedPixelFormat(format));
    const std::size_t dataSize = Magnum::Implementation::compressedImageDataSizeFor(image.storage(), blockSize, blockDataSize, size);

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyCompressedPixelStoragePack(image.storage(), blockSize, blockDataSize);
    Context::current().state().texture.getCompressedImageImplementation(*this, level, data.size(), data);
    image = CompressedImage<dimensions>{image.storage(), UnsignedInt(format), blockSize, blockDataSize, size, Utility::move(data), flags};
}

template void MAGNUM_GL_EXPORT AbstractTexture::compressedImage<1>(GLint, CompressedImage<1>&, ImageFlags1D);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedImage<2>(GLint, CompressedImage<2>&, ImageFlags2D);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedImage<3>(GLint, CompressedImage<3>&, ImageFlags3D);

template<UnsignedInt dimensions> void AbstractTexture::compressedImage(const GLint level, const BasicMutableCompressedImageView<dimensions>& image) {
    #ifndef CORRADE_NO_ASSERT
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(*this, level);

    CORRADE_ASSERT(image.data().data() != nullptr || !size.product(),
        "GL::AbstractTexture::compressedImage(): image view is nullptr", );
    CORRADE_ASSERT(image.size() == size,
        "GL::AbstractTexture::compressedImage(): expected image view size" << size << "but got" << image.size(), );

    /* Check that the internal texture format matches (compressed
       GL::TextureFormat values are same as GL::CompressedPixelFormat).
       Zero-init to avoid a non-deterministic message in the assert below if
       the drivers are extra shitty and don't implement this query (Intel
       Windows drivers, I'm talking about you). */
    {
        GLint format{};
        Context::current().state().texture.getLevelParameterivImplementation(*this, level, GL_TEXTURE_INTERNAL_FORMAT, &format);
        CORRADE_ASSERT(compressedPixelFormat(image.format()) == CompressedPixelFormat(format),
            "GL::AbstractTexture::compressedImage(): expected image view format" << CompressedPixelFormat(format) << "but got" << compressedPixelFormat(image.format()), );
    }
    #endif

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyCompressedPixelStoragePack(image);
    Context::current().state().texture.getCompressedImageImplementation(*this, level, image.data().size(), image.data());
}

template void MAGNUM_GL_EXPORT AbstractTexture::compressedImage<1>(GLint, const BasicMutableCompressedImageView<1>&);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedImage<2>(GLint, const BasicMutableCompressedImageView<2>&);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedImage<3>(GLint, const BasicMutableCompressedImageView<3>&);

template<UnsignedInt dimensions> void AbstractTexture::compressedImage(const GLint level, CompressedBufferImage<dimensions>& image, BufferUsage usage) {
    const Math::Vector<dimensions, Int> size = DataHelper<dimensions>::imageSize(*this, level);

    /* Get internal texture format, determine its properties (compressed
       GL::TextureFormat values are same as GL::CompressedPixelFormat) and
       calculate data size for those. Yes, if the format is unknown to Magnum,
       this will blow up. But that's likely a very rare scenario that isn't
       worth implementing (and is rather impossible to test), and the user can
       always query into a view with block properties specified in that case.

       The format is zero-init to have it deterministically assert inside
       compressedPixelFormatBlockSize() if the drivers are extra shitty and
       don't implement this query (Intel Windows drivers, I'm talking about
       you), otherwise it could give back a value that could randomly work, or
       cause OOMs, crashes and such. */
    GLint format{};
    Context::current().state().texture.getLevelParameterivImplementation(*this, level, GL_TEXTURE_INTERNAL_FORMAT, &format);
    const Vector3i blockSize = compressedPixelFormatBlockSize(CompressedPixelFormat(format));
    const UnsignedInt blockDataSize = compressedPixelFormatBlockDataSize(CompressedPixelFormat(format));
    const std::size_t dataSize = Magnum::Implementation::compressedImageDataSizeFor(image.storage(), blockSize, blockDataSize, size);

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), CompressedPixelFormat(format), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), CompressedPixelFormat(format), size, nullptr, usage);
    /* The setData() call above updates the block properties, so just verify
       they're the same as the ones used here as the ones from the umage get
       used in applyCompressedPixelStoragePack() below */
    CORRADE_INTERNAL_ASSERT(blockSize == image.blockSize() && blockDataSize == image.blockDataSize());

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyCompressedPixelStoragePack(image);
    Context::current().state().texture.getCompressedImageImplementation(*this, level, dataSize, nullptr);
}

template void MAGNUM_GL_EXPORT AbstractTexture::compressedImage<1>(GLint, CompressedBufferImage<1>&, BufferUsage);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedImage<2>(GLint, CompressedBufferImage<2>&, BufferUsage);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedImage<3>(GLint, CompressedBufferImage<3>&, BufferUsage);

template<UnsignedInt dimensions> void AbstractTexture::subImage(const GLint level, const RangeTypeFor<dimensions, Int>& range, Image<dimensions>& image, const ImageFlags<dimensions> flags) {
    /* Reallocate only if needed */
    const Math::Vector<dimensions, Int> size = range.size();
    const std::size_t dataSize = Magnum::Implementation::imageDataSizeFor(image, size);
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    image = Image<dimensions>{image.storage(), image.format(), image.formatExtra(), image.pixelSize(), size, Utility::move(data), flags};
    subImage(level, range, BasicMutableImageView<dimensions>(image));
}

template void MAGNUM_GL_EXPORT AbstractTexture::subImage<1>(GLint, const Range1Di&, Image<1>&, ImageFlags1D);
template void MAGNUM_GL_EXPORT AbstractTexture::subImage<2>(GLint, const Range2Di&, Image<2>&, ImageFlags2D);
template void MAGNUM_GL_EXPORT AbstractTexture::subImage<3>(GLint, const Range3Di&, Image<3>&, ImageFlags3D);

template<UnsignedInt dimensions> void AbstractTexture::subImage(const GLint level, const RangeTypeFor<dimensions, Int>& range, const BasicMutableImageView<dimensions>& image) {
    CORRADE_ASSERT(image.data().data() != nullptr || !(Math::Vector<dimensions, Int>(range.size()).product()),
        "GL::AbstractTexture::subImage(): image view is nullptr", );
    CORRADE_ASSERT(image.size() == range.size(),
        "GL::AbstractTexture::subImage(): expected image view size" << range.size() << "but got" << image.size(), );

    /* Explicitly create if not already because the texture might have been
       created w/ the DSA extension disabled but below a DSA API is used */
    createIfNotAlready();

    const Math::Vector<dimensions, Int> size = range.size();
    const Vector3i paddedOffset = Vector3i::pad<dimensions>(range.min());
    const Vector3i paddedSize = Vector3i::pad(size, 1);

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyPixelStoragePack(image.storage());
    glGetTextureSubImage(_id, level, paddedOffset.x(), paddedOffset.y(), paddedOffset.z(), paddedSize.x(), paddedSize.y(), paddedSize.z(), GLenum(pixelFormat(image.format())), GLenum(pixelType(image.format(), image.formatExtra())), image.data().size(), image.data());
}

template void MAGNUM_GL_EXPORT AbstractTexture::subImage<1>(GLint, const Range1Di&, const BasicMutableImageView<1>&);
template void MAGNUM_GL_EXPORT AbstractTexture::subImage<2>(GLint, const Range2Di&, const BasicMutableImageView<2>&);
template void MAGNUM_GL_EXPORT AbstractTexture::subImage<3>(GLint, const Range3Di&, const BasicMutableImageView<3>&);

template<UnsignedInt dimensions> void AbstractTexture::subImage(const GLint level, const RangeTypeFor<dimensions, Int>& range, BufferImage<dimensions>& image, const BufferUsage usage) {
    /* Explicitly create if not already because the texture might have been
       created w/ the DSA extension disabled but below a DSA API is used */
    createIfNotAlready();

    const Math::Vector<dimensions, Int> size = range.size();
    const std::size_t dataSize = Magnum::Implementation::imageDataSizeFor(image, size);
    const Vector3i paddedOffset = Vector3i::pad<dimensions>(range.min());
    const Vector3i paddedSize = Vector3i::pad(size, 1);

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), image.format(), image.type(), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), image.format(), image.type(), size, nullptr, usage);

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyPixelStoragePack(image.storage());
    glGetTextureSubImage(_id, level, paddedOffset.x(), paddedOffset.y(), paddedOffset.z(), paddedSize.x(), paddedSize.y(), paddedSize.z(), GLenum(image.format()), GLenum(image.type()), dataSize, nullptr);
}

template void MAGNUM_GL_EXPORT AbstractTexture::subImage<1>(GLint, const Range1Di&, BufferImage<1>&, BufferUsage);
template void MAGNUM_GL_EXPORT AbstractTexture::subImage<2>(GLint, const Range2Di&, BufferImage<2>&, BufferUsage);
template void MAGNUM_GL_EXPORT AbstractTexture::subImage<3>(GLint, const Range3Di&, BufferImage<3>&, BufferUsage);

template<UnsignedInt dimensions> void AbstractTexture::compressedSubImage(const GLint level, const RangeTypeFor<dimensions, Int>& range, CompressedImage<dimensions>& image, const ImageFlags<dimensions> flags) {
    /* Explicitly create if not already because the texture might have been
       created w/ the DSA extension disabled but below a DSA API is used */
    createIfNotAlready();

    const Math::Vector<dimensions, Int> size = range.size();
    const Vector3i paddedOffset = Vector3i::pad<dimensions>(range.min());
    const Vector3i paddedSize = Vector3i::pad(size, 1);

    /* Get internal texture format, determine its properties (compressed
       GL::TextureFormat values are same as GL::CompressedPixelFormat) and
       calculate data size for those. Yes, if the format is unknown to Magnum,
       this will blow up. But that's likely a very rare scenario that isn't
       worth implementing (and is rather impossible to test), and the user can
       always query into a view with block properties specified in that case.

       The format is zero-init to have it deterministically assert inside
       compressedPixelFormatBlockSize() if the drivers are extra shitty and
       don't implement this query (Intel Windows drivers, I'm talking about
       you), otherwise it could give back a value that could randomly work, or
       cause OOMs, crashes and such. */
    GLint format{};
    Context::current().state().texture.getLevelParameterivImplementation(*this, level, GL_TEXTURE_INTERNAL_FORMAT, &format);
    const Vector3i blockSize = compressedPixelFormatBlockSize(CompressedPixelFormat(format));
    const UnsignedInt blockDataSize = compressedPixelFormatBlockDataSize(CompressedPixelFormat(format));
    const std::size_t dataSize = Magnum::Implementation::compressedImageDataSizeFor(image.storage(), blockSize, blockDataSize, size);

    /* Reallocate only if needed */
    Containers::Array<char> data{image.release()};
    if(data.size() < dataSize)
        data = Containers::Array<char>{dataSize};

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyCompressedPixelStoragePack(image.storage(), blockSize, blockDataSize);
    glGetCompressedTextureSubImage(_id, level, paddedOffset.x(), paddedOffset.y(), paddedOffset.z(), paddedSize.x(), paddedSize.y(), paddedSize.z(), data.size(), data);
    image = CompressedImage<dimensions>{CompressedPixelFormat(format), size, Utility::move(data), flags};
}

template void MAGNUM_GL_EXPORT AbstractTexture::compressedSubImage<1>(GLint, const Range1Di&, CompressedImage<1>&, ImageFlags1D);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedSubImage<2>(GLint, const Range2Di&, CompressedImage<2>&, ImageFlags2D);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedSubImage<3>(GLint, const Range3Di&, CompressedImage<3>&, ImageFlags3D);

template<UnsignedInt dimensions> void AbstractTexture::compressedSubImage(const GLint level, const RangeTypeFor<dimensions, Int>& range, const BasicMutableCompressedImageView<dimensions>& image) {
    CORRADE_ASSERT(image.data().data() != nullptr || !(Math::Vector<dimensions, Int>(range.size()).product()),
        "GL::AbstractTexture::compressedSubImage(): image view is nullptr", );
    CORRADE_ASSERT(image.size() == range.size(),
        "GL::AbstractTexture::compressedSubImage(): expected image view size" << range.size() << "but got" << image.size(), );

    /* Explicitly create if not already because the texture might have been
       created w/ the DSA extension disabled but below a DSA API is used */
    createIfNotAlready();

    #ifndef CORRADE_NO_ASSERT
    /* Check that the internal texture format matches (compressed
       GL::TextureFormat values are same as GL::CompressedPixelFormat).
       Zero-init to avoid a non-deterministic message in the assert below if
       the drivers are extra shitty and don't implement this query (Intel
       Windows drivers, I'm talking about you). */
    {
        GLint format{};
        Context::current().state().texture.getLevelParameterivImplementation(*this, level, GL_TEXTURE_INTERNAL_FORMAT, &format);

        CORRADE_ASSERT(compressedPixelFormat(image.format()) == CompressedPixelFormat(format),
            "GL::AbstractTexture::compressedSubImage(): expected image view format" << CompressedPixelFormat(format) << "but got" << compressedPixelFormat(image.format()), );
    }
    #endif

    const Vector3i paddedOffset = Vector3i::pad<dimensions>(range.min());
    const Vector3i paddedSize = Vector3i::pad<dimensions>(range.size(), 1);

    Buffer::unbindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyCompressedPixelStoragePack(image);
    glGetCompressedTextureSubImage(_id, level, paddedOffset.x(), paddedOffset.y(), paddedOffset.z(), paddedSize.x(), paddedSize.y(), paddedSize.z(), image.data().size(), image.data());
}

template void MAGNUM_GL_EXPORT AbstractTexture::compressedSubImage<1>(GLint, const Range1Di&, const BasicMutableCompressedImageView<1>&);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedSubImage<2>(GLint, const Range2Di&, const BasicMutableCompressedImageView<2>&);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedSubImage<3>(GLint, const Range3Di&, const BasicMutableCompressedImageView<3>&);

template<UnsignedInt dimensions> void AbstractTexture::compressedSubImage(const GLint level, const RangeTypeFor<dimensions, Int>& range, CompressedBufferImage<dimensions>& image, const BufferUsage usage) {
    /* Explicitly create if not already because the texture might have been
       created w/ the DSA extension disabled but below a DSA API is used */
    createIfNotAlready();

    const Math::Vector<dimensions, Int> size = range.size();
    const Vector3i paddedOffset = Vector3i::pad<dimensions>(range.min());
    const Vector3i paddedSize = Vector3i::pad(size, 1);

    /* Get internal texture format, determine its properties (compressed
       GL::TextureFormat values are same as GL::CompressedPixelFormat) and
       calculate data size for those. Yes, if the format is unknown to Magnum,
       this will blow up. But that's likely a very rare scenario that isn't
       worth implementing (and is rather impossible to test), and the user can
       always query into a view with block properties specified in that case.

       The format is zero-init to have it deterministically assert inside
       compressedPixelFormatBlockSize() if the drivers are extra shitty and
       don't implement this query (Intel Windows drivers, I'm talking about
       you), otherwise it could give back a value that could randomly work, or
       cause OOMs, crashes and such. */
    GLint format{};
    Context::current().state().texture.getLevelParameterivImplementation(*this, level, GL_TEXTURE_INTERNAL_FORMAT, &format);
    const Vector3i blockSize = compressedPixelFormatBlockSize(CompressedPixelFormat(format));
    const UnsignedInt blockDataSize = compressedPixelFormatBlockDataSize(CompressedPixelFormat(format));
    const std::size_t dataSize = Magnum::Implementation::compressedImageDataSizeFor(image.storage(), blockSize, blockDataSize, size);

    /* Reallocate only if needed */
    if(image.dataSize() < dataSize)
        image.setData(image.storage(), CompressedPixelFormat(format), size, {nullptr, dataSize}, usage);
    else
        image.setData(image.storage(), CompressedPixelFormat(format), size, nullptr, usage);
    /* The setData() call above updates the block properties, so just verify
       they're the same as the ones used here as the ones from the umage get
       used in applyCompressedPixelStoragePack() below */
    CORRADE_INTERNAL_ASSERT(blockSize == image.blockSize() && blockDataSize == image.blockDataSize());

    image.buffer().bindInternal(Buffer::TargetHint::PixelPack);
    Context::current().state().renderer.applyCompressedPixelStoragePack(image);
    glGetCompressedTextureSubImage(_id, level, paddedOffset.x(), paddedOffset.y(), paddedOffset.z(), paddedSize.x(), paddedSize.y(), paddedSize.z(), dataSize, nullptr);
}

template void MAGNUM_GL_EXPORT AbstractTexture::compressedSubImage<1>(GLint, const Range1Di&, CompressedBufferImage<1>&, BufferUsage);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedSubImage<2>(GLint, const Range2Di&, CompressedBufferImage<2>&, BufferUsage);
template void MAGNUM_GL_EXPORT AbstractTexture::compressedSubImage<3>(GLint, const Range3Di&, CompressedBufferImage<3>&, BufferUsage);
#endif

#ifndef MAGNUM_TARGET_GLES
Math::Vector<1, GLint> AbstractTexture::DataHelper<1>::compressedBlockSize(const GLenum target, const TextureFormat format) {
    GLint value;
    glGetInternalformativ(target, GLenum(format), GL_TEXTURE_COMPRESSED_BLOCK_WIDTH, 1, &value);
    return value;
}

Vector2i AbstractTexture::DataHelper<2>::compressedBlockSize(const GLenum target, const TextureFormat format) {
    Vector2i value{NoInit};
    glGetInternalformativ(target, GLenum(format), GL_TEXTURE_COMPRESSED_BLOCK_WIDTH, 1, &value.x());
    glGetInternalformativ(target, GLenum(format), GL_TEXTURE_COMPRESSED_BLOCK_HEIGHT, 1, &value.y());
    return value;
}

Vector3i AbstractTexture::DataHelper<3>::compressedBlockSize(const GLenum target, const TextureFormat format) {
    /** @todo use real value when OpenGL has proper queries for 3D compression formats */
    const Vector2i value = DataHelper<2>::compressedBlockSize(target, format);
    /* If the 2D size is zero (e.g. when the format is uncompressed), return a
       zero in 3D as well */
    return value == Vector2i{} ? Vector3i{} : Vector3i{value, 1};
}
#endif

#ifndef MAGNUM_TARGET_GLES
Math::Vector<1, GLint> AbstractTexture::DataHelper<1>::imageSize(AbstractTexture& texture, const GLint level) {
    Math::Vector<1, GLint> value;
    Context::current().state().texture.getLevelParameterivImplementation(texture, level, GL_TEXTURE_WIDTH, &value[0]);
    return value;
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
Vector2i AbstractTexture::DataHelper<2>::imageSize(AbstractTexture& texture, const GLint level) {
    const Implementation::TextureState& state = Context::current().state().texture;

    Vector2i value;
    state.getLevelParameterivImplementation(texture, level, GL_TEXTURE_WIDTH, &value[0]);
    state.getLevelParameterivImplementation(texture, level, GL_TEXTURE_HEIGHT, &value[1]);
    return value;
}

Vector3i AbstractTexture::DataHelper<3>::imageSize(AbstractTexture& texture, const GLint level) {
    const Implementation::TextureState& state = Context::current().state().texture;

    Vector3i value;
    state.getLevelParameterivImplementation(texture, level, GL_TEXTURE_WIDTH, &value[0]);
    state.getLevelParameterivImplementation(texture, level, GL_TEXTURE_HEIGHT, &value[1]);
    state.getLevelParameterivImplementation(texture, level, GL_TEXTURE_DEPTH, &value[2]);
    return value;
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::DataHelper<1>::setStorage(AbstractTexture& texture, const GLsizei levels, const TextureFormat internalFormat, const Math::Vector< 1, GLsizei >& size) {
    Context::current().state().texture.storage1DImplementation(texture, levels, internalFormat, size);
}
#endif

void AbstractTexture::DataHelper<2>::setStorage(AbstractTexture& texture, const GLsizei levels, const TextureFormat internalFormat, const Vector2i& size) {
    Context::current().state().texture.storage2DImplementation(texture, levels, internalFormat, size);
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::DataHelper<3>::setStorage(AbstractTexture& texture, const GLsizei levels, const TextureFormat internalFormat, const Vector3i& size) {
    Context::current().state().texture.storage3DImplementation(texture, levels, internalFormat, size);
}
#endif

#if !defined(MAGNUM_TARGET_GLES2) && !defined(MAGNUM_TARGET_WEBGL)
void AbstractTexture::DataHelper<2>::setStorageMultisample(AbstractTexture& texture, const GLsizei samples, const TextureFormat internalFormat, const Vector2i& size, const GLboolean fixedSampleLocations) {
    Context::current().state().texture.storage2DMultisampleImplementation(texture, samples, internalFormat, size, fixedSampleLocations);
}

void AbstractTexture::DataHelper<3>::setStorageMultisample(AbstractTexture& texture, const GLsizei samples, const TextureFormat internalFormat, const Vector3i& size, const GLboolean fixedSampleLocations) {
    Context::current().state().texture.storage3DMultisampleImplementation(texture, samples, internalFormat, size, fixedSampleLocations);
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::DataHelper<1>::setImage(AbstractTexture& texture, const GLint level, const TextureFormat internalFormat, const ImageView1D& image) {
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    texture.bindInternal();
    glTexImage1D(texture._target, level, GLint(internalFormat), image.size()[0], 0, GLenum(pixelFormat(image.format())), GLenum(pixelType(image.format(), image.formatExtra())), image.data());
}

void AbstractTexture::DataHelper<1>::setCompressedImage(AbstractTexture& texture, const GLint level, const CompressedImageView1D& image) {
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    texture.bindInternal();
    glCompressedTexImage1D(texture._target, level, GLenum(compressedPixelFormat(image.format())), image.size()[0], 0, Implementation::occupiedCompressedImageDataSize(image), image.data());
}

void AbstractTexture::DataHelper<1>::setImage(AbstractTexture& texture, const GLint level, const TextureFormat internalFormat, BufferImage1D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    texture.bindInternal();
    glTexImage1D(texture._target, level, GLint(internalFormat), image.size()[0], 0, GLenum(image.format()), GLenum(image.type()), nullptr);
}

void AbstractTexture::DataHelper<1>::setCompressedImage(AbstractTexture& texture, const GLint level, CompressedBufferImage1D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    texture.bindInternal();
    glCompressedTexImage1D(texture._target, level, GLenum(image.format()), image.size()[0], 0, Implementation::occupiedCompressedImageDataSize(image), nullptr);
}

void AbstractTexture::DataHelper<1>::setSubImage(AbstractTexture& texture, const GLint level, const Math::Vector<1, GLint>& offset, const ImageView1D& image) {
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    Context::current().state().texture.subImage1DImplementation(texture, level, offset, image.size(), pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data());
}

void AbstractTexture::DataHelper<1>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Math::Vector<1, GLint>& offset, const CompressedImageView1D& image) {
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    Context::current().state().texture.compressedSubImage1DImplementation(texture, level, offset, image.size(), compressedPixelFormat(image.format()), image.data(), Implementation::occupiedCompressedImageDataSize(image));
}

void AbstractTexture::DataHelper<1>::setSubImage(AbstractTexture& texture, const GLint level, const Math::Vector<1, GLint>& offset, BufferImage1D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    Context::current().state().texture.subImage1DImplementation(texture, level, offset, image.size(), image.format(), image.type(), nullptr);
}

void AbstractTexture::DataHelper<1>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Math::Vector<1, GLint>& offset, CompressedBufferImage1D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    Context::current().state().texture.compressedSubImage1DImplementation(texture, level, offset, image.size(), image.format(), nullptr, Implementation::occupiedCompressedImageDataSize(image));
}
#endif

void AbstractTexture::DataHelper<2>::setImage(AbstractTexture& texture, const GLenum target, const GLint level, const TextureFormat internalFormat, const ImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    Context::current().state().texture.image2DImplementation(texture, target, level, internalFormat, image.size(), pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data()
        #ifdef MAGNUM_TARGET_GLES2
        + Magnum::Implementation::pixelStorageSkipOffset(image)
        #endif
        , image.storage());
}

void AbstractTexture::DataHelper<2>::setCompressedImage(AbstractTexture& texture, const GLenum target, const GLint level, const CompressedImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    texture.bindInternal();
    glCompressedTexImage2D(target, level, GLenum(compressedPixelFormat(image.format())), image.size().x(), image.size().y(), 0, Implementation::occupiedCompressedImageDataSize(image), image.data());
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<2>::setImage(AbstractTexture& texture, const GLenum target, const GLint level, const TextureFormat internalFormat, BufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    texture.bindInternal();
    glTexImage2D(target, level, GLint(internalFormat), image.size().x(), image.size().y(), 0, GLenum(image.format()), GLenum(image.type()), nullptr);
}

void AbstractTexture::DataHelper<2>::setCompressedImage(AbstractTexture& texture, const GLenum target, const GLint level, CompressedBufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    texture.bindInternal();
    glCompressedTexImage2D(target, level, GLenum(image.format()), image.size().x(), image.size().y(), 0, Implementation::occupiedCompressedImageDataSize(image), nullptr);
}
#endif

void AbstractTexture::DataHelper<2>::setSubImage(AbstractTexture& texture, const GLint level, const Vector2i& offset, const ImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    Context::current().state().texture.subImage2DImplementation(texture, level, offset, image.size(), pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data()
        #ifdef MAGNUM_TARGET_GLES2
        + Magnum::Implementation::pixelStorageSkipOffset(image)
        #endif
        , image.storage());
}

void AbstractTexture::DataHelper<2>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Vector2i& offset, const CompressedImageView2D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    Context::current().state().texture.compressedSubImage2DImplementation(texture, level, offset, image.size(), compressedPixelFormat(image.format()), image.data(), Implementation::occupiedCompressedImageDataSize(image));
}

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<2>::setSubImage(AbstractTexture& texture, const GLint level, const Vector2i& offset, BufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    Context::current().state().texture.subImage2DImplementation(texture, level, offset, image.size(), image.format(), image.type(), nullptr, image.storage());
}

void AbstractTexture::DataHelper<2>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Vector2i& offset, CompressedBufferImage2D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    Context::current().state().texture.compressedSubImage2DImplementation(texture, level, offset, image.size(), image.format(), nullptr, Implementation::occupiedCompressedImageDataSize(image));
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::DataHelper<3>::setImage(AbstractTexture& texture, const GLint level, const TextureFormat internalFormat, const ImageView3D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    Context::current().state().texture.image3DImplementation(texture, level, internalFormat, image.size(), pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data()
        #ifdef MAGNUM_TARGET_GLES2
        + Magnum::Implementation::pixelStorageSkipOffset(image)
        #endif
        , image.storage());
}

void AbstractTexture::DataHelper<3>::setCompressedImage(AbstractTexture& texture, const GLint level, const CompressedImageView3D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    texture.bindInternal();
    #ifndef MAGNUM_TARGET_GLES2
    glCompressedTexImage3D(texture._target, level, GLenum(compressedPixelFormat(image.format())), image.size().x(), image.size().y(), image.size().z(), 0, Implementation::occupiedCompressedImageDataSize(image), image.data());
    #else
    glCompressedTexImage3DOES(texture._target, level, GLenum(compressedPixelFormat(image.format())), image.size().x(), image.size().y(), image.size().z(), 0, Implementation::occupiedCompressedImageDataSize(image), image.data());
    #endif
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<3>::setImage(AbstractTexture& texture, const GLint level, const TextureFormat internalFormat, BufferImage3D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    texture.bindInternal();
    glTexImage3D(texture._target, level, GLint(internalFormat), image.size().x(), image.size().y(), image.size().z(), 0, GLenum(image.format()), GLenum(image.type()), nullptr);
}

void AbstractTexture::DataHelper<3>::setCompressedImage(AbstractTexture& texture, const GLint level, CompressedBufferImage3D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    texture.bindInternal();
    glCompressedTexImage3D(texture._target, level, GLenum(image.format()), image.size().x(), image.size().y(), image.size().z(), 0, Implementation::occupiedCompressedImageDataSize(image), nullptr);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::DataHelper<3>::setSubImage(AbstractTexture& texture, const GLint level, const Vector3i& offset, const ImageView3D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    Context::current().state().texture.subImage3DImplementation(texture, level, offset, image.size(), pixelFormat(image.format()), pixelType(image.format(), image.formatExtra()), image.data()
        #ifdef MAGNUM_TARGET_GLES2
        + Magnum::Implementation::pixelStorageSkipOffset(image)
        #endif
        , image.storage());
}

void AbstractTexture::DataHelper<3>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Vector3i& offset, const CompressedImageView3D& image) {
    #ifndef MAGNUM_TARGET_GLES2
    Buffer::unbindInternal(Buffer::TargetHint::PixelUnpack);
    #endif
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    Context::current().state().texture.compressedSubImage3DImplementation(texture, level, offset, image.size(), compressedPixelFormat(image.format()), image.data(), Implementation::occupiedCompressedImageDataSize(image));
}
#endif

#ifndef MAGNUM_TARGET_GLES2
void AbstractTexture::DataHelper<3>::setSubImage(AbstractTexture& texture, const GLint level, const Vector3i& offset, BufferImage3D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyPixelStorageUnpack(image.storage());
    Context::current().state().texture.subImage3DImplementation(texture, level, offset, image.size(), image.format(), image.type(), nullptr, image.storage());
}

void AbstractTexture::DataHelper<3>::setCompressedSubImage(AbstractTexture& texture, const GLint level, const Vector3i& offset, CompressedBufferImage3D& image) {
    image.buffer().bindInternal(Buffer::TargetHint::PixelUnpack);
    Context::current().state().renderer.applyCompressedPixelStorageUnpack(image);
    Context::current().state().texture.compressedSubImage3DImplementation(texture, level, offset, image.size(), image.format(), nullptr, Implementation::occupiedCompressedImageDataSize(image));
}
#endif

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::DataHelper<1>::invalidateSubImage(AbstractTexture& texture, const GLint level, const Math::Vector<1, GLint>& offset, const Math::Vector<1, GLint>& size) {
    Context::current().state().texture.invalidateSubImageImplementation(texture, level, {offset[0], 0, 0}, {size[0], 1, 1});
}
#endif

void AbstractTexture::DataHelper<2>::invalidateSubImage(AbstractTexture& texture, const GLint level, const Vector2i& offset, const Vector2i& size) {
    Context::current().state().texture.invalidateSubImageImplementation(texture, level, {offset, 0}, {size, 1});
}

void AbstractTexture::DataHelper<3>::invalidateSubImage(AbstractTexture& texture, const GLint level, const Vector3i& offset, const Vector3i& size) {
    Context::current().state().texture.invalidateSubImageImplementation(texture, level, offset, size);
}

#ifndef MAGNUM_TARGET_GLES
void AbstractTexture::DataHelper<1>::setWrapping(AbstractTexture& texture, const Math::Vector<1, SamplerWrapping>& wrapping) {
    Context::current().state().texture.parameteriImplementation(texture, GL_TEXTURE_WRAP_S, GLint(wrapping[0]));
}
#endif

void AbstractTexture::DataHelper<2>::setWrapping(AbstractTexture& texture, const Math::Vector2<SamplerWrapping>& wrapping) {
    const Implementation::TextureState& state = Context::current().state().texture;

    state.parameteriImplementation(texture, GL_TEXTURE_WRAP_S, GLint(wrapping.x()));
    state.parameteriImplementation(texture, GL_TEXTURE_WRAP_T, GLint(wrapping.y()));
}

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
void AbstractTexture::DataHelper<3>::setWrapping(AbstractTexture& texture, const Math::Vector3<SamplerWrapping>& wrapping) {
    const Implementation::TextureState& state = Context::current().state().texture;

    state.parameteriImplementation(texture, GL_TEXTURE_WRAP_S, GLint(wrapping.x()));
    state.parameteriImplementation(texture, GL_TEXTURE_WRAP_T, GLint(wrapping.y()));
    #ifndef MAGNUM_TARGET_GLES2
    state.parameteriImplementation(texture, GL_TEXTURE_WRAP_R, GLint(wrapping.z()));
    #else
    state.parameteriImplementation(texture, GL_TEXTURE_WRAP_R_OES, GLint(wrapping.z()));
    #endif
}
#endif

}}
