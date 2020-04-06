#ifndef GLCANVAS_H_
#define GLCANVAS_H_

#ifndef __APPLE__
    #include <GL/glew.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_opengl.h>

#ifdef __APPLE__
    #include <OpenGL/glu.h>
#else
    #include <glu.h>
#endif

#include "canvas.hpp"
#include "battlefieldlayout.hpp"

namespace neurocid {

using std::string;

class BattleField;
class Facility;
class Object;

class SDL2Canvas : public Canvas {
private:
	Coord width_;
	Coord height_;
	BattleFieldLayout bfl_;
	SDL_Window* window_ = NULL;
	SDL_GLContext gl_context_ = NULL;
	GLuint vbo_ = 0;
	GLuint vertex_shader_ = 0;
	GLuint fragment_shader_ = 0;
	GLuint shader_program_ = 0;
	GLint pos_attrib_ = -1;
	GLint shipLocationUL = 0;

	string vss_;
	string fss_;

	string loadResource(const string& filename);
	void fail_out(const char* error_message, const char* error_detail);
	void fail_out_sdl(const char* error_message);
	void teardown_opengl();
	void teardown_sdl();
	void init_sdl();
	void init_opengl();
public:
	SDL2Canvas(Coord width, Coord height, BattleFieldLayout& bfl);
	virtual ~SDL2Canvas();

	virtual void zoomIn() override;
	virtual void zoomOut() override;
	virtual void left() override;
	virtual void right() override;
	virtual void up() override;
	virtual void down() override;

	virtual void update() override;
	virtual void clear() override;
	virtual void render(BattleField& field) override;
	virtual void reset() override;

	void resize(Coord width, Coord height) override {
		width_ = width;
		height_ = height;
	}

	Coord width() const override;
	Coord height() const override;

	SDL_Surface* getSurface() const override {
		return NULL;
	}
};
}

#endif
