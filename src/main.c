#include <GL/glew.h>
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "atom.h"
#include "molecule.h"
#include "shader.h"
#include "widget.h"

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1920

int main(int argc, char* argv[]) {
    Molecule* mol = molecule_from_file("./molecules.txt");
    print_adjacency_list(mol);
  
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
      return 1;
    }
    if(TTF_Init() < 0){
      SDL_Log("TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
      return 1;
    }

    // Set OpenGL version (example: 3.3 Core)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow(
        "Molecule Simulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );

    if (window == NULL) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create OpenGL Context 
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if(!glContext){
      SDL_Log("OpenGL Context could not be created! SDL_Error: %s\n", SDL_GetError());
      return 1;
    }

    // Init OpenGL Loader (Glew or Glad)
    glewExperimental = GL_TRUE;
    if(glewInit() != GLEW_OK){
      printf("Failed to initialize GLEW\n");
      return 1;
    }

    glEnable(GL_PROGRAM_POINT_SIZE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(5);

    // Shader Programs
    float* molecule_vertices = flatten_molecule_positions(mol);
    unsigned int molecule_bond_vertex_count = 0;
    float* molecule_bond_vertices = flatten_molecule_bonds(mol, &molecule_bond_vertex_count);
    
    GLuint shaderProgram_orange = create_shader_program("./shader/vertex_shader.glsl", "./shader/fragment_shader_Orange.glsl");
    GLuint shaderProgram_dynamicColor = create_shader_program("./shader/vertex_shader.glsl", "./shader/fragment_shader_dynamicColor.glsl");

    // Widgets
    GLuint shaderProgram_button = create_shader_program("./shader/vertex_shader_widget.glsl", "./shader/fragment_shader_widget.glsl");
    GLuint shaderProgram_texture = create_shader_program("./shader/vertex_shader_texture.glsl", "./shader/fragment_shader_texture.glsl");
    
    /*
       Store Vertex Data within memory on the GPU using Vertex Buffer Object (VBO)
     */
    // Init vertex buffer objects (VBO)
    unsigned int VBO_atom, VBO_bond;
    glGenBuffers(1, &VBO_atom);
    glGenBuffers(1, &VBO_bond);
    /*
       Tell OpenGL how to interpret Vertex Data using vertex array object (VAO)
     */
    unsigned int VAO[2];
    glGenVertexArrays(2, VAO);

    // first Triangle setup
    // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO[0]);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO_atom);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mol->atom_num * 3, molecule_vertices, GL_STATIC_DRAW);
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    // second Triangle setup
    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_bond);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * molecule_bond_vertex_count * 3, molecule_bond_vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    SDL_Event event;
    SDL_Keycode keycode;
    int quit = 0;

    Button my_button = {100, 100, 100.0f, 50.0f, {1.0f, 1.0f, 1.0f}, false, false, 0, 0};
    init_button(&my_button);
    TextLabel* label  = init_TextLabel(100, 250, 100, 50);
    create_OpenGL_text_label(label, "abcdefghi", "/home/oliver/.local/share/fonts/Arial.ttf", (SDL_Color){250,250,250,255}, 20);

    int mouse_x, mouse_y;

    while (!quit) {
      SDL_GetMouseState(&mouse_x, &mouse_y);

      while (SDL_PollEvent(&event) != 0){
        switch (event.type) {
          // Check if user wants to quit app
          case SDL_QUIT:
            quit = 1;
            break;

          case SDL_KEYDOWN:
            keycode = event.key.keysym.sym;
            switch (keycode) {
              case SDLK_ESCAPE:
                quit = 1;
                break;
            }
            break;

          case SDL_MOUSEBUTTONDOWN:
            break;

          default:
            break;
        }
      }
      // OpenGL Rendering
      glClearColor(0.18f, 0.18f, 0.18f, 1.0f); // Dark Grey
      glClear(GL_COLOR_BUFFER_BIT); // Background

      render_button(&my_button, shaderProgram_button, WINDOW_WIDTH, WINDOW_HEIGHT, mouse_x, mouse_y);
      render_text_texture(label, shaderProgram_texture, WINDOW_WIDTH, WINDOW_HEIGHT);
      
      // Draw first Triangle in Orange
      glUseProgram(shaderProgram_orange);
      glBindVertexArray(VAO[0]);
      glDrawArrays(GL_POINTS, 0, mol->atom_num);

      // ticks from ms to seconds
      float ticks = SDL_GetTicks() / 1000.0f;
      if(ticks >= 3){
        create_OpenGL_text_label(label, "3 seconds", "/home/oliver/.local/share/fonts/Arial.ttf", (SDL_Color){250,250,250,255}, 20);
      }
      float greenValue = sin(ticks);

      // Draw Second Triangle in dynamic color
      glUseProgram(shaderProgram_dynamicColor);
      int vertexColorLocation = glGetUniformLocation(shaderProgram_dynamicColor, "ourColor");
      // set "ourColor" in fragmentShaderSource_ourColor
      glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

      glBindVertexArray(VAO[1]);
      glDrawArrays(GL_LINES, 0, molecule_bond_vertex_count);

      SDL_GL_SwapWindow(window);
    }

    /*
       Clean Up
     */
    glDeleteVertexArrays(2, VAO);
    glDeleteBuffers(1, &VBO_atom);
    glDeleteBuffers(1, &VBO_bond);
    glDeleteProgram(shaderProgram_orange);
    glDeleteProgram(shaderProgram_dynamicColor);
    glDeleteProgram(shaderProgram_button);
    glDeleteProgram(shaderProgram_texture);

    delete_button(&my_button);
    delete_molecule(mol);
    delete_label(label);

    TTF_Quit();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

