#pragma once

#include <glad/glad.h>

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

        void cleanup()
        {
            glDeleteProgram(id);
        }
    };

    struct ComputeShader
    {
        GLint id;

        ComputeShader()
        {
            id = -1;
        }

        ComputeShader(const char *cs_path)
        {
            id = glCreateProgram();
            GLint cs = gl_util::new_shader(cs_path, GL_COMPUTE_SHADER);
            glCompileShader(cs);
            glAttachShader(id, cs);
            gl_util::link_shader(id);
            glDetachShader(id, cs);
            glDeleteShader(cs);
        }

        void use()
        {
            glUseProgram(id);
        }

        void cleanup()
        {
            glDeleteProgram(id);
        }
    };
    
    void MessageCallback(GLenum source,
                         GLenum type,
                         GLuint id,
                         GLenum severity,
                         GLsizei length,
                         const GLchar *message,
                         const void *userParam)
    {
        printf("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
               (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
               type, severity, message);
    }

}
