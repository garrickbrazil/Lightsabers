/***************************************************************
 *	Garrick Brazil, Edward Cana, Devin Holland, Jeffrey Ring
 *	CS-420: Lightsaber Fight
 *	Dr. Giuseppe Turini
 *	Created on 8/28/2013
/****************************************************************/


#include <GL/glew.h>
#include <GL/freeglut.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>
#include <stdio.h>

#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

// Window properties
int windowWidth = 800;
int windowHeight = 600;
int windowX = 100;
int windowY = 100;
CvSize size;

// Saber end points
float saber1_b1[3], saber1_b2[3];	
float saber2_b1[3], saber2_b2[3];

// Saber1 default settings
int saber1_c1 = 94;
int saber1_c2 = 127;
int saber1_c3 = 30;
int saber1_cm1 = 119;
int saber1_cm2 = 194;
int saber1_cm3 = 117;

// Saber 1 color
float saber1_r = 0;
float saber1_g = 1.0; 
float saber1_b = 0;

// Saber 2 default settings
int saber2_c1 = 134;
int saber2_c2 = 98;
int saber2_c3 = 20;
int saber2_cm1 = 146;
int saber2_cm2 = 151;
int saber2_cm3 = 93;

// Saber 2 color
float saber2_r = 1.0;
float saber2_g = 0.0; 
float saber2_b = 0;

// Misc
int smooth_count = 1;
int minPoints = 5;
int cylinderLayers = 60;
bool showThres1 = false;
bool showThres2 = false;

// Images used for processing
CvCapture* capture;
IplImage* frame;
IplImage * hsv_frame;
IplImage * thresholded_1, *thresholded_2;

// Containers and images.
std::vector<cv::Vec3f> circles_1, circles_2;
std::vector< std::vector<cv::Point> > contours_1, contours_2;
std::vector< std::vector<cv::Point> > contoursPoly_1, contoursPoly_2;
std::vector<cv::Rect> bb_1, bb_2;
std::vector<cv::Point2f> center_1, center_2;
std::vector<float> radius_1, radius_2;

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

	// Calculate distances
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
void drawSaber(float* po1, float* po2, float r, float g, float b){

	int width = texFrame.size().width;
	int height =texFrame.size().height;

	// p1 = smallest, p2 = largest
	float* p1, *p2, radius, ratio;
	
	// Temporary variables
	double x, y, aX,aY,aZ,bX,bY,bZ, rad;

	// Adjust smallest and largest
	if (po1[2] < po2[2]){ p1 = po1; p2 = po2; }
	else{ p1 = po2; p2 = po1; }

	// OpenGL settings
	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	//glShadeModel( GL_SMOOTH );
	
	radius = .1;					// set radius
	ratio = width*1.0/height;		// ratio

	glPushMatrix();

		for (int i = 0; i < cylinderLayers; i++){
			double ran = (rand()%101)/100.0;		
			x = ratio*p2[0]/width;
			y = p2[1]/height;

			if(i == 0) rad = (.28 +  (.8*i)/(cylinderLayers))*p1[2]/width;
			else rad = (.28 - .008*ran +  (.8*i*i*i*i*i)/(1.0*cylinderLayers*cylinderLayers*cylinderLayers*cylinderLayers*cylinderLayers))*p1[2]/width;
			aZ = .0;
			bZ = (((p2[2]/width)/(p1[2]/width))/(p1[2]/width))/100;


			aX = ratio*p1[0]/(width);
			aY = (height - p1[1])/height;

			bX = (ratio*p2[0]/width) - (x - ((.65 - x) * bZ + x));
			bY = ((height - p2[1])/height) + (y - ((.5 - y) * bZ + y));

			if (i == 0) glColor4f(1,1,1,1);
			else glColor4f(r*(1-(1.0*i)/cylinderLayers), g*(1-(1.0*i)/cylinderLayers), b*(1-(1.0*i)/cylinderLayers), .20*(1-(1.0*i*i*i*i)/(cylinderLayers*cylinderLayers*cylinderLayers*cylinderLayers)));
			
			renderCylinder_convenient(aX,aY,aZ,bX,bY,bZ,rad,10);
		}

	glPopMatrix();
    
}

