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

#if defined(INSTANCED_OBJECT_ID) && !defined(GL_ES) && !defined(NEW_GLSL)
#extension GL_EXT_gpu_shader4: require
#endif

#if defined(UNIFORM_BUFFERS) && defined(TEXTURE_ARRAYS) && !defined(GL_ES)
#extension GL_ARB_shader_bit_encoding: require
#endif

#if defined(SHADER_STORAGE_BUFFERS) && !defined(GL_ES)
#extension GL_ARB_shader_storage_buffer_object: require
#endif

#ifdef MULTI_DRAW
#ifndef GL_ES
#extension GL_ARB_shader_draw_parameters: require
#else /* covers WebGL as well */
#extension GL_ANGLE_multi_draw: require
#endif
#endif

#ifndef NEW_GLSL
#define in attribute
#define out varying
#endif

#ifndef RUNTIME_CONST
#define const
#endif

/* Both classic uniforms and uniform buffers */

#ifdef DYNAMIC_PER_VERTEX_JOINT_COUNT
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = PER_VERTEX_JOINT_COUNT_LOCATION)
#endif
uniform mediump uvec2 perVertexJointCount
    #ifndef GL_ES
    = uvec2(PER_VERTEX_JOINT_COUNT, SECONDARY_PER_VERTEX_JOINT_COUNT)
    #endif
    ;
#endif

/* Uniforms */

#ifndef UNIFORM_BUFFERS
#ifdef TWO_DIMENSIONS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 9)
#endif
uniform highp mat3 transformationProjectionMatrix
    #ifndef GL_ES
    = mat3(1.0)
    #endif
    ;
#elif defined(THREE_DIMENSIONS)
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 9)
#endif
uniform highp mat4 transformationMatrix
    #ifndef GL_ES
    = mat4(1.0)
    #endif
    ;
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 10)
#endif
uniform highp mat4 projectionMatrix
    #ifndef GL_ES
    = mat4(1.0)
    #endif
    ;
#else
#error
#endif

#ifdef VERTEX_ID
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 5)
#endif
uniform lowp vec2 colorMapOffsetScale
    #ifndef GL_ES
    = vec2(1.0/512.0, 1.0/256.0)
    #endif
    ;
#define colorMapOffset colorMapOffsetScale.x
#define colorMapScale colorMapOffsetScale.y
#endif

#if defined(TANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 11)
#endif
uniform highp mat3 normalMatrix
    #ifndef GL_ES
    = mat3(1.0)
    #endif
    ;

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 13)
#endif
uniform highp float lineLength
    #ifndef GL_ES
    = 1.0
    #endif
    ;
#endif

#ifdef TEXTURE_TRANSFORMATION
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 7)
#endif
uniform mediump mat3 textureMatrix
    #ifndef GL_ES
    = mat3(1.0)
    #endif
    ;
#endif

#ifdef TEXTURE_ARRAYS
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 8)
#endif
/* mediump is just 2^10, which might not be enough, this is 2^16 */
uniform highp uint textureLayer; /* defaults to zero */
#endif

#ifdef JOINT_COUNT
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 14)
#endif
#ifdef TWO_DIMENSIONS
uniform mat3 jointMatrices[JOINT_COUNT]
    #ifndef GL_ES
    = mat3[](JOINT_MATRIX_INITIALIZER)
    #endif
    ;
#elif defined(THREE_DIMENSIONS)
uniform mat4 jointMatrices[JOINT_COUNT]
    #ifndef GL_ES
    = mat4[](JOINT_MATRIX_INITIALIZER)
    #endif
    ;
#else
#error
#endif

#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = PER_INSTANCE_JOINT_COUNT_LOCATION)
#endif
uniform uint perInstanceJointCount; /* defaults to zero */
#endif

/* Uniform / shader storage buffers */

#else
/* For SSBOs, the per-draw, material and joint arrays are unbounded */
#ifdef SHADER_STORAGE_BUFFERS
#define DRAW_COUNT
#define MATERIAL_COUNT
/* Define JOINT_COUNT only if there are any per-vertex attributes, otherwise
   the buffer would be useless */
#if defined(PER_VERTEX_JOINT_COUNT) || defined(SECONDARY_PER_VERTEX_JOINT_COUNT)
#define JOINT_COUNT
#endif
#define BUFFER_OR_UNIFORM buffer
#define BUFFER_READONLY readonly
#else
#define BUFFER_OR_UNIFORM uniform
#define BUFFER_READONLY
#endif

