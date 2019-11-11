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

#include "MeshData.h"

#include "Magnum/Math/Color.h"
#include "Magnum/Trade/Implementation/arrayUtilities.h"

namespace Magnum { namespace Trade {

MeshIndexData::MeshIndexData(const MeshIndexType type, const Containers::ArrayView<const void> data) noexcept: type{type}, data{reinterpret_cast<const Containers::ArrayView<const char>&>(data)} {
    CORRADE_ASSERT(!data.empty(),
        "Trade::MeshIndexData: index array can't be empty, create a non-indexed mesh instead", );
    CORRADE_ASSERT(data.size()%meshIndexTypeSize(type) == 0,
        "Trade::MeshIndexData: view size" << data.size() << "does not correspond to" << type, );
}

MeshAttributeData::MeshAttributeData(const MeshAttributeName name, const MeshAttributeType type, const Containers::StridedArrayView1D<const char>& data) noexcept: name{name}, type{type}, data{data} {
    /** @todo support zero / negative stride? would be hard to transfer to GL */
    CORRADE_ASSERT(data.empty() || std::ptrdiff_t(meshAttributeTypeSize(type)) <= data.stride(),
        "Trade::MeshAttributeData: view stride" << data.stride() << "is not large enough to contain" << type, );
    CORRADE_ASSERT(
        (name == MeshAttributeName::Position &&
            (type == MeshAttributeType::Vector2 ||
             type == MeshAttributeType::Vector3)) ||
        (name == MeshAttributeName::Normal &&
            (type == MeshAttributeType::Vector3)) ||
        (name == MeshAttributeName::Color &&
            (type == MeshAttributeType::Vector3 ||
             type == MeshAttributeType::Vector4)) ||
        (name == MeshAttributeName::TextureCoordinates &&
            (type == MeshAttributeType::Vector2)) ||
        isMeshAttributeNameCustom(name) /* can be any type */,
        "Trade::MeshAttributeData:" << type << "is not a valid type for" << name, );
}

Containers::Array<MeshAttributeData> meshAttributeDataNonOwningArray(const Containers::ArrayView<const MeshAttributeData> view) {
    /* Ugly, eh? */
    return Containers::Array<Trade::MeshAttributeData>{const_cast<Trade::MeshAttributeData*>(view.data()), view.size(), reinterpret_cast<void(*)(Trade::MeshAttributeData*, std::size_t)>(Trade::Implementation::nonOwnedArrayDeleter)};
}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: _indexType{indices.type}, _primitive{primitive}, _indexDataFlags{DataFlag::Owned|DataFlag::Mutable}, _vertexDataFlags{DataFlag::Owned|DataFlag::Mutable}, _importerState{importerState}, _indexData{std::move(indexData)}, _vertexData{std::move(vertexData)}, _attributes{std::move(attributes)}, _indices{indices.data} {
    /* Save vertex count. It's a strided array view, so the size is not
       depending on type. */
    if(_attributes.empty()) {
        CORRADE_ASSERT(indices.type != MeshIndexType{},
            "Trade::MeshData: indices are expected to be valid if there are no attributes and vertex count isn't passed explicitly", );
        /** @todo some better value? attributeless indexed with defined vertex count? */
        _vertexCount = 0;
    } else _vertexCount = _attributes[0].data.size();

    CORRADE_ASSERT(!_indices.empty() || !_indexData,
        "Trade::MeshData: indexData passed for a non-indexed mesh", );
    CORRADE_ASSERT(_indices.empty() || (_indices.begin() >= _indexData.begin() && _indices.end() <= _indexData.end()),
        "Trade::MeshData: indices are not contained in passed indexData array", );
    CORRADE_ASSERT(!_attributes.empty() || !_vertexData,
        "Trade::MeshData: vertexData passed for an attribute-less mesh", );
    CORRADE_ASSERT(_vertexCount || !_vertexData,
        "Trade::MeshData: vertexData passed for a mesh with zero vertices", );

    #ifndef CORRADE_NO_ASSERT
    /* Not checking what's already checked in MeshIndexData / MeshAttributeData
       constructors */
    for(std::size_t i = 0; i != _attributes.size(); ++i) {
        const MeshAttributeData& attribute = _attributes[i];
        CORRADE_ASSERT(attribute.data.size() == _vertexCount,
            "Trade::MeshData: attribute" << i << "has" << attribute.data.size() << "vertices but" << _vertexCount << "expected", );
        CORRADE_ASSERT(attribute.data.empty() || (&attribute.data.front() >= _vertexData.begin() && &attribute.data.back() + meshAttributeTypeSize(attribute.type) <= _vertexData.end()),
            "Trade::MeshData: attribute" << i << "is not contained in passed vertexData array", );
    }
    #endif
}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, std::move(indexData), indices, std::move(vertexData), Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, const Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(indexData.data())), indexData.size(), Implementation::nonOwnedArrayDeleter}, indices, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(vertexData.data())), vertexData.size(), Implementation::nonOwnedArrayDeleter}, std::move(attributes), importerState} {
    CORRADE_ASSERT(!(indexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned index data but" << indexDataFlags, );
    CORRADE_ASSERT(!(vertexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned vertex data but" << vertexDataFlags, );
    _indexDataFlags = indexDataFlags;
    _vertexDataFlags = vertexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, const Containers::ArrayView<const void> vertexData, const std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, indexDataFlags, indexData, indices, vertexDataFlags, vertexData, Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(indexData.data())), indexData.size(), Implementation::nonOwnedArrayDeleter}, indices, std::move(vertexData), std::move(attributes), importerState} {
    CORRADE_ASSERT(!(indexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned index data but" << indexDataFlags, );
    _indexDataFlags = indexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, Containers::Array<char>&& vertexData, const std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, indexDataFlags, indexData, indices, std::move(vertexData), Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: MeshData{primitive, std::move(indexData), indices, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(vertexData.data())), vertexData.size(), Implementation::nonOwnedArrayDeleter}, std::move(attributes), importerState} {
    CORRADE_ASSERT(!(vertexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned vertex data but" << vertexDataFlags, );
    _vertexDataFlags = vertexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, const DataFlags vertexDataFlags, const Containers::ArrayView<const void> vertexData, const std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, std::move(indexData), indices, vertexDataFlags, vertexData, Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: MeshData{primitive, {}, MeshIndexData{}, std::move(vertexData), std::move(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& vertexData, const std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, std::move(vertexData), Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, Containers::Array<MeshAttributeData>&& attributes, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(vertexData.data())), vertexData.size(), Implementation::nonOwnedArrayDeleter}, std::move(attributes), importerState} {
    CORRADE_ASSERT(!(vertexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned vertex data but" << vertexDataFlags, );
    _vertexDataFlags = vertexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags vertexDataFlags, Containers::ArrayView<const void> vertexData, std::initializer_list<MeshAttributeData> attributes, const void* const importerState): MeshData{primitive, vertexDataFlags, vertexData, Implementation::initializerListToArrayWithDefaultDeleter(attributes), importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, Containers::Array<char>&& indexData, const MeshIndexData& indices, const void* const importerState) noexcept: MeshData{primitive, std::move(indexData), indices, {}, {}, importerState} {}

MeshData::MeshData(const MeshPrimitive primitive, const DataFlags indexDataFlags, const Containers::ArrayView<const void> indexData, const MeshIndexData& indices, const void* const importerState) noexcept: MeshData{primitive, Containers::Array<char>{const_cast<char*>(static_cast<const char*>(indexData.data())), indexData.size(), Implementation::nonOwnedArrayDeleter}, indices, importerState} {
    CORRADE_ASSERT(!(indexDataFlags & DataFlag::Owned),
        "Trade::MeshData: can't construct with non-owned index data but" << indexDataFlags, );
    _indexDataFlags = indexDataFlags;
}

MeshData::MeshData(const MeshPrimitive primitive, const UnsignedInt vertexCount, const void* const importerState) noexcept: _vertexCount{vertexCount}, _indexType{}, _primitive{primitive}, _indexDataFlags{DataFlag::Owned|DataFlag::Mutable}, _vertexDataFlags{DataFlag::Owned|DataFlag::Mutable}, _importerState{importerState} {}

MeshData::~MeshData() = default;

MeshData::MeshData(MeshData&&) noexcept = default;

MeshData& MeshData::operator=(MeshData&&) noexcept = default;

Containers::ArrayView<char> MeshData::mutableIndexData() & {
    CORRADE_ASSERT(_indexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableIndexData(): index data not mutable", {});
    return _indexData;
}

Containers::ArrayView<char> MeshData::mutableVertexData() & {
    CORRADE_ASSERT(_vertexDataFlags & DataFlag::Mutable,
        "Trade::MeshData::mutableVertexData(): vertex data not mutable", {});
    return _vertexData;
}

UnsignedInt MeshData::indexCount() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indexCount(): the mesh is not indexed", {});
    return _indices.size()/meshIndexTypeSize(_indexType);
}

MeshIndexType MeshData::indexType() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indexType(): the mesh is not indexed", {});
    return _indexType;
}

MeshAttributeName MeshData::attributeName(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeName(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id].name;
}

MeshAttributeType MeshData::attributeType(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeType(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id].type;
}

std::size_t MeshData::attributeOffset(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeOffset(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return static_cast<const char*>(_attributes[id].data.data()) - _vertexData.data();
}

UnsignedInt MeshData::attributeStride(UnsignedInt id) const {
    CORRADE_ASSERT(id < _attributes.size(),
        "Trade::MeshData::attributeStride(): index" << id << "out of range for" << _attributes.size() << "attributes", {});
    return _attributes[id].data.stride();
}

UnsignedInt MeshData::attributeCount(const MeshAttributeName name) const {
    UnsignedInt count = 0;
    for(const MeshAttributeData& attribute: _attributes)
        if(attribute.name == name) ++count;
    return count;
}

UnsignedInt MeshData::attributeFor(const MeshAttributeName name, UnsignedInt id) const {
    for(std::size_t i = 0; i != _attributes.size(); ++i) {
        if(_attributes[i].name != name) continue;
        if(id-- == 0) return i;
    }

    #ifdef CORRADE_NO_ASSERT
    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    #else
    return ~UnsignedInt{};
    #endif
}

MeshAttributeType MeshData::attributeType(MeshAttributeName name, UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeType(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attributeType(attributeId);
}

std::size_t MeshData::attributeOffset(MeshAttributeName name, UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeOffset(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attributeOffset(attributeId);
}

UnsignedInt MeshData::attributeStride(MeshAttributeName name, UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(name, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::attributeStride(): index" << id << "out of range for" << attributeCount(name) << name << "attributes", {});
    return attributeStride(attributeId);
}

namespace {
    template<class T> Containers::Array<UnsignedInt> convertIndices(const Containers::ArrayView<const char> data) {
        const auto input = Containers::arrayCast<const T>(data);
        Containers::Array<UnsignedInt> output{input.size()};
        for(std::size_t i = 0; i != input.size(); ++i) output[i] = input[i];
        return output;
    }
}

Containers::Array<UnsignedInt> MeshData::indices() const {
    CORRADE_ASSERT(isIndexed(),
        "Trade::MeshData::indices(): the mesh is not indexed", {});

    switch(_indexType) {
        case MeshIndexType::UnsignedByte: return convertIndices<UnsignedByte>(_indices);
        case MeshIndexType::UnsignedShort: return convertIndices<UnsignedShort>(_indices);
        case MeshIndexType::UnsignedInt: return convertIndices<UnsignedInt>(_indices);
    }

    CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

namespace {

template<class Output, class Input = Output> Containers::Array<Output> copyAsArray(MeshAttributeType type, const Containers::StridedArrayView1D<const char>& data) {
    CORRADE_INTERNAL_ASSERT(type == Implementation::meshAttributeTypeFor<Input>());
    const auto input = Containers::arrayCast<const Input>(data);
    Containers::Array<Output> output{input.size()};
    for(std::size_t i = 0, max = input.size(); i != max; ++i)
        output[i] = input[i];
    return output;
}

}

Containers::Array<Vector2> MeshData::positions2D(const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttributeName::Position, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::positions2D(): index" << id << "out of range for" << attributeCount(MeshAttributeName::Position) << "position attributes", {});
    const MeshAttributeData& attribute = _attributes[attributeId];

    /* Copy 2D positions as-is, for 3D positions ignore Z */
    if(attribute.type == MeshAttributeType::Vector2)
        return copyAsArray<Vector2>(attribute.type, attribute.data);
    else if(attribute.type == MeshAttributeType::Vector3)
        return copyAsArray<Vector2>(MeshAttributeType::Vector2, attribute.data);
    else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<Vector3> MeshData::positions3D(const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttributeName::Position, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::positions3D(): index" << id << "out of range for" << attributeCount(MeshAttributeName::Position) << "position attributes", {});
    const MeshAttributeData& attribute = _attributes[attributeId];

    /* For 2D positions set Z to zero, copy 3D positions as-is */
    if(attribute.type == MeshAttributeType::Vector2) {
        Containers::Array<Vector3> output{attribute.data.size()};
        const auto input = Containers::arrayCast<const Vector2>(attribute.data);
        for(std::size_t i = 0, max = input.size(); i != max; ++i)
            output[i] = Vector3{input[i], 0.0f};
        return output;
    } else if(attribute.type == MeshAttributeType::Vector3) {
        return copyAsArray<Vector3>(attribute.type, attribute.data);
    } else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<Vector3> MeshData::normals(const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttributeName::Normal, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::normals(): index" << id << "out of range for" << attributeCount(MeshAttributeName::Normal) << "normal attributes", {});
    const MeshAttributeData& attribute = _attributes[attributeId];

    return copyAsArray<Vector3>(attribute.type, attribute.data);
}

Containers::Array<Vector2> MeshData::textureCoordinates2D(const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttributeName::TextureCoordinates, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::textureCoordinates2D(): index" << id << "out of range for" << attributeCount(MeshAttributeName::TextureCoordinates) << "texture coordinate attributes", {});
    const MeshAttributeData& attribute = _attributes[attributeId];

    return copyAsArray<Vector2>(attribute.type, attribute.data);
}

Containers::Array<Color4> MeshData::colors(const UnsignedInt id) const {
    const UnsignedInt attributeId = attributeFor(MeshAttributeName::Color, id);
    CORRADE_ASSERT(attributeId != ~UnsignedInt{}, "Trade::MeshData::colors(): index" << id << "out of range for" << attributeCount(MeshAttributeName::Color) << "color attributes", {});
    const MeshAttributeData& attribute = _attributes[attributeId];

    if(_attributes[attributeId].type == MeshAttributeType::Vector3)
        return copyAsArray<Color4, Color3>(attribute.type, attribute.data);
    else if(_attributes[attributeId].type == MeshAttributeType::Vector4)
        return copyAsArray<Color4, Color4>(attribute.type, attribute.data);
    else CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
}

Containers::Array<char> MeshData::releaseIndexData() {
    _indexType = MeshIndexType{}; /* so isIndexed() returns false */
    _indices = nullptr;
    return std::move(_indexData);
}

Containers::Array<char> MeshData::releaseVertexData() {
    _attributes = nullptr;
    return std::move(_vertexData);
}

Debug& operator<<(Debug& debug, const MeshAttributeName value) {
    debug << "Trade::MeshAttributeName" << Debug::nospace;

    if(UnsignedByte(value) >= UnsignedByte(MeshAttributeName::Custom))
        return debug << "::Custom(" << Debug::nospace << (UnsignedByte(value) - UnsignedByte(MeshAttributeName::Custom)) << Debug::nospace << ")";

    switch(value) {
        /* LCOV_EXCL_START */
        #define _c(value) case MeshAttributeName::value: return debug << "::" << Debug::nospace << #value;
        _c(Position)
        _c(Normal)
        _c(TextureCoordinates)
        _c(Color)
        #undef _c
        /* LCOV_EXCL_STOP */

        /* To silence compiler warning about unhandled values */
        case MeshAttributeName::Custom: CORRADE_ASSERT_UNREACHABLE(); /* LCOV_EXCL_LINE */
    }

    return debug << "(" << Debug::nospace << reinterpret_cast<void*>(UnsignedByte(value)) << Debug::nospace << ")";
}

}}
