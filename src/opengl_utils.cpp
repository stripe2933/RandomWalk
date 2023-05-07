//
// Created by gomkyung2 on 2023/04/29.
//

#include "opengl_utils.hpp"

#include <cassert>
#include <array>
#include <stdexcept>
#include <numeric>

#include <fmt/core.h>


GLuint createShader(GLenum type, const char *source) {
    assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);

    auto handle = glCreateShader(type);
    glShaderSource(handle, 1, &source, nullptr);
    glCompileShader(handle);

    static GLint success;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
    if (!success){
        static std::array<char, 512> info_log;
        glGetShaderInfoLog(handle, info_log.size(), nullptr, info_log.data());

        throw std::runtime_error { fmt::format("Failed to compile shader: {}", info_log.data()) };
    }

    return handle;
}

GLuint createProgram(std::initializer_list<GLuint> shaders) {
    auto handle = glCreateProgram();
    for (auto shader : shaders){
        glAttachShader(handle, shader);
    }

    glLinkProgram(handle);

    static GLint success;
    glGetProgramiv(handle, GL_LINK_STATUS, &success);
    if (!success){
        static std::array<char, 512> info_log;
        glGetProgramInfoLog(handle, info_log.size(), nullptr, info_log.data());

        throw std::runtime_error { fmt::format("Failed to link shaders to program: {}", info_log.data()) };
    }

    return handle;
}

GLuint createVertexBuffer(std::span<const float> vertices, std::initializer_list<GLuint> attribute_sizes, GLenum usage) {
    assert(usage == GL_STATIC_DRAW ||
           usage == GL_DYNAMIC_DRAW ||
           usage == GL_STREAM_DRAW);

    GLuint handle;
    glGenBuffers(1, &handle);

    // Set buffer data.
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizei>(sizeof(float) * vertices.size()), vertices.data(), usage);

    // Set buffer attributes.
    GLuint stride = sizeof(float) * std::reduce(attribute_sizes.begin(), attribute_sizes.end(), 0U);
    for (auto index = 0U, offset = 0U; auto attribute_size : attribute_sizes){
        glVertexAttribPointer(index, static_cast<GLint>(attribute_size), GL_FLOAT, GL_FALSE,
                              static_cast<GLsizei>(stride), reinterpret_cast<const void *>(offset));
        glEnableVertexAttribArray(index);

        index++;
        offset += sizeof(float) * attribute_size;
    }

    return handle;
}