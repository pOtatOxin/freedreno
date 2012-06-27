/*
 * Copyright (c) 2012 Rob Clark <robdclark@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>

#include "freedreno.h"
#include "redump.h"

int main(int argc, char **argv)
{
	struct fd_state *state;
	struct fd_surface *surface;

	float vertices[] = {
			+0.0, +0.8, 0.0,
			-0.8, +0.4, 0.0,
			-0.6, -0.5, 0.0,
			+0.0, -0.8, 0.0,
			+0.6, -0.5, 0.0,
			+0.8, +0.4, 0.0,
	};

	float colors[] = {
			1.0, 1.0, 1.0, 1.0,
			1.0, 0.0, 0.0, 1.0,
			1.0, 1.0, 0.0, 1.0,
			0.0, 1.0, 0.0, 1.0,
			0.0, 1.0, 1.0, 1.0,
			0.0, 0.0, 1.0, 1.0,
	};

#if 0
	const char *vertex_shader_source =
		"attribute vec4 aPosition;    \n"
		"attribute vec4 aColor;       \n"
		"                             \n"
		"varying vec4 vColor;         \n"
		"                             \n"
		"void main()                  \n"
		"{                            \n"
		"    vColor = aColor;         \n"
		"    gl_Position = aPosition; \n"
		"}                            \n";
	const char *fragment_shader_source =
		"precision mediump float;     \n"
		"                             \n"
		"varying vec4 vColor;         \n"
		"                             \n"
		"void main()                  \n"
		"{                            \n"
		"    gl_FragColor = vColor;   \n"
		"}                            \n";
#else
	const uint32_t vertex_shader_bin[] = {
			0x00052003, 0x00001000, 0xc2000000, 0x00001005,
			0x00001000, 0xc4000000, 0x00001006, 0x00002000,
			0x00000000, 0x1b481000, 0x00263688, 0x00000010,
			0x11482000, 0x40393a88, 0x0000000c, 0x140f803e,
			0x00000000, 0xe2020200, 0x140f8000, 0x00000000,
			0xe2010100,
	};
	const uint32_t fragment_shader_bin[] = {
			0x00000000, 0x1001c400, 0x20000000, 0x140f8000,
			0x00000000, 0xe2000000,
	};
#endif

	DEBUG_MSG("----------------------------------------------------------------");
	RD_START("fd-triangle-smoothed", "");

	state = fd_init();
	if (!state)
		return -1;

	surface = fd_surface_new(state, 256, 256);
	if (!surface)
		return -1;

	fd_make_current(state, surface);

	fd_vertex_shader_attach_bin(state, vertex_shader_bin,
			sizeof(vertex_shader_bin));
	fd_fragment_shader_attach_bin(state, fragment_shader_bin,
			sizeof(fragment_shader_bin));

	fd_link(state);

	fd_clear(state, 0xff000000);

	fd_attribute_pointer(state, "aPosition", 3, 6, vertices);
	fd_attribute_pointer(state, "aColor", 4, 6, colors);

	fd_draw_arrays(state, GL_TRIANGLE_FAN, 0, 6);

	fd_flush(state);

	fd_dump_bmp(surface, "fan-smoothed.bmp");

	fd_fini(state);

	RD_END();

	return 0;
}
