#pragma once

#include <glad/glad.h>
#include <lodepng.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

namespace gl_util
{
    void printProgramLog(GLuint program)
    {
        if (glIsProgram(program))
        {
            int infoLogLength = 0;
            int maxLength = infoLogLength;

            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            char *infoLog = new char[maxLength];

            glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
            if (infoLogLength > 0)
            {
                printf("%s\n", infoLog);
            }

            delete[] infoLog;
        }
        else
        {
            printf("Name %d is not a program\n", program);
        }
    }

    void printShaderLog(GLuint shader)
    {
        if (glIsShader(shader))
        {
            int infoLogLength = 0;
            int maxLength = infoLogLength;

            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            char *infoLog = new char[maxLength];

            glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
            if (infoLogLength > 0)
            {
                printf("%s\n", infoLog);
            }

            delete[] infoLog;
        }
        else
        {
            printf("Name %d is not a shader\n", shader);
        }
    }

    GLuint new_shader(const char *path, GLenum shader_type)
    {

        GLuint shader = glCreateShader(shader_type);

        std::ifstream t(path);
        std::stringstream buffer;
        buffer << t.rdbuf();

        std::string str = buffer.str();
        const char *c_str = str.c_str();

        glShaderSource(shader, 1, &c_str, NULL);

        glCompileShader(shader);

        GLint fShaderCompiled = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &fShaderCompiled);

        if (fShaderCompiled != GL_TRUE)
        {
            printf("Unable to compile shader %d!\n", shader);
            printShaderLog(shader);
            return -1;
        }

        return shader;
    }

    void link_shader(GLuint program)
    {

        glLinkProgram(program);

        GLint programSuccess = GL_TRUE;
        glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
        if (programSuccess != GL_TRUE)
        {
            printf("Error linking program %d!\n", program);
            gl_util::printProgramLog(program);
        }
    }

    struct Mesh
    {

    public:
        Mesh()
        {
            mVAO = 0;
            mVBOs = std::vector<GLuint>();
        }

        GLuint mVAO;
        std::vector<GLuint> mVBOs;

        void init()
        {
            glGenVertexArrays(1, &mVAO);
            glBindVertexArray(mVAO);
        }

        void cleanup()
        {
            for (GLuint b : mVBOs)
            {
                glDeleteBuffers(1, &b);
            }
            glDeleteVertexArrays(1, &mVAO);
        }

        template <typename T>
        void attach_buffer(T *data, uint32_t len, GLenum buffer_type)
        {
            GLuint gVBO = 0;

            glGenBuffers(1, &gVBO);
            glBindBuffer(buffer_type, gVBO);
            glBufferData(buffer_type, len * sizeof(T), data, GL_STATIC_DRAW);

            mVBOs.push_back(gVBO);
        }

        void bind()
        {
            glBindVertexArray(mVAO);
        }

        void draw_elements(GLenum mode, GLsizei count)
        {
            glDrawElements(mode, count, GL_UNSIGNED_INT, NULL);
        }
    };

    struct Program
    {
        GLint id;

        Program()
        {
            id = -1;
        }

        Program(const char *vs_path, const char *fs_path, const char *gs_path = nullptr)
        {
            id = glCreateProgram();

            GLint vs = gl_util::new_shader(vs_path, GL_VERTEX_SHADER);
            glAttachShader(id, vs);
            GLint gs = 0;
            if (gs_path != nullptr)
            {
                gs = gl_util::new_shader(gs_path, GL_GEOMETRY_SHADER);
                glAttachShader(id, gs);
            }
            GLint fs = gl_util::new_shader(fs_path, GL_FRAGMENT_SHADER);
            glAttachShader(id, fs);

            gl_util::link_shader(id);

            glDetachShader(id, vs);
            if (gs_path != nullptr)
            {
                glDetachShader(id, gs);
            }
            glDetachShader(id, fs);

            glDeleteShader(vs);
            if (gs_path != nullptr)
            {
                glDeleteShader(gs);
            }
            glDeleteShader(fs);
        }

        void use()
        {
            glUseProgram(id);
        }
    };

    struct Texture
    {
        GLuint id;

        bool load_png(const char *filename)
        {

            unsigned error;
            unsigned char *image = 0;
            unsigned width, height;

            error = lodepng_decode32_file(&image, &width, &height, filename);
            if (error){
                printf("error %u: %s\n", error, lodepng_error_text(error));
                return false;
            }

            GLuint id;
            glGenTextures(1, &id);
            glBindTexture(GL_TEXTURE_2D, id);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

            delete (image);

            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            //glGenerateMipmap(GL_TEXTURE_2D);

            return true;
        }

        Texture()
        {
            id = 0;
        }
    };
}