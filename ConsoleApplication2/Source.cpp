#include <iostream>
#include <math.h>
#include <vector>
#include <time.h>
#include "glut.h"
using namespace std;
//the number of particles
int particleCount = 100;
//global mouse stuff
int mState[20] = { GLUT_UP };
int button;
float posX;
float posY;
//structure which stores relevant variables, radius, velocity, position and colour
struct Point
{
	double particleRadius = 0.5;
	float velocityX, velocityY;
	float x, y;
	unsigned char r, g, b, a;
};

//a vector of Point structs called points which stores the information 
std::vector< Point > points;

//applies forces
void applyForce(int index, float forceX, float forceY)
{
	points[index].velocityX += forceX;
	points[index].velocityY += forceY;
	points[index].x += forceX;
	points[index].y += forceY;
}

//does the distance calculation needed to detect collision(not used at all currently)
double distanceCalculate(double x1, double y1, double x2, double y2)
{
	double x = x1 - x2; //calculating number to square in next step
	double y = y1 - y2;
	double distance;

	distance = (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);       //calculating Euclidean distance
	distance = sqrt(distance);

	return distance;
}

//function to push balls towards mouse
void mouseCallBack(int button, int state, int mouseX, int mouseY)
{
	//janky fix to convert mouse coordinates to screen coordinates 
	mouseX = mouseX * 100 / 900 - 50;
	mouseY = -mouseY * 100 / 900 + 50;
	posX = mouseX;
	posY = mouseY;
	//array for the button state
	mState[button] = state;
}

//deals with left click movement
void mouseControl()
{
	
	if (mState[0] == GLUT_UP)
	{
		//if button state is up do nothing
	}
	else
	{// state = GLUT_DOWN
		for (int i = 0; i<particleCount; i++)
		{
			float distanceX = points[i].x - posX;
			float distanceY = points[i].y - posY;
			//move position and velocity by a small fraction of distance
			applyForce(i, -distanceX*0.001, -distanceY*0.001);
			//applyForce(i, -distanceX*0.01, -distanceY*0.01);
		}
	}
}

//callback function for the particles following the mouse
void mouseMotionCallBack(int mouseX, int mouseY)
{
	mouseX = mouseX * 100 / 900 - 50;
	mouseY = -mouseY * 100 / 900 + 50;
	posX = mouseX;
	posY = mouseY;
	
}
//function that does all the collision and most of the distance calculations. a bit bloated but works fine.
void collisionCheck()
{
	//this loop makes the initial colour of all the particles white. (1,1,1)
	for (int i = 0; i<particleCount; i++)
	{
		points[i].r = 1;
		points[i].g = 1;
		points[i].b = 1;
	}
	//this loop and sequence of if statements deals with collisions against the edge of the screen.
	for (int i = 0; i<particleCount; i++)
	{
		if (points[i].x >= 50)
		{
			points[i].velocityX *= -0.9;
			points[i].x = 50;
		}
		if (points[i].x <= -50)
		{
			points[i].velocityX *= -0.9;
			points[i].x = -50;
		}
		if (points[i].y >= 50)
		{
			points[i].velocityY *= -0.9;
			points[i].y = 50;
		}
		if (points[i].y <= -50)
		{
			points[i].velocityY *= -0.9;
			points[i].y = -50;
		}
	}
	
	//this nested loop deals with collision checking and some basic maths
	for (size_t i = 0; i < particleCount-1; i++)
	{
		for (size_t j = i+1; j < particleCount; j++)
		{
			//check j != to i 
			//check pairs of objects
			
			float rads = points[i].particleRadius + points[j].particleRadius;
			float distanceX = (points[i].x - points[j].x);
			float distanceY = (points[i].y - points[j].y);
			float distanceSquared = (distanceX*distanceX) + (distanceY*distanceY);
			
			//if the distance between the particles is less than or equal to the combined radius apply forces and turn particles red.
			if (distanceSquared<= rads*rads)
				//distanceCalculate(points[i].x, points[i].y, points[j].x, points[j].y) <=
			{
				float distance = sqrt(distanceSquared);
				float overlap = rads - distance;
				float normalX = distanceX / distance;
				float normalY = distanceY / distance;
				float forceX = normalX * overlap * 0.5;
				float forceY = normalY * overlap * 0.5;

				applyForce(i, forceX, forceY);
				points[i].r = 1;
				points[i].g = 0;
				points[i].b = 0;
				applyForce(j, -forceX, -forceY);
				points[j].r = 1;
				points[j].g = 0;
				points[j].b = 0;
			}
		}
	}
}

//opengl stuff
void changeSize(int w, int h) 
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if (h == 0)
		h = 1;

	float ratio = w * 1.0 / h;

	// Use the Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set the correct perspective.
	gluPerspective(45, ratio, 1, 100);

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);

}
//opengl rendering stuff. updates 60 times a second(or something).
void renderScene(void) 
{
	
	mouseControl();
	
	
	//gives the particles their initial position and velocity.
	for (int i = 0; i<particleCount; i++)
	{
		points[i].x += points[i].velocityX;
		points[i].y += points[i].velocityY;

		//adds drag (velocityX *0.999)(velocityY*0.999) per frame
		points[i].velocityX *= 0.999;
		points[i].velocityY *= 0.999;
	}
	//the collision checking function
	collisionCheck();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50, 50, -50, 50, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// draws the particles onto the screen particles are made of glutSolidSpheres
	for (size_t i = 0; i < particleCount; i++)
	{
		//push current matrix onto the stack
		glPushMatrix();
		//moves (places) the particles at the random coordinates provided by the rand function(in main)
		glTranslated(points[i].x, points[i].y, -1);
		//generic colour provided elsewhere
		glColor3d(points[i].r, points[i].g, points[i].b);
		//particles of number particleCount radius = particleRadius
		glutSolidSphere(points[i].particleRadius, 50, 50);
		//pop the current matrix off the stack
		glPopMatrix();
	}
	//flushes the buffer
	glFlush();
	//swaps between the front buffer and the back buffer
	glutSwapBuffers();
	//updates the display
	glutPostRedisplay();
}
//main entry point *snigger*
int main(int argc, char **argv) {
	//init randomizer
	srand(static_cast <unsigned> (time(0)));
	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	//window position
	glutInitWindowPosition(100, 100);
	//window size and name
	glutInitWindowSize(900, 900);
	glutCreateWindow("HPCParticles");

	// register callback functions
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutMouseFunc(mouseCallBack);
	glutMotionFunc(mouseMotionCallBack);
	// generate random locations and random starting velocities for the particles
	for (size_t i = 0; i < particleCount; i++)
	{
		Point pt;
		pt.velocityX = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100.0)) - 50.0)*0.01;
		pt.velocityY = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100.0)) - 50.0)*0.01;
		pt.x = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100.0)) - 50.0;
		pt.y = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 100.0)) - 50.0;
		points.push_back(pt);
	}
	
	// enter GLUT event processing loop
	glutMainLoop();

	return 1;
}