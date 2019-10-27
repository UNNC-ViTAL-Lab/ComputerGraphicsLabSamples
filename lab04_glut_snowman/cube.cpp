#include <stdio.h>  
#define GLUT_DISABLE_ATEXIT_HACK  
#include <gl/glut.h>  
  
GLfloat AngleX;  
GLfloat AngleY;  
  
void display(void)  
{  
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
    glMatrixMode(GL_MODELVIEW);  
    glLoadIdentity();  
	glRotatef(AngleX,1.0f,0.0f,0.0f);  
    glRotatef(AngleY,0.0f,1.0f,0.0f);  

    // Multicolored 每 FRONT
	glBegin(GL_POLYGON);
		glNormal3d(0.0f, 0.0f, 1.0f);
		glColor3f( 1.0f, 0.0f, 0.0f ); // red
		glVertex3f( 1.0f, -1.0f, 1.0f ); 
		glColor3f( 0.0f, 1.0f, 0.0f ); // green    
		glVertex3f( 1.0f, 1.0f, 1.0f ); 
		glColor3f( 0.0f, 0.0f, 1.0f ); // blue  
		glVertex3f( -1.0f, 1.0f, 1.0f ); 
		glColor3f( 1.0f, 0.0f, 1.0f );     
		glVertex3f( -1.0f, -1.0f, 1.0f);// purple 
    glEnd(); 
    // White 每 BACK
	glBegin(GL_POLYGON);  
		glNormal3d(0.0f, 0.0f, -1.0f);
		glColor3f(   1.0f,  1.0f, 1.0f );
		glVertex3f(  1.0f, -1.0f, -1.0f );
		glVertex3f(  1.0f, 1.0f, -1.0f );
		glVertex3f( -1.0f, 1.0f, -1.0f );
		glVertex3f( -1.0f, -1.0f, -1.0f );
    glEnd();  
	// Blue 每 TOP
	glBegin(GL_POLYGON);
		glNormal3d(0.0f, 1.0f, 0.0f);
		glColor3f( 0.0f, 0.0f, 1.0f );
		glVertex3f( 1.0f, 1.0f, 1.0f );
		glVertex3f( 1.0f, 1.0f, -1.0f );
		glVertex3f( -1.0f, 1.0f, -1.0f );
		glVertex3f( -1.0f, 1.0f, 1.0f );
    glEnd(); 
	// Red 每 BOTTOM
    glBegin(GL_POLYGON); 
		glNormal3d(0.0f, -1.0f, 0.0f);
		glColor3f( 1.0f,  0.0f,  0.0f );
		glVertex3f( -1.0f, -1.0f, 1.0f );
		glVertex3f( -1.0f, -1.0f, -1.0f );
		glVertex3f( 1.0f, -1.0f, -1.0f );
		glVertex3f( 1.0f, -1.0f, 1.0f );
    glEnd();  
	// Green 每 LEFT  
    glBegin(GL_POLYGON);
		glNormal3d(-1.0f, 0.0f, 0.0f);
		glColor3f(   0.0f,  1.0f,  0.0f );
		glVertex3f( -1.0f, -1.0f, 1.0f );
		glVertex3f( -1.0f, 1.0f, 1.0f );
		glVertex3f( -1.0f, 1.0f, -1.0f );
		glVertex3f( -1.0f, -1.0f, -1.0f );
    glEnd();  
	// Purple 每 RIGH
    glBegin(GL_POLYGON);  
		glNormal3d(1.0f, 0.0f, 0.0f);
		glColor3f(  1.0f,  0.0f,  1.0f );
		glVertex3f( 1.0f, -1.0f, -1.0f );
		glVertex3f( 1.0f, 1.0f, -1.0f );
		glVertex3f( 1.0f, 1.0f, 1.0f );
		glVertex3f( 1.0f, -1.0f, 1.0f );
    glEnd();
    glutSwapBuffers();  
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

void main(int argc,char* argv[])  
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
