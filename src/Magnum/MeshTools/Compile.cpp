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

#include "Compile.h"

#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StridedArrayView.h>
#include <Corrade/Containers/ArrayViewStl.h> /** @todo remove once MeshDataXD is gone */

#include "Magnum/GL/Buffer.h"
#include "Magnum/GL/Mesh.h"
#include "Magnum/Math/Vector3.h"
#include "Magnum/Math/Color.h"
#include "Magnum/MeshTools/CompressIndices.h"
#include "Magnum/MeshTools/GenerateNormals.h"
#include "Magnum/MeshTools/Duplicate.h"
#include "Magnum/MeshTools/Interleave.h"
#include "Magnum/Trade/MeshData.h"

#ifdef MAGNUM_BUILD_DEPRECATED
#define _MAGNUM_NO_DEPRECATED_MESHDATA /* So it doesn't yell here */

#include "Magnum/Trade/MeshData2D.h"
#include "Magnum/Trade/MeshData3D.h"
#endif

/* This header is included only privately and doesn't introduce any linker
   dependency, thus it's completely safe */
#include "Magnum/Shaders/Generic.h"

namespace Magnum { namespace MeshTools {

GL::Mesh compile(const Trade::MeshData& meshData, CompileFlags flags) {
    /* If we want to generate normals, prepare a new mesh data and recurse,
       with the flags unset */
    if(meshData.primitive() == MeshPrimitive::Triangles && (flags & (CompileFlag::GenerateFlatNormals|CompileFlag::GenerateSmoothNormals))) {
        CORRADE_ASSERT(meshData.attributeCount(Trade::MeshAttributeName::Position),
            "MeshTools::compile(): the mesh has no positions, can't generate normals", GL::Mesh{});
        /* Right now this could fire only if we have 2D positions, which is
           unlikely; in the future it might fire once packed types are added */
        CORRADE_ASSERT(meshData.attributeType(Trade::MeshAttributeName::Position) == MeshAttributeType::Vector3,
            "MeshTools::compile(): can't generate normals for positions of type" << meshData.attributeType(Trade::MeshAttributeName::Position), GL::Mesh{});

        /* If the data already have a normal array, reuse its location,
           otherwise mix in an extra one */
        Trade::MeshAttributeData normalAttribute;
        Containers::ArrayView<const Trade::MeshAttributeData> extra;
        if(!meshData.hasAttribute(Trade::MeshAttributeName::Normal)) {
            normalAttribute = Trade::MeshAttributeData{
                Trade::MeshAttributeName::Normal, MeshAttributeType::Vector3,
                nullptr};
            extra = {&normalAttribute, 1};
        /* If we reuse a normal location, expect correct type. Again this won't
           fire now, but might in the future once packed types are added */
        } else CORRADE_ASSERT(meshData.attributeType(Trade::MeshAttributeName::Normal) == MeshAttributeType::Vector3,
            "MeshTools::compile(): can't generate normals into type", GL::Mesh{});

        /* If we want flat normals, we need to first duplicate everything using
           the index buffer. Otherwise just interleave the potential extra
           normal attribute in. */
        Trade::MeshData generated{MeshPrimitive::Points, 0};
        if(flags & CompileFlag::GenerateFlatNormals && meshData.isIndexed())
            generated = duplicate(meshData, extra);
        else
            generated = interleave(meshData, extra);

        /* Generate the normals. If we don't have the index buffer, we can only
           generate flat ones. */
        if(flags & CompileFlag::GenerateFlatNormals || !meshData.isIndexed())
            generateFlatNormalsInto(
                generated.attribute<Vector3>(Trade::MeshAttributeName::Position),
                generated.mutableAttribute<Vector3>(Trade::MeshAttributeName::Normal));
        else
            generateSmoothNormalsInto(generated.indices(),
                generated.attribute<Vector3>(Trade::MeshAttributeName::Position),
                generated.mutableAttribute<Vector3>(Trade::MeshAttributeName::Normal));

        return compile(generated, flags & ~(CompileFlag::GenerateFlatNormals|CompileFlag::GenerateSmoothNormals));
    }

    flags &= ~(CompileFlag::GenerateFlatNormals|CompileFlag::GenerateSmoothNormals);
    CORRADE_INTERNAL_ASSERT(!flags);
    return compile(meshData);
}

GL::Mesh compile(const Trade::MeshData& meshData) {
    GL::Buffer indices{NoCreate};
    if(meshData.isIndexed()) {
        indices = GL::Buffer{GL::Buffer::TargetHint::ElementArray};
        indices.setData(meshData.indexData());
    }

    GL::Buffer vertices{GL::Buffer::TargetHint::Array};
    vertices.setData(meshData.vertexData());

    return compile(meshData, std::move(indices), std::move(vertices));
}

GL::Mesh compile(const Trade::MeshData& meshData, GL::Buffer& indices, GL::Buffer& vertices) {
    return compile(meshData, GL::Buffer::wrap(indices.id(), GL::Buffer::TargetHint::ElementArray), GL::Buffer::wrap(vertices.id(), GL::Buffer::TargetHint::Array));
}

GL::Mesh compile(const Trade::MeshData& meshData, GL::Buffer& indices, GL::Buffer&& vertices) {
    return compile(meshData, GL::Buffer::wrap(indices.id(), GL::Buffer::TargetHint::ElementArray), std::move(vertices));
}

GL::Mesh compile(const Trade::MeshData& meshData, GL::Buffer&& indices, GL::Buffer& vertices) {
    return compile(meshData, std::move(indices), GL::Buffer::wrap(vertices.id(), GL::Buffer::TargetHint::Array));
}

GL::Mesh compile(const Trade::MeshData& meshData, GL::Buffer&& indices, GL::Buffer&& vertices) {
    CORRADE_ASSERT((!meshData.isIndexed() || indices.id()) && vertices.id(),
        "MeshTools::compile(): invalid external buffer(s)", GL::Mesh{});

    /* Basics */
    GL::Mesh mesh;
    mesh.setPrimitive(meshData.primitive());

    /* Vertex data */
    GL::Buffer verticesRef = GL::Buffer::wrap(vertices.id(), GL::Buffer::TargetHint::Array);
    for(UnsignedInt i = 0; i != meshData.attributeCount(); ++i) {
        Containers::Optional<GL::DynamicAttribute> attribute;
        switch(meshData.attributeName(i)) {
            case Trade::MeshAttributeName::Position:
                if(meshData.attributeType(i) == MeshAttributeType::Vector2)
                    attribute.emplace(Shaders::Generic2D::Position{});
                else if(meshData.attributeType(i) == MeshAttributeType::Vector3)
                    attribute.emplace(Shaders::Generic3D::Position{});
                else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
                break;
            case Trade::MeshAttributeName::Normal:
                CORRADE_INTERNAL_ASSERT(meshData.attributeType(i) == MeshAttributeType::Vector3);
                attribute.emplace(Shaders::Generic3D::Normal{});
                break;
            case Trade::MeshAttributeName::TextureCoordinates:
                CORRADE_INTERNAL_ASSERT(meshData.attributeType(i) == MeshAttributeType::Vector2);
                /** @todo have Generic2D derived from Generic that has all
                    attribute definitions common for 2D and 3D */
                attribute.emplace(Shaders::Generic2D::TextureCoordinates{});
                break;
            case Trade::MeshAttributeName::Color:
                /** @todo have Generic2D derived from Generic that has all
                    attribute definitions common for 2D and 3D */
                if(meshData.attributeType(i) == MeshAttributeType::Vector3)
                    attribute.emplace(Shaders::Generic2D::Color3{});
                else if(meshData.attributeType(i) == MeshAttributeType::Vector4)
                    attribute.emplace(Shaders::Generic2D::Color4{});
                else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
                break;

            /* So it doesn't yell that we didn't handle a known attribute */
            case Trade::MeshAttributeName::Custom: break; /* LCOV_EXCL_LINE */
        }

        if(!attribute) {
            Warning{} << "MeshTools::compile(): ignoring unknown attribute" << meshData.attributeName(i);
            continue;
        }

        /* For the first attribute move the buffer in, for all others use the
           reference */
        /** @todo FFS! the dynamic variant needs an enable_if, i don't want to
            cast like this */
        if(vertices.id()) mesh.addVertexBuffer(std::move(vertices),
            GLintptr(meshData.attributeOffset(i)),
            GLsizei(meshData.attributeStride(i)), *attribute);
        else mesh.addVertexBuffer(verticesRef,
            GLintptr(meshData.attributeOffset(i)),
            GLsizei(meshData.attributeStride(i)), *attribute);
    }

    if(meshData.isIndexed()) {
        mesh.setIndexBuffer(std::move(indices), 0, meshData.indexType())
            .setCount(meshData.indexCount());
    } else mesh.setCount(meshData.vertexCount());

    return mesh;
}

#ifdef MAGNUM_BUILD_DEPRECATED
CORRADE_IGNORE_DEPRECATED_PUSH
GL::Mesh compile(const Trade::MeshData2D& meshData) {
    GL::Mesh mesh;
    mesh.setPrimitive(meshData.primitive());

    /* Decide about stride and offsets */
    UnsignedInt stride = sizeof(Shaders::Generic2D::Position::Type);
    UnsignedInt textureCoordsOffset = sizeof(Shaders::Generic2D::Position::Type);
    UnsignedInt colorsOffset = sizeof(Shaders::Generic2D::Position::Type);
    if(meshData.hasTextureCoords2D()) {
        stride += sizeof(Shaders::Generic2D::TextureCoordinates::Type);
        colorsOffset += sizeof(Shaders::Generic2D::TextureCoordinates::Type);
    }
    if(meshData.hasColors())
        stride += sizeof(Shaders::Generic2D::Color4::Type);

    /* Create vertex buffer */
    GL::Buffer vertexBuffer{GL::Buffer::TargetHint::Array};
    GL::Buffer vertexBufferRef = GL::Buffer::wrap(vertexBuffer.id(), GL::Buffer::TargetHint::Array);

    /* Interleave positions and put them in with ownership transfer, use the
       ref for the rest */
    Containers::Array<char> data = MeshTools::interleave(
        meshData.positions(0),
        stride - sizeof(Shaders::Generic2D::Position::Type));
    mesh.addVertexBuffer(std::move(vertexBuffer), 0,
        Shaders::Generic2D::Position(),
        stride - sizeof(Shaders::Generic2D::Position::Type));

    /* Add also texture coordinates, if present */
    if(meshData.hasTextureCoords2D()) {
        MeshTools::interleaveInto(data,
            textureCoordsOffset,
            meshData.textureCoords2D(0),
            stride - textureCoordsOffset - sizeof(Shaders::Generic2D::TextureCoordinates::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            textureCoordsOffset,
            Shaders::Generic2D::TextureCoordinates(),
            stride - textureCoordsOffset - sizeof(Shaders::Generic2D::TextureCoordinates::Type));
    }

    /* Add also colors, if present */
    if(meshData.hasColors()) {
        MeshTools::interleaveInto(data,
            colorsOffset,
            meshData.colors(0),
            stride - colorsOffset - sizeof(Shaders::Generic3D::Color4::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            colorsOffset,
            Shaders::Generic3D::Color4(),
            stride - colorsOffset - sizeof(Shaders::Generic3D::Color4::Type));
    }

    /* Fill vertex buffer with interleaved data */
    vertexBufferRef.setData(data, GL::BufferUsage::StaticDraw);

    /* If indexed, fill index buffer and configure indexed mesh */
    if(meshData.isIndexed()) {
        Containers::Array<char> indexData;
        MeshIndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(meshData.indices());

        GL::Buffer indexBuffer{GL::Buffer::TargetHint::ElementArray};
        indexBuffer.setData(indexData, GL::BufferUsage::StaticDraw);
        mesh.setCount(meshData.indices().size())
            .setIndexBuffer(std::move(indexBuffer), 0, indexType, indexStart, indexEnd);

    /* Else set vertex count */
    } else mesh.setCount(meshData.positions(0).size());

    return mesh;
}

std::tuple<GL::Mesh, std::unique_ptr<GL::Buffer>, std::unique_ptr<GL::Buffer>> compile(const Trade::MeshData2D& meshData, GL::BufferUsage) {
    return std::make_tuple(compile(meshData),
        std::unique_ptr<GL::Buffer>{new GL::Buffer{NoCreate}},
        std::unique_ptr<GL::Buffer>{meshData.isIndexed() ? new GL::Buffer{NoCreate} : nullptr});
}

GL::Mesh compile(const Trade::MeshData3D& meshData, CompileFlags flags) {
    GL::Mesh mesh;
    mesh.setPrimitive(meshData.primitive());

    const bool generateNormals = flags & (CompileFlag::GenerateFlatNormals|CompileFlag::GenerateSmoothNormals) && meshData.primitive() == MeshPrimitive::Triangles;

    /* Decide about stride and offsets */
    UnsignedInt stride = sizeof(Shaders::Generic3D::Position::Type);
    const UnsignedInt normalOffset = sizeof(Shaders::Generic3D::Position::Type);
    UnsignedInt textureCoordsOffset = sizeof(Shaders::Generic3D::Position::Type);
    UnsignedInt colorsOffset = sizeof(Shaders::Generic3D::Position::Type);
    if(meshData.hasNormals() || generateNormals) {
        stride += sizeof(Shaders::Generic3D::Normal::Type);
        textureCoordsOffset += sizeof(Shaders::Generic3D::Normal::Type);
        colorsOffset += sizeof(Shaders::Generic3D::Normal::Type);
    }
    if(meshData.hasTextureCoords2D()) {
        stride += sizeof(Shaders::Generic3D::TextureCoordinates::Type);
        colorsOffset += sizeof(Shaders::Generic3D::TextureCoordinates::Type);
    }
    if(meshData.hasColors())
        stride += sizeof(Shaders::Generic3D::Color4::Type);

    /* Create vertex buffer */
    GL::Buffer vertexBuffer{GL::Buffer::TargetHint::Array};
    GL::Buffer vertexBufferRef = GL::Buffer::wrap(vertexBuffer.id(), GL::Buffer::TargetHint::Array);

    /* Indirect reference to the mesh data -- either directly the original mesh
       data or processed ones */
    Containers::StridedArrayView1D<const Vector3> positions;
    Containers::StridedArrayView1D<const Vector3> normals;
    Containers::StridedArrayView1D<const Vector2> textureCoords2D;
    Containers::StridedArrayView1D<const Color4> colors;
    bool useIndices; /**< @todo turn into a view once compressIndices() takes views */

    /* If the mesh has no normals, we want to generate them and the mesh is an
       indexed triangle mesh, duplicate all attributes, otherwise just
       reference the original data */
    Containers::Array<Vector3> positionStorage;
    Containers::Array<Vector3> normalStorage;
    Containers::Array<Vector2> textureCoords2DStorage;
    Containers::Array<Color4> colorStorage;
    if(generateNormals) {
        /* If we want flat normals and the mesh is indexed, duplicate all
           attributes */
        if(flags & CompileFlag::GenerateFlatNormals && meshData.isIndexed()) {
            positionStorage = duplicate(
                Containers::stridedArrayView(meshData.indices()), Containers::stridedArrayView(meshData.positions(0)));
            positions = Containers::arrayView(positionStorage);
            if(meshData.hasTextureCoords2D()) {
                textureCoords2DStorage = duplicate(
                    Containers::stridedArrayView(meshData.indices()),
                    Containers::stridedArrayView(meshData.textureCoords2D(0)));
                textureCoords2D = Containers::arrayView(textureCoords2DStorage);
            }
            if(meshData.hasColors()) {
                colorStorage = duplicate(
                    Containers::stridedArrayView(meshData.indices()),
                    Containers::stridedArrayView(meshData.colors(0)));
                colors = Containers::arrayView(colorStorage);
            }
        } else {
            positions = meshData.positions(0);
            if(meshData.hasTextureCoords2D())
                textureCoords2D = meshData.textureCoords2D(0);
            if(meshData.hasColors())
                colors = meshData.colors(0);
        }

        if(flags & CompileFlag::GenerateFlatNormals || !meshData.isIndexed()) {
            normalStorage = generateFlatNormals(positions);
            useIndices = false;
        } else {
            normalStorage = generateSmoothNormals(meshData.indices(), positions);
            useIndices = true;
        }

        normals = Containers::arrayView(normalStorage);

    } else {
        positions = meshData.positions(0);
        if(meshData.hasNormals()) normals = meshData.normals(0);
        if(meshData.hasTextureCoords2D()) textureCoords2D = meshData.textureCoords2D(0);
        if(meshData.hasColors()) colors = meshData.colors(0);
        useIndices = meshData.isIndexed();
    }

    /* Interleave positions and put them in with ownership transfer, use the
       ref for the rest */
    Containers::Array<char> data = MeshTools::interleave(
        positions,
        stride - sizeof(Shaders::Generic3D::Position::Type));
    mesh.addVertexBuffer(std::move(vertexBuffer), 0,
        Shaders::Generic3D::Position(),
        stride - sizeof(Shaders::Generic3D::Position::Type));

    /* Add also normals, if present */
    if(normals) {
        MeshTools::interleaveInto(data,
            normalOffset,
            normals,
            stride - normalOffset - sizeof(Shaders::Generic3D::Normal::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            normalOffset,
            Shaders::Generic3D::Normal(),
            stride - normalOffset - sizeof(Shaders::Generic3D::Normal::Type));
    }

    /* Add also texture coordinates, if present */
    if(textureCoords2D) {
        MeshTools::interleaveInto(data,
            textureCoordsOffset,
            textureCoords2D,
            stride - textureCoordsOffset - sizeof(Shaders::Generic3D::TextureCoordinates::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            textureCoordsOffset,
            Shaders::Generic3D::TextureCoordinates(),
            stride - textureCoordsOffset - sizeof(Shaders::Generic3D::TextureCoordinates::Type));
    }

    /* Add also colors, if present */
    if(colors) {
        MeshTools::interleaveInto(data,
            colorsOffset,
            colors,
            stride - colorsOffset - sizeof(Shaders::Generic3D::Color4::Type));
        mesh.addVertexBuffer(vertexBufferRef, 0,
            colorsOffset,
            Shaders::Generic3D::Color4(),
            stride - colorsOffset - sizeof(Shaders::Generic3D::Color4::Type));
    }

    /* Fill vertex buffer with interleaved data */
    vertexBufferRef.setData(data, GL::BufferUsage::StaticDraw);

    /* If indexed (and the mesh didn't have the vertex data duplicated for flat
       normals), fill index buffer and configure indexed mesh */
    if(useIndices) {
        Containers::Array<char> indexData;
        MeshIndexType indexType;
        UnsignedInt indexStart, indexEnd;
        std::tie(indexData, indexType, indexStart, indexEnd) = MeshTools::compressIndices(meshData.indices());

        GL::Buffer indexBuffer{GL::Buffer::TargetHint::ElementArray};
        indexBuffer.setData(indexData, GL::BufferUsage::StaticDraw);
        mesh.setCount(meshData.indices().size())
            .setIndexBuffer(std::move(indexBuffer), 0, indexType, indexStart, indexEnd);

    /* Else set vertex count */
    } else mesh.setCount(positions.size());

    return mesh;
}

std::tuple<GL::Mesh, std::unique_ptr<GL::Buffer>, std::unique_ptr<GL::Buffer>> compile(const Trade::MeshData3D& meshData, GL::BufferUsage) {
    return std::make_tuple(compile(meshData),
        std::unique_ptr<GL::Buffer>{new GL::Buffer{NoCreate}},
        std::unique_ptr<GL::Buffer>{meshData.isIndexed() ? new GL::Buffer{NoCreate} : nullptr});
}
CORRADE_IGNORE_DEPRECATED_POP
#endif

}}
