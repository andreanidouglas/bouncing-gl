#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "bouncing.h"


#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600


// TODO: Factor out Canvas/Pixel to separate header
typedef struct _pix {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

} Pix;

typedef struct _canvas {
    Pix *pixels;    
} Canvas;

int canvas_init(Canvas *c){
    c->pixels = malloc(WINDOW_HEIGHT * WINDOW_WIDTH * sizeof(Pix));
    if (c->pixels == NULL) {
        return 0;
    }
    
    memset(c->pixels, 0, WINDOW_HEIGHT * WINDOW_WIDTH * sizeof(Pix));
    return 1;
}

void canvas_destroy(Canvas c) {
    free(c.pixels);
}

void canvas_clear(Canvas *c) {
    Pix p = { .r = 51, .g = 51, .b = 51, .a = 255};
    for (int i = 0; i < WINDOW_HEIGHT * WINDOW_WIDTH; ++i) {
        c->pixels[i] = p;
    }
}



void canvas_draw(Canvas *c, Grid g) {
    int grid_size = grid_get_size(g);
    int grid_width = grid_get_width(g);

    for (int i = 0; i < grid_size; ++i) {
        if (g.boxes[i] != NULL) {
            //draw box on canvas
            Box b = *g.boxes[i];
            int box_idx = b.pos.x + b.pos.y * grid_width;
            Pix p = { .r = 255, .g = 255, .b = 0, .a = 255};

            for (int h = 0; h < b.h; ++h){
                for (int w = 0; w < b.w; ++w) {
                    c->pixels[box_idx + (w + h * WINDOW_WIDTH)] = p;
                }
            }
        }
    }
}

void canvas_save_to_ppm(Canvas c, char* fileout_path) {
    FILE *f = fopen(fileout_path, "wb");
    unsigned char color[3];
    if (f == NULL) {
        fprintf(stderr, "[ERROR]: could not open file for save: %s\n", strerror(errno));

    }
    fprintf(f, "P6\n%d %d\n255\n", WINDOW_WIDTH, WINDOW_HEIGHT);

    for (int i = 0; i < WINDOW_HEIGHT * WINDOW_WIDTH; ++i) {
        Pix p = c.pixels[i];
        color[0] = p.r;
        color[1] = p.g;
        color[2] = p.b;
        (void) fwrite(color, 1, 3, f);
    }
    fflush(f);
    fclose(f);
}

void gl_message_callback(GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar* message,
                     const void* userParam)
{
    (void) source;
    (void) id;
    (void) length;
    (void) userParam;
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
}


void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "[ERROR]: GLFW %d - %s\n", error, description);
}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    (void) mods;
    (void) scancode;
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static const GLchar* vert_shader = "#version 330 core\n" \
                                   "out vec2 uv;\n" \
                                   "void main ()\n" \
                                   "{\n" \
                                   "    uv = vec2(gl_VertexID & 1, gl_VertexID >> 1);\n" \
                                   "    gl_Position = vec4(2.0 * uv -1, 0.0, 1.0);\n" \
                                   "}";

// TODO: Identify why the texture is upside down.
static const GLchar* frag_shader = "#version 330 core\n"\
                                   "out vec4 color;\n" \
                                   "in vec2 uv;\n" \
                                   "uniform sampler2D frame;\n" \
                                   "void main ()\n" \
                                   "{\n" \
                                   "    vec4 pixel = texture(frame, vec2(uv.x, uv.y));\n" \
                                   "    color = vec4(pixel.xyz, 1.0f);\n" \
                                   "}";


const char *shader_type_as_cstr(GLuint shader)
{
    switch (shader) {
    case GL_VERTEX_SHADER:
        return "GL_VERTEX_SHADER";
    case GL_FRAGMENT_SHADER:
        return "GL_FRAGMENT_SHADER";
    default:
        return "(Unknown)";
    }
}

