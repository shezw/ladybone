/*
 * Copyright (c) 2026, Ladybird contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>
#include <stddef.h>

namespace Web::WebGL {

using GLboolean = unsigned char;
using GLbitfield = unsigned int;
using GLchar = char;
using GLubyte = unsigned char;
using GLenum = unsigned int;
using GLfloat = float;
using GLint = int;
using GLint64 = long long;
using GLintptr = long long;
using GLsizei = int;
using GLsizeiptr = long long;
using GLsync = void*;
using GLuint = unsigned int;

using EGLAttrib = long long;
using EGLBoolean = unsigned int;
using EGLConfig = void*;
using EGLContext = void*;
using EGLDisplay = void*;
using EGLImage = void*;
using EGLint = int;
using EGLSurface = void*;

using PFNEGLQUERYDMABUFFORMATSEXTPROC = EGLBoolean (*)(EGLDisplay, EGLint, EGLint*, EGLint*);
using PFNEGLQUERYDMABUFMODIFIERSEXTPROC = EGLBoolean (*)(EGLDisplay, EGLint, EGLint, EGLint*, unsigned long long*, EGLBoolean*, EGLint*);

inline constexpr EGLDisplay EGL_NO_DISPLAY = nullptr;
inline constexpr EGLConfig EGL_NO_CONFIG_KHR = nullptr;
inline constexpr EGLContext EGL_NO_CONTEXT = nullptr;
inline constexpr EGLImage EGL_NO_IMAGE = nullptr;
inline constexpr EGLSurface EGL_NO_SURFACE = nullptr;
inline constexpr EGLAttrib EGL_DEFAULT_DISPLAY = 0;

enum : EGLAttrib {
    EGL_ALPHA_SIZE = 1,
    EGL_BACK_BUFFER,
    EGL_BIND_TO_TEXTURE_TARGET_ANGLE,
    EGL_BLUE_SIZE,
    EGL_CONTEXT_CLIENT_VERSION,
    EGL_CONTEXT_OPENGL_BACKWARDS_COMPATIBLE_ANGLE,
    EGL_CONTEXT_WEBGL_COMPATIBILITY_ANGLE,
    EGL_DEPTH_SIZE,
    EGL_DMA_BUF_PLANE0_FD_EXT,
    EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT,
    EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT,
    EGL_DMA_BUF_PLANE0_OFFSET_EXT,
    EGL_DMA_BUF_PLANE0_PITCH_EXT,
    EGL_EXTENSIONS_ENABLED_ANGLE,
    EGL_FALSE = 0,
    EGL_GREEN_SIZE = 16,
    EGL_HEIGHT,
    EGL_IOSURFACE_ANGLE,
    EGL_IOSURFACE_PLANE_ANGLE,
    EGL_LINUX_DMA_BUF_EXT,
    EGL_LINUX_DRM_FOURCC_EXT,
    EGL_NONE,
    EGL_OPENGL_ES2_BIT,
    EGL_PBUFFER_BIT,
    EGL_PLATFORM_ANGLE_ANGLE,
    EGL_PLATFORM_ANGLE_NATIVE_PLATFORM_TYPE_ANGLE,
    EGL_PLATFORM_ANGLE_TYPE_ANGLE,
    EGL_PLATFORM_ANGLE_TYPE_METAL_ANGLE,
    EGL_PLATFORM_ANGLE_TYPE_OPENGL_ANGLE,
    EGL_PLATFORM_SURFACELESS_MESA,
    EGL_RED_SIZE,
    EGL_RENDERABLE_TYPE,
    EGL_ROBUST_RESOURCE_INITIALIZATION_ANGLE,
    EGL_STENCIL_SIZE,
    EGL_SURFACE_TYPE,
    EGL_TEXTURE_2D,
    EGL_TEXTURE_FORMAT,
    EGL_TEXTURE_INTERNAL_FORMAT_ANGLE,
    EGL_TEXTURE_RECTANGLE_ANGLE,
    EGL_TEXTURE_RGBA,
    EGL_TEXTURE_TARGET,
    EGL_TEXTURE_TYPE_ANGLE,
    EGL_TRUE = 1,
    EGL_WIDTH = 48,
};

enum : GLenum {
    GL_ACTIVE_ATTRIBUTES = 1,
    GL_ACTIVE_TEXTURE,
    GL_ACTIVE_UNIFORMS,
    GL_ACTIVE_UNIFORM_BLOCKS,
    GL_ALIASED_LINE_WIDTH_RANGE,
    GL_ALIASED_POINT_SIZE_RANGE,
    GL_ALPHA,
    GL_ALPHA_BITS,
    GL_ANY_SAMPLES_PASSED,
    GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
    GL_ARRAY_BUFFER,
    GL_ARRAY_BUFFER_BINDING,
    GL_ATTACHED_SHADERS,
    GL_BGRA_EXT,
    GL_BLEND,
    GL_BLEND_COLOR,
    GL_BLEND_DST_ALPHA,
    GL_BLEND_DST_RGB,
    GL_BLEND_EQUATION_ALPHA,
    GL_BLEND_EQUATION_RGB,
    GL_BLEND_SRC_ALPHA,
    GL_BLEND_SRC_RGB,
    GL_BLUE_BITS,
    GL_BUFFER_SIZE,
    GL_BUFFER_USAGE,
    GL_COLOR,
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_BUFFER_BIT,
    GL_COLOR_CLEAR_VALUE,
    GL_COLOR_WRITEMASK,
    GL_COMPILE_STATUS,
    GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
    GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
    GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
    GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
    GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
    GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,
    GL_COPY_READ_BUFFER,
    GL_COPY_READ_BUFFER_BINDING,
    GL_COPY_WRITE_BUFFER,
    GL_COPY_WRITE_BUFFER_BINDING,
    GL_CULL_FACE,
    GL_CULL_FACE_MODE,
    GL_CURRENT_PROGRAM,
    GL_CURRENT_QUERY,
    GL_CURRENT_VERTEX_ATTRIB,
    GL_DELETE_STATUS,
    GL_DEPTH,
    GL_DEPTH24_STENCIL8,
    GL_DEPTH_ATTACHMENT,
    GL_DEPTH_BITS,
    GL_DEPTH_BUFFER_BIT,
    GL_DEPTH_CLEAR_VALUE,
    GL_DEPTH_COMPONENT24,
    GL_DEPTH_FUNC,
    GL_DEPTH_RANGE,
    GL_DEPTH_STENCIL,
    GL_DEPTH_STENCIL_ATTACHMENT,
    GL_DEPTH_TEST,
    GL_DEPTH_WRITEMASK,
    GL_DITHER,
    GL_DRAW_FRAMEBUFFER,
    GL_DRAW_FRAMEBUFFER_BINDING,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER_BINDING,
    GL_EXTENSIONS,
    GL_FALSE = 0,
    GL_FRAGMENT_SHADER = 65,
    GL_FRAGMENT_SHADER_DERIVATIVE_HINT,
    GL_FRAGMENT_SHADER_DERIVATIVE_HINT_OES,
    GL_FRAMEBUFFER,
    GL_FRAMEBUFFER_BINDING,
    GL_FRAMEBUFFER_COMPLETE,
    GL_FRONT_FACE,
    GL_GENERATE_MIPMAP_HINT,
    GL_GREEN_BITS,
    GL_IMPLEMENTATION_COLOR_READ_FORMAT,
    GL_IMPLEMENTATION_COLOR_READ_TYPE,
    GL_INFO_LOG_LENGTH,
    GL_INVALID_ENUM,
    GL_INVALID_OPERATION,
    GL_INVALID_VALUE,
    GL_LINE_WIDTH,
    GL_LINK_STATUS,
    GL_LUMINANCE,
    GL_LUMINANCE_ALPHA,
    GL_MAP_READ_BIT,
    GL_MAX_3D_TEXTURE_SIZE,
    GL_MAX_ARRAY_TEXTURE_LAYERS,
    GL_MAX_COLOR_ATTACHMENTS,
    GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
    GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
    GL_MAX_COMBINED_UNIFORM_BLOCKS,
    GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
    GL_MAX_CUBE_MAP_TEXTURE_SIZE,
    GL_MAX_DRAW_BUFFERS,
    GL_MAX_ELEMENTS_INDICES,
    GL_MAX_ELEMENTS_VERTICES,
    GL_MAX_ELEMENT_INDEX,
    GL_MAX_FRAGMENT_INPUT_COMPONENTS,
    GL_MAX_FRAGMENT_UNIFORM_BLOCKS,
    GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
    GL_MAX_FRAGMENT_UNIFORM_VECTORS,
    GL_MAX_PROGRAM_TEXEL_OFFSET,
    GL_MAX_RENDERBUFFER_SIZE,
    GL_MAX_SAMPLES,
    GL_MAX_SERVER_WAIT_TIMEOUT,
    GL_MAX_TEXTURE_IMAGE_UNITS,
    GL_MAX_TEXTURE_LOD_BIAS,
    GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,
    GL_MAX_TEXTURE_SIZE,
    GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS,
    GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS,
    GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS,
    GL_MAX_UNIFORM_BLOCK_SIZE,
    GL_MAX_UNIFORM_BUFFER_BINDINGS,
    GL_MAX_VARYING_COMPONENTS,
    GL_MAX_VARYING_VECTORS,
    GL_MAX_VERTEX_ATTRIBS,
    GL_MAX_VERTEX_OUTPUT_COMPONENTS,
    GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
    GL_MAX_VERTEX_UNIFORM_BLOCKS,
    GL_MAX_VERTEX_UNIFORM_COMPONENTS,
    GL_MAX_VERTEX_UNIFORM_VECTORS,
    GL_MAX_VIEWPORT_DIMS,
    GL_MIN_PROGRAM_TEXEL_OFFSET,
    GL_NO_ERROR,
    GL_NUM_SAMPLE_COUNTS,
    GL_PACK_ALIGNMENT,
    GL_PACK_ROW_LENGTH,
    GL_PACK_SKIP_PIXELS,
    GL_PACK_SKIP_ROWS,
    GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_PACK_BUFFER_BINDING,
    GL_PIXEL_UNPACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER_BINDING,
    GL_POLYGON_OFFSET_FACTOR,
    GL_POLYGON_OFFSET_FILL,
    GL_POLYGON_OFFSET_UNITS,
    GL_QUERY_RESULT,
    GL_QUERY_RESULT_AVAILABLE,
    GL_RASTERIZER_DISCARD,
    GL_RED_BITS,
    GL_RENDERBUFFER,
    GL_RENDERBUFFER_ALPHA_SIZE,
    GL_RENDERBUFFER_BINDING,
    GL_RENDERBUFFER_BLUE_SIZE,
    GL_RENDERBUFFER_DEPTH_SIZE,
    GL_RENDERBUFFER_GREEN_SIZE,
    GL_RENDERBUFFER_HEIGHT,
    GL_RENDERBUFFER_INTERNAL_FORMAT,
    GL_RENDERBUFFER_RED_SIZE,
    GL_RENDERBUFFER_SAMPLES,
    GL_RENDERBUFFER_STENCIL_SIZE,
    GL_RENDERBUFFER_WIDTH,
    GL_RENDERER,
    GL_REQUESTABLE_EXTENSIONS_ANGLE,
    GL_RGB,
    GL_RGBA,
    GL_SAMPLER_BINDING,
    GL_SAMPLES,
    GL_SAMPLE_ALPHA_TO_COVERAGE,
    GL_SAMPLE_BUFFERS,
    GL_SAMPLE_COVERAGE,
    GL_SAMPLE_COVERAGE_INVERT,
    GL_SAMPLE_COVERAGE_VALUE,
    GL_SCISSOR_BOX,
    GL_SCISSOR_TEST,
    GL_SHADER_SOURCE_LENGTH,
    GL_SHADER_TYPE,
    GL_SHADING_LANGUAGE_VERSION,
    GL_STENCIL,
    GL_STENCIL_ATTACHMENT,
    GL_STENCIL_BACK_FAIL,
    GL_STENCIL_BACK_FUNC,
    GL_STENCIL_BACK_PASS_DEPTH_FAIL,
    GL_STENCIL_BACK_PASS_DEPTH_PASS,
    GL_STENCIL_BACK_REF,
    GL_STENCIL_BACK_VALUE_MASK,
    GL_STENCIL_BACK_WRITEMASK,
    GL_STENCIL_BITS,
    GL_STENCIL_BUFFER_BIT,
    GL_STENCIL_CLEAR_VALUE,
    GL_STENCIL_FAIL,
    GL_STENCIL_FUNC,
    GL_STENCIL_INDEX8,
    GL_STENCIL_PASS_DEPTH_FAIL,
    GL_STENCIL_PASS_DEPTH_PASS,
    GL_STENCIL_REF,
    GL_STENCIL_TEST,
    GL_STENCIL_VALUE_MASK,
    GL_STENCIL_WRITEMASK,
    GL_SUBPIXEL_BITS,
    GL_TEXTURE_2D,
    GL_TEXTURE_2D_ARRAY,
    GL_TEXTURE_3D,
    GL_TEXTURE_BASE_LEVEL,
    GL_TEXTURE_BINDING_2D,
    GL_TEXTURE_BINDING_2D_ARRAY,
    GL_TEXTURE_BINDING_CUBE_MAP,
    GL_TEXTURE_COMPARE_FUNC,
    GL_TEXTURE_COMPARE_MODE,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_IMMUTABLE_FORMAT,
    GL_TEXTURE_IMMUTABLE_LEVELS,
    GL_TEXTURE_MAG_FILTER,
    GL_TEXTURE_MAX_ANISOTROPY_EXT,
    GL_TEXTURE_MAX_LEVEL,
    GL_TEXTURE_MAX_LOD,
    GL_TEXTURE_MIN_FILTER,
    GL_TEXTURE_MIN_LOD,
    GL_TEXTURE_RECTANGLE_ANGLE,
    GL_TEXTURE_WRAP_R,
    GL_TEXTURE_WRAP_S,
    GL_TEXTURE_WRAP_T,
    GL_TRANSFORM_FEEDBACK_ACTIVE,
    GL_TRANSFORM_FEEDBACK_BINDING,
    GL_TRANSFORM_FEEDBACK_BUFFER,
    GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
    GL_TRANSFORM_FEEDBACK_BUFFER_MODE,
    GL_TRANSFORM_FEEDBACK_PAUSED,
    GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
    GL_TRANSFORM_FEEDBACK_VARYINGS,
    GL_TRUE = 1,
    GL_UNIFORM_ARRAY_STRIDE = 210,
    GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
    GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
    GL_UNIFORM_BLOCK_BINDING,
    GL_UNIFORM_BLOCK_DATA_SIZE,
    GL_UNIFORM_BLOCK_INDEX,
    GL_UNIFORM_BLOCK_NAME_LENGTH,
    GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER,
    GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER,
    GL_UNIFORM_BUFFER,
    GL_UNIFORM_BUFFER_BINDING,
    GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
    GL_UNIFORM_IS_ROW_MAJOR,
    GL_UNIFORM_MATRIX_STRIDE,
    GL_UNIFORM_OFFSET,
    GL_UNIFORM_SIZE,
    GL_UNIFORM_TYPE,
    GL_UNPACK_ALIGNMENT,
    GL_UNPACK_IMAGE_HEIGHT,
    GL_UNPACK_ROW_LENGTH,
    GL_UNPACK_SKIP_IMAGES,
    GL_UNPACK_SKIP_PIXELS,
    GL_UNPACK_SKIP_ROWS,
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_SHORT_4_4_4_4,
    GL_UNSIGNED_SHORT_5_5_5_1,
    GL_UNSIGNED_SHORT_5_6_5,
    GL_VALIDATE_STATUS,
    GL_VENDOR,
    GL_VERSION,
    GL_VERTEX_ARRAY_BINDING,
    GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,
    GL_VERTEX_ATTRIB_ARRAY_DIVISOR,
    GL_VERTEX_ATTRIB_ARRAY_DIVISOR_ANGLE,
    GL_VERTEX_ATTRIB_ARRAY_ENABLED,
    GL_VERTEX_ATTRIB_ARRAY_INTEGER,
    GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
    GL_VERTEX_ATTRIB_ARRAY_POINTER,
    GL_VERTEX_ATTRIB_ARRAY_SIZE,
    GL_VERTEX_ATTRIB_ARRAY_STRIDE,
    GL_VERTEX_ATTRIB_ARRAY_TYPE,
    GL_VERTEX_SHADER,
    GL_VIEWPORT,
    GL_WAIT_FAILED,
};

struct StubValue {
    template<typename T>
    constexpr operator T() const { return {}; }
};

template<typename... Args>
constexpr StubValue gl_stub(Args&&...)
{
    return {};
}

}

#define glGetString(...) static_cast<Web::WebGL::GLubyte const*>(nullptr)
#define glMapBufferRange(...) static_cast<void*>(nullptr)
#define eglGetProcAddress(...) static_cast<void*>(nullptr)

#define glActiveTexture(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glAttachShader(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBeginQuery(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBeginTransformFeedback(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindAttribLocation(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindBuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindBufferBase(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindBufferRange(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindFramebuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindRenderbuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindSampler(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindTexture(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindTransformFeedback(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindVertexArray(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBindVertexArrayOES(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBlendColor(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBlendEquation(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBlendEquationSeparate(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBlendFunc(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBlendFuncSeparate(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBlitFramebuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBufferData(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glBufferSubData(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCheckFramebufferStatus(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glClear(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glClearBufferfi(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glClearBufferfv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glClearBufferiv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glClearBufferuiv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glClearColor(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glClearDepthf(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glClearStencil(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glClientWaitSync(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glColorMask(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCompileShader(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCompressedTexImage2DRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCompressedTexImage3DRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCompressedTexSubImage2DRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCompressedTexSubImage3DRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCopyBufferSubData(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCopyTexImage2D(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCopyTexSubImage2D(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCreateProgram(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCreateShader(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glCullFace(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteBuffers(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteFramebuffers(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteProgram(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteQueries(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteRenderbuffers(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteSamplers(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteShader(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteSync(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteTextures(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteTransformFeedbacks(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteVertexArrays(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDeleteVertexArraysOES(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDepthFunc(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDepthMask(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDepthRangef(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDetachShader(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDisable(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDisableVertexAttribArray(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDrawArrays(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDrawArraysInstanced(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDrawArraysInstancedANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDrawBuffers(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDrawBuffersEXT(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDrawElements(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDrawElementsInstanced(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDrawElementsInstancedANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glDrawRangeElements(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glEGLImageTargetTexture2DOES(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glEnable(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glEnableVertexAttribArray(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glEndQuery(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glEndTransformFeedback(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glFenceSync(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glFinish(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glFlush(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glFramebufferRenderbuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glFramebufferTexture2D(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glFramebufferTextureLayer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glFrontFace(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGenBuffers(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGenFramebuffers(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGenQueries(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGenRenderbuffers(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGenSamplers(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGenTextures(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGenTransformFeedbacks(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGenVertexArrays(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGenVertexArraysOES(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGenerateMipmap(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetActiveAttrib(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetActiveUniform(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetActiveUniformBlockName(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetActiveUniformBlockivRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetActiveUniformsiv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetAttribLocation(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetBooleanvRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetBufferParameterivRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetError(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetFloatv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetFloatvRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetInteger64vRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetIntegerv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetIntegervRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetInternalformativRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetProgramInfoLog(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetProgramiv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetProgramivRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetQueryObjectuivRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetRenderbufferParameterivRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetShaderInfoLog(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetShaderPrecisionFormat(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetShaderSource(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetShaderiv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetShaderivRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetSynciv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetTexParameterfvRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetTexParameterivRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetUniformBlockIndex(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetUniformIndices(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetUniformLocation(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetVertexAttribPointervRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetVertexAttribfvRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glGetVertexAttribivRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glHint(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glInvalidateFramebuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glInvalidateSubFramebuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glIsBuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glIsEnabled(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glIsFramebuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glIsProgram(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glIsRenderbuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glIsShader(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glIsTexture(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glIsVertexArray(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glIsVertexArrayOES(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glLineWidth(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glLinkProgram(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glPauseTransformFeedback(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glPixelStorei(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glPolygonOffset(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glReadBuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glReadPixelsRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glRenderbufferStorage(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glRenderbufferStorageMultisample(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glRequestExtensionANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glResumeTransformFeedback(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glSampleCoverage(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glSamplerParameterf(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glSamplerParameteri(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glScissor(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glShaderSource(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glStencilFunc(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glStencilFuncSeparate(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glStencilMask(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glStencilMaskSeparate(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glStencilOp(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glStencilOpSeparate(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glTexImage2DRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glTexImage3DRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glTexParameterf(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glTexParameteri(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glTexStorage2D(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glTexStorage3D(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glTexSubImage2DRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glTexSubImage3DRobustANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glTransformFeedbackVaryings(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform1f(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform1fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform1i(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform1iv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform1ui(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform1uiv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform2f(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform2fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform2i(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform2iv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform2ui(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform2uiv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform3f(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform3fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform3i(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform3iv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform3ui(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform3uiv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform4f(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform4fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform4i(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform4iv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform4ui(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniform4uiv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniformBlockBinding(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniformMatrix2fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniformMatrix2x3fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniformMatrix2x4fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniformMatrix3fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniformMatrix3x2fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniformMatrix3x4fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniformMatrix4fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniformMatrix4x2fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUniformMatrix4x3fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUnmapBuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glUseProgram(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glValidateProgram(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttrib1f(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttrib1fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttrib2f(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttrib2fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttrib3f(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttrib3fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttrib4f(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttrib4fv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttribDivisor(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttribDivisorANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttribI4i(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttribI4iv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttribI4ui(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttribI4uiv(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttribIPointer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glVertexAttribPointer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glViewport(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define glWaitSync(...) Web::WebGL::gl_stub(__VA_ARGS__)

#define eglBindTexImage(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglChooseConfig(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglCreateContext(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglCreateImage(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglCreatePbufferFromClientBuffer(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglDestroyContext(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglDestroyImage(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglDestroySurface(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglGetConfigAttrib(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglGetCurrentContext(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglGetPlatformDisplay(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglInitialize(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglMakeCurrent(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglReleaseTexImage(...) Web::WebGL::gl_stub(__VA_ARGS__)
#define eglWaitUntilWorkScheduledANGLE(...) Web::WebGL::gl_stub(__VA_ARGS__)
