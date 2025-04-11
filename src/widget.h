#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

typedef struct {
  float x, y, width, height;
  float color[3];
  bool hover, active;
  GLuint vao, vbo;
} Button;

typedef struct {
  GLuint vao, vbo;
  GLuint textureID;
  SDL_Color color;
} TextLabel;

typedef struct {
  char buffer[256];
  bool active;
  GLuint texture;
  int width;
  int height;
  SDL_Rect box;
  TTF_Font* font;
} TextInput;

void init_button(Button* btn);
void render_button(Button* btn, GLuint shader, int win_width, int win_height, int mouse_x, int mouse_y);
void delete_button(Button* btn);

// Text Label
TextLabel* init_TextLabel(float x, float y, float w, float h);
GLuint SDL_surface_to_OpenGL_texture(SDL_Surface* surface);
void create_OpenGL_text_label(TextLabel* label, const char* message, const char* fontFile,
    SDL_Color color, int fontSize);
void render_text_texture(TextLabel* label, GLuint shader, int w_width, int w_height);
void delete_label(TextLabel* label);

GLuint SDLsurface_to_texture(SDL_Surface* surface);
void TextInput_init(TextInput* input, TTF_Font* font, SDL_Rect box);
void TextInput_handleEvent(TextInput* input, SDL_Event* e);
void TextInput_render(TextInput* input, int window_width, int window_height);
void TextInput_destroy(TextInput* input);
void TextInput_updateTexture(TextInput* input);