/* With SSBOs DRAW_COUNT is defined to be empty, +0 makes the condition not
   cause a compile error */
#if defined(SHADER_STORAGE_BUFFERS) || DRAW_COUNT+0 > 1
#ifdef EXPLICIT_UNIFORM_LOCATION
layout(location = 1)
#endif
uniform highp uint drawOffset
    #ifndef GL_ES
    = 0u
    #endif
    ;
#else
#define drawOffset 0u
#endif

#ifdef TWO_DIMENSIONS
layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 1
    #endif
) BUFFER_OR_UNIFORM TransformationProjection {
    /* Can't be a mat3 because of ANGLE, see DrawUniform in Phong.vert for
       details */
    BUFFER_READONLY highp mat3x4 transformationProjectionMatrices[DRAW_COUNT];
};
#elif defined(THREE_DIMENSIONS)
layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 0
    #endif
) BUFFER_OR_UNIFORM Projection {
    BUFFER_READONLY highp mat4 projectionMatrix;
};

layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 1
    #endif
) BUFFER_OR_UNIFORM Transformation {
    BUFFER_READONLY highp mat4 transformationMatrices[DRAW_COUNT];
};
#else
#error
#endif

#ifdef TEXTURE_TRANSFORMATION
struct TextureTransformationUniform {
    highp vec4 rotationScaling;
    highp vec4 offsetLayerReserved;
    #define textureTransformation_offset offsetLayerReserved.xy
    #define textureTransformation_layer offsetLayerReserved.z
};

layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 3
    #endif
) BUFFER_OR_UNIFORM TextureTransformation {
    BUFFER_READONLY TextureTransformationUniform textureTransformations[DRAW_COUNT];
};
#endif

/* Keep in sync with MeshVisualizer.geom and MeshVisualizer.frag. Can't
   "outsource" to a common file because the #extension directives need to be
   always before any code. */
struct DrawUniform {
    #ifdef THREE_DIMENSIONS
    /* Can't be a mat3 because of ANGLE, see Phong.vert for details */
    highp mat3x4 normalMatrix;
    #elif !defined(TWO_DIMENSIONS)
    #error
    #endif
    highp uvec4 materialIdReservedObjectIdJointOffsetPerInstanceJointCountReserved;
    #define draw_materialIdReserved materialIdReservedObjectIdJointOffsetPerInstanceJointCountReserved.x
    #define draw_objectId materialIdReservedObjectIdJointOffsetPerInstanceJointCountReserved.y
    #define draw_jointOffsetPerInstanceJointCount materialIdReservedObjectIdJointOffsetPerInstanceJointCountReserved.z
};

layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 2
    #endif
) BUFFER_OR_UNIFORM Draw {
    BUFFER_READONLY DrawUniform draws[DRAW_COUNT];
};

/* Keep in sync with MeshVisualizer.geom and MeshVisualizer.frag. Can't
   "outsource" to a common file because the #extension directives need to be
   always before any code. */
struct MaterialUniform {
    lowp vec4 color;
    lowp vec4 wireframeColor;
    lowp vec4 wireframeWidthColorMapOffsetColorMapScaleLineWidth;
    #define material_wireframeWidth wireframeWidthColorMapOffsetColorMapScaleLineWidth.x
    #define material_colorMapOffset wireframeWidthColorMapOffsetColorMapScaleLineWidth.y
    #define material_colorMapScale wireframeWidthColorMapOffsetColorMapScaleLineWidth.z
    #define material_lineWidth wireframeWidthColorMapOffsetColorMapScaleLineWidth.w
    lowp vec4 lineLengthSmoothnessReservedReserved;
    #define material_lineLength lineLengthSmoothnessReservedReserved.x
    #define material_smoothness lineLengthSmoothnessReservedReserved.y
};

layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 4
    #endif
) BUFFER_OR_UNIFORM Material {
    BUFFER_READONLY MaterialUniform materials[MATERIAL_COUNT];
};

#ifdef JOINT_COUNT
layout(std140
    #if defined(EXPLICIT_BINDING) || defined(SHADER_STORAGE_BUFFERS)
    , binding = 6
    #endif
) BUFFER_OR_UNIFORM Joint {
    BUFFER_READONLY highp
        #ifdef TWO_DIMENSIONS
        /* Can't be a mat3 because of ANGLE, see DrawUniform in Phong.vert for
           details */
        mat3x4
        #elif defined(THREE_DIMENSIONS)
        mat4
        #else
        #error
        #endif
    jointMatrices[JOINT_COUNT];
};
#endif
#endif

