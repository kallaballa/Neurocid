/*
  do next: multiply view and projection matrices with GSL and pass result
  to vertex shader.
*/

#ifndef __APPLE__
    #include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>

#ifdef __APPLE__
    #include <OpenGL/glu.h>
#else
    #include <glu.h>
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static const int CLM_WIN_WIDTH = 1000;
static const int CLM_WIN_HEIGHT = 600;

static SDL_Window* window = NULL;
//static SDL_Renderer* display_renderer = NULL;
static SDL_GLContext gl_context = NULL;
static GLuint vbo = 0;
static GLuint vertex_shader = 0;
static GLuint fragment_shader = 0;
static GLuint shader_program = 0;
static GLint pos_attrib = -1;

static FILE* vsf = NULL;
static FILE* fsf = NULL;
static GLchar* vss = NULL;
static GLchar* fss = NULL;

static void fail_out(const char* error_message, const char* error_detail) {
    fprintf(stderr, "\nError %s: %s\n", error_message, error_detail);
    exit(EXIT_FAILURE);
}

static void fail_out_sdl(const char* error_message) {
    fail_out(error_message, SDL_GetError());
}

static void teardown_opengl(void) {
    if (shader_program != 0) {
        glDeleteProgram(shader_program);
    }
    if (fragment_shader != 0) {
        glDeleteShader(fragment_shader);
    }
    if (vertex_shader != 0) {
        glDeleteShader(vertex_shader);
    }
    if (vbo != 0) {
        glDeleteBuffers(1, &vbo);
    }
}

static void teardown_sdl(void) {
    teardown_opengl();
    if (gl_context != NULL) {
        SDL_GL_DeleteContext(gl_context);
    }
    // ???
    if (window != NULL) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

static void init_sdl(void) {
    atexit(teardown_sdl);
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fail_out_sdl("initialising SDL");
    }
    window = SDL_CreateWindow("cellmate", SDL_WINDOWPOS_CENTERED,
     SDL_WINDOWPOS_CENTERED, CLM_WIN_WIDTH, CLM_WIN_HEIGHT,
     SDL_WINDOW_OPENGL);

    if (window == NULL) {
        fail_out_sdl("creating window");
    }
    /*display_renderer = SDL_GetRenderer(window);
    
    SDL_RendererInfo displayRendererInfo;
    
    SDL_GetRendererInfo(display_renderer, &displayRendererInfo);
    */
    /*if (displayRendererInfo == 0) {
        printf("oh fuck!\n");
        fail_out_sdl("getting renderer info");
    }


    if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0) {// ||
     //(displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
        printf("oh fuck!\n");
        fail_out_sdl("initialising OpenGL");
    }*/

    gl_context = SDL_GL_CreateContext(window);
    if (gl_context == NULL) {
        fail_out_sdl("creating OpenGL context");
    }
    
#ifndef __APPLE__
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fail_out("initialising GLEW", (const char *)glewGetErrorString(err));
    }
#endif

    /*Uint32 flags = SDL_RENDERER_ACCELERATED;
    if () {
        flags |= SDL_RENDERER_PRESENTVSYNC;
    }
    renderer = SDL_CreateRenderer(window, -1, flags);
    if (renderer == NULL) {
        bail_out_sdl("creating renderer");
    }*/
}

