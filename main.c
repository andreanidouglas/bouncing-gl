#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "bouncing.h"


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
                                   "layout (location = 0) in vec3 aPos;\n" \
                                   "void main () \n" \
                                   "{\n" \
                                   "    gl_Position = vec4(aPos, 1.0);\n" \
                                   "}";

static const GLchar* frag_shader = "#version 330 core\n"\
                                   "out vec4 FragColor;\n" \
                                   "void main ()\n" \
                                   "{\n" \
                                   "    FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n" \
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


    GLfloat vertices[] = {
        -0.1f, -0.1f, 0.0f,
         0.1f, -0.1f, 0.0f,
         0.0f,  0.1f, 0.0f
    };

    // initialize vao
    GLuint vao;
    glCreateVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // initialize vbo and bind it to the active vao
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    
    



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


    while(!glfwWindowShouldClose(window)) {
        
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);

        glDrawArrays(GL_TRIANGLES, 0, 3);


        glfwSwapBuffers(window);
        glfwPollEvents();

    }


    printf("[INFO]: Exiting successfully\n");
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
