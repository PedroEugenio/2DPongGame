// SummerGame.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <Windows.h>

#include <glut.h>
#include <gl\gl.h>
#include <gl\glu.h>

#include <math.h>

#include "RgbImage.h" //background

#include <mmsystem.h>	//sound


#define PI    3.1415927

// keycodes
#define VK_A 0x41
#define VK_Z 0x5A

//window variables
int width = 800;
int height = 500;
int frame_rate = 1000 / 60; //60 fps

//score variables
int score_player1 = 0;
int score_player2 = 0;
char buffer1[15];
char buffer2[5];

//rackets settings
int racket_width = 15;
int racket_height = 80;
int racket_speed = 6;

// left racket position
float racket_left_x = 20.0f;
float racket_left_y = (height/2) - (racket_height / 2);

// right racket position
float racket_right_x = width - racket_width - 30;
float racket_right_y = (height/2) - (racket_height/2);

// ball
float ball_pos_x = width / 2;
float ball_pos_y = height / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
int ball_size = 10;
int ball_speed = 10;


// Functions Declaration
///////////////////////////////////////////////////////////////////////////////////////////
const char *score_string(int player1, int player2);
void draw_scene(void);
void update(int value);
void scene_2d(int width, int height);
void draw_text(float x, float y, const char *text);
void drawRect(float x, float y, float width, float height);
void keyboard();
void ball();

void vec2_norm(float &x, float &y);

//////////////////////////////////////////////////////////////////////////////////////////
// Read a texture map from a BMP bitmap file.

char* filename = "beach2.bmp";

void loadTextureFromFile(char *filename)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
	glEnable(GL_DEPTH_TEST);

	RgbImage theTexMap(filename);

	// Pixel alignment: each row is word aligned (aligned to a 4 byte boundary)
	// Therefore, no need to call glPixelStore( GL_UNPACK_ALIGNMENT, ... );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, theTexMap.GetNumCols(),
		theTexMap.GetNumRows(), GL_RGB, GL_UNSIGNED_BYTE,
		theTexMap.ImageData());
}

const char *score_string(int player1, int player2){ //convert integers scores into chars
	_itoa(score_player1, buffer1, 10);
	_itoa(score_player2, buffer2, 10);
	strcat(buffer1, " : ");
	strcat(buffer1, buffer2);

	return buffer1;
}


void draw_scene(void){
	// clean window
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//draw scene
	glClearColor(0.3, 0.3, 1.0, 0.0);
	
	drawRect(ball_pos_x, ball_pos_y, ball_size, ball_size); //ball
	drawRect(racket_left_x, racket_left_y, racket_width, racket_height); //left rackect
	drawRect(racket_right_x, racket_right_y, racket_width, racket_height); //right racket
	draw_text(width / 2 - 10, height - 30, score_string(score_player1, score_player2)); //score

	glClear(GL_COLOR_BUFFER_BIT);

	//Background
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glLoadIdentity();

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(0.0, height, 0.0);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(width, height, 0.0);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(width, 0.0, 0.0);

	glEnd();
	glutSwapBuffers();
	glDisable(GL_TEXTURE_2D);
}


void keyboard() {
	// left racket
	if (GetAsyncKeyState(VK_A) && racket_left_y <=(height-racket_height)) 
		racket_left_y += racket_speed;
	if (GetAsyncKeyState(VK_Z) && racket_left_y >= 0)
		racket_left_y -= racket_speed;

	// right racket
	if (GetAsyncKeyState(VK_UP) && racket_right_y <= (height - racket_height))
		racket_right_y += racket_speed;
	if (GetAsyncKeyState(VK_DOWN) && racket_right_y >= 0)
		racket_right_y -= racket_speed;
}


void update(int value) {
	keyboard();

	ball();

	glutTimerFunc(frame_rate, update, 0);

	//Redisplay frame
	glutPostRedisplay();
}

void scene_2d(int width, int height) {  //Configurations for 2D
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void draw_text(float x, float y, const char *text) {
	int len = strlen(text);
	glRasterPos2f(x, y);
	for (int i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
	}
}

void drawRect(float x, float y, float width, float height) {
	glBegin(GL_QUADS);
	glVertex2f(x, y);
	glVertex2f(x + width, y);
	glVertex2f(x + width, y + height);
	glVertex2f(x, y + height);
	glEnd();
}


void ball() {
	// fly a bit
	ball_pos_x += ball_dir_x * ball_speed;
	ball_pos_y += ball_dir_y * ball_speed;

	// hit by left racket
	if (ball_pos_x <= racket_left_x + racket_width && ball_pos_x >= racket_left_x 
		&& ball_pos_y <= racket_left_y + racket_height 
		&& ball_pos_y >= racket_left_y) {
		// set fly direction depending on where it hit the racket
		// (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
		float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
		ball_dir_x = fabs(ball_dir_x); // force it to be positive
		ball_dir_y = t;
	}

	// hit by right racket
	if (ball_pos_x >= racket_right_x && ball_pos_x <= (racket_right_x + racket_width +  ball_size)
		&& ball_pos_y <= (racket_right_y + racket_height) 
		&& ball_pos_y >= racket_right_y) {
		// set fly direction depending on where it hit the racket
		// (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
		float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
		ball_dir_x = -fabs(ball_dir_x); // force it to be negative
		ball_dir_y = t;
	}

	// hit left wall
	if (ball_pos_x < 0) {
		++score_player2;
		ball_pos_x = width / 2;
		ball_pos_y = height / 2;
		ball_dir_x = fabs(ball_dir_x); // force it to be positive
		ball_dir_y = 0;
	}

	// hit right wall
	if (ball_pos_x > width) {
		++score_player1;
		ball_pos_x = width / 2;
		ball_pos_y = height / 2;
		ball_dir_x = -fabs(ball_dir_x); // force it to be negative
		ball_dir_y = 0;
	}

	// hit top wall
	if (ball_pos_y > height - (2*ball_size)) {
		ball_dir_y = -fabs(ball_dir_y); // force it to be negative
	}

	// hit bottom wall
	if (ball_pos_y < 0) {
		ball_dir_y = fabs(ball_dir_y); // force it to be positive
	}

	// make sure that length of dir stays at 1
	vec2_norm(ball_dir_x, ball_dir_y);
}

void vec2_norm(float& x, float &y) {
	// sets a vectors length to 1 (which means that x + y == 1)
	float length = sqrt((x * x) + (y * y));
	if (length != 0.0f) {
		length = 1.0f / length;
		x *= length;
		y *= length;
	}
}

int main(int argc, char** argv)
{
	// initialize GLUT
	glutInit(&argc, argv);
	FreeConsole();
	// create GLUT window
	glutInitWindowSize(width, height);
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - 500) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - 500) / 2);
	glutCreateWindow("Summer Game");
	// Display
	loadTextureFromFile(filename);
	glutDisplayFunc(draw_scene);
	glutTimerFunc(frame_rate, update, 0);

	//background sound
	sndPlaySoundA("beach.wav", SND_ASYNC | SND_LOOP); //SND_ASYNC | SND_LOOP to reproduce as background sound

	// setup 2d scene
	scene_2d(width, height);
	
	// waiting for events
	glutMainLoop();

	return EXIT_SUCCESS;
}

