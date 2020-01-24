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

#include "Interleave.h"

#include <Corrade/Utility/Algorithms.h>

#include "Magnum/Math/Functions.h"
#include "Magnum/Trade/MeshData.h"

namespace Magnum { namespace MeshTools {

bool isInterleaved(const Trade::MeshData& data) {
    /* There is nothing, so yes it is (because there is nothing we could do
       to make it interleaved anyway) */
    if(!data.attributeCount()) return true;

    const UnsignedInt stride = data.attributeStride(0);
    std::size_t minOffset = data.attributeOffset(0);
    std::size_t maxOffset = minOffset;
    for(UnsignedInt i = 1; i != data.attributeCount(); ++i) {
        if(data.attributeStride(i) != stride) return false;

        const std::size_t offset = data.attributeOffset(i);
        minOffset = Math::min(minOffset, offset);
        maxOffset = Math::max(maxOffset, offset + meshAttributeTypeSize(data.attributeType(i)));
    }

    return maxOffset - minOffset <= stride;
}

Trade::MeshData interleavedLayout(const Trade::MeshData& data, const UnsignedInt vertexCount, const Containers::ArrayView<const Trade::MeshAttributeData> extra) {
    /* If there are no attributes, bail -- return an empty mesh with desired
       vertex count but nothing else */
    if(!data.attributeCount() && extra.empty())
        return Trade::MeshData{data.primitive(), vertexCount};

    const bool interleaved = isInterleaved(data);

    /* If the mesh is already interleaved, use the original stride to
       preserve all padding, but remove the initial offset. Otherwise calculate
       a tightly-packed stride. */
    std::size_t stride;
    std::size_t minOffset;
    if(interleaved && data.attributeCount()) {
        stride = data.attributeStride(0);
        minOffset = ~std::size_t{};
        for(UnsignedInt i = 0, max = data.attributeCount(); i != max; ++i)
            minOffset = Math::min(minOffset, data.attributeOffset(i));
    } else {
        stride = 0;
        minOffset = 0;
        for(UnsignedInt i = 0, max = data.attributeCount(); i != max; ++i)
            stride += meshAttributeTypeSize(data.attributeType(i));
    }

    /* Add the extra attributes and explicit padding */
    std::size_t extraAttributeCount = 0;
    for(std::size_t i = 0; i != extra.size(); ++i) {
        if(extra[i].type() == MeshAttributeType{}) {
            CORRADE_ASSERT(extra[i].data().stride() > 0 || stride >= std::size_t(-extra[i].data().stride()),
                "MeshTools::interleavedLayout(): negative padding" << extra[i].data().stride() << "in extra attribute" << i << "too large for stride" << stride, (Trade::MeshData{MeshPrimitive::Points, 0}));
            stride += extra[i].data().stride();
        } else {
            stride += meshAttributeTypeSize(extra[i].type());
            ++extraAttributeCount;
        }
    }

    /* Allocate new data and attribute array */
    Containers::Array<char> vertexData{Containers::NoInit, stride*vertexCount};
    Containers::Array<Trade::MeshAttributeData> attributeData{data.attributeCount() + extraAttributeCount};

    /* Copy existing attribute layout. If the original is already interleaved,
       preserve relative attribute offsets, otherwise pack tightly. */
    std::size_t offset = 0;
    for(UnsignedInt i = 0; i != data.attributeCount(); ++i) {
        if(interleaved) offset = data.attributeOffset(i) - minOffset;

        attributeData[i] = Trade::MeshAttributeData{
            data.attributeName(i), data.attributeType(i),
            Containers::StridedArrayView1D<void>{vertexData, vertexData + offset,
            vertexCount, std::ptrdiff_t(stride)}};

        if(!interleaved) offset += meshAttributeTypeSize(data.attributeType(i));
    }

    /* In case the original is already interleaved, set the offset for extra
       attribs to the original stride to preserve also potential padding at the
       end. */
    if(interleaved && data.attributeCount())
        offset = data.attributeStride(0);

    /* Mix in the extra attributes */
    UnsignedInt attributeIndex = data.attributeCount();
    for(UnsignedInt i = 0; i != extra.size(); ++i) {
        /* Padding, only adjust the offset for next attribute */
        if(extra[i].type() == MeshAttributeType{}) {
            offset += extra[i].data().stride();
            continue;
        }

        attributeData[attributeIndex++] = Trade::MeshAttributeData{
            extra[i].name(), extra[i].type(), Containers::StridedArrayView1D<void>{vertexData, vertexData + offset,
            vertexCount, std::ptrdiff_t(stride)}};

        offset += meshAttributeTypeSize(extra[i].type());
    }

    return Trade::MeshData{data.primitive(), std::move(vertexData), std::move(attributeData)};
}

Trade::MeshData interleavedLayout(const Trade::MeshData& data, const UnsignedInt vertexCount, const std::initializer_list<Trade::MeshAttributeData> extra) {
    return interleavedLayout(data, vertexCount, Containers::arrayView(extra));
}

Trade::MeshData interleave(Trade::MeshData&& data, const Containers::ArrayView<const Trade::MeshAttributeData> extra) {
    /* If there are no attributes and no index buffer, bail -- the vertex count
       is the only property we can transfer. If this wouldn't be done, the
       return at the end would assert as vertex count is only passed implicitly
       via attributes (which there are none). */
    if(!data.attributeCount() && extra.empty() && !data.isIndexed())
        return Trade::MeshData{data.primitive(), data.vertexCount()};

    /* Transfer the indices unchanged, in case the mesh is indexed */
    Containers::Array<char> indexData;
    Trade::MeshIndexData indices;
    if(data.isIndexed()) {
        /* If we can steal the data, do it */
        if(data.indexDataFlags() & Trade::DataFlag::Owned) {
            indices = Trade::MeshIndexData{data.indices()};
            indexData = data.releaseIndexData();
        } else {
            indexData = Containers::Array<char>{data.indexData().size()};
            Utility::copy(data.indexData(), indexData);
            indices = Trade::MeshIndexData{data.indexType(),
                Containers::ArrayView<const void>{indexData + data.indexOffset(), data.indices().size()[0]*data.indices().size()[1]}};
        }
    }

    const bool interleaved = isInterleaved(data);

    /* If the mesh is already interleaved and we don't have anything extra,
       steal that data as well */
    Containers::Array<char> vertexData;
    Containers::Array<Trade::MeshAttributeData> attributeData;
    if(interleaved && extra.empty() && (data.vertexDataFlags() & Trade::DataFlag::Owned)) {
        attributeData = data.releaseAttributeData();
        vertexData = data.releaseVertexData();

    /* Otherwise do it the hard way */
    } else {
        /* Calculate the layout */
        Trade::MeshData layout = interleavedLayout(data, data.vertexCount(), extra);

        /* Copy existing attributes to new locations */
        for(UnsignedInt i = 0; i != data.attributeCount(); ++i)
            Utility::copy(data.attribute(i), layout.mutableAttribute(i));

        /* Mix in the extra attributes */
        UnsignedInt attributeIndex = data.attributeCount();
        for(UnsignedInt i = 0; i != extra.size(); ++i) {
            /* Padding, ignore */
            if(extra[i].type() == MeshAttributeType{}) continue;

            /* Copy the attribute in, if it is non-empty, otherwise keep the
               memory uninitialized */
            if(extra[i].data()) {
                CORRADE_ASSERT(extra[i].data().size() == data.vertexCount(),
                    "MeshTools::interleave(): extra attribute" << i << "expected to have" << data.vertexCount() << "items but got" << extra[i].data().size(),
                    (Trade::MeshData{MeshPrimitive::Triangles, 0}));
                const std::size_t attributeTypeSize = meshAttributeTypeSize(extra[i].type());
                /** @todo ugh write an arrayCast variant for this (how?) */
                const Containers::StridedArrayView2D<const char> attributeData{
                    /* The view size is there only for a size assert, we're
                       pretty sure the view is valid */
                    {static_cast<const char*>(extra[i].data().data()), ~std::size_t{}},
                    {extra[i].data().size(), attributeTypeSize},
                    {extra[i].data().stride(), 1}};
                Utility::copy(attributeData, layout.mutableAttribute(attributeIndex));
            }

            ++attributeIndex;
        }

        /* Release the data from the layout to pack them into the output */
        vertexData = layout.releaseVertexData();
        attributeData = layout.releaseAttributeData();
    }

    return Trade::MeshData{data.primitive(), std::move(indexData), indices,
        std::move(vertexData), std::move(attributeData)};
}

Trade::MeshData interleave(Trade::MeshData&& data, const std::initializer_list<Trade::MeshAttributeData> extra) {
    return interleave(std::move(data), Containers::arrayView(extra));
}

Trade::MeshData interleave(const Trade::MeshData& data, const Containers::ArrayView<const Trade::MeshAttributeData> extra) {
    Containers::ArrayView<const char> indexData;
    Trade::MeshIndexData indices;
    if(data.isIndexed()) {
        indexData = data.indexData();
        indices = Trade::MeshIndexData{data.indices()};
    }

    return interleave(Trade::MeshData{data.primitive(),
        {}, indexData, indices,
        {}, data.vertexData(), Trade::meshAttributeDataNonOwningArray(data.attributeData())
    }, extra);

}

Trade::MeshData interleave(const Trade::MeshData& data, const std::initializer_list<Trade::MeshAttributeData> extra) {
    return interleave(std::move(data), Containers::arrayView(extra));
}

}}
