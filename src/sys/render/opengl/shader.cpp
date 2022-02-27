#include "shader.h"
#include "../../log/log.h"

namespace Alkahest
{
    Ref<Shader> Shader::create(const char* vertShaderSourceFile, const char* fragShaderSourceFile)
    {
        return CreateRef<OpenGLShader>(vertShaderSourceFile, fragShaderSourceFile);
    }

    std::string OpenGLShader::readSourceFile(const char* path)
    {
        std::ifstream fileIn(path, std::ios::in);
        if (fileIn)
        {
            std::string data;
            fileIn.seekg(0, std::ios::end);
            data.resize(fileIn.tellg());
            fileIn.seekg(0, std::ios::beg);
            fileIn.read(&data[0], data.size());
            fileIn.close();
            return data;
        }
        throw(errno);
    }

    OpenGLShader::OpenGLShader(const char* vertexShaderSourceFile, const char* fragmentShaderSourceFile)
    {
        std::string vertShaderCode;
        std::string fragShaderCode;

        try
        {
            // Read shader source files
            vertShaderCode = readSourceFile(vertexShaderSourceFile);
            fragShaderCode = readSourceFile(fragmentShaderSourceFile);
        }
        catch (int e)
        {
            logError("Error reading shader source files! Error: {}", std::strerror(e));
            return;
        }

        // Convert from std::string to c_str
        const char* vertexShaderSource = vertShaderCode.c_str();
        const char* fragmentShaderSource = fragShaderCode.c_str();

        // Create shader, add source data, and compile
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, 0);
        glCompileShader(vertexShader);

        GLint result = GL_FALSE;

        // Check the vertShader compile status
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);
            // The length includes the NULL terminator
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

            // Delete the shader because we don't need it
            glDeleteShader(vertexShader);

            // Process infolog
            logError("Error compiling vertex shader! Error: {}", infoLog.data());

            // Return because we can't get the shaders working
            return;
        }

        // Create frag shader, add source data, and compile
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, 0);
        glCompileShader(fragmentShader);

        // reset the result value
        result = GL_FALSE;
        
        // Check the fragShader compile status
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);
            // Length includes null terminator
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

            // Delete both shaders because we don't need them anymore
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);

            // Process infolog
            logError("Error compiling fragment shader! Error: {}", infoLog.data());

            // Return because we can't get the shaders working
            return;
        }

        // Create program, attach shaders, and compile (link)
        m_id = glCreateProgram();
        glAttachShader(m_id, vertexShader);
        glAttachShader(m_id, fragmentShader);
        glLinkProgram(m_id);
        glValidateProgram(m_id);

        // Reset result
        result = GL_FALSE;

        // Query program compilation (link) status
        glGetProgramiv(m_id, GL_LINK_STATUS, &result);
        if (result == GL_FALSE)
        {
            int maxLength;
            glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);
            // length includes NULL terminator
            std::vector<GLchar> programError(maxLength);
            glGetProgramInfoLog(m_id, maxLength, &maxLength, &programError[0]);

            // Detach and delete shaders because we don't need them
            glDetachShader(m_id, vertexShader);
            glDeleteShader(vertexShader);

            glDetachShader(m_id, fragmentShader);
            glDeleteShader(fragmentShader);

            // Delete program since we can't use it
            glDeleteProgram(m_id);

            // Reset the ID so we don't think we have a valid shader
            m_id = 0;

            // Process infolog
            logError("Error linking shader program! Error: {}", programError.data());

            // Return since we broke everything
            return;
        }

        // Reset result
        result = GL_FALSE;

        // Query program validation status
        glGetProgramiv(m_id, GL_VALIDATE_STATUS, &result);
        if (result == GL_FALSE)
        {
            int maxLength;
            glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);
            // length includes NULL terminator
            std::vector<GLchar> programError(maxLength);
            glGetProgramInfoLog(m_id, maxLength, &maxLength, &programError[0]);

            // Detach and delete shaders because we don't need them
            glDetachShader(m_id, vertexShader);
            glDeleteShader(vertexShader);

            glDetachShader(m_id, fragmentShader);
            glDeleteShader(fragmentShader);

            // Delete program since we can't use it
            glDeleteProgram(m_id);

            // Reset the ID so we don't think we have a valid shader
            m_id = 0;

            // Process infolog
            logError("Error validating shader program! Error: {}", programError.data());

            // Return since we broke everything
            return;
        }

        // Mark the shaders for deletion
        // NOTE: they won't be deleted until they are detached from the program,
        // either manually or via glDeleteProgram()
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void OpenGLShader::activate()
    {
        if (m_id > 0)
            glUseProgram(m_id);
        else
            logError("Cannot activate invalid shader!");
    }

    void OpenGLShader::deactivate()
    {
        glUseProgram(0);
    }

    void OpenGLShader::destroy()
    {
        glDeleteProgram(m_id);

        // Reset the ID so we don't think we have a valid shader
        m_id = 0;
    }
}