/*********************************************************
 * Method: init
 * Purpose: initialize basic elements of program
*********************************************************/
void init()
{
	
	// Setup
	glClearColor( 0.4f, 0.4f, 0.4f, 0.0f );
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
	glBlendFunc(GL_ONE, GL_ONE);
	//glEnable(GL_POLYGON_STIPPLE);
	//glBlendFunc( GL_SRC_ALPHA_SATURATE, GL_ONE );
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
		case '1':{
			
			if(!showThres1){
				
				cv::namedWindow("Saber - 1", CV_WINDOW_NORMAL);

				// Settings page 
				cv::createTrackbar("c1", "Saber - 1", &saber1_c1, 255, NULL);
				cv::createTrackbar("c2", "Saber - 1", &saber1_c2, 255, NULL);
				cv::createTrackbar("c3", "Saber - 1", &saber1_c3, 255, NULL);
				cv::createTrackbar("cm1", "Saber - 1", &saber1_cm1, 255, NULL);
				cv::createTrackbar("cm2", "Saber - 1", &saber1_cm2, 255, NULL);
				cv::createTrackbar("cm3", "Saber - 1", &saber1_cm3, 255, NULL);
				cv::createTrackbar("smooth", "Saber - 1", &smooth_count, 15, NULL);
				cv::createTrackbar("min points", "Saber - 1", &minPoints, 80, NULL);
				cv::createTrackbar("cylinder", "Saber - 1", &cylinderLayers, 100, NULL);
			}
			else {
				cvDestroyWindow("Saber - 1");
				cvDestroyWindow("Threshold - 1");
			}

			showThres1 = !showThres1;
			break;
		}
		case '2':{
			
			if(!showThres2){
				
				cv::namedWindow("Saber - 2", CV_WINDOW_NORMAL);

				// Settings page 
				cv::createTrackbar("c1", "Saber - 2", &saber2_c1, 255, NULL);
				cv::createTrackbar("c2", "Saber - 2", &saber2_c2, 255, NULL);
				cv::createTrackbar("c3", "Saber - 2", &saber2_c3, 255, NULL);
				cv::createTrackbar("cm1", "Saber - 2", &saber2_cm1, 255, NULL);
				cv::createTrackbar("cm2", "Saber - 2", &saber2_cm2, 255, NULL);
				cv::createTrackbar("cm3", "Saber - 2", &saber2_cm3, 255, NULL);
				cv::createTrackbar("smooth", "Saber - 2", &smooth_count, 15, NULL);
				cv::createTrackbar("min points", "Saber - 2", &minPoints, 80, NULL);
				cv::createTrackbar("cylinder", "Saber - 2", &cylinderLayers, 100, NULL);
			}
			else {
				cvDestroyWindow("Saber - 2");
				cvDestroyWindow("Threshold - 2");
			}

			showThres2 = !showThres2;
			break;
		}
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
    gluLookAt(.65, .5, 1, .65, .5, 0.0, 0.0, 1, 0 );

	// Texturing
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, videoTexture );	

	
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


	// Draw light saber
	if(saber1_b1 != NULL && saber1_b2 != NULL){
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		drawSaber(saber1_b1, saber1_b2, saber1_r, saber1_g, saber1_b);
		drawSaber(saber2_b1, saber2_b2, saber2_r, saber2_g, saber2_b);
		glDisable(GL_BLEND);
	}


	/*********************************************************************
	* cvShowImage( "Camera", frame );							// original
	* cvShowImage( "HSV", hsv_frame);							// hsv frame
	*********************************************************************/
	

	// Double buffering.
	glutSwapBuffers();
}

