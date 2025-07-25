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

#include "AbstractQuery.h"

#ifndef MAGNUM_TARGET_WEBGL
#include <Corrade/Containers/String.h>
#endif
#include <Corrade/Utility/Assert.h>

#include "Magnum/GL/Context.h"
#ifndef MAGNUM_TARGET_WEBGL
#include "Magnum/GL/Implementation/DebugState.h"
#endif
#include "Magnum/GL/Implementation/QueryState.h"
#include "Magnum/GL/Implementation/State.h"

namespace Magnum { namespace GL {

AbstractQuery::AbstractQuery(GLenum target): _target{target}, _flags{ObjectFlag::DeleteOnDestruction} {
    Context::current().state().query.createImplementation(*this);
}

AbstractQuery::~AbstractQuery() {
    /* Moved out or not deleting on destruction, nothing to do */
    if(!_id || !(_flags & ObjectFlag::DeleteOnDestruction))
        return;

    #ifndef MAGNUM_TARGET_GLES2
    glDeleteQueries(1, &_id);
    #else
    glDeleteQueriesEXT(1, &_id);
    #endif
}

void AbstractQuery::createImplementationDefault(AbstractQuery& self) {
    #ifndef MAGNUM_TARGET_GLES2
    glGenQueries(1, &self._id);
    #else
    glGenQueriesEXT(1, &self._id);
    #endif
}

#ifndef MAGNUM_TARGET_GLES
void AbstractQuery::createImplementationDSA(AbstractQuery& self) {
    glCreateQueries(self._target, 1, &self._id);
    self._flags |= ObjectFlag::Created;
}

void AbstractQuery::createImplementationDSAExceptXfbOverflow(AbstractQuery& self) {
    if(self._target == GL_TRANSFORM_FEEDBACK_OVERFLOW || self._target == GL_TRANSFORM_FEEDBACK_STREAM_OVERFLOW)
        createImplementationDefault(self);
    else
        createImplementationDSA(self);
}

void AbstractQuery::createImplementationDSAExceptPipelineStats(AbstractQuery& self) {
    if(self._target == GL_VERTICES_SUBMITTED ||
       self._target == GL_PRIMITIVES_SUBMITTED ||
       self._target == GL_VERTEX_SHADER_INVOCATIONS ||
       self._target == GL_TESS_CONTROL_SHADER_PATCHES ||
       self._target == GL_TESS_EVALUATION_SHADER_INVOCATIONS ||
       self._target == GL_GEOMETRY_SHADER_INVOCATIONS ||
       self._target == GL_GEOMETRY_SHADER_PRIMITIVES_EMITTED ||
       self._target == GL_FRAGMENT_SHADER_INVOCATIONS ||
       self._target == GL_COMPUTE_SHADER_INVOCATIONS ||
       self._target == GL_CLIPPING_INPUT_PRIMITIVES ||
       self._target == GL_CLIPPING_OUTPUT_PRIMITIVES)
        createImplementationDefault(self);
    else
        createImplementationDSA(self);
}
#endif

#ifndef MAGNUM_TARGET_WEBGL
Containers::String AbstractQuery::label() const {
    #ifndef MAGNUM_TARGET_GLES2
    return Context::current().state().debug.getLabelImplementation(GL_QUERY, _id);
    #else
    return Context::current().state().debug.getLabelImplementation(GL_QUERY_KHR, _id);
    #endif
}

AbstractQuery& AbstractQuery::setLabel(const Containers::StringView label) {
    #ifndef MAGNUM_TARGET_GLES2
    Context::current().state().debug.labelImplementation(GL_QUERY, _id, label);
    #else
    Context::current().state().debug.labelImplementation(GL_QUERY_KHR, _id, label);
    #endif
    return *this;
}
#endif

bool AbstractQuery::resultAvailable() {
    GLuint result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT_AVAILABLE, &result);
    #else
    glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT_AVAILABLE_EXT, &result);
    #endif
    return result == GL_TRUE;
}

template<> UnsignedInt AbstractQuery::result<UnsignedInt>() {
    UnsignedInt result;
    #ifndef MAGNUM_TARGET_GLES2
    glGetQueryObjectuiv(_id, GL_QUERY_RESULT, &result);
    #else
    glGetQueryObjectuivEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #endif
    return result;
}

template<> bool AbstractQuery::result<bool>() { return result<UnsignedInt>() != 0; }

template<> Int AbstractQuery::result<Int>() {
    Int result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjectiv(_id, GL_QUERY_RESULT, &result);
    #else
    glGetQueryObjectivEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #endif
    return result;
}

template<> UnsignedLong AbstractQuery::result<UnsignedLong>() {
    UnsignedLong result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjectui64v(_id, GL_QUERY_RESULT, &result);
    #else
    glGetQueryObjectui64vEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #endif
    return result;
}

template<> Long AbstractQuery::result<Long>() {
    Long result;
    #ifndef MAGNUM_TARGET_GLES
    glGetQueryObjecti64v(_id, GL_QUERY_RESULT, &result);
    #else
    glGetQueryObjecti64vEXT(_id, GL_QUERY_RESULT_EXT, &result);
    #endif
    return result;
}

void AbstractQuery::begin() {
    #ifndef MAGNUM_TARGET_GLES2
    glBeginQuery(_target, _id);
    #else
    glBeginQueryEXT(_target, _id);
    #endif
}

void AbstractQuery::end() {
    #ifndef MAGNUM_TARGET_GLES2
    glEndQuery(_target);
    #else
    glEndQueryEXT(_target);
    #endif
}

}}
