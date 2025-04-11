#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include "shader.h"

char* load_shader_code(const char* filepath){
  FILE* file = fopen(filepath, "r");
  if(!file){
    fprintf(stderr, "Error: Could not open shader file %s\n", filepath);
    return NULL;
  }

  // Get file size
  fseek(file, 0, SEEK_END);
  long int size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* shader_code = (char*)malloc(size + 1); // + 1 because we need to store a trailing \0
  if(!shader_code){
    fprintf(stderr, "Error: Failed to allocate memory for shader_code\n");
    fclose(file);
    return NULL;
  }

  // sizeof(char) = 1 but for readability we write sizeof(char)
  fread(shader_code, sizeof(char), size, file);
  shader_code[size] = '\0'; // Null terminate the string
  fclose(file);

  return shader_code;
}

GLuint compile_shader(const char* shader_code, GLenum shader_type){
  // Create Shader Object
  GLuint shader = glCreateShader(shader_type);

  // Attach Shader source code to Shader Object
  glShaderSource(shader, 1, &shader_code, NULL);
  glCompileShader(shader);

  // Error Handling
  int success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if(!success){
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s", infoLog);
    return -1;
  }

  return shader;
}

GLuint create_shader_program(const char* vertex_shader_filepath, const char* fragment_shader_filepath){
  // Load vertex and fragment shader file content
  char* vertex_shader_code = load_shader_code(vertex_shader_filepath);
  char* fragment_shader_code = load_shader_code(fragment_shader_filepath);
  if(!vertex_shader_code || !fragment_shader_code)
    return -1;
  
  GLuint vertex_shader = compile_shader(vertex_shader_code, GL_VERTEX_SHADER);
  GLuint fragment_shader = compile_shader(fragment_shader_code, GL_FRAGMENT_SHADER);
  
  // Create and get first Shader program ID
  GLuint shader_program = glCreateProgram();
  // Attach compiled Shaders
  glAttachShader(shader_program, vertex_shader);
  glAttachShader(shader_program, fragment_shader);
  // Link Shaders
  glLinkProgram(shader_program);
  
  // Error Handling
  int success;
  char infoLog[512];
  glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
  if(!success){
    glGetProgramInfoLog(shader_program, 512, NULL, infoLog);
    printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s", infoLog);
    glDeleteProgram(shader_program);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return -1;
  }
  
  // Clean Up
  glDeleteShader(vertex_shader);
  glDeleteShader(fragment_shader);

  return shader_program;
}
