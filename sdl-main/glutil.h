#ifndef GLUTIL_H
#define GLUTIL_H

#include <GL/gl.h>

/* Returns the shader id upon success and 0 on failure. */
GLuint glutil_compile_shader_file(const char *path, GLenum shader_type);

/* Returns the shader id upon success and 0 on failure. */
GLuint glutil_compile_shader(const char *const src, GLint *src_len,
                             GLenum shader_type);

/* 
 * The varargs is a list of the shaders to be linked.
 * Return the shader program id upon success and 0 on failure.
 */
GLuint glutil_link_shaders(GLuint shader_prog_id, GLuint shader_id_0, ...);

#endif