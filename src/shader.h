#include <GL/glew.h>

char* load_shader_code(const char* filepath);
GLuint compile_shader(const char* shader_code, GLenum shader_type);
GLuint create_shader_program(const char* vertex_shader_filepath, const char* fragment_shader_filepath);