static void init_opengl() {
    float vertices[] = {
         -0.5f,  0.0f, 0.5f, // front lower
         0.5f,  0.0f, 0.5f,
         -0.5f,  1.0f, 0.5f,
         -0.5f, 1.0f, 0.5f, // front upper
         0.5f, 0.0f, 0.5f,
         0.5f, 1.0f, 0.5f,
         0.5f, 1.0f, 0.5f, // right top
         0.5f, 0.0f, 1.5f,
         0.5f, 1.0f, 1.5f,
         -0.5f, 1.0f, 0.5f, // left bottom
         -0.5f, 0.0f, 1.5f,
         -0.5f, 0.0f, 0.5f,
         -0.5f, 1.0f, 0.5f, // left top
         -0.5f, 1.0f, 1.5f,
         -0.5f, 0.0f, 1.5f,
         0.5f, 0.0f, 0.5f, // right bottom
         0.5f, 0.0f, 1.5f,
         0.5f, 1.0f, 1.5f,
         -0.5f, 1.0f, 0.5f, // top front
         0.5f, 1.0f, 0.5f,
         0.5f, 1.0f, 1.5f,
         -0.5f, 1.0f, 0.5f, // top back
         0.5f, 1.0f, 1.5f,
         -0.5f, 1.0f, 1.5f,
    };
    GLfloat proj_mat[] = {
        0.28, 0.0, 0.0, 0.0,
        0.0, 0.4, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    GLfloat view_mat[] = {
        /*1.0, 0.0, 0.0, 0.0,
        0.0, cos(M_PI / 8.0), -1.0 * sin(M_PI / 8.0), 0.0,
        0.0, sin(M_PI / 8.0), cos(M_PI / 8.0), 0.0,
        0.0, 0.0, 0.0, 1.0*/
        0.809, 0.0, 0.543, 0.0,
        0.225, 0.924, -0.310, 0.0,
        -0.543, 0.383, 0.748, 0.0,
        0.0, 0.0, 0.0, 1.0
    };

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const GLchar **)(&vss), NULL);
    glCompileShader(vertex_shader);
    GLint status;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        int loglen;
        char logbuffer[1000];
        glGetShaderInfoLog(vertex_shader, sizeof(logbuffer), &loglen, logbuffer);
        fprintf(stderr, "OpenGL Shader Compile Error:\n%.*s", loglen, logbuffer);
        fail_out("error","compiling vertex shader");
    }
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, (const GLchar **)(&fss), NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        int loglen;
        char logbuffer[1000];
        glGetShaderInfoLog(fragment_shader, sizeof(logbuffer), &loglen, logbuffer);
        fprintf(stderr, "OpenGL Shader Compile Error:\n%.*s", loglen, logbuffer);
        fail_out("error","compiling fragment shader");
    }
    shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    //glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        int loglen;
        char logbuffer[1000];
        glGetProgramInfoLog(shader_program, sizeof(logbuffer), &loglen, logbuffer);
        fprintf(stderr, "OpenGL Program Linker Error:\n%.*s", loglen, logbuffer);
    }
    glUseProgram(shader_program);
    if (glGetError() != GL_NO_ERROR) {
        fail_out("using program", "???");
    }
    pos_attrib = glGetAttribLocation(shader_program, "position");
    if (pos_attrib == -1) {
        printf("oh fuck!\n");
        fail_out("error!!","getting attribute location");
    }

    glEnableVertexAttribArray(pos_attrib);

    glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glUniformMatrix4fv(glGetUniformLocation(shader_program, "view_mat"), 1,//6,
     GL_FALSE, view_mat);

    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR) {
        //fail_out("drawing arrays", "???");
        printf("%s\n", gluErrorString(errCode));
    }


    glUniformMatrix4fv(glGetUniformLocation(shader_program, "proj_mat"), 1,//6,
     GL_FALSE, proj_mat);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

static void render() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 24);
    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR) {
        //fail_out("drawing arrays", "???");
        printf("%s\n", gluErrorString(errCode));
    }
    SDL_GL_SwapWindow(window);
    //SDL_RenderPresent(display_renderer);
}


static bool handle_events(void) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            return false;
        } else if (e.type == SDL_KEYDOWN) {
            if ((e.key.keysym.sym == SDLK_q)) {
                return false;
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    /* Due to macro shenanigans in SDL.h, we shouldn't use main(void)
       (breaks Windows compatibility AFAIK); but we're not using the
       arguments, so to suppress compiler warnings, we do this: */
    (void)argc;
    (void)argv;

    // TODO: redundancy below
    vsf = fopen("cube.vesh", "r");
    if (vsf == NULL) {
        fail_out("opening file", "cube.vesh");
    }
    fseek(vsf, 0, SEEK_END);
    long file_size = ftell(vsf);
    rewind(vsf);
    vss = malloc(file_size * sizeof(char));
    fread(vss, sizeof(char), file_size, vsf);
    fclose(vsf);

    fsf = fopen("cube.frsh", "r");
    if (fsf == NULL) {
        fail_out("opening file", "cube.frsh");
    }
    fseek(fsf, 0, SEEK_END);
    file_size = ftell(fsf);
    rewind(fsf);
    fss = malloc(file_size * sizeof(char));
    fread(fss, sizeof(char), file_size, fsf);
    fclose(fsf);
    
    init_sdl();
    init_opengl();
    
    /*
    int major = 0;
    int minor = 0;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    printf("%d, %d\n", major, minor);
    */
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    
    bool running = true;
    while (running) {
        running = handle_events();
        render();
        SDL_Delay(1);
    }
    free(vss);
    
}

// https://www.gnu.org/software/gsl/manual/html_node/Vectors.html#Vectors
// http://comments.gmane.org/gmane.comp.lib.gsl.general/1087
// https://gist.github.com/jmbr/668083