/* Inputs */

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = POSITION_ATTRIBUTE_LOCATION)
#endif
#ifdef TWO_DIMENSIONS
in highp vec2 position;
#elif defined(THREE_DIMENSIONS)
in highp vec4 position;
#else
#error
#endif

#if defined(TANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION)
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TANGENT_ATTRIBUTE_LOCATION)
#endif
in highp vec4 tangent;
#endif

#ifdef BITANGENT_DIRECTION
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = BITANGENT_ATTRIBUTE_LOCATION)
#endif
in highp vec3 bitangent;
#endif

#if defined(NORMAL_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION)
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = NORMAL_ATTRIBUTE_LOCATION)
#endif
in highp vec3 normal;
#endif

#ifdef TEXTURED
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TEXTURECOORDINATES_ATTRIBUTE_LOCATION)
#endif
in mediump vec2 textureCoordinates;
#endif

#ifdef JOINT_COUNT
#if PER_VERTEX_JOINT_COUNT
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = WEIGHTS_ATTRIBUTE_LOCATION)
#endif
in mediump vec4 weights;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = JOINTIDS_ATTRIBUTE_LOCATION)
#endif
in mediump uvec4 jointIds;
#endif

#if SECONDARY_PER_VERTEX_JOINT_COUNT
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = SECONDARY_WEIGHTS_ATTRIBUTE_LOCATION)
#endif
in mediump vec4 secondaryWeights;

#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = SECONDARY_JOINTIDS_ATTRIBUTE_LOCATION)
#endif
in mediump uvec4 secondaryJointIds;
#endif
#endif

#ifdef INSTANCED_TEXTURE_OFFSET
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TEXTURE_OFFSET_ATTRIBUTE_LOCATION)
#endif
in mediump
    #ifndef TEXTURE_ARRAYS
    vec2
    #else
    vec3
    #endif
    instancedTextureOffset;
#endif

#if defined(WIREFRAME_RENDERING) && defined(NO_GEOMETRY_SHADER)
#if (!defined(GL_ES) && __VERSION__ < 140) || (defined(GL_ES) && __VERSION__ < 300)
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = 4)
#endif
in lowp float vertexIndex;
#define gl_VertexID int(vertexIndex)
#endif
#endif

#ifdef INSTANCED_OBJECT_ID
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = OBJECT_ID_ATTRIBUTE_LOCATION)
#endif
in highp uint instanceObjectId;
#endif

#ifdef INSTANCED_TRANSFORMATION
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = TRANSFORMATION_MATRIX_ATTRIBUTE_LOCATION)
#endif
in highp
    #ifdef TWO_DIMENSIONS
    mat3
    #else
    mat4
    #endif
    instancedTransformationMatrix;

#if defined(TANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
#ifdef EXPLICIT_ATTRIB_LOCATION
layout(location = NORMAL_MATRIX_ATTRIBUTE_LOCATION)
#endif
in highp mat3 instancedNormalMatrix;
#endif
#endif

/* Outputs */

#ifdef TEXTURED
out mediump
    #ifndef TEXTURE_ARRAYS
    vec2
    #else
    vec3
    #endif
    #ifdef NO_GEOMETRY_SHADER
    interpolatedTextureCoordinates
    #else
    interpolatedVsTextureCoordinates
    #endif
    ;
#endif

#if defined(WIREFRAME_RENDERING) && defined(NO_GEOMETRY_SHADER)
out vec3 barycentric;
#endif

#ifdef INSTANCED_OBJECT_ID
#ifdef NO_GEOMETRY_SHADER
flat out highp uint interpolatedInstanceObjectId;
#else
flat out highp uint interpolatedVsInstanceObjectId;
#endif
#endif

#ifdef VERTEX_ID
#ifdef NO_GEOMETRY_SHADER
out highp float interpolatedMappedVertexId;
#else
out highp float interpolatedVsMappedVertexId;
#endif
#endif

#ifdef PRIMITIVE_ID_FROM_VERTEX_ID
#ifdef NO_GEOMETRY_SHADER
flat out highp uint interpolatedPrimitiveId;
#else
flat out highp uint interpolatedVsPrimitiveId;
#endif
#endif

