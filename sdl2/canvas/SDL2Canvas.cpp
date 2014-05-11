#include "SDL2Canvas.hpp"
#include "battlefield.hpp"
#include <fstream>
#include <sstream>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


/*static bool handle_events(void) {
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
}*/

namespace neurocid {

using std::ifstream;
using std::stringstream;

string SDL2Canvas::loadResource(const string& filename) {
	ifstream inFile(filename);
	stringstream strStream;
	strStream << inFile.rdbuf();
	return strStream.str();
}

void SDL2Canvas::fail_out(const char* error_message, const char* error_detail) {
    fprintf(stderr, "\nError %s: %s\n", error_message, error_detail);
    exit(EXIT_FAILURE);
}

void SDL2Canvas::fail_out_sdl(const char* error_message) {
    fail_out(error_message, SDL_GetError());
}

void SDL2Canvas::teardown_opengl() {
    if (shader_program_ != 0) {
        glDeleteProgram(shader_program_);
    }
    if (fragment_shader_ != 0) {
        glDeleteShader(fragment_shader_);
    }
    if (vertex_shader_ != 0) {
        glDeleteShader(vertex_shader_);
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
    }
}

void SDL2Canvas::teardown_sdl() {
    teardown_opengl();
    if (gl_context_ != NULL) {
        SDL_GL_DeleteContext(gl_context_);
    }
    // ???
    if (window_ != NULL) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

void SDL2Canvas::init_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fail_out_sdl("initialising SDL");
    }
    window_ = SDL_CreateWindow("cellmate", SDL_WINDOWPOS_CENTERED,
     SDL_WINDOWPOS_CENTERED, width_, height_,
     SDL_WINDOW_OPENGL);

    if (window_ == NULL) {
        fail_out_sdl("creating window");
    }

    gl_context_ = SDL_GL_CreateContext(window_);
    if (gl_context_ == NULL) {
        fail_out_sdl("creating OpenGL context");
    }
    
#ifndef __APPLE__
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fail_out("initialising GLEW", (const char *)glewGetErrorString(err));
    }
#endif
}

void SDL2Canvas::init_opengl() {
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
        0.01, 0.0, 0.0, 0.0,
        0.0, 0.01, 0.0, 0.0,
        0.0, 0.0, 0.01, 0.0,
        0.0, 0.0, 0.0, 1.0
    };

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
    const char * vssText = vss_.c_str();

    glShaderSource(vertex_shader_, 1, (const GLchar **)(&vssText), NULL);
    glCompileShader(vertex_shader_);
    GLint status;
    glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        int loglen;
        char logbuffer[1000];
        glGetShaderInfoLog(vertex_shader_, sizeof(logbuffer), &loglen, logbuffer);
        fprintf(stderr, "OpenGL Shader Compile Error:\n%.*s", loglen, logbuffer);
        fail_out("error","compiling vertex shader");
    }
    fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
    const char * fssText = fss_.c_str();
    glShaderSource(fragment_shader_, 1, (const GLchar **)(&fssText), NULL);
    glCompileShader(fragment_shader_);
    glGetShaderiv(fragment_shader_, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        int loglen;
        char logbuffer[1000];
        glGetShaderInfoLog(fragment_shader_, sizeof(logbuffer), &loglen, logbuffer);
        fprintf(stderr, "OpenGL Shader Compile Error:\n%.*s", loglen, logbuffer);
        fail_out("error","compiling fragment shader");
    }
    shader_program_ = glCreateProgram();

    glAttachShader(shader_program_, vertex_shader_);
    glAttachShader(shader_program_, fragment_shader_);
    //glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shader_program_);
    glGetProgramiv(shader_program_, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        int loglen;
        char logbuffer[1000];
        glGetProgramInfoLog(shader_program_, sizeof(logbuffer), &loglen, logbuffer);
        fprintf(stderr, "OpenGL Program Linker Error:\n%.*s", loglen, logbuffer);
    }
    glUseProgram(shader_program_);
    if (glGetError() != GL_NO_ERROR) {
        fail_out("using program", "???");
    }
    pos_attrib_ = glGetAttribLocation(shader_program_, "position");
    if (pos_attrib_ == -1) {
        printf("oh fuck!\n");
        fail_out("error!!","getting attribute location");
    }

    glEnableVertexAttribArray(pos_attrib_);

    glVertexAttribPointer(pos_attrib_, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "view_mat"), 1,//6,
     GL_FALSE, view_mat);

    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR) {
        //fail_out("drawing arrays", "???");
        printf("%s\n", gluErrorString(errCode));
    }


    glUniformMatrix4fv(glGetUniformLocation(shader_program_, "proj_mat"), 1,//6,
     GL_FALSE, proj_mat);

    shipLocationUL = glGetUniformLocation(shader_program_, "loc");

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

SDL2Canvas::SDL2Canvas(Coord width, Coord height, BattleFieldLayout& bfl) :
		width_(width),
		height_(height),
		bfl_(bfl) {
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
	vss_ = loadResource("cube.vesh");
	fss_ = loadResource("cube.frsh");
    init_sdl();
    init_opengl();
}

SDL2Canvas::~SDL2Canvas() {
}


void SDL2Canvas::zoomIn()  {
}

void SDL2Canvas::zoomOut()  {
}

void SDL2Canvas::left()  {
}

void SDL2Canvas::right()  {
}

void SDL2Canvas::up()  {
}

void SDL2Canvas::down()  {
}


void SDL2Canvas::update()  {
}

void SDL2Canvas::clear()  {
}

void SDL2Canvas::render(BattleField& field)  {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    for(Population& pop : field.teams_) {
		for(Ship& ship : pop) {
			if(!ship.dead_) {
				// draw ship
				glUniform3f(shipLocationUL, ship.loc_.x_, ship.loc_.y_, 0);
			    glDrawArrays(GL_TRIANGLES, 0, 24);
			}

			if(ship.explode_) {
				ship.explode_ = false;
			}

			if(ship.crashed_) {
				ship.crashed_ = false;
			}

			for(Projectile* p : ship.projectiles_) {
				if(p->explode_) {
					p->explode_ = false;
				}

				if(!p->dead_) {
					// draw projectiles
				}
			}
		}

		for(Facility& f : pop.facilities_) {
			if(f.captured_) {
				f.captured_ = false;
			}
		}
	}

    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR) {
        //fail_out("drawing arrays", "???");
        printf("%s\n", gluErrorString(errCode));
    }
    SDL_GL_SwapWindow(window_);
}

void SDL2Canvas::reset()  {
}


Coord SDL2Canvas::width() const  {
	return width_;
}

Coord SDL2Canvas::height() const  {
	return height_;
}

}

