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
CvSize size;

// Storage for circles
CvMemStorage* storage;
CvSeq* circles;

// Saber end points
float * saber1_b1, *saber1_b2;	

// Default settings
int saber1_c1 = 40;
int saber1_c2 = 90;
int saber1_c3 = 39;
int saber1_cm1 = 125;
int saber1_cm2 = 200;
int saber1_cm3 = 172;
int saber1_p1 = 100;
int saber1_p2 = 38;

// Misc
int smooth_count = 3;

// Images used for processing
CvCapture* capture;
IplImage* frame;
IplImage * hsv_frame;
IplImage * thresholded;

// Video and frame
VideoCapture videoCapture;
Mat texFrame;
GLuint videoTexture;

/*********************************************************
 * Method: renderCylinder
 * Purpose: renders a cylinder with a quad
*********************************************************/
void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions,GLUquadricObj *quadric){
	float vx = x2-x1;
	float vy = y2-y1;
	float vz = z2-z1;

	// Handle the degenerate case of z1 == z2 with an approximation
	if(vz == 0){
		vz = .0001;
	}

	float v = sqrt( vx*vx + vy*vy + vz*vz );
	float ax = 57.2957795*acos( vz/v );
	if ( vz < 0.0 )
		ax = -ax;
	float rx = -vy*vz;
	float ry = vx*vz;
	
	glPushMatrix();

	//draw the cylinder body
	glTranslatef( x1,y1,z1 );
	glRotatef(ax, rx, ry, 0.0);
	gluQuadricOrientation(quadric,GLU_OUTSIDE);
	gluCylinder(quadric, radius, radius, v, subdivisions, 1);

	//draw the first cap
	gluQuadricOrientation(quadric,GLU_INSIDE);
	gluDisk( quadric, 0.0, radius, subdivisions, 1);
	glTranslatef( 0,0,v );

	//draw the second cap
	gluQuadricOrientation(quadric,GLU_OUTSIDE);
	gluDisk( quadric, 0.0, radius, subdivisions, 1);
	glPopMatrix();
}

/*********************************************************
 * Method: renderCylinder_convenient
 * Purpose: renders a cylinder with a quad
*********************************************************/
void renderCylinder_convenient(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions)
	{
	//the same quadric can be re-used for drawing many cylinders
	GLUquadricObj *quadric=gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
	renderCylinder(x1,y1,z1,x2,y2,z2,radius,subdivisions,quadric);
	gluDeleteQuadric(quadric);
}


