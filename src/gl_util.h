
#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

namespace gl_util
{
    void printProgramLog(GLuint program)
    {
        // Make sure name is shader
        if (glIsProgram(program))
        {
            // Program log length
            int infoLogLength = 0;
            int maxLength = infoLogLength;

            // Get info string length
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // Allocate string
            char *infoLog = new char[maxLength];

            // Get info log
            glGetProgramInfoLog(program, maxLength, &infoLogLength, infoLog);
            if (infoLogLength > 0)
            {
                // Print Log
                printf("%s\n", infoLog);
            }

            // Deallocate string
            delete[] infoLog;
        }
        else
        {
            printf("Name %d is not a program\n", program);
        }
    }

    void printShaderLog(GLuint shader)
    {
        // Make sure name is shader
        if (glIsShader(shader))
        {
            // Shader log length
            int infoLogLength = 0;
            int maxLength = infoLogLength;

            // Get info string length
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // Allocate string
            char *infoLog = new char[maxLength];

            // Get info log
            glGetShaderInfoLog(shader, maxLength, &infoLogLength, infoLog);
            if (infoLogLength > 0)
            {
                // Print Log
                printf("%s\n", infoLog);
            }

            // Deallocate string
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

        // Set fragment source
        glShaderSource(shader, 1, &c_str, NULL);

        // Compile fragment source
        glCompileShader(shader);

        // Check fragment shader for errors
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

        // Check for errors
        GLint programSuccess = GL_TRUE;
        glGetProgramiv(program, GL_LINK_STATUS, &programSuccess);
        if (programSuccess != GL_TRUE)
        {
            printf("Error linking program %d!\n", program);
            gl_util::printProgramLog(program);
        }
    }

    class Mesh
    {

    public:
        Mesh()
        {
            mVAO = 0;
            glGenVertexArrays(1, &mVAO);
            glBindVertexArray(mVAO);

            mVBOs = std::vector<GLuint>();
        }

        ~Mesh()
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

    private:
        GLuint mVAO;
        std::vector<GLuint> mVBOs;
    };

}
