/**
* Author: Jaden Thakur
* Assignment: Rise of the AI
* Date due: 2023-11-18, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

// #defines

#define GL_SILENCE_DEPRECATION
#define GL_GLEXT_PROTOTYPES 1
#define LOG(argument) std::cout << argument << std::endl;
#define FIXED_TIMESTEP 0.0166666f
#define MAP_WIDTH 12
#define MAP_HEIGHT 10

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

// includes
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include <vector>
#include "Entity.h"
#include "Utility.h"
#include "Map.h"
#include "SDL_mixer.h"
#include "cmath"
#include <ctime>
#include <cstdlib>
#include <vector>



struct GameState
{
    Map* map;
    Entity* player;
    Entity* ui;
    Entity* enemies;
};

// Globals 
const int WINDOW_WIDTH = 1000,
WINDOW_HEIGHT = 600;

const float BG_RED = 0.388f, // #63ADF2
BG_GREEN = 0.678f,
BG_BLUE = 0.949f,
BG_OPACITY = 1.0f;

const int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

const float MILLISECONDS_IN_SECOND = 1000.0;
const char PLAYER_SPRITESHEET_FILEPATH[] = "assets/Player_Spritesheet.png";
const char SPIKY_SPRITESHEET_FILEPATH[] = "assets/Spiky_Spritesheet.png";
const char JUMPY_SPRITESHEET_FILEPATH[] = "assets/Jumpy_Spritesheet.png";
const char DASHY_SPRITESHEET_FILEPATH[] = "assets/Dashy_Spritesheet.png";
const char MAP_FILEPATH[] = "assets/tileset.png";
const char font_texture[] = "assets/font.png";

GLuint font_texture_id;
glm::vec3 ui_element_position = glm::vec3(3.0f, -5.0f, 0.0f);

const int NUMBER_OF_TEXTURES = 1;
const GLint LEVEL_OF_DETAIL = 0;
const GLint TEXTURE_BORDER = 0;


GameState g_state;

SDL_Window* g_display_window;
bool go = false;
bool win = false;
bool g_game_is_running = true;
std::string endscreen_text;

ShaderProgram g_program;
glm::mat4 g_view_matrix, g_projection_matrix;

float g_previous_ticks = 0.0f;
float g_accumulator = 0.0f;

void initialise()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    g_display_window = SDL_CreateWindow("Bonker",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    font_texture_id = Utility::load_texture(font_texture);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);

#ifdef _WINDOWS
    glewInit();
#endif

    // Viewport Setup 
    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(0.5f, 10.5f, -9.5f, 0.5f, -1.0f, 1.0f);

    g_program.set_projection_matrix(g_projection_matrix);
    g_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_program.get_program_id());

    glClearColor(BG_RED, BG_GREEN, BG_BLUE, BG_OPACITY);


    // Map Stuff
    // 
    GLuint map_texture_id = Utility::load_texture(MAP_FILEPATH);
    unsigned int MAP_DATA[] =
    {
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 0, 3, 4, 5, 0, 0, 3, 4, 5, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2,
        2, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 2,
        2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 1, 2,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    };


   
    g_state.map = new Map(MAP_WIDTH, MAP_HEIGHT, MAP_DATA, map_texture_id, 1.0f, 7, 1);



    // Player Stuff
    // Create Player
    g_state.player = new Entity();
    g_state.player->set_position(glm::vec3(1.0f, 0.0f, 0.0f));
    g_state.player->set_movement(glm::vec3(0.0f));
    g_state.player->set_speed(3.0f);
    g_state.player->set_acceleration(glm::vec3(0.0f, -4.905f, 0.0f));
    g_state.player->m_texture_id = Utility::load_texture(PLAYER_SPRITESHEET_FILEPATH);
    g_state.player->set_entity_type(PLAYER);
    g_state.player->m_animation[g_state.player->LEFT] = new int[4] { 1, 3, 5, 7 };
    g_state.player->m_animation[g_state.player->RIGHT] = new int[4] { 0, 2, 4, 6 };
    g_state.player->m_animation_indices = g_state.player->m_animation[g_state.player->LEFT];
    g_state.player->m_animation_time = 0.0f;
    g_state.player->m_animation_frames = 4;
    g_state.player->m_animation_index = 0;
    g_state.player->m_animation_cols = 2;
    g_state.player->m_animation_rows = 4;


    g_state.enemies = new Entity[3];

    // Create Jumpy
    g_state.enemies[0].set_position(glm::vec3(8.0f, 0.0f, 0.0f));
    g_state.enemies[0].set_movement(glm::vec3(0.0f));
    g_state.enemies[0].set_speed(0.5f);
    g_state.enemies[0].set_acceleration(glm::vec3(0.0f, -4.905f, 0.0f));
    g_state.enemies[0].m_texture_id = Utility::load_texture(JUMPY_SPRITESHEET_FILEPATH);
    g_state.enemies[0].set_entity_type(ENEMY);
    g_state.enemies[0].set_enemy_type(JUMPY);
    g_state.enemies[0].m_animation[g_state.enemies[0].LEFT] = new int[2] { 1, 3};
    g_state.enemies[0].m_animation[g_state.enemies[0].RIGHT] = new int[2] { 0, 2};
    g_state.enemies[0].m_animation_indices = g_state.enemies[0].m_animation[g_state.enemies[0].LEFT];
    g_state.enemies[0].m_animation_time = 0.0f;
    g_state.enemies[0].m_animation_frames = 2;
    g_state.enemies[0].m_animation_index = 0;
    g_state.enemies[0].m_animation_cols = 2;
    g_state.enemies[0].m_animation_rows = 2;

    // Create Dashy

    g_state.enemies[1].set_position(glm::vec3(5.0f, 0.0f, 0.0f));
    g_state.enemies[1].set_movement(glm::vec3(0.0f));
    g_state.enemies[1].set_speed(0.5f);
    g_state.enemies[1].set_acceleration(glm::vec3(0.0f, -4.905f, 0.0f));
    g_state.enemies[1].m_texture_id = Utility::load_texture(DASHY_SPRITESHEET_FILEPATH);
    g_state.enemies[1].set_entity_type(ENEMY);
    g_state.enemies[1].set_enemy_type(DASHY);
    g_state.enemies[1].m_animation[g_state.enemies[1].LEFT] = new int[2] { 1, 3};
    g_state.enemies[1].m_animation[g_state.enemies[1].RIGHT] = new int[2] { 0, 2};
    g_state.enemies[1].m_animation_indices = g_state.enemies[1].m_animation[g_state.enemies[1].LEFT];
    g_state.enemies[1].m_animation_time = 0.0f;
    g_state.enemies[1].m_animation_frames = 2;
    g_state.enemies[1].m_animation_index = 0;
    g_state.enemies[1].m_animation_cols = 2;
    g_state.enemies[1].m_animation_rows = 2;

    // Create Spiky
    g_state.enemies[2].set_position(glm::vec3(3.0f, 0.0f, 0.0f));
    g_state.enemies[2].set_movement(glm::vec3(0.0f));
    g_state.enemies[2].set_speed(0.0f);
    g_state.enemies[2].set_acceleration(glm::vec3(0.0f, -4.905f, 0.0f));
    g_state.enemies[2].m_texture_id = Utility::load_texture(SPIKY_SPRITESHEET_FILEPATH);
    g_state.enemies[2].set_entity_type(ENEMY);
    g_state.enemies[2].set_enemy_type(SPIKY);
    g_state.enemies[2].m_animation[g_state.enemies[2].LEFT] = new int[1] {0};
    g_state.enemies[2].m_animation[g_state.enemies[2].RIGHT] = new int[1] {1};
    g_state.enemies[2].m_animation_indices = g_state.enemies[2].m_animation[g_state.enemies[2].LEFT];
    g_state.enemies[2].m_animation_time = 0.0f;
    g_state.enemies[2].m_animation_frames = 1;
    g_state.enemies[2].m_animation_index = 0;
    g_state.enemies[2].m_animation_cols = 2;
    g_state.enemies[2].m_animation_rows = 1;
 

    // Needed stuff
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{

    // Stop player from moving without input
    g_state.player->set_movement(glm::vec3(0.0f));

    // One Click Events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type) {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_game_is_running = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                // Quit
                g_game_is_running = false;
                break;
            case SDLK_SPACE:
                if (g_state.player->m_collided_bottom)
                {
                    g_state.player->m_is_jumping = true;
                }
                break;
            default:
                break;
            }
        default:
            break;
        }
    }


    // Holding Down Keys
    const Uint8* key_state = SDL_GetKeyboardState(NULL);

    if (key_state[SDL_SCANCODE_A])
    {
        g_state.player->move_left();
        g_state.player->m_animation_indices = g_state.player->m_animation[g_state.player->LEFT];
    }
    if (key_state[SDL_SCANCODE_D])
    {
        g_state.player->move_right();
        g_state.player->m_animation_indices = g_state.player->m_animation[g_state.player->RIGHT];
    }


    // normalize movement
    if (glm::length(g_state.player->get_movement()) > 1.0f)
    {
        g_state.player->set_movement(
            glm::normalize(
                g_state.player->get_movement()
            )
        );
    }
}

void update()
{
    // delta time
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;

    delta_time += g_accumulator;

    if (delta_time < FIXED_TIMESTEP)
    {
        g_accumulator = delta_time;
        return;
    }

    while (delta_time >= FIXED_TIMESTEP)
    {
        g_state.player->update(FIXED_TIMESTEP, g_state.player, g_state.enemies, 3, g_state.map);
        for (size_t i = 0; i < 3; i++) {
            g_state.enemies[i].update(FIXED_TIMESTEP, g_state.player, NULL, 0, g_state.map);
        }
        delta_time -= FIXED_TIMESTEP;
    }

    g_accumulator = delta_time;

    if (g_state.player->dead) {
        go = true;
    }

    for (size_t i = 0; i < 3; i++) {
        if (g_state.enemies[i].m_is_active) {
            win = false;
            break;
        }
        win = true;
    }

    if (win == true) {
        go = true;
    }
    


}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // deactivate all of the platforms and player on game over
    if (go) {
        g_state.player->deactivate();
    }

    // render everything except UI
    else {
        g_state.map->render(&g_program);
        g_state.player->render(&g_program);

        for (size_t i = 0; i < 3; i++) {
            g_state.enemies[i].render(&g_program);
        }
    }
    

    

    // render UI element based on win or lose
    if (win && go) {
        endscreen_text = "You Won!";
        Utility::draw_text(&g_program, font_texture_id, endscreen_text, 0.5f, 0.000001f, ui_element_position);
    }
    else if (go && !win) {
        endscreen_text = "You Lost!";
        Utility::draw_text(&g_program, font_texture_id, endscreen_text, 0.5f, 0.000001f, ui_element_position);
    }


    // Swap window
    SDL_GL_SwapWindow(g_display_window);
}

void shutdown()
{
    SDL_Quit();

    // delete everything 
    delete[] g_state.enemies;
    for (size_t i = 0; i < 3; i++) {
        g_state.enemies = nullptr;
    }
    delete g_state.map;
    g_state.map = nullptr;
    delete g_state.player;
    g_state.player = nullptr;
}

// GAME LOOP
int main(int argc, char* argv[])
{
    initialise();

    while (g_game_is_running)
    {
        process_input();
        update();
        render();
    }

    shutdown();
    return 0;
}