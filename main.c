#include <assert.h>

#define TZ_WINDOW_INLINE
#include "tz/tz/gl/window.h"

#define TZ_SHADER_INLINE
#include "tz/tz/gl/shader.h"

#define TZ_MAT4_INLINE
#include "tz/tz/math/mat4.h"

static GLuint vao, vbo;
static tz_window g_w;
static tz_shader g_s;

struct camera_t {
	float x, y, z;
	float rho, phi, theta;
};

struct camera_t g_camera;

static GLchar *readfile(const char *name, size_t *len) {
	char *data;
	SDL_RWops *rw = SDL_RWFromFile(name, "rb");
	assert(rw && "failed to read file.");

	*len = SDL_RWseek(rw, 0, SEEK_END);
	data = malloc(*len+1);
	data[*len] = '\0';

	SDL_RWseek(rw, 0, SEEK_SET);
	SDL_RWread(rw, data, *len, 1);
	SDL_RWclose(rw);
	return data;
}

static void create_raymarching_canvas(void) {
	static const GLfloat strip[] = { // use the whole screen
		-1, -1,
		-1,  1,
		 1, -1,
		 1,  1,
	};

	GLchar *vert, *frag;
	size_t len;


	shader_create(&g_s);
	if ((vert = readfile("./tzeda.vert", &len))) {
		shader_compile(&g_s, vert, GL_VERTEX_SHADER);
		free(vert);
	}
	else SDL_assert("shader tzeda.vert not found");

	if ((frag = readfile("./tzeda.frag", &len))) {
		shader_compile(&g_s, frag, GL_FRAGMENT_SHADER);
		free(frag);
	}
	else SDL_assert("shader tzeda.frag not found");

	shader_link(&g_s);
	shader_bind(&g_s);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof strip, strip,  GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

static void setup_cam(struct camera_t *me, tz_mat4 *o) {
	float sp = sin(me->phi),   cp = cos(me->phi),
	      st = sin(me->theta), ct = cos(me->theta),
	      r  = me->rho;
	tz_vec4 eye= tz_vec4_mkp(r*st*cp,  r*ct, r*st*sp);
	tz_vec4 at = tz_vec4_mkp(me->x,   me->y,   me->z);
	tz_vec4 up = tz_vec4_mkv(0, 1, 0);
	tz_mat4_lookat(o, tz_vec4_add(eye,at), at, up);
	//tz_mat4_print(o);
	//printf("\n");
}

static void draw_canvas(void) {
	tz_mat4 MV, P;

	int w,h;
	SDL_GetWindowSize(g_w.win, &w, &h);
	tz_mat4_perspective(&P, 90, w/(float)h, 1, 100);
	setup_cam(&g_camera, &MV);
	glUniformMatrix4fv(0, 1, GL_TRUE, MV.f);
	glUniformMatrix4fv(1, 1, GL_TRUE,  P.f);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static float fps(unsigned tick)
{
	static unsigned n=1;
	return (float)(1000*n++)/tick;
}

int main(int argc, const char *argv[]) {
	g_camera.rho = 10;

	SDL_Init(SDL_INIT_EVERYTHING);
	tz_window_create(&g_w, 800, 800, false);

	create_raymarching_canvas();

	SDL_Event e;
	while (1) {
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_q) {
					goto finalize_program;
				}
				if (e.key.keysym.sym == SDLK_d) { g_camera.x -= 1; }
				if (e.key.keysym.sym == SDLK_a) { g_camera.x += 1; }
				if (e.key.keysym.sym == SDLK_s) { g_camera.z -= 1; }
				if (e.key.keysym.sym == SDLK_w) { g_camera.z += 1; }
				if (e.key.keysym.sym == SDLK_r) { g_camera.y -= 1; }
				if (e.key.keysym.sym == SDLK_f) { g_camera.y += 1; }
				break;
			case SDL_MOUSEMOTION: {
				int x, y;
				if (SDL_GetMouseState(&x, &y)) {
					g_camera.phi   -= M_PI*e.motion.xrel/800.0;
					g_camera.theta += M_PI*e.motion.yrel/800.0;
				}
				break;
			}
			case SDL_MOUSEWHEEL: {
				float inc = SDL_GetModState() ? 0.1 : 1;
				g_camera.rho += inc*e.wheel.y;
				printf("%f\n", g_camera.rho);
				printf("%d\n", e.wheel.y);
				break;
			}}
		}

		draw_canvas();
		tz_window_flip(&g_w);
		printf("fps: %f\n", fps(SDL_GetTicks()));
	}

finalize_program:
	shader_destroy(&g_s);

	SDL_Quit();
	return 0;
}
