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

#include "AbstractObject.h"

#include <Corrade/Containers/ArrayView.h>
#include <Corrade/Containers/EnumSet.hpp>
#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif
#include <Corrade/Utility/Assert.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/Implementation/State.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif

namespace Magnum { namespace GL {

Debug& operator<<(Debug& debug, const ObjectFlag value) {
    debug << "GL::ObjectFlag" << Debug::nospace;

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case ObjectFlag::value: return debug << "::" #value;
        _c(Created)
        _c(DeleteOnDestruction)
        #undef _c
        /* LCOV_EXCL_STOP */
    }

    return debug << "(" << Debug::nospace << Debug::hex << UnsignedByte(value) << Debug::nospace << ")";
}

Debug& operator<<(Debug& debug, const ObjectFlags value) {
    return Containers::enumSetDebugOutput(debug, value, "GL::ObjectFlags{}", {
        ObjectFlag::Created,
        ObjectFlag::DeleteOnDestruction
    });
}

#ifndef MAGNUM_TARGET_WEBGL
namespace {
    inline GLenum extTypeFromKhrIdentifier(GLenum khrIdentifier) {
        switch(khrIdentifier) {
            #ifndef MAGNUM_TARGET_GLES2
            case GL_BUFFER:
            #else
            case GL_BUFFER_KHR:
            #endif
                return GL_BUFFER_OBJECT_EXT;

            #ifndef MAGNUM_TARGET_GLES2
            case GL_SHADER:
            #else
            case GL_SHADER_KHR:
            #endif
                return GL_SHADER_OBJECT_EXT;

            #ifndef MAGNUM_TARGET_GLES2
            case GL_PROGRAM:
            #else
            case GL_PROGRAM_KHR:
            #endif
                return GL_PROGRAM_OBJECT_EXT;

            #ifndef MAGNUM_TARGET_GLES2
            case GL_VERTEX_ARRAY:
            #else
            case GL_VERTEX_ARRAY_KHR:
            #endif
                return GL_VERTEX_ARRAY_OBJECT_EXT;

            #ifndef MAGNUM_TARGET_GLES2
            case GL_QUERY:
            #else
            case GL_QUERY_KHR:
            #endif
                return GL_QUERY_OBJECT_EXT;

            /** @todo Why isn't `GL_PROGRAM_PIPELINE_KHR` in ES's KHR_debug? */
            #ifndef MAGNUM_TARGET_GLES2
            case GL_PROGRAM_PIPELINE:
            #else
            case 0x82E4: //GL_PROGRAM_PIPELINE_KHR:
            #endif
                return GL_PROGRAM_PIPELINE_OBJECT_EXT;

            /**
             * @todo Shouldn't ES2's KHR_debug have `GL_TRANSFORM_FEEDBACK_KHR`
             *      instead of `GL_TRANSFORM_FEEDBACK`? (it's a new enum in 2.0)
             *      Also the original @gl_extension{EXT,debug_label} mentions it
             *      only for ES3 (i.e. no mention of @gl_extension{EXT,transform_feedback})
             */
            case GL_TRANSFORM_FEEDBACK:
            #ifndef MAGNUM_TARGET_GLES2
            case GL_SAMPLER:
            #else
            case GL_SAMPLER_KHR:
            #endif
            case GL_TEXTURE:
            case GL_RENDERBUFFER:
            case GL_FRAMEBUFFER:
                return khrIdentifier;
        }

        CORRADE_INTERNAL_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }
}

Int AbstractObject::maxLabelLength() {
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>())
        return 0;

    GLint& value = Context::current().state().debug.maxLabelLength;

    if(value == 0) {
        #ifndef MAGNUM_TARGET_GLES2
        glGetIntegerv(GL_MAX_LABEL_LENGTH, &value);
        #else
        glGetIntegerv(GL_MAX_LABEL_LENGTH_KHR, &value);
        #endif
    }

    return value;
}

void AbstractObject::labelImplementationNoOp(GLenum, GLuint, Containers::StringView) {}

#ifndef MAGNUM_TARGET_GLES2
void AbstractObject::labelImplementationKhrDesktopES32(const GLenum identifier, const GLuint name, const Containers::StringView label) {
    glObjectLabel(identifier, name, label.size(), label.data());
}
#endif

#ifdef MAGNUM_TARGET_GLES
void AbstractObject::labelImplementationKhrES(const GLenum identifier, const GLuint name, const Containers::StringView label) {
    glObjectLabelKHR(identifier, name, label.size(), label.data());
}
#endif

void AbstractObject::labelImplementationExt(const GLenum identifier, const GLuint name, const Containers::StringView label) {
    const GLenum type = extTypeFromKhrIdentifier(identifier);
    glLabelObjectEXT(type, name, label.size(), label.data());
}

Containers::String AbstractObject::getLabelImplementationNoOp(GLenum, GLuint) { return  {}; }

#ifndef MAGNUM_TARGET_GLES2
Containers::String AbstractObject::getLabelImplementationKhrDesktopES32(const GLenum identifier, const GLuint name) {
    /* Get label size (w/o null terminator). Specifying 0 as size is not
       allowed, thus we pass the maximum instead. */
    GLsizei size = 0;
    glGetObjectLabel(identifier, name, maxLabelLength(), &size, nullptr);

    /* The storage already includes the null terminator */
    Containers::String label{NoInit, std::size_t(size)};
    glGetObjectLabel(identifier, name, size+1, nullptr, label.data());
    return label;
}
#endif

#ifdef MAGNUM_TARGET_GLES
Containers::String AbstractObject::getLabelImplementationKhrES(const GLenum identifier, const GLuint name) {
    /* Get label size (w/o null terminator). Specifying 0 as size is not
       allowed, thus we pass the maximum instead. */
    GLsizei size = 0;
    glGetObjectLabelKHR(identifier, name, maxLabelLength(), &size, nullptr);

    /* The storage already includes the null terminator */
    Containers::String label{NoInit, std::size_t(size)};
    glGetObjectLabelKHR(identifier, name, size+1, nullptr, label.data());
    return label;
}
#endif

Containers::String AbstractObject::getLabelImplementationExt(const GLenum identifier, const GLuint name) {
    const GLenum type = extTypeFromKhrIdentifier(identifier);

    /* Get label size (w/o null terminator) */
    GLsizei size = 0;
    glGetObjectLabelEXT(type, name, 0, &size, nullptr);

    /* The storage already includes the null terminator */
    Containers::String label{NoInit, std::size_t(size)};
    glGetObjectLabelEXT(type, name, size+1, nullptr, label.data());
    return label;
}
#endif

}}
