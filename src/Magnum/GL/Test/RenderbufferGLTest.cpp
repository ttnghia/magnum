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

#include <Corrade/TestSuite/Compare/Numeric.h>

#include "Magnum/GL/Context.h"
#include "Magnum/GL/Extensions.h"
#include "Magnum/GL/OpenGLTester.h"
#include "Magnum/GL/Renderbuffer.h"
#include "Magnum/GL/RenderbufferFormat.h"
#include "Magnum/Math/Vector2.h"

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif

namespace Magnum { namespace GL { namespace Test { namespace {

struct RenderbufferGLTest: OpenGLTester {
    explicit RenderbufferGLTest();

    void construct();
    void constructMove();
    void wrap();
    void wrapCreateIfNotAlready();

    #ifndef MAGNUM_TARGET_WEBGL
    void label();
    #endif

    void setStorage();
    #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
    void setStorageMultisample();
    #endif
};

RenderbufferGLTest::RenderbufferGLTest() {
    addTests({&RenderbufferGLTest::construct,
              &RenderbufferGLTest::constructMove,
              &RenderbufferGLTest::wrap,
              &RenderbufferGLTest::wrapCreateIfNotAlready,

              #ifndef MAGNUM_TARGET_WEBGL
              &RenderbufferGLTest::label,
              #endif

              &RenderbufferGLTest::setStorage,

              #if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
              &RenderbufferGLTest::setStorageMultisample
              #endif
              });
}

#ifndef MAGNUM_TARGET_WEBGL
using namespace Containers::Literals;
#endif

void RenderbufferGLTest::construct() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() << "is not supported.");
    #endif

    {
        const Renderbuffer renderbuffer;

        MAGNUM_VERIFY_NO_GL_ERROR();
        CORRADE_VERIFY(renderbuffer.id() > 0);
        CORRADE_COMPARE_AS(renderbuffer.flags(),
            ObjectFlag::DeleteOnDestruction,
            TestSuite::Compare::GreaterOrEqual);
    }

    MAGNUM_VERIFY_NO_GL_ERROR();
}

void RenderbufferGLTest::constructMove() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() << "is not supported.");
    #endif

    Renderbuffer a;
    const Int id = a.id();

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(id > 0);

    Renderbuffer b(Utility::move(a));

    CORRADE_COMPARE(a.id(), 0);
    CORRADE_COMPARE(b.id(), id);

    Renderbuffer c;
    const Int cId = c.id();
    c = Utility::move(b);

    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_VERIFY(cId > 0);
    CORRADE_COMPARE(b.id(), cId);
    CORRADE_COMPARE(c.id(), id);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<Renderbuffer>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<Renderbuffer>::value);
}

void RenderbufferGLTest::wrap() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() << "is not supported.");
    #endif

    GLuint id;
    glGenRenderbuffers(1, &id);

    /* Releasing won't delete anything */
    {
        auto renderbuffer = Renderbuffer::wrap(id, ObjectFlag::DeleteOnDestruction);
        CORRADE_COMPARE(renderbuffer.release(), id);
    }

    /* ...so we can wrap it again */
    Renderbuffer::wrap(id);
    glDeleteRenderbuffers(1, &id);
}

void RenderbufferGLTest::wrapCreateIfNotAlready() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() << "is not supported.");
    #endif

    /* Make an object and ensure it's created */
    Renderbuffer renderbuffer;
    renderbuffer.setStorage(RenderbufferFormat::RGBA4, {4, 4});
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(renderbuffer.flags(), ObjectFlag::Created|ObjectFlag::DeleteOnDestruction);

    /* Wrap into another object without ObjectFlag::Created being set, which is
       a common usage pattern to make non-owning references. Then calling an
       API that internally does createIfNotAlready() shouldn't assert just
       because Created isn't set but the object is bound, instead it should
       just mark it as such when it discovers it. Here the "already bound" case
       only happens if GL_ARB_direct_state_access is disabled. */
    Renderbuffer wrapped = Renderbuffer::wrap(renderbuffer.id());
    CORRADE_COMPARE(wrapped.flags(), ObjectFlags{});

    #ifndef MAGNUM_TARGET_WEBGL
    wrapped.label();
    MAGNUM_VERIFY_NO_GL_ERROR();
    CORRADE_COMPARE(wrapped.flags(), ObjectFlag::Created);
    #else
    CORRADE_SKIP("No API that would call createIfNotAlready() on WebGL, can't test.");
    #endif
}

#ifndef MAGNUM_TARGET_WEBGL
void RenderbufferGLTest::label() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() << "is not supported.");
    #endif
    if(!Context::current().isExtensionSupported<Extensions::KHR::debug>() &&
       !Context::current().isExtensionSupported<Extensions::EXT::debug_label>())
        CORRADE_SKIP("Required extension is not available");

    Renderbuffer renderbuffer;
    CORRADE_COMPARE(renderbuffer.label(), "");
    MAGNUM_VERIFY_NO_GL_ERROR();

    /* Test the string size gets correctly used, instead of relying on null
       termination */
    renderbuffer.setLabel("MyRenderbuffer!"_s.exceptSuffix(1));
    MAGNUM_VERIFY_NO_GL_ERROR();

    CORRADE_COMPARE(renderbuffer.label(), "MyRenderbuffer");
    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

void RenderbufferGLTest::setStorage() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() << "is not supported.");
    #endif

    Renderbuffer renderbuffer;

    #ifndef MAGNUM_TARGET_GLES2
    renderbuffer.setStorage(RenderbufferFormat::RGBA8, {128, 128});
    #else
    renderbuffer.setStorage(RenderbufferFormat::RGBA4, {128, 128});
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();
}

#if !(defined(MAGNUM_TARGET_GLES2) && defined(MAGNUM_TARGET_WEBGL))
void RenderbufferGLTest::setStorageMultisample() {
    #ifndef MAGNUM_TARGET_GLES
    if(!Context::current().isExtensionSupported<Extensions::ARB::framebuffer_object>())
        CORRADE_SKIP(Extensions::ARB::framebuffer_object::string() << "is not supported.");
    #elif defined(MAGNUM_TARGET_GLES2)
    if(!Context::current().isExtensionSupported<Extensions::ANGLE::framebuffer_multisample>() &&
       !Context::current().isExtensionSupported<Extensions::NV::framebuffer_multisample>())
        CORRADE_SKIP("Neither" << Extensions::ANGLE::framebuffer_multisample::string() << "nor" << Extensions::NV::framebuffer_multisample::string() << "is supported.");
    #endif

    Renderbuffer renderbuffer;

    #ifndef MAGNUM_TARGET_GLES2
    renderbuffer.setStorageMultisample(Renderbuffer::maxSamples(), RenderbufferFormat::RGBA8, {128, 128});
    #else
    renderbuffer.setStorageMultisample(Renderbuffer::maxSamples(), RenderbufferFormat::RGBA4, {128, 128});
    #endif

    MAGNUM_VERIFY_NO_GL_ERROR();
}
#endif

}}}}

CORRADE_TEST_MAIN(Magnum::GL::Test::RenderbufferGLTest)
