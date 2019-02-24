#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

#include "glutil.h"

static char *glutil_load_file(const char *path, size_t *sz)
{
    struct stat st;
    ssize_t read_sz;
    char *buf;
    int err;
    int fd;

    fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return NULL;
    }

    err = fstat(fd, &st);
    if (err == -1) {
        perror("stat");
        buf = NULL;
        goto err_close;
    }

    buf = malloc(st.st_size + 1);

    read_sz = read(fd, buf, st.st_size);
    if (read_sz != st.st_size) {
        perror("read");
        free(buf);
        buf = NULL;
        goto err_close;
    }
    buf[st.st_size] = '\0';

    if (sz)
        *sz = st.st_size;

err_close:
    close(fd);
    return buf;
}

GLuint glutil_compile_shader(const char *const src, GLint *src_len,
                             GLenum shader_type)
{
    GLint compile_status;
    GLint info_log_len;
    GLuint shader_id;
    char *info_log;

    shader_id = glCreateShader(shader_type);
    if (!shader_id)
        return 0;

    /*
     * The length cast may cause things to overflow into the sign bit, I'll deal
     * with it once I see a shader that long. :-)
     */
    glShaderSource(shader_id, 1, &src, src_len);

    glCompileShader(shader_id);

    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_len);

    if (!compile_status) {
        info_log = alloca(info_log_len + 1);
        glGetShaderInfoLog(shader_id, info_log_len, &info_log_len, info_log);
        fprintf(stderr, "Shader compilation failed:\n%s\n", info_log);
        glDeleteShader(shader_id);
        shader_id = 0;
    }

    return shader_id;
}

GLuint glutil_compile_shader_file(const char *path, GLenum shader_type)
{
    GLuint shader_id;
    size_t src_len;
    char *src;

    src = glutil_load_file(path, &src_len);
    if (!src) {
        fprintf(stderr, "Loading shader file \"%s\" failed\n", path);
        return 0;
    }

    shader_id = glutil_compile_shader(src, (GLint *)&src_len, shader_type);

    free(src);
    return shader_id;
}

GLuint glutil_link_shaders(GLuint shader_prog_id, GLuint shader_id_0, ...)
{
    va_list shaders;
    GLuint id;
    GLint link_status;
    GLint info_log_len;
    char *info_log;

    glAttachShader(shader_prog_id, shader_id_0);

    va_start(shaders, shader_id_0);
    while ((id = va_arg(shaders, GLuint)))
        glAttachShader(shader_prog_id, id);
    va_end(shaders);

    glLinkProgram(shader_prog_id);

    glGetProgramiv(shader_prog_id, GL_LINK_STATUS, &link_status);
    glGetProgramiv(shader_prog_id, GL_INFO_LOG_LENGTH, &info_log_len);
    if (!link_status) {
        info_log = alloca(info_log_len + 1);
        glGetProgramInfoLog(shader_prog_id, info_log_len, &info_log_len, info_log);
        fprintf(stderr, "Shader linking failed:\n%s\n", info_log);
        return 0;
    }

    return shader_prog_id;
}
