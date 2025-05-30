#ifndef Magnum_MeshTools_Compile_h
#define Magnum_MeshTools_Compile_h
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

#ifdef MAGNUM_TARGET_GL
/** @file
 * @brief Enum @ref Magnum::MeshTools::CompileFlag, enum set @ref Magnum::MeshTools::CompileFlags, function @ref Magnum::MeshTools::compile(), @ref Magnum::MeshTools::compiledPerVertexJointCount()
 */
#endif

#include "Magnum/configure.h"

#ifdef MAGNUM_TARGET_GL
#include <Corrade/Containers/EnumSet.h>

#include "Magnum/Magnum.h"
#include "Magnum/GL/GL.h"
#include "Magnum/Trade/Trade.h"
#include "Magnum/MeshTools/visibility.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#include <Corrade/Utility/Macros.h>
#endif

namespace Magnum { namespace MeshTools {

/**
@brief Mesh compilation flag
@m_since{2019,10}

@note This enum is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref CompileFlags, @ref compile(const Trade::MeshData&, CompileFlags)
*/
enum class CompileFlag: UnsignedByte {
    /**
     * If the mesh is @ref MeshPrimitive::Triangles, generates normals using
     * @ref MeshTools::generateFlatNormals(). If the mesh is not a triangle
     * mesh or doesn't have 3D positions, this flag does nothing. If the mesh
     * already has its own normals, these get replaced. If
     * @ref CompileFlag::GenerateSmoothNormals is specified together with this
     * flag, this flag gets a priority.
     */
    GenerateFlatNormals = 1 << 0,

    /**
     * If the mesh @ref MeshPrimitive::Triangles, generates normals using
     * @ref MeshTools::generateSmoothNormals() based on triangle adjacency
     * information from the index buffer. If the mesh is not indexed, this
     * behaves the same as @ref CompileFlag::GenerateFlatNormals. If the mesh
     * is not a triangle mesh or doesn't have 3D positions, this flag does
     * nothing. If the mesh already has its own normals, these get replaced.
     */
    GenerateSmoothNormals = 1 << 1,

