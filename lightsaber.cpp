/***************************************************************
 *	Garrick Brazil, Edward Cana, Devin Holland, Jeffrey Ring
 *	CS-420: Lightsaber Fight
 *	Dr. Giuseppe Turini
 *	Created on 8/28/2013
/****************************************************************/

#include "lightsaber.h"

// Window properties
int windowWidth = 800;
int windowHeight = 600;
int windowX = 100;
int windowY = 100;

// Video and frame
VideoCapture videoCapture;
Mat frame;

// Texture
GLuint videoTexture;


/*********************************************************
 * Method: draw
 * Purpose: draws the lightsaber scene
*********************************************************/
void draw()
{
    
}

/*********************************************************
 * Method: modifyFrame
 * Purpose: modifies frame for lightsaber scene
*********************************************************/
void modifyFrame(Mat inputFrame)
{
    
}

/*********************************************************
 * Method: displayText
 * Purpose: used to show text information to user
*********************************************************/
void displayText()
{
    
}

/*********************************************************
 * Method: init
 * Purpose: initialize basic elements of program
*********************************************************/
void init()
{
	// Setup
	glClearColor( 0.5f, 0.5f, 0.5f, 0.0f );
	glShadeModel( GL_FLAT );
	glEnable( GL_DEPTH_TEST );
	
	// Texturing
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glGenTextures( 1, &videoTexture );
	glBindTexture( GL_TEXTURE_2D, videoTexture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}


/*********************************************************
 * Method: reshape
 * Purpose: occures when window resizes
*********************************************************/
void reshape( int w, int h )
{
	// Adjust window properties
	windowWidth = w;
	windowHeight = h;
}

/*********************************************************
 * Method: keyboard
 * Purpose: callback for keyboard keyboard
*********************************************************/
void keyboard( unsigned char key, int x, int y )
{
	switch( key )
    {
		case 'q':{
			exit(1);
			break;
		}
		default:
        {
            break;
        }
	}
	glutPostRedisplay();
}


/*********************************************************
 * Method: display
 * Purpose: function called for screen updating
*********************************************************/
void display()
{
	
    // Clear buffers
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glViewport( 0, 0, (GLsizei) windowWidth, (GLsizei) windowHeight );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    
	// Texturing
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, videoTexture );
	
    modifyFrame(frame);

	if( frame.data)
    {
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, frame.size().width, frame.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data );
    }

    // Configure texture mapping flipping (vertical) the video frame
    glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -1, -1.0f, 0.0f );
		glTexCoord2f( 1.0f, 1.0f ); glVertex3f( 1, -1.0f, 0.0f );
		glTexCoord2f( 1.0f, 0.0f ); glVertex3f( 1, 1.0f, 0.0f );
		glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -1, 1.0f, 0.0f );
    glEnd();

	glDisable( GL_TEXTURE_2D );
    
    draw();
    displayText();
    
	// Double buffering.
	glutSwapBuffers();
}

/*********************************************************
 * Method: idle
 * Purpose: callback for system idling (no events)
*********************************************************/
void idle()
{
    
	// Get frame from video
	videoCapture.grab();
	videoCapture.retrieve(frame);

    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    // Setup webcam
    videoCapture.open(1);
    if (!videoCapture.isOpened())
    {
        printf("Could not open the camera");
        return -1;
    }
    
	// Grab first frame
	videoCapture.grab();
	videoCapture.retrieve(frame);
    
	// Set window to frame size
    windowWidth = frame.size().width;
    windowHeight = frame.size().height;
    
    // Setup the glut window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowX, windowY);
    glutCreateWindow("CS-420 - Light Saber");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    
	// Start openGL loop
    glutMainLoop();
    
    videoCapture.release();
    frame.release();
    
    return 0;
    
}