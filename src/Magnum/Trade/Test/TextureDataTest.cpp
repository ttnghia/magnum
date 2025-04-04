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

#include <Corrade/Containers/String.h>
#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Trade/TextureData.h"

namespace Magnum { namespace Trade { namespace Test { namespace {

struct TextureDataTest: TestSuite::Tester {
    explicit TextureDataTest();

    void construct();
    void constructCopy();
    void constructMove();

    void debugType();
    void debugTypePacked();
};

TextureDataTest::TextureDataTest() {
    addTests({&TextureDataTest::construct,
              &TextureDataTest::constructCopy,
              &TextureDataTest::constructMove,

              &TextureDataTest::debugType,
              &TextureDataTest::debugTypePacked});
}

void TextureDataTest::construct() {
    const int a{};
    const TextureData data{TextureType::CubeMap,
        SamplerFilter::Linear,
        SamplerFilter::Nearest,
        SamplerMipmap::Nearest,
        {SamplerWrapping::Repeat, SamplerWrapping::ClampToEdge, SamplerWrapping::MirroredRepeat},
        42,
        &a};

    CORRADE_COMPARE(data.type(), TextureType::CubeMap);
    CORRADE_COMPARE(data.minificationFilter(), SamplerFilter::Linear);
    CORRADE_COMPARE(data.magnificationFilter(), SamplerFilter::Nearest);
    CORRADE_COMPARE(data.mipmapFilter(), SamplerMipmap::Nearest);
    CORRADE_COMPARE(data.wrapping(), (Math::Vector3<SamplerWrapping>{SamplerWrapping::Repeat, SamplerWrapping::ClampToEdge, SamplerWrapping::MirroredRepeat}));
    CORRADE_COMPARE(data.image(), 42);
    CORRADE_COMPARE(data.importerState(), &a);
}

void TextureDataTest::constructCopy() {
    CORRADE_VERIFY(!std::is_copy_constructible<TextureData>{});
    CORRADE_VERIFY(!std::is_copy_assignable<TextureData>{});
}

void TextureDataTest::constructMove() {
    const int a{};
    TextureData data{TextureType::CubeMap,
        SamplerFilter::Linear,
        SamplerFilter::Nearest,
        SamplerMipmap::Nearest,
        {SamplerWrapping::Repeat, SamplerWrapping::ClampToEdge, SamplerWrapping::MirroredRepeat},
        42,
        &a};

    TextureData b{Utility::move(data)};

    CORRADE_COMPARE(b.type(), TextureType::CubeMap);
    CORRADE_COMPARE(b.minificationFilter(), SamplerFilter::Linear);
    CORRADE_COMPARE(b.magnificationFilter(), SamplerFilter::Nearest);
    CORRADE_COMPARE(b.mipmapFilter(), SamplerMipmap::Nearest);
    CORRADE_COMPARE(b.wrapping(), (Math::Vector3<SamplerWrapping>{SamplerWrapping::Repeat, SamplerWrapping::ClampToEdge, SamplerWrapping::MirroredRepeat}));
    CORRADE_COMPARE(b.image(), 42);
    CORRADE_COMPARE(b.importerState(), &a);

    const int c{};
    TextureData d{TextureType::Texture2D,
        SamplerFilter::Nearest,
        SamplerFilter::Linear,
        SamplerMipmap::Base,
        SamplerWrapping::ClampToEdge,
        13,
        &c};
    d = Utility::move(b);

    CORRADE_COMPARE(d.type(), TextureType::CubeMap);
    CORRADE_COMPARE(d.minificationFilter(), SamplerFilter::Linear);
    CORRADE_COMPARE(d.magnificationFilter(), SamplerFilter::Nearest);
    CORRADE_COMPARE(d.mipmapFilter(), SamplerMipmap::Nearest);
    CORRADE_COMPARE(d.wrapping(), (Math::Vector3<SamplerWrapping>{SamplerWrapping::Repeat, SamplerWrapping::ClampToEdge, SamplerWrapping::MirroredRepeat}));
    CORRADE_COMPARE(d.image(), 42);
    CORRADE_COMPARE(d.importerState(), &a);

    CORRADE_VERIFY(std::is_nothrow_move_constructible<TextureData>::value);
    CORRADE_VERIFY(std::is_nothrow_move_assignable<TextureData>::value);
}

void TextureDataTest::debugType() {
    Containers::String out;

    Debug(&out) << TextureType::Texture3D << TextureType(0xbe);
    CORRADE_COMPARE(out, "Trade::TextureType::Texture3D Trade::TextureType(0xbe)\n");
}

void TextureDataTest::debugTypePacked() {
    Containers::String out;
    /* Last is not packed, ones before should not make any flags persistent */
    Debug(&out) << Debug::packed << TextureType::Texture3D << Debug::packed << TextureType(0xbe) << TextureType::Texture2D;
    CORRADE_COMPARE(out, "Texture3D 0xbe Trade::TextureType::Texture2D\n");
}

}}}}

CORRADE_TEST_MAIN(Magnum::Trade::Test::TextureDataTest)