/*********************************************************
 * Method: idle
 * Purpose: callback for system idling (no events)
*********************************************************/
void idle()
{
    
	// Get one frame
	frame = cvQueryFrame( capture );
	if( !frame ){
		fprintf( stderr, "ERROR: frame is null...\n" );
		exit(-1);
	}

	/**************************************************************************
	 * Saber 1 Detection here
	/*************************************************************************/

	// Covert color space to HSV
	cvCvtColor(frame, hsv_frame, CV_BGR2HSV);

	// Saber 1 ranges
	CvScalar hsv_min = cvScalar(saber1_c1, saber1_c2, saber1_c3);
	CvScalar hsv_max = cvScalar(saber1_cm1, saber1_cm2, saber1_cm3);

	// Filter out colors which are out of range
	cvInRangeS(hsv_frame, hsv_min, hsv_max, thresholded_1);

	// Detector works better with some smoothing of the image
	for (int i = 0; i < smooth_count; i++){
		cvSmooth( thresholded_1, thresholded_1, CV_GAUSSIAN, 9, 9 );
		cvSmooth( thresholded_1, thresholded_1, CV_MEDIAN, 9, 9 );
	}

	
	if(cvGetWindowHandle("Saber - 1")) cvShowImage( "Threshold - 1", thresholded_1 );
	cv::Mat thresholdImg = thresholded_1;
    
	
	// Find contours
	cv::findContours( thresholdImg, contours_1, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
		
	// Contours refinement
	contoursPoly_1.resize( contours_1.size() );
	bb_1.resize( contours_1.size() );
	center_1.resize( contours_1.size() );
	radius_1.resize( contours_1.size() );

	// Refine contours and compute BB and min enclosing circle.
	for( size_t i = 0; i < contours_1.size(); i++ ) { 
		
		// Approximates the contours
		cv::approxPolyDP( cv::Mat( contours_1[i] ), contoursPoly_1[i], 3.0, true );
		
		// Calculates bounding box of a 2D point set
		bb_1[i] = cv::boundingRect( cv::Mat( contoursPoly_1[i] ) );
		
		// Finds a circle with min area enclosing a 2D point set.
		cv::minEnclosingCircle( cv::Mat( contoursPoly_1[i] ), center_1[i], radius_1[i] ); 
	 }
		
	// Draw contours + bounding boxes + enclosing circles.
	texFrame = frame;
	int count = 0;

	// Draw found objects
	for( size_t i = 0; i< contours_1.size(); i++ ) {
		
		if(contoursPoly_1[i].size() > minPoints){
			if(count == 0){
				saber1_b1[0] = center_1[i].x;
				saber1_b1[1] = center_1[i].y;
				saber1_b1[2] = radius_1[i];
			}
			else if(count == 1){
				saber1_b2[0] = center_1[i].x;
				saber1_b2[1] = center_1[i].y;
				saber1_b2[2] = radius_1[i];
			}
			cv::Scalar color = cv::Scalar( 255, 0, 255 );
			cv::drawContours( texFrame, contoursPoly_1, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
			cv::rectangle( texFrame, bb_1[i].tl(), bb_1[i].br(), color, 2, 8, 0 );
			cv::circle( texFrame, center_1[i], (int)radius_1[i], color, 2, 8, 0 ); 
			count++;
		}
	}
		

	/**************************************************************************
	 * Saber 2 Detection here
	/*************************************************************************/
	
	// Saber 1 ranges
	CvScalar hsv_min_2 = cvScalar(saber2_c1, saber2_c2, saber2_c3);
	CvScalar hsv_max_2 = cvScalar(saber2_cm1, saber2_cm2, saber2_cm3);

	// Filter out colors which are out of range
	cvInRangeS(hsv_frame, hsv_min_2, hsv_max_2, thresholded_2);

	// Detector works better with some smoothing of the image
	for (int i = 0; i < smooth_count; i++){
		cvSmooth( thresholded_2, thresholded_2, CV_GAUSSIAN, 9, 9 );
		cvSmooth( thresholded_2, thresholded_2, CV_MEDIAN, 9, 9 );
	}

	
	if(cvGetWindowHandle("Saber - 2")) cvShowImage( "Threshold - 2", thresholded_2 );
	cv::Mat thresholdImg_2 = thresholded_2;
    
	
	// Find contours
	cv::findContours( thresholdImg_2, contours_2, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
		
	// Contours refinement
	contoursPoly_2.resize( contours_2.size() );
	bb_2.resize( contours_2.size() );
	center_2.resize( contours_2.size() );
	radius_2.resize( contours_2.size() );

	// Refine contours and compute BB and min enclosing circle.
	for( size_t i = 0; i < contours_2.size(); i++ ) { 
		
		// Approximates the contours
		cv::approxPolyDP( cv::Mat( contours_2[i] ), contoursPoly_2[i], 3.0, true );
		
		// Calculates bounding box of a 2D point set
		bb_2[i] = cv::boundingRect( cv::Mat( contoursPoly_2[i] ) );
		
		// Finds a circle with min area enclosing a 2D point set.
		cv::minEnclosingCircle( cv::Mat( contoursPoly_2[i] ), center_2[i], radius_2[i] ); 
	 }
		
	// Draw contours + bounding boxes + enclosing circles.
	texFrame = frame;
	count = 0;

	// Draw found objects
	for( size_t i = 0; i< contours_2.size(); i++ ) {
		
		if(contoursPoly_2[i].size() > minPoints){
			if(count == 0){
				saber2_b1[0] = center_2[i].x;
				saber2_b1[1] = center_2[i].y;
				saber2_b1[2] = radius_2[i];
			}
			else if(count == 1){
				saber2_b2[0] = center_2[i].x;
				saber2_b2[1] = center_2[i].y;
				saber2_b2[2] = radius_2[i];
			}
			cv::Scalar color = cv::Scalar( 255, 0, 255 );
			cv::drawContours( texFrame, contoursPoly_2, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
			cv::rectangle( texFrame, bb_2[i].tl(), bb_2[i].br(), color, 2, 8, 0 );
			cv::circle( texFrame, center_2[i], (int)radius_2[i], color, 2, 8, 0 ); 
			count++;
		}
	}


    glutPostRedisplay();
}

/*********************************************************
 * Method: main
 * Purpose: main program
*********************************************************/
int main(int argc, char** argv){

	srand (time(NULL));

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

	// Modified images
	hsv_frame = cvCreateImage(size, IPL_DEPTH_8U, 3);
	thresholded_1 = cvCreateImage(size, IPL_DEPTH_8U, 1);
	thresholded_2 = cvCreateImage(size, IPL_DEPTH_8U, 1);

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