/*********************************************************
 * Method: drawSaber
 * Purpose: draws the lightsaber scene
*********************************************************/
void drawSaber(float* po1, float* po2,int width,int height){

	/* TODO Cleanup*/

	float* p1, *p2;
	if (po1[2] < po2[2]){
		p1 = po1;
		p2 = po2;
	}
	else{
		p1 = po2;
		p2 = po1;
	}

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glShadeModel( GL_SMOOTH );
	glEnable( GL_LIGHTING );
	
	float r,g,b, shoulderLenght, radius;
	r = 1.0f;
	g = 0.0f;
	b = 0.0f;
	shoulderLenght = 3.0f;
	radius = .1;
	double ratio = width*1.0/height;
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glPushMatrix();

		glPushMatrix();
			
			double x, y, aX,aY,aZ,bX,bY,bZ, rad;
			
			// Inside
			glPushMatrix();

				x = ratio*p2[0]/width;
				y = p2[1]/height;

				rad = .28*p1[2]/width;
				aZ = .0;
				bZ = (((p2[2]/width)/(p1[2]/width))/(p1[2]/width))/100;//rad * (p2[2]/width)/(p1[2]/width)*10.0;//(bz-100)/100.0;
				//printf("%lf    %lf    %lf    %lf     %lf\n",(p2[2]/width)/(p1[2]/width), rad, ratio*p2[0]/width, (height-p2[1])/height, (y - ((.5 - y) * rad * (p2[2]/width)/(p1[2]/width)*10.0 + y)));
			
				aX = ratio*p1[0]/(width);
				aY = (height - p1[1])/height;
			
				bX = (ratio*p2[0]/width) - (x - ((.5 - x) * bZ + x));// ratio*((bx - 100)/100.0);
				bY = ((height - p2[1])/height) + (y - ((.5 - y) * bZ + y));//+ ((by-100)/100.0);

				glColor4f(1, 1, 1, 1);
				glScalef( 1.0f, 1.0, 1.0f );
				renderCylinder_convenient(aX,aY,aZ,bX,bY,bZ,rad,10);
			glPopMatrix();

			
			// Outside 1
			glPushMatrix();
			
				glColor4f( r + .1, g + .1, b + .1, 1 ); 
				x = ratio*p2[0]/width;
				y = p2[1]/height;
				
				rad = .5*p1[2]/width;
				aZ = .0;
				bZ = (((p2[2]/width)/(p1[2]/width))/(p1[2]/width))/100;//rad * (p2[2]/width)/(p1[2]/width)*10.0;//(bz-100)/100.0;

				aX = ratio*p1[0]/(width);
				aY = (height - p1[1])/height;
			
				bX = (ratio*p2[0]/width) - (x - ((.5 - x) * bZ + x));// ratio*((bx - 100)/100.0);
				bY = ((height - p2[1])/height) + (y - ((.5 - y) * bZ + y));//+ ((by-100)/100.0);
				renderCylinder_convenient(aX,aY,aZ,bX,bY,bZ,rad,10);
			glPopMatrix();
			
			// Outside 2
			glPushMatrix();
			
				glColor4f( r, g, b, .9 ); 
				x = ratio*p2[0]/width;
				y = p2[1]/height;
				
				rad = .55*p1[2]/width;
				aZ = .0;
				bZ = (((p2[2]/width)/(p1[2]/width))/(p1[2]/width))/100;//rad * (p2[2]/width)/(p1[2]/width)*10.0;//(bz-100)/100.0;

				aX = ratio*p1[0]/(width);
				aY = (height - p1[1])/height;
			
				bX = (ratio*p2[0]/width) - (x - ((.5 - x) * bZ + x));// ratio*((bx - 100)/100.0);
				bY = ((height - p2[1])/height) + (y - ((.5 - y) * bZ + y));//+ ((by-100)/100.0);
				renderCylinder_convenient(aX,aY,aZ,bX,bY,bZ,rad,10);
			glPopMatrix();
			
			// Outside 3
			glPushMatrix();
			
				glColor4f( r -.1, g-.1, b-.1, .8 ); 
				x = ratio*p2[0]/width;
				y = p2[1]/height;
				
				rad = .6*p1[2]/width;
				aZ = .0;
				bZ = (((p2[2]/width)/(p1[2]/width))/(p1[2]/width))/100;//rad * (p2[2]/width)/(p1[2]/width)*10.0;//(bz-100)/100.0;

				aX = ratio*p1[0]/(width);
				aY = (height - p1[1])/height;
			
				bX = (ratio*p2[0]/width) - (x - ((.5 - x) * bZ + x));// ratio*((bx - 100)/100.0);
				bY = ((height - p2[1])/height) + (y - ((.5 - y) * bZ + y));//+ ((by-100)/100.0);
				renderCylinder_convenient(aX,aY,aZ,bX,bY,bZ,rad,10);
			glPopMatrix();
			
			// Outside 4
			glPushMatrix();
			
				glColor4f( r -.2, g-.2, b-.2, .6 ); 
				x = ratio*p2[0]/width;
				y = p2[1]/height;
				
				rad = .65*p1[2]/width;
				aZ = .0;
				bZ = (((p2[2]/width)/(p1[2]/width))/(p1[2]/width))/100;//rad * (p2[2]/width)/(p1[2]/width)*10.0;//(bz-100)/100.0;

				aX = ratio*p1[0]/(width);
				aY = (height - p1[1])/height;
			
				bX = (ratio*p2[0]/width) - (x - ((.5 - x) * bZ + x));// ratio*((bx - 100)/100.0);
				bY = ((height - p2[1])/height) + (y - ((.5 - y) * bZ + y));//+ ((by-100)/100.0);
				renderCylinder_convenient(aX,aY,aZ,bX,bY,bZ,rad,10);
			glPopMatrix();

		glPopMatrix();
	glPopMatrix();
	glDisable(GL_BLEND);
    
}

/*********************************************************
 * Method: init
 * Purpose: initialize basic elements of program
*********************************************************/
void init()
{
	// Setup
	glClearColor( 0.2f, 0.2f, 0.2f, 0.0f );
	glShadeModel( GL_SMOOTH );
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
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	glEnable( GL_DEPTH_TEST );
	
	// Enabling blending (important for saber)
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA_SATURATE, GL_ONE );
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

	// Viewport setup
	glViewport( 0, 0, (GLsizei) windowWidth, (GLsizei) windowHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60.0, (GLfloat) windowWidth / (GLfloat) windowHeight, 0.1, 10.0 );

	// Modelview setup
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    gluLookAt(.5, .5, 1, .5, .5, 0.0, 0.0, 1, 0 );

	// Texturing
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, videoTexture );	
	texFrame = frame;
	
	// Draw light saber
	if(saber1_b1 != NULL && saber1_b2 != NULL){
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc( GL_SRC_ALPHA_SATURATE, GL_ONE );
		//drawSaber(saber1_b1, saber1_b2, texFrame.size().width, texFrame.size().height);
		glDisable(GL_BLEND);
	}


	// Frame texture
	if( texFrame.data){
		glEnable(GL_TEXTURE_2D);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, texFrame.size().width, texFrame.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, texFrame.data );
    }
	
    glPushMatrix();
	
		// Calculate frame aspect ratio
		double ratio = texFrame.size().width*1.0/texFrame.size().height;

		// Draw frame in OpenGL
		glBegin( GL_QUADS );
			glTexCoord2f( 0.0f, 1.0f ); glVertex3f( 0, 0.0f, 0.0f );
			glTexCoord2f( 1.0f, 1.0f ); glVertex3f( ratio, 0.0f, 0.0f );
			glTexCoord2f( 1.0f, 0.0f ); glVertex3f( ratio, 1.0f, 0.0f );
			glTexCoord2f( 0.0f, 0.0f ); glVertex3f( 0, 1.0f, 0.0f );
		glEnd();
	
    glPopMatrix();

	// Disable texturing
	glDisable( GL_TEXTURE_2D );

	/*********************************************************************
	* cvShowImage( "Camera", frame );							// original
	* cvShowImage( "HSV", hsv_frame);							// hsv frame
	*********************************************************************/
	cvShowImage( "After Color Filtering", thresholded );	// thresholded 

	// Double buffering.
	glutSwapBuffers();
}

