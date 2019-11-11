#include <stdio.h>
#include <math.h>
#define GLUT_DISABLE_ATEXIT_HACK
#include <gl/glut.h>

GLfloat AngleX;
GLfloat AngleY;

float t = 0;

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    glLoadIdentity();
    glTranslatef(0, -0.5f, 0);
    glutSolidSphere(0.75, 25, 25);

    glLoadIdentity();
    glTranslatef(0, 0.75, 0);
    glutSolidSphere(0.5, 25, 25);

    glLoadIdentity();
    // world
    glTranslatef(0, -0.5f, 0);
    // cube parent
    glRotatef(sin(t) * 20 + 30, 0, 0, 1);
    // cube coordinate system
    glScalef(1, 0.15f, 0.15f);
    glTranslatef(0.7f, 0, 0);
    glutSolidCube(1);

    glutSwapBuffers();

    t += 0.1f;

    glutPostRedisplay();
}

void reshape(int w, int h)
{
    GLfloat aspect = (GLfloat)w / (GLfloat)h;
    GLfloat nRange = 2.0f;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w<=h)
    {   glOrtho(-nRange, nRange, -nRange * aspect, nRange * aspect, -nRange, nRange);  }
    else
    {  glOrtho(-nRange, nRange, -nRange / aspect, nRange / aspect, -nRange, nRange);  }
}

void init()
{
    AngleX = 30.0f;
    AngleY = -45.0f;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DITHER);
    glShadeModel(GL_SMOOTH);
}

int main(int argc,char* argv[])
{
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500,500);
    glutCreateWindow("OpenGL Cube");
   	init();
    glutDisplayFunc(display);
	glutReshapeFunc(reshape);
    glutMainLoop();
}
