//
// Created by gomkyung2 on 2023/04/29.
//

#ifndef OPENGLTRIANGLE_OPENGL_UTILS_HPP
#define OPENGLTRIANGLE_OPENGL_UTILS_HPP

#include <span>
#include <GL/glew.h>

GLuint createShader(GLenum type, const char *source);

GLuint createProgram(std::initializer_list<GLuint> shaders);

GLuint createVertexBuffer(std::span<const float> vertices, std::initializer_list<GLuint> attribute_sizes, GLenum usage);

GLuint createIndexBuffer(std::span<const unsigned short> indices, GLenum usage);

#endif //OPENGLTRIANGLE_OPENGL_UTILS_HPP