    /**
     * By default, @ref compile() warns when it encounters custom attributes,
     * morph target attributes and attributes with an implementation-specific
     * format, as those get ignored by it. If you're binding those manually
     * with @ref compile(const Trade::MeshData&, GL::Buffer&, GL::Buffer&) or
     * handling them in some other way on the application side already, use
     * this flag to suppress the warning messages.
     * @m_since{2020,06}
     */
    NoWarnOnCustomAttributes = 1 << 2
};

/**
@brief Mesh compilation flags
@m_since{2019,10}

@note This enum set is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref compile(const Trade::MeshData&, CompileFlags)
*/
typedef Containers::EnumSet<CompileFlag> CompileFlags;

CORRADE_ENUMSET_OPERATORS(CompileFlags)

/**
@brief Compile OpenGL mesh data
@m_since{2020,06}

Configures a mesh for a @ref Shaders::GenericGL shader with a vertex buffer and
possibly also an index buffer, if the mesh is indexed.

-   If the mesh contains @ref Trade::MeshAttribute::Position, these are bound
    to the @ref Shaders::GenericGL2D::Position attribute if they are 2D or to
    @ref Shaders::GenericGL3D::Position if they are 3D.
-   If the mesh contains @ref Trade::MeshAttribute::Tangent, these are bound to
    @ref Shaders::GenericGL3D::Tangent4 or @ref Shaders::GenericGL3D::Tangent
    based on their type.
-   If the mesh contains @ref Trade::MeshAttribute::Bitangent, these are bound
    to @ref Shaders::GenericGL3D::Bitangent. However, if the mesh contains a
    @ref Trade::MeshAttribute::ObjectId as well, only the first appearing of
    the two is bound. The second is ignored with a warning as they share the
    same binding slot.
-   If the mesh contains @ref Trade::MeshAttribute::Normal or if
    @ref CompileFlag::GenerateFlatNormals /
    @ref CompileFlag::GenerateSmoothNormals is set, these are bound to
    @ref Shaders::GenericGL3D::Normal.
-   If the mesh contains @ref Trade::MeshAttribute::TextureCoordinates, these
    are bound to @ref Shaders::GenericGL::TextureCoordinates.
-   If the mesh contains @ref Trade::MeshAttribute::Color, these are bound to
    @ref Shaders::GenericGL::Color3 / @relativeref{Shaders::GenericGL,Color4}
    based on their type.
-   If the mesh contains @ref Trade::MeshAttribute::JointIds and
    @ref Trade::MeshAttribute::Weights, these are bound to
    @ref Shaders::GenericGL::JointIds / @relativeref{Shaders::GenericGL,SecondaryJointIds} and
    @relativeref{Shaders::GenericGL,Weights} / @relativeref{Shaders::GenericGL,SecondaryWeights} according to rules
    described in @ref compiledPerVertexJointCount().
-   If the mesh contains @ref Trade::MeshAttribute::ObjectId, these are bound
    to @ref Shaders::GenericGL::ObjectId. However, if the mesh contains a
    @ref Trade::MeshAttribute::Bitangent as well, only the first appearing of
    the two is bound. The second is ignored with a warning as they share the
    same binding slot.
-   Custom attributes and known attributes of implementation-specific vertex
    formats are ignored with a warning. See
    @ref compile(const Trade::MeshData&, GL::Buffer&, GL::Buffer&)
    for an example showing how to bind them manually, and
    @ref CompileFlag::NoWarnOnCustomAttributes to suppress the warning.
-   Implementation-specific @ref Magnum::MeshPrimitive and
    @ref Magnum::MeshIndexType values are passed as-is with
    @ref meshPrimitiveUnwrap() and @ref meshIndexTypeUnwrap(). It's the user
    responsibility to ensure an implementation-specific value is valid in this
    context.
-   The index buffer is expected to be contiguous (size of the index type equal
    to @ref Trade::MeshData::indexStride()). OpenGL doesn't support interleaved
    index buffers. In case the @ref MeshIndexType is implementation-specific,
    this condition can't be checked and the buffer is assumed to be contiguous.
-   Stride of all attributes is expected to be positive. OpenGL doesn't support
    zero and negative strides.

If normal generation is not requested, @ref Trade::MeshData::indexData() and
@ref Trade::MeshData::vertexData() are uploaded as-is without any further
modifications, keeping the original layout and vertex formats. If
@ref CompileFlag::GenerateSmoothNormals is requested, vertex data is
interleaved together with the generated normals; if
@ref CompileFlag::GenerateFlatNormals is requested, the mesh is first
deindexed and then the vertex data is interleaved together with the generated
normals.

The generated mesh owns the index and vertex buffers and there's no possibility
to access them afterwards. For alternative solutions see the
@ref compile(const Trade::MeshData&, GL::Buffer&, GL::Buffer&) overloads.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref shaders-generic
*/
MAGNUM_MESHTOOLS_EXPORT GL::Mesh compile(const Trade::MeshData& mesh, CompileFlags flags);

/**
 * @overload
 * @m_since{2020,06}
 */
/* Separately because this one doesn't rely on duplicate() / interleave() /
   generate*Normals() and thus the exe can be smaller when using this function
   directly */
MAGNUM_MESHTOOLS_EXPORT GL::Mesh compile(const Trade::MeshData& mesh);

/**
@brief Compile mesh data using external buffers
@m_since{2020,06}

Assumes the whole vertex / index data are already uploaded to @p indices /
@p vertices and sets up the mesh using those. Can be used to have a single
index/vertex buffer when multiple @ref Trade::MeshData instances share the same
data arrays, or to allow buffer access later. For example:

@snippet MeshTools-gl.cpp compile-external

Another use case is specifying additional vertex attributes that are not
recognized by the function itself. You can choose among various r-value
overloads depending on whether you want to have the index/vertex buffers owned
by the mesh or not:

@snippet MeshTools-gl.cpp compile-external-attributes

If @p mesh is not indexed, the @p indices parameter is ignored --- in that case
you can pass a @ref NoCreate "NoCreate"-d instance to avoid allocating an
unnecessary OpenGL buffer object.

Compared to @ref compile(const Trade::MeshData&, CompileFlags), this function
implicitly enables the @ref CompileFlag::NoWarnOnCustomAttributes flag,
assuming that custom attributes and attributes with implementation-specific
formats are explicitly handled on the application side.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.
*/
MAGNUM_MESHTOOLS_EXPORT GL::Mesh compile(const Trade::MeshData& mesh, GL::Buffer& indices, GL::Buffer& vertices);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT GL::Mesh compile(const Trade::MeshData& mesh, GL::Buffer& indices, GL::Buffer&& vertices);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT GL::Mesh compile(const Trade::MeshData& mesh, GL::Buffer&& indices, GL::Buffer& vertices);

/**
 * @overload
 * @m_since{2020,06}
 */
MAGNUM_MESHTOOLS_EXPORT GL::Mesh compile(const Trade::MeshData& mesh, GL::Buffer&& indices, GL::Buffer&& vertices);

#ifdef MAGNUM_BUILD_DEPRECATED
/**
@brief Compile 2D mesh data
@m_deprecated_since{2020,06} Use @ref compile(const Trade::MeshData&, CompileFlags)
    instead.

Configures a mesh for @ref Shaders::GenericGL2D shader with vertex buffer and
possibly also an index buffer, if the mesh is indexed. Positions are bound to
@ref Shaders::GenericGL2D::Position attribute. If the mesh contains texture
coordinates, these are bound to @ref Shaders::GenericGL2D::TextureCoordinates
attribute. If the mesh contains colors, these are bound to @ref Shaders::GenericGL3D::Color4
attribute. No data compression or index optimization (except for index buffer
packing) is done, both the vertex buffer and the index buffer (if any) is owned
by the mesh, both created with @ref GL::BufferUsage::StaticDraw.

This is just a convenience function for creating generic meshes, you might want
to use @ref interleave() and @ref compressIndices() functions together with
@ref GL::Mesh::setPrimitive(), @ref GL::Mesh::setCount(),
@ref GL::Mesh::addVertexBuffer(), @ref GL::Mesh::setIndexBuffer() instead for
greater flexibility.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref shaders-generic
*/
CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_DEPRECATED("use compile(const Trade::MeshData&) instead") MAGNUM_MESHTOOLS_EXPORT GL::Mesh compile(const Trade::MeshData2D& meshData);
CORRADE_IGNORE_DEPRECATED_POP

/**
@brief Compile 3D mesh data
@m_deprecated_since{2020,06} Use @ref compile(const Trade::MeshData&, CompileFlags)
    instead.

Configures mesh for @ref Shaders::GenericGL3D shader with vertex buffer and
possibly also index buffer, if the mesh is indexed. Positions are bound to
@ref Shaders::GenericGL3D::Position attribute. If the mesh contains normals,
they are bound to @ref Shaders::GenericGL3D::Normal attribute, texture
coordinates are bound to @ref Shaders::GenericGL3D::TextureCoordinates
attribute. If the mesh contains colors, they are bound to
@ref Shaders::GenericGL3D::Color4 attribute. No data compression or index
optimization (except for index buffer packing) is done, both the vertex buffer
and the index buffer (if any) is owned by the mesh, both created with
@ref GL::BufferUsage::StaticDraw.

This is just a convenience function for creating generic meshes, you might want
to use @ref interleave() and @ref compressIndices() functions together with
@ref GL::Mesh::setPrimitive(), @ref GL::Mesh::setCount(),
@ref GL::Mesh::addVertexBuffer(), @ref GL::Mesh::setIndexBuffer() instead for
greater flexibility.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@see @ref shaders-generic
*/
CORRADE_IGNORE_DEPRECATED_PUSH
CORRADE_DEPRECATED("use compile(const Trade::MeshData&, CompileFlags) instead") MAGNUM_MESHTOOLS_EXPORT GL::Mesh compile(const Trade::MeshData3D& meshData, CompileFlags flags = {});
CORRADE_IGNORE_DEPRECATED_POP
#endif

#ifndef MAGNUM_TARGET_GLES2
/**
@brief Compiled per-vertex joint count for given mesh data
@m_since_latest

Returns the count of bound primary and secondary per-vertex joint IDs and
weights that a mesh returned from @ref compile(const Trade::MeshData&, CompileFlags)
would contain. The function goes over all @ref Trade::MeshAttribute::JointIds
and @relativeref{Trade::MeshAttribute,Weights} attributes present in the mesh
and assigns them to the primary and secondary binding points:

-   If the mesh contains just one instance of joint ID and weight attributes
    and their @ref Trade::MeshData::attributeArraySize() is not larger than
    4, they occupy just the primary binding slot. The second returned value
    is @cpp 0 @ce.
-   If the mesh contains more than one instance of joint ID and weight
    attributes and array size of the first instance is not larger than 4, the
    first instance goes to the primary binding slot and the first up to 4
    array components of the second instance go to the secondary slot. Remaining
    array components of the second instance and all remaining instances of
    joint ID and weight attributes are ignored.
-   If array size of the first instance of joint ID and weight attributes is
    larger than 4, the first slot uses the first 4 array components and the
    second the next up to 4 array components. Remaining array components of the
    first instance and all remaining instances of joint ID and weight
    attributes are ignored.

Useful to get subsequently fed to
@ref Shaders::FlatGL::Configuration::setJointCount() or to
@ref Shaders::FlatGL::setPerVertexJointCount() if
@ref Shaders::FlatGL::Flag::DynamicPerVertexJointCount is enabled, and
similarly with other builtin shaders. See @ref shaders-usage-skinning for a
high-level introduction.

@note This function is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@requires_gles30 Skinning implementation in builtin shaders requires integer
    support which is not available in OpenGL ES 2.0, thus neither this function
    is defined in OpenGL ES 2.0 builds.
@requires_webgl20 Skinning implementation in builtin shaders requires integer
    support which is not available in WebGL 1.0, thus neither this function is
    defined in WebGL 1.0 builds.
*/
MAGNUM_MESHTOOLS_EXPORT Containers::Pair<UnsignedInt, UnsignedInt> compiledPerVertexJointCount(const Trade::MeshData& mesh);
#endif

}}
#else
#error this header is available only in the OpenGL build
#endif

#endif
