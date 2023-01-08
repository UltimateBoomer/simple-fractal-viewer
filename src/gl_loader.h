#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#define GLEW_STATIC
#define GLFW_INCLUDE_NONE

#include "GL/glew.h"
#include "GLFW/glfw3.h"

// Create shader program with vertex shader and fragment shader
GLuint load_shaders(std::string vert_file, std::string frag_file)
{
    GLuint vert_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint frag_id = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vert_src;
    {
        std::ifstream fs(vert_file);
        if (fs.is_open())
        {
            std::stringstream ss;
            ss << fs.rdbuf();
            vert_src = ss.str();
        }
        else
        {
            std::cerr << "Failed to open shader file: " << vert_file << std::endl;
            return 0;
        }
    }

    std::string frag_src;
    {
        std::ifstream fs(frag_file);
        if (fs.is_open())
        {
            std::stringstream ss;
            ss << fs.rdbuf();
            frag_src = ss.str();
        }
        else
        {
            std::cerr << "Failed to open shader file: " << frag_file << std::endl;
            return 0;
        }
    }

    GLint result;
    GLint info_log_len;

    std::cout << "Compiling vertex shader: " << vert_file << std::endl;
    const char* vert_src_ptr = vert_src.c_str();
    glShaderSource(vert_id, 1, &vert_src_ptr, NULL);
    glCompileShader(vert_id);
    glGetShaderiv(vert_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vert_id, GL_INFO_LOG_LENGTH, &info_log_len);
    if (info_log_len > 0)
    {
        std::vector<char> err_msg(info_log_len + 1);
        glGetShaderInfoLog(vert_id, info_log_len, NULL, &err_msg[0]);
        std::cerr << err_msg[0] << std::endl;
    }

    std::cout << "Compiling fragment shader: " << frag_file << std::endl;
    const char* frag_src_ptr = frag_src.c_str();
    glShaderSource(frag_id, 1, &frag_src_ptr, NULL);
    glCompileShader(frag_id);
    glGetShaderiv(frag_id, GL_COMPILE_STATUS, &result);
    glGetShaderiv(frag_id, GL_INFO_LOG_LENGTH, &info_log_len);
    if (info_log_len > 0)
    {
        std::vector<char> err_msg(info_log_len + 1);
        glGetShaderInfoLog(frag_id, info_log_len, NULL, &err_msg[0]);
        std::cerr << err_msg[0] << std::endl;
    }

    std::cout << "Linking program" << std::endl;
    GLuint program = glCreateProgram();
    glAttachShader(program, vert_id);
    glAttachShader(program, frag_id);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_len);
    if (info_log_len > 0)
    {
        std::vector<char> err_msg(info_log_len + 1);
        glGetProgramInfoLog(frag_id, info_log_len, NULL, &err_msg[0]);
        std::cerr << err_msg[0] << std::endl;
    }

    glDetachShader(program, vert_id);
    glDetachShader(program, frag_id);
    glDeleteShader(vert_id);
    glDeleteShader(frag_id);

    return program;
}
