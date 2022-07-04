#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#include <GL/glew.h>

#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600

void MessageCallback(GLenum source,
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


void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    (void) window;
    glViewport(
        width / 2 - WINDOW_WIDTH / 2,
        height / 2 - WINDOW_HEIGHT / 2,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);
}

typedef enum {
    POSITION_ATTRIB = 0,
    COLOR_ATTRIB,
    UV_ATTRIB
} bGlAttribEnums;

#define MAX_ATTRIBUTES 100
typedef struct {
    bGlAttribEnums  type;
    size_t          offset;
    size_t          stride;
} bglAttribute;

#define MAX_UNIFORMS 100
#define UNIF_NAME_SIZE 20
typedef struct {
    char    name[UNIF_NAME_SIZE];
} bglUniform;

typedef struct {
    GLuint  vao;
    GLuint  vbo;
    bool    have_data;
} bGlBuffers;

typedef struct {
    GLuint          program;
    bGlBuffers       *buffers;
    bglAttribute    attributes[MAX_ATTRIBUTES];
    size_t          attributes_count;
    bglUniform      uniforms[MAX_UNIFORMS];
} bGlData;

bGlData* NewData() {
    bGlData *data = malloc(sizeof(bGlData));
    data->attributes_count = 0;
    return data;
}

void destroy_data(bGlData *data) {
    free(data->buffers);
    free(data);
}

void data_create_program(bGlData *data) {
    GLuint program = glCreateProgram();
    data->program = program;
}

void data_create_buffers(bGlData *data) {
    data->buffers = malloc(sizeof(bGlBuffers));

    glGenVertexArrays(1, &data->buffers->vao);
    glBindVertexArray(data->buffers->vao);

    glGenBuffers(1, &data->buffers->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, data->buffers->vbo);
    data->buffers->have_data = false;
}

void data_push_initial_buffer_f32(bGlData *data, GLfloat *buf, size_t buf_size) {
    glBindBuffer(GL_ARRAY_BUFFER, data->buffers->vbo);
    //glBufferData(GL_ARRAY_BUFFER, buf_size, 0, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, buf_size, buf, GL_STATIC_DRAW);

    data->buffers->have_data = true;
}

void data_create_vbo_buffer_f32(bGlData *data, bglAttribute attribute, size_t num_components, bool normalized) {
    assert((data->attributes_count + 1) < MAX_ATTRIBUTES);
    glBindBuffer(GL_ARRAY_BUFFER, data->buffers->vbo);

    //assert(data->buffers->have_data);
    const bGlAttribEnums attrib = attribute.type;
    const size_t offset = attribute.offset;
    const size_t stride = attribute.stride;
    
    glVertexAttribPointer(attrib,
                          num_components,
                          GL_FLOAT,
                          GL_FALSE,
                          num_components * sizeof(float),
                          (const void *) 0);
    glEnableVertexAttribArray(attrib);

    data->attributes[data->attributes_count] = attribute;
    data->attributes_count++;
}

typedef GLuint bGlShaderType;
bool compile_shader(GLuint *shader, bGlShaderType type, const char* source) {
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint compiled = 0;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compiled);

    if (!compiled) {
        GLchar message[1024];
        GLsizei message_size = 0;
        glGetShaderInfoLog(*shader, sizeof(message), &message_size, message);
        fprintf(stderr, "ERROR: could not compile shader\n");
        fprintf(stderr, "%.*s\n", message_size, message);
        return false;
    }

    return true;
}

bool data_compile_and_link_program(bGlData *data, char *vert_shader_src, char *frag_shader_src) {
    GLuint vert_shader;
    GLuint frag_shader;

    if(!compile_shader(&vert_shader, GL_VERTEX_SHADER, vert_shader_src)) {
        return false;
    }  
    if(!compile_shader(&frag_shader, GL_FRAGMENT_SHADER, frag_shader_src)) {
        return false;
    }

    glAttachShader(data->program, vert_shader);
    glAttachShader(data->program, frag_shader);
    glLinkProgram(data->program);

    int linked = 0;
    glGetProgramiv(data->program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLsizei message_size = 0;
        GLchar message[1024];

        glGetProgramInfoLog(data->program, sizeof(message), &message_size, message);
        fprintf(stderr, "Program Linking: %.*s\n", message_size, message);
        return false;
    }

    return true;
}

void bGlRender(bGlData *data, GLfloat *triangle, size_t triangle_size) {

    glBindVertexArray(data->buffers->vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);

}

int main () {
    if (!glfwInit()) {
        fprintf(stderr, "could not initialize glfw.\n");
        return EXIT_FAILURE;
    } 

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow *const window = 
        glfwCreateWindow(WINDOW_WIDTH,
                         WINDOW_HEIGHT,
                         "Bouncing",
                         NULL,
                         NULL);

    // simple triangle
    GLfloat triangle[] = {
        0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.0f, 0.5f, 0.0f,
    };

    if (window == NULL)
    {
        fprintf(stderr, "could not create glfw window.\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);

    if (GLEW_OK != glewInit()) {
        fprintf(stderr, "could not load glew (OpenGL).\n");
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    glfwSetFramebufferSizeCallback(window, window_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    char* vertShaderSrc = "#version 330 core\n"
    "layout (location = 0) in vec3 pos;\n"
    "void main() {\n"
    "    gl_Position = vec4(pos, 1.0);\n"
    "\n"
    "}\n\0";

    char* fragShaderSrc = "#version 330 core\n"
    "out vec4 fragColor;\n"
    "void main() {\n"
    "    fragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);\n"
    "}\n\0";

    bGlData *data = NewData();
    data_create_program(data);
    data_create_buffers(data);
    
    
    bglAttribute attribute;
    attribute.type = POSITION_ATTRIB;
    attribute.stride = 3*sizeof(float);
    attribute.offset = 0;
    data_create_vbo_buffer_f32(data, attribute, 3, true);
    data_push_to_buffer_f32(data, triangle, sizeof(triangle));

    if(!data_compile_and_link_program(data, vertShaderSrc, fragShaderSrc)) {
        return EXIT_FAILURE;
    }
   

    data_push_to_buffer_f32(data, triangle, sizeof(triangle));
    data_create_vbo_buffer_f32(data, attribute, 3, GL_FALSE);
    
    glUseProgram(data->program);

    data->attributes[data->attributes_count] = attribute;
    data->attributes_count++;
    
    glClearColor(0.8f, 0.8f, 0.8f, 1.0);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        bGlRender(data, triangle, sizeof(triangle));
        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }    
}