int compile_shader_source(const GLchar *source, GLenum shader_type, GLuint *shader) {

    *shader = glCreateShader(shader_type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint compiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
        fprintf(stderr, "[ERROR]: could not compile %s\n", shader_type_as_cstr(shader_type));
        fprintf(stderr, "%.*s\n", message_size, message);
        return 0;
    }

    
    return 1;
}

int link_shader_program(GLuint vert_shader, GLuint frag_shader, GLuint *program) {
    *program = glCreateProgram();

    glAttachShader(*program, vert_shader);
    glAttachShader(*program, frag_shader);
    glLinkProgram(*program);

    GLint linked = 0;
    glGetProgramiv(*program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLsizei message_size = 0;
        GLchar message[1024];

        glGetProgramInfoLog(*program, sizeof(message), &message_size, message);
        fprintf(stderr, "[ERROR]: Program Linking: %.*s\n", message_size, message);
        return 0;
    }

    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    return 1;

}


int main () {

    GLFWwindow *window;

    

    if (!glfwInit()){
        fprintf(stderr, "[ERROR]: Could not initialize glfw\n");
        return 1;
    }

    glfwSetErrorCallback(glfw_error_callback);



    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Bouncing ball", NULL, NULL);
    if (!window) {
        fprintf(stderr, "[ERROR]: Could not glfw window\n");
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "[ERROR]: %s\n", glewGetErrorString(err));
        return 1;
    }

    
    glfwSetKeyCallback(window, glfw_key_callback);

    if (glDebugMessageCallback != NULL) {
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(gl_message_callback, 0);
    }
    
    // initialize vao
    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    

    // TODO: Remove dead code
    // TODO: Factor OpenGL code out of the main function
    /** unused code
     
    GLfloat vertices[] = {
        -0.1f, -0.1f, 0.0f,
         0.1f, -0.1f, 0.0f,
         0.0f,  0.1f, 0.0f
    };

    // initialize vbo and bind it to the active vao
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);
    end unused code **/
    
    GLuint vert = 0;
    if (!compile_shader_source(vert_shader, GL_VERTEX_SHADER, &vert)) {
        return 1;
    }

    GLuint frag = 0;
    if (!compile_shader_source(frag_shader, GL_FRAGMENT_SHADER, &frag)) {
        return 1;
    }

    GLuint program = 0;
    if (!link_shader_program(vert, frag, &program)) {
        return 1;
    }

    // Initialize simulation logic
    // TODO: Factor the simulation piece out of the main function
    Grid grid;
    grid_init(&grid);

    Box b1 = box_init( 30, 30, 200, 200);
    Box b2 = box_init(240, 30, 200, 200);
    Box b3 = box_init(450, 30, 200, 200);

    place_box_in_grid(&grid, &b1);
    place_box_in_grid(&grid, &b2);
    place_box_in_grid(&grid, &b3);

    
    box_set_vel(&b1, veci2d_init(1, 1));
    box_set_vel(&b2, veci2d_init(2, 2));
    box_set_vel(&b3, veci2d_init(-1, -1));

    Canvas canvas;
    if (!canvas_init(&canvas)) {
        fprintf(stderr, "[ERROR]: could not initialize canvas.");
        return 1;
    };
    
    canvas_clear(&canvas);
    canvas_draw(&canvas, grid);

    char* fileout = "canvas.ppm";
    canvas_save_to_ppm(canvas, fileout);

    // Create texture
    glBindVertexArray(vao);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas.pixels);


    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        

        glUseProgram(program);
        glBindVertexArray(vao);

        glTextureSubImage2D(texture, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, canvas.pixels);

        grid_step(&grid);
        canvas_clear(&canvas);
        canvas_draw(&canvas, grid);

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        

    }

    printf("[INFO]: Exiting successfully\n");
    glfwDestroyWindow(window);
    glfwTerminate();

    grid_destroy(grid);
    canvas_destroy(canvas);
    
    return 0;
}
