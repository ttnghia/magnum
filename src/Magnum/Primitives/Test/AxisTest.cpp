/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019
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

#include <Corrade/TestSuite/Tester.h>

#include "Magnum/Mesh.h"
#include "Magnum/Math/Color.h"
#include "Magnum/Primitives/Axis.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace Primitives { namespace Test { namespace {

struct AxisTest: TestSuite::Tester {
    explicit AxisTest();

    void twoDimensions();
    void threeDimensions();
};

AxisTest::AxisTest() {
    addTests({&AxisTest::twoDimensions,
              &AxisTest::threeDimensions});
}

void AxisTest::twoDimensions() {
    Trade::MeshData axis = Primitives::axis2D();

    CORRADE_COMPARE(axis.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(axis.isIndexed());
    CORRADE_COMPARE(axis.indexCount(), 12);
    CORRADE_COMPARE(axis.vertexCount(), 8);
    CORRADE_COMPARE(axis.attributeCount(), 2);
    CORRADE_COMPARE(axis.indices<UnsignedShort>()[5], 3);
    CORRADE_COMPARE(axis.attribute<Vector2>(Trade::MeshAttributeName::Position)[3],
        (Vector2{0.9f, -0.1f}));
    CORRADE_COMPARE(axis.attribute<Color3>(Trade::MeshAttributeName::Color)[6],
        (Color3{0.0f, 1.0f, 0.0f}));
}

void AxisTest::threeDimensions() {
    Trade::MeshData axis = Primitives::axis3D();

    CORRADE_COMPARE(axis.primitive(), MeshPrimitive::Lines);
    CORRADE_VERIFY(axis.isIndexed());
    CORRADE_COMPARE(axis.indexCount(), 18);
    CORRADE_COMPARE(axis.vertexCount(), 12);
    CORRADE_COMPARE(axis.attributeCount(), 2);
    CORRADE_COMPARE(axis.indices<UnsignedShort>()[12], 8);
    CORRADE_COMPARE(axis.attribute<Vector3>(Trade::MeshAttributeName::Position)[6],
        (Vector3{0.1f, 0.9f, 0.0f}));
    CORRADE_COMPARE(axis.attribute<Color3>(Trade::MeshAttributeName::Color)[4],
        (Color3{0.0f, 1.0f, 0.0f}));
}

}}}}

CORRADE_TEST_MAIN(Magnum::Primitives::Test::AxisTest)