/*********************************************************
 * Method: idle
 * Purpose: callback for system idling (no events)
*********************************************************/
void idle()
{
    
	// Saber 1 ranges
	CvScalar hsv_min = cvScalar(saber1_c1, saber1_c2, saber1_c3, 0);
	CvScalar hsv_max = cvScalar(saber1_cm1, saber1_cm2, saber1_cm3, 0);
	
	// Get one frame
	frame = cvQueryFrame( capture );
	if( !frame ){
		fprintf( stderr, "ERROR: frame is null...\n" );
		exit(-1);
	}

	// Covert color space to HSV
	cvCvtColor(frame, hsv_frame, CV_BGR2HSV);

	// Filter out colors which are out of range
	cvInRangeS(hsv_frame, hsv_min, hsv_max, thresholded);
	
	// Clear memory for storage and reallocate **
	cvReleaseMemStorage(&storage);
	storage = cvCreateMemStorage(0);
	
	// Hough detector works better with some smoothing of the image
	for (int i = 0; i < smooth_count; i++)
		cvSmooth( thresholded, thresholded, CV_GAUSSIAN, 9, 9 );

	circles = cvHoughCircles(thresholded, storage, CV_HOUGH_GRADIENT, 2,
		thresholded->height/16, saber1_p1, saber1_p2, 1, 800);

	
	// Draw circles for --DEBUG
	for (int i = 0; i < circles->total; i++){
		
		// Get current circle
		float* p = (float*)cvGetSeqElem( circles, i );
		
		// Draw point
		cvCircle( frame, cvPoint(cvRound(p[0]),cvRound(p[1])),
		3, CV_RGB(0,255,0), -1, 8, 0 );
		
		// Draw circle
		cvCircle( frame, cvPoint(cvRound(p[0]),cvRound(p[1])),
		cvRound(p[2]), CV_RGB(255,0,0), 3, 8, 0 );
	}
	

	// If circles
	if(circles ->total == 2){
		saber1_b1 = (float*)cvGetSeqElem( circles, 0 );
		saber1_b2 = (float*)cvGetSeqElem( circles, 1 );
	}

    glutPostRedisplay();
}

/*********************************************************
 * Method: main
 * Purpose: main program
*********************************************************/
int main(int argc, char** argv){
	
	// Video cam size --FIX
	size = cvSize(640,480);

	// Open capture device
	capture = cvCaptureFromCAM( 1 );
	
	// Check for opening error
	if( !capture ){
		fprintf( stderr, "ERROR: capture is NULL \n" );
		return -1;
	}
	
	/****************************************************************
	* cvNamedWindow( "Camera", CV_WINDOW_AUTOSIZE );
	* cvNamedWindow( "HSV", CV_WINDOW_AUTOSIZE );
	* cvNamedWindow( "EdgeDetection", CV_WINDOW_AUTOSIZE );
	****************************************************************/
	cv::namedWindow("Settings", CV_WINDOW_AUTOSIZE);

	// Settings page 
	cv::createTrackbar("c1", "Settings", &saber1_c1, 255, NULL);
	cv::createTrackbar("c2", "Settings", &saber1_c2, 255, NULL);
	cv::createTrackbar("c3", "Settings", &saber1_c3, 255, NULL);
	cv::createTrackbar("cm1", "Settings", &saber1_cm1, 255, NULL);
	cv::createTrackbar("cm2", "Settings", &saber1_cm2, 255, NULL);
	cv::createTrackbar("cm3", "Settings", &saber1_cm3, 255, NULL);
	cv::createTrackbar("param1", "Settings", &saber1_p1, 200, NULL);
	cv::createTrackbar("param2", "Settings", &saber1_p2, 200, NULL);

	// Modified images
	hsv_frame = cvCreateImage(size, IPL_DEPTH_8U, 3);
	thresholded = cvCreateImage(size, IPL_DEPTH_8U, 1);
	
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
    
	// Start loop
    glutMainLoop();
    
    cvReleaseCapture( &capture );
	cvDestroyWindow( "mywindow" );
    return 0;
    
}