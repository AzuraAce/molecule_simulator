#include "widget.h"

void init_button(Button* btn) {
    float quad[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f
    };

    glGenVertexArrays(1, &btn->vao);
    glGenBuffers(1, &btn->vbo);

    glBindVertexArray(btn->vao);
    glBindBuffer(GL_ARRAY_BUFFER, btn->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void render_button(Button* btn, GLuint shader, int win_width, int win_height, int mouse_x, int mouse_y) {
    // Detect hover
    btn->hover = (mouse_x >= btn->x && mouse_x <= btn->x + btn->width &&
                    mouse_y >= btn->y && mouse_y <= btn->y + btn->height);

    float draw_color[3] = { btn->color[0], btn->color[1], btn->color[2] };
    float alpha = btn->hover ? 1.0f : 0.6f; // More transparent when not hovered

    glEnable(GL_BLEND);

    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "u_resolution"), (float)win_width, (float)win_height);
    glUniform2f(glGetUniformLocation(shader, "u_position"), btn->x, btn->y);
    glUniform2f(glGetUniformLocation(shader, "u_size"), btn->width, btn->height);
    glUniform3f(glGetUniformLocation(shader, "u_color"), draw_color[0], draw_color[1], draw_color[2]);
    glUniform1f(glGetUniformLocation(shader, "u_alpha"), alpha);

    glBindVertexArray(btn->vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisable(GL_BLEND);
}

void delete_button(Button* btn){
  glDeleteVertexArrays(1, &btn->vao);
  glDeleteBuffers(1, &btn->vbo);
}
