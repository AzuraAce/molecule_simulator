#include "widget.h"
#include <SDL2/SDL.h>

TextLabel* init_TextLabel(float x, float y, float w, float h){
  TextLabel* label = (TextLabel*)malloc(sizeof(TextLabel));

  float vertices[] = {
      // positions                         // texture coords
      x,     y + h,   0.0f, 1.0f,  // top-left
      x,     y,       0.0f, 0.0f,  // bottom-left
      x + w, y,       1.0f, 0.0f,  // bottom-right

      x,     y + h,   0.0f, 1.0f,  // top-left
      x + w, y,       1.0f, 0.0f,  // bottom-right
      x + w, y + h,   1.0f, 1.0f   // top-right;
  };

  glGenVertexArrays(1, &(label->vao));
  glGenBuffers(1, &(label->vbo));

  glBindVertexArray(label->vao);
  glBindBuffer(GL_ARRAY_BUFFER, label->vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Position attribute (location = 0)
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  // TexCoord attribute (location = 1)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  return label;
}

GLuint SDL_surface_to_OpenGL_texture(SDL_Surface* surface){
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    // Tell OpenGl to read byte-by-byte
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Get known RGBA format
    SDL_Surface* formatted = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formatted->w, formatted->h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, formatted->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_FreeSurface(formatted);
    return tex;
}

void create_OpenGL_text_label(TextLabel* label, const char* message, const char* fontFile,
    SDL_Color color, int fontSize)
{
    //Open the font
    TTF_Font* font = TTF_OpenFont(fontFile, fontSize);
    if (font == NULL){
        fprintf(stderr, "Error: Failed to open font with path: %s\n", fontFile);
        return;
    }   
    //We need to first render to a surface as that's what TTF_RenderText
    //returns, then load that surface into a texture
    SDL_Surface* surf = TTF_RenderUTF8_Blended(font, message, color);
    if (surf == NULL){
        TTF_CloseFont(font);
        fprintf(stderr, "Error: Failed to create SDL_Surface\n");
        return;
    }
    label->textureID = SDL_surface_to_OpenGL_texture(surf);

    //Clean up the surface and font
    SDL_FreeSurface(surf);
    TTF_CloseFont(font);

    return;
}

void render_text_texture(TextLabel* label, GLuint shader, int w_width, int w_height){
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      float proj[16] = {
        2.0f / w_width,    0.0f,             0.0f, 0.0f,
        0.0f,             -2.0f / w_height,  0.0f, 0.0f,
        0.0f,              0.0f,            -1.0f, 0.0f,
       -1.0f,              1.0f,             0.0f, 1.0f
      };

    GLint loc = glGetUniformLocation(shader, "projection");
  glUseProgram(shader);
     glUniformMatrix4fv(loc, 1, GL_FALSE, proj);
  glBindTexture(GL_TEXTURE_2D, label->textureID);
  glBindVertexArray(label->vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
  
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}

void delete_label(TextLabel* label){
  glDeleteVertexArrays(1, &(label->vao));
  glDeleteBuffers(1, &(label->vbo));
  glDeleteTextures(1, &(label->textureID));

  free(label);
}
