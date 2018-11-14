/*
 * Copyright (c) 2014 University of Michigan, Ann Arbor.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of Michigan, Ann Arbor. The name of the University
 * may not be used to endorse or promote products derived from this
 * software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Author: Sugih Jamin
*/
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

//#define GLFW_DLL
#include <glfw3.h>

#define NENDS 2           /* number of end "points" to draw */

GLFWwindow* wd;           /* window desciptor/handle */
GLint width = 640;
GLint height = 480;         /* initial window width and height */
int fbwidth, fbheight;    /* framebuffer width and height (Retina display) */

GLfloat ends[NENDS][2];   /* array of 2D points */

/* function where all the actual drawing happens */
void
disp()
{
	int i;

	/* color buffer must be cleared each time */
	glClear(GL_COLOR_BUFFER_BIT);

	/* draw the line using the preset color and line thickness  */
	glBegin(GL_LINES);
	for (i = 0; i < NENDS; ++i) {
		glVertex2fv(ends[i]);
	}
	glEnd();

	/* GLFW is ALWAYS double buffered; will call glFlush() */
	glfwSwapBuffers(wd);

	return;
}

/* Callback functions for GLFW */

void
init_data()
{
	/* initialize line's end points to be fractions of window dimensions */
	ends[0][0] = (float) 0.25*fbwidth;  /* (0,0) is the lower left corner */
	ends[0][1] = (float) 0.75*fbheight;
	ends[1][0] = (float) 0.75*fbwidth;
	ends[1][1] = (float) 0.25*fbheight;

	return;
}

/* Called when framebuffer is resized, e.g., when window is resized
 * OR when the same size buffer is moved across Retina and non-Retina displays
 * when running Mac OS X.
 * NOT called automatically when window is first created.
 * Called by GLFW BEFORE reshape().
*/
void
fbreshape(GLFWwindow *wd, int w, int h)
{
	/* save new framebuffer dimensions */
	fbwidth = w;
	fbheight = h;

	/* do an orthographic parallel projection with the view volume
	   set to first quadrant, fixed to the initial window dimension */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (float)fbwidth, 0.0, (float)fbheight, -1.f, 1.f);

	/* Tell OpenGL to use the whole window for drawing.
	   Note that we don't resize the view volume, so
	   the viewport will show the whole view volume
	   shrunk/stretched to fit the current view port. */
	glViewport(0, 0, (GLsizei)fbwidth, (GLsizei)fbheight);

	init_data();

	return;
}

/* Called when window is resized.
 * NOT called automatically when window is first created.
 * Called by GLFW AFTER fbreshape().
*/
void
reshape(GLFWwindow *wd, int w, int h)
{
	/* save new screen dimensions */
	width = w;
	height = h;

	return;
}

/* Called when window is closed,
   also when 'q' or ESC is hit. */
void
quit(GLFWwindow *wd)
{
	glfwDestroyWindow(wd);
	glfwTerminate();
	exit(0);
}

void
kbd(GLFWwindow* wd, int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE) { // function is called first on GLFW_PRESS.
		return;
	}

	switch (key) {
	case GLFW_KEY_ESCAPE:
		quit(wd);
		break;
	default:
		break;
	}

	return;
}

void
charhd(GLFWwindow* wd, unsigned int key)
{
	switch (key) {
	case 'q':
		quit(wd);
		break;
	default:
		break;
	}

	return;
}

void
err(int errcode, const char* desc)
{
	fprintf(stderr, "%d: %s\n", errcode, desc);

	return;
}

void
initgl()
{
	/* clear color buffer to white */
	glClearColor(1.0, 1.0, 1.0, 0.0);

	/* set the line color to black and thickness to 3 pixels */
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(3.0);

	/* no transformation */
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return;
}

int
main(int argc, char *argv[])
{
	glfwSetErrorCallback(err);
	if (!glfwInit()) {
		exit(1);
	}

	/* create the window and its associated OpenGL context */
	wd = glfwCreateWindow(width, height, "Experiment with line drawing",
		NULL, NULL);
	if (!wd) {
		glfwTerminate();
		exit(1);
	}
	/* make the window's context the current context */
	glfwMakeContextCurrent(wd);

	/* With Retina display on Mac OS X, GLFW's FramebufferSize
	   is different from WindowSize */
	glfwGetFramebufferSize(wd, &fbwidth, &fbheight);

	/* shape view port */
	fbreshape(wd, fbwidth, fbheight);

	/* --- register callbacks with GLFW --- */

	/* register function to handle window resizes */
	/* With Retina display on Mac OS X GLFW's FramebufferSize
	   is different from WindowSize */
	glfwSetFramebufferSizeCallback(wd, fbreshape);
	glfwSetWindowSizeCallback(wd, reshape);

	/* register function to handle window close */
	glfwSetWindowCloseCallback(wd, quit);

	/* register function to handle keyboard input */
	glfwSetKeyCallback(wd, kbd);      // general keyboard input
	glfwSetCharCallback(wd, charhd);  // simpler specific character handling

	/* --- draw line --- */

	initgl();

	disp();
	do {
		glfwWaitEvents();
	} while (!glfwWindowShouldClose(wd));

	exit(0);
}
