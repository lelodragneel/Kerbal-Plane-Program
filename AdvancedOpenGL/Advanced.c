//#include <freeglut.h>
//
//GLint windowWidth = 400;
//GLint windowHeight = 400;
//GLint mousePressed = 0;
//GLfloat mouseX, mouseY;
//
//void myIdle()
//{
//	glutPostRedisplay();
//}
//
//void initializeGL()
//{
//	glClearColor(0.0, 0.0, 0.0, 1.0);
//
//	glEnable(GL_DEPTH_TEST);
//
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	glEnable(GL_LINE_SMOOTH);
//
//	glPointSize(8.0);
//
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluOrtho2D(0.0, 1.0, 0.0, 1.0);
//}
//
//void myDisplay()
//{
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	
//	if (mousePressed)
//	{
//		glColor4f(0.0, 1.0, 0.0, 1.0);
//		glBegin(GL_POINTS);
//			glVertex2f(mouseX, mouseY);
//		glEnd();
//	}
//
//	//glColor4f(0.0, 0.0, 1.0, 1.0);
//	//glBegin(GL_TRIANGLES);
//	//	glVertex2f(0.5, 0.75);
//	//	glVertex2f(0.0, 0.0);
//	//	glVertex2f(1.0, 0.0);
//	//glEnd();
//
//	glColor4f(0.0, 0.0, 1.0, 1.0);
//	glRasterPos2f(0.05, 0.95);
//	glutBitmapString(GLUT_BITMAP_9_BY_15, "Lawrence");
//
//	glutSwapBuffers();
//}
//
//void myReshape(int newWidth, int newHeight)
//{
//	glViewport(0.0, 0.0, newWidth, newHeight);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	gluOrtho2D(0.0, (GLfloat) newWidth / (GLfloat) windowWidth, 0.0, (GLfloat) newHeight / (GLfloat) windowHeight);
//}
//
//void myMouse(GLint button, GLint state, GLint x, GLint y)
//{
//	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
//	{
//		mousePressed = 1;
//
//		mouseX = (GLfloat) x / (GLfloat) windowWidth;
//		mouseY = (GLfloat) windowHeight - (GLfloat) y;
//		mouseY = mouseY / (GLfloat) windowHeight;
//		
//		glutPostRedisplay();
//	}
//
//}
//
//void myKeyboard(unsigned char key, int x, int y) 
//{
//	if (key == 27)
//		exit(0);
//}
//
//void myMenu(int entryID)
//{
//	if (entryID == 2)
//		exit(0);
//	glutPostRedisplay();
//}
//
//void main(int argc, char** argv)
//{
//	glutInit(&argc, argv);
//	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
//	glutInitWindowSize(windowWidth, windowHeight);
//	glutInitWindowPosition(10, 10);
//	glutCreateWindow("Advanced");
//
//	glutIdleFunc(myIdle);
//	glutDisplayFunc(myDisplay);
//	glutMouseFunc(myMouse);
//	//glutMotionFunc(myMotion);
//	//glutPassiveMotionFunc(myPassiveMotion);
//	glutReshapeFunc(myReshape);
//	glutKeyboardFunc(myKeyboard);
//
//	glutCreateMenu(myMenu);
//	glutAddMenuEntry("Reset", 1);
//	glutAddMenuEntry("Quit", 2);
//	glutAttachMenu(GLUT_RIGHT_BUTTON);
//
//	initializeGL();
//	glutMainLoop();
//}