#ifdef TANGENT_DIRECTION
out highp vec4 tangentEndpoint;
#endif
#if defined(BITANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION)
out highp vec4 bitangentEndpoint;
#endif
#ifdef NORMAL_DIRECTION
out highp vec4 normalEndpoint;
#endif

#ifdef MULTI_DRAW
flat out highp uint
    #ifdef NO_GEOMETRY_SHADER
    drawId
    #else
    vsDrawId
    #endif
    ;
#endif

void main() {
    #ifdef UNIFORM_BUFFERS
    #ifdef MULTI_DRAW
    #ifdef NO_GEOMETRY_SHADER
    drawId
    #else
    vsDrawId
    #define drawId vsDrawId
    #endif
    = drawOffset + uint(
        #ifndef GL_ES
        gl_DrawIDARB /* Using GL_ARB_shader_draw_parameters, not GLSL 4.6 */
        #else
        gl_DrawID
        #endif
        );
    #else
    #define drawId drawOffset
    #endif

    #ifdef TWO_DIMENSIONS
    highp const mat3 transformationProjectionMatrix = mat3(transformationProjectionMatrices[drawId]);
    #elif defined(THREE_DIMENSIONS)
    highp const mat4 transformationMatrix = transformationMatrices[drawId];
    #else
    #error
    #endif
    #if defined(TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
    mediump const mat3 normalMatrix = mat3(draws[drawId].normalMatrix);
    #endif
    /* With SSBOs MATERIAL_COUNT is defined to be empty, +0 makes the condition
       not cause a compile error */
    #if defined(SHADER_STORAGE_BUFFERS) || MATERIAL_COUNT+0 > 1
    mediump const uint materialId = draws[drawId].draw_materialIdReserved & 0xffffu;
    #else
    #define materialId 0u
    #endif
    lowp float colorMapOffset = materials[materialId].material_colorMapOffset;
    lowp float colorMapScale = materials[materialId].material_colorMapScale;
    #if defined(TANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
    highp float lineLength = materials[materialId].material_lineLength;
    #endif
    #ifdef TEXTURE_TRANSFORMATION
    mediump const mat3 textureMatrix = mat3(textureTransformations[drawId].rotationScaling.xy, 0.0, textureTransformations[drawId].rotationScaling.zw, 0.0, textureTransformations[drawId].textureTransformation_offset, 1.0);
    #ifdef TEXTURE_ARRAYS
    highp const uint textureLayer = floatBitsToUint(textureTransformations[drawId].textureTransformation_layer);
    #endif
    #endif
    #ifdef JOINT_COUNT
    mediump const uint jointOffset = (draws[drawId].draw_jointOffsetPerInstanceJointCount & 0xffffu) + uint(gl_InstanceID)*(draws[drawId].draw_jointOffsetPerInstanceJointCount >> 16 & 0xffffu);
    #endif
    #else
    #ifdef JOINT_COUNT
    mediump const uint jointOffset = uint(gl_InstanceID)*perInstanceJointCount;
    #endif
    #endif

    #ifdef JOINT_COUNT
    #ifdef TWO_DIMENSIONS
    mat3 skinMatrix = mat3(0.0);
    #elif defined(THREE_DIMENSIONS)
    mat4 skinMatrix = mat4(0.0);
    #else
    #error
    #endif
    #if PER_VERTEX_JOINT_COUNT
    for(uint i = 0u; i != PER_VERTEX_JOINT_COUNT
        #ifdef DYNAMIC_PER_VERTEX_JOINT_COUNT
        && i != perVertexJointCount.x
        #endif
    ; ++i)
        skinMatrix += weights[i]*
            #ifdef TWO_DIMENSIONS
            mat3 /* need to slice because it's a mat3x4 due to ANGLE, see
                    DrawUniform in Phong.vert for details */
            #endif
            (jointMatrices[jointOffset + jointIds[i]]);
    #endif
    #if SECONDARY_PER_VERTEX_JOINT_COUNT
    for(uint i = 0u; i != SECONDARY_PER_VERTEX_JOINT_COUNT
        #ifdef DYNAMIC_PER_VERTEX_JOINT_COUNT
        && i != perVertexJointCount.y
        #endif
    ; ++i)
        skinMatrix += secondaryWeights[i]*
            #ifdef TWO_DIMENSIONS
            mat3 /* need to slice because it's a mat3x4 due to ANGLE, see
                    DrawUniform in Phong.vert for details */
            #endif
            (jointMatrices[jointOffset + secondaryJointIds[i]]);
    #endif
    #endif

    #ifdef TWO_DIMENSIONS
    gl_Position.xywz = vec4(transformationProjectionMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        #ifdef JOINT_COUNT
        skinMatrix*
        #endif
        vec3(position, 1.0), 0.0);
    #elif defined(THREE_DIMENSIONS)
    highp const vec4 transformedPosition4 = transformationMatrix*
        #ifdef INSTANCED_TRANSFORMATION
        instancedTransformationMatrix*
        #endif
        #ifdef JOINT_COUNT
        skinMatrix*
        #endif
        position;
    gl_Position = projectionMatrix*transformedPosition4;
    #else
    #error
    #endif

    #if defined(TANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION) || defined(BITANGENT_DIRECTION) || defined(NORMAL_DIRECTION)
    mat3 finalNormalMatrix = normalMatrix
        #ifdef INSTANCED_TRANSFORMATION
        *instancedNormalMatrix
        #endif
        ;
    #endif
    #ifdef TANGENT_DIRECTION
    tangentEndpoint = projectionMatrix*(transformedPosition4 + vec4(normalize(finalNormalMatrix*tangent.xyz)*lineLength, 0.0));
    #endif
    #ifdef BITANGENT_FROM_TANGENT_DIRECTION
    vec3 bitangent = cross(normal, tangent.xyz)*tangent.w;
    #endif
    #if defined(BITANGENT_DIRECTION) || defined(BITANGENT_FROM_TANGENT_DIRECTION)
    bitangentEndpoint = projectionMatrix*(transformedPosition4 + vec4(normalize(finalNormalMatrix*bitangent)*lineLength, 0.0));
    #endif
    #ifdef NORMAL_DIRECTION
    normalEndpoint = projectionMatrix*(transformedPosition4 + vec4(normalize(finalNormalMatrix*normal)*lineLength, 0.0));
    #endif

    #ifdef TEXTURED
    /* Texture coordinates, if needed */
    #ifdef NO_GEOMETRY_SHADER
    interpolatedTextureCoordinates
    #else
    interpolatedVsTextureCoordinates
    #endif
        .xy =
        #ifdef TEXTURE_TRANSFORMATION
        (textureMatrix*vec3(
            #ifdef INSTANCED_TEXTURE_OFFSET
            instancedTextureOffset.xy +
            #endif
            textureCoordinates, 1.0)).xy
        #else
        textureCoordinates
        #endif
        ;
    #ifdef TEXTURE_ARRAYS
    #ifdef NO_GEOMETRY_SHADER
    interpolatedTextureCoordinates
    #else
    interpolatedVsTextureCoordinates
    #endif
        .z = float(
        #ifdef INSTANCED_TEXTURE_OFFSET
        uint(instancedTextureOffset.z) +
        #endif
        textureLayer
    );
    #endif
    #endif

    #if defined(WIREFRAME_RENDERING) && defined(NO_GEOMETRY_SHADER)
    barycentric = vec3(0.0);

    #ifdef SUBSCRIPTING_WORKAROUND
    #ifndef NEW_GLSL
    int i = int(mod(vertexIndex, 3.0));
    #else
    int i = gl_VertexID % 3;
    #endif
         if(i == 0) barycentric.x = 1.0;
    else if(i == 1) barycentric.y = 1.0;
    else            barycentric.z = 1.0;
    #elif !defined(NEW_GLSL)
    barycentric[int(mod(vertexIndex, 3.0))] = 1.0;
    #else
    barycentric[gl_VertexID % 3] = 1.0;
    #endif
    #endif

    #ifdef INSTANCED_OBJECT_ID
    #ifdef NO_GEOMETRY_SHADER
    interpolatedInstanceObjectId
    #else
    interpolatedVsInstanceObjectId
    #endif
        = instanceObjectId;
    #endif
    #ifdef VERTEX_ID
    #ifdef NO_GEOMETRY_SHADER
    interpolatedMappedVertexId
    #else
    interpolatedVsMappedVertexId
    #endif
        = colorMapOffset + float(gl_VertexID)*colorMapScale;
    #endif
    #ifdef PRIMITIVE_ID_FROM_VERTEX_ID
    #ifdef NO_GEOMETRY_SHADER
    interpolatedPrimitiveId
    #else
    interpolatedVsPrimitiveId
    #endif
        = uint(gl_VertexID/3);
    #endif
}
