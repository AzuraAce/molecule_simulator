#include "widget.h"

void TextInput_init(TextInput *input, TTF_Font *font, SDL_Rect box){
  if (!input) {
    fprintf(stderr, "TextInput pointer is NULL\n");
    return;
  }
  if (!font) {
      fprintf(stderr, "Font is NULL (check TTF_OpenFont)\n");
      return;
  }

  memset(input, 0, sizeof(TextInput));
  input->font = font;
  input->box = box;
  input->buffer[0] = '\0';
  TextInput_updateTexture(input);
}

GLuint SDLsurface_to_texture(SDL_Surface *surface){
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  GLenum format = (surface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;

  glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return texture;
}

void TextInput_updateTexture(TextInput *input){
  if (!input) {
      fprintf(stderr, "TextInput is NULL\n");
      return;
  }
  if (!input->font) {
      fprintf(stderr, "Font in TextInput is NULL\n");
      return;
  }

  if (input->texture) {
      glDeleteTextures(1, &input->texture);
      input->texture = 0;
  }
 
  const char* renderText = (*input->buffer) ? input->buffer : " ";

  SDL_Color color = {255, 255, 255, 255};
  SDL_Surface* surface = TTF_RenderText_Blended(input->font, renderText, color);
  if (!surface) {
      fprintf(stderr, "TTF_RenderText_Blended failed: %s\n", TTF_GetError());
      return;
  }

  input->texture = SDLsurface_to_texture(surface);
  input->width = surface->w;
  input->height = surface->h;
  SDL_FreeSurface(surface);
}

void TextInput_handleEvent(TextInput* input, SDL_Event* e) {
    if (e->type == SDL_MOUSEBUTTONDOWN) {
        int x = e->button.x, y = e->button.y;
        input->active = SDL_PointInRect(&(SDL_Point){x, y}, &input->box);
    } else if (e->type == SDL_TEXTINPUT && input->active) {
        if (strlen(input->buffer) + strlen(e->text.text) < sizeof(input->buffer) - 1) {
            strcat(input->buffer, e->text.text);
            TextInput_updateTexture(input);
        }
    } else if (e->type == SDL_KEYDOWN && input->active) {
        if (e->key.keysym.sym == SDLK_BACKSPACE) {
            size_t len = strlen(input->buffer);
            if (len > 0) {
                input->buffer[len - 1] = '\0';
                TextInput_updateTexture(input);
            }
        }
    }
}

void TextInput_render(TextInput* input, int win_w, int win_h) {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0, win_w, win_h, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    // Draw box
    glColor3f(1.0f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(input->box.x, input->box.y);
    glVertex2f(input->box.x + input->box.w, input->box.y);
    glVertex2f(input->box.x + input->box.w, input->box.y + input->box.h);
    glVertex2f(input->box.x, input->box.y + input->box.h);
    glEnd();

    glColor3f(1, input->active ? 1 : 0.5, 1);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
    glVertex2f(input->box.x, input->box.y);
    glVertex2f(input->box.x + input->box.w, input->box.y);
    glVertex2f(input->box.x + input->box.w, input->box.y + input->box.h);
    glVertex2f(input->box.x, input->box.y + input->box.h);
    glEnd();

    if (input->texture) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, input->texture);
        glColor3f(1, 1, 1);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex2f(input->box.x + 5, input->box.y + 5);
        glTexCoord2f(1, 0); glVertex2f(input->box.x + 5 + input->width, input->box.y + 5);
        glTexCoord2f(1, 1); glVertex2f(input->box.x + 5 + input->width, input->box.y + 5 + input->height);
        glTexCoord2f(0, 1); glVertex2f(input->box.x + 5, input->box.y + 5 + input->height);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }

    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}

void TextInput_destroy(TextInput* input) {
    if (input->texture) glDeleteTextures(1, &input->texture);
}
