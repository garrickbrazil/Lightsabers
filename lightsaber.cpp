/***************************************************************
 *	Garrick Brazil, Edward Cana, Devin Holland, Jeffrey Ring
 *	CS-420: Lightsaber Fight
 *	Dr. Giuseppe Turini
 *	Created on 8/28/2013
/****************************************************************/


#include <GL/glew.h>
#include <GL/freeglut.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

using namespace cv;

char quote[6][80];
int numberOfQuotes=0,i;
double openTime = 8000;

// Saber 1 Settings
int saber1_c1 = 41;
int saber1_c2 = 30;
int saber1_c3 = 3;
int saber1_cm1 = 75;
int saber1_cm2 = 174;
int saber1_cm3 = 96;
int saber1_color = 1;

// Saber 1 Color
float saber1_r = 0;
float saber1_g = 1;
float saber1_b = 0;
int saber1_time;

// Saber 2 Settings
int saber2_c1 = 138;
int saber2_c2 = 64;
int saber2_c3 = 22;
int saber2_cm1 = 162;
int saber2_cm2 = 148;
int saber2_cm3 = 71;
int saber2_color = 0;

// Saber 2 Color
float saber2_r = 1;
float saber2_g = 0.0;
float saber2_b = 0;
int saber2_time;

// Window properties
int windowWidth = 800;
int windowHeight = 600;
int windowX = 100;
int windowY = 100;

// Animation timing
int lastTime_1 = 0.0;
int lastTime_2 = 0.0;
float aniTime = 280;

// Animation drawn 0.0 -> 1.0
float portionDrawn_1 = 0.0;
float portionDrawn_2 = 0.0;

// Sabers enabling
bool saber1 = false;
bool saber2 = false;

// Current sabers
float saber1_b1[3], saber1_b2[3];
float saber2_b1[3], saber2_b2[3];

// Misc
int minPoints = 5;
int cylinderLayers = 45;
const int blurCount = 4;

// Settings pages
bool showThres1 = false;
bool showThres2 = false;

// Previous distances
double s1_b1_dX = 0, s1_b1_dY;
double s2_b1_dX = 0, s2_b1_dY;

// Previous sabers
float saber1_b1_past[3][3] = {0}, saber1_b2_past[3][3] = {0};
float saber2_b1_past[3][3] = {0}, saber2_b2_past[3][3] = {0};

// Interpolated sabers
float saber1_b1_inter[blurCount*blurCount][3] = {0}, saber1_b2_inter[blurCount*blurCount][3] = {0};
float saber2_b1_inter[blurCount*blurCount][3] = {0}, saber2_b2_inter[blurCount*blurCount][3] = {0};

int openerTime, lastOpenerTime;

// Images used for processing
CvCapture *capture;
IplImage *frame;
IplImage *hsv_frame;
IplImage *thresholded_1, *thresholded_2;

// Containers and images.
std::vector<cv::Vec3f> circles_1, circles_2;
std::vector< std::vector<cv::Point> > contours_1, contours_2;
std::vector< std::vector<cv::Point> > contoursPoly_1, contoursPoly_2;
std::vector<cv::Rect> bb_1, bb_2;
std::vector<cv::Point2f> center_1, center_2;
std::vector<float> radius_1, radius_2;

// Video and frame
Mat texFrame;
GLuint videoTexture;

/*********************************************************
 * Method: distance
 * Purpose: calculates the distance between two points
 *********************************************************/
double distance(float *p1, float x, float y){
	
	// Distances in x & y
	float dx = x - p1[0];
	float dy = y - p1[1];

	// Distance formula
	return sqrt(pow(dx,2) + pow(dy,2));
}

/*********************************************************
 * Method: updateColor1
 * Purpose: updates color of saber1
 *********************************************************/
void updateColor1(int color, void*nothing){

	switch (color){
	case 0:
		saber1_r = 1;
		saber1_g = 0;
		saber1_b = 0;
		break;
	case 1:
		saber1_r = 0;
		saber1_g = 1;
		saber1_b = 0;
		break;
	case 2:
		saber1_r = 0;
		saber1_g = 0;
		saber1_b = 1;
		break;
	case 3:
		saber1_r = 1;
		saber1_g = 1;
		saber1_b = 0;
		break;
	case 4:
		saber1_r = 1;
		saber1_g = 0;
		saber1_b = 1;
		break;
	case 5:
		saber1_r = 0;
		saber1_g = 1;
		saber1_b = 1;
		break;
	case 6:
		saber1_r = 1;
		saber1_g = .5;
		saber1_b = 0;
		break;
	case 7:
		saber1_r = 1;
		saber1_g = 0;
		saber1_b = 0.5;
		break;
	case 8:
		saber1_r = 0;
		saber1_g = 1;
		saber1_b = .5;
		break;
	case 9:
		saber1_r = .5;
		saber1_g = 1;
		saber1_b = 0;
		break;
	case 10:
		saber1_r = .5;
		saber1_g = 0;
		saber1_b = 1;
		break;
	case 11:
		saber1_r = 0;
		saber1_g = .5;
		saber1_b = 1;
		break;
	}
}

/*********************************************************
 * Method: updateColor
 * Purpose: updates color of saber2
 *********************************************************/
void updateColor2(int color, void*nothing){

	switch (color){
	case 0:
		saber2_r = 1;
		saber2_g = 0;
		saber2_b = 0;
		break;
	case 1:
		saber2_r = 0;
		saber2_g = 1;
		saber2_b = 0;
		break;
	case 2:
		saber2_r = 0;
		saber2_g = 0;
		saber2_b = 1;
		break;
	case 3:
		saber2_r = 1;
		saber2_g = 1;
		saber2_b = 0;
		break;
	case 4:
		saber2_r = 1;
		saber2_g = 0;
		saber2_b = 1;
		break;
	case 5:
		saber2_r = 0;
		saber2_g = 1;
		saber2_b = 1;
		break;
	case 6:
		saber2_r = 1;
		saber2_g = .5;
		saber2_b = 0;
		break;
	case 7:
		saber2_r = 1;
		saber2_g = 0;
		saber2_b = 0.5;
		break;
	case 8:
		saber2_r = 0;
		saber2_g = 1;
		saber2_b = .5;
		break;
	case 9:
		saber2_r = .5;
		saber2_g = 1;
		saber2_b = 0;
		break;
	case 10:
		saber2_r = .5;
		saber2_g = 0;
		saber2_b = 1;
		break;
	case 11:
		saber2_r = 0;
		saber2_g = .5;
		saber2_b = 1;
		break;
	}
}

/*********************************************************
 * Method: renderCylinder
 * Purpose: renders a cylinder with a quad
 *********************************************************/
void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions){
	
	// Temporary quadric
	GLUquadricObj *quadric=gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);

	// Distances in x, y, z
	float vx = x2-x1;
	float vy = y2-y1;
	float vz = z2-z1;
    
	// Handle the degenerate case of z1 == z2 with an approximation
	if(vz == 0) vz = .0001;
    
	// Calculate rotations
	float v = sqrt( vx*vx + vy*vy + vz*vz );
	float ax = 57.2957795*acos( vz/v );
	if ( vz < 0.0 ) ax = -ax;
	float rx = -vy*vz;
	float ry = vx*vz;
    
	glPushMatrix();
    
		// Draw the cylinder body
		glTranslatef( x1,y1,z1 );
		glRotatef(ax, rx, ry, 0.0);
		gluQuadricOrientation(quadric,GLU_OUTSIDE);
		gluCylinder(quadric, radius, radius, v, subdivisions, 1);
    
		// Draw the first cap
		gluQuadricOrientation(quadric,GLU_INSIDE);
		gluDisk( quadric, 0.0, radius, subdivisions, 1);
		glTranslatef( 0,0,v );
    
		// Draw the second cap
		gluQuadricOrientation(quadric,GLU_OUTSIDE);
		gluDisk( quadric, 0.0, radius, subdivisions, 1);
	glPopMatrix();

	// Delete quadric
	gluDeleteQuadric(quadric);
}


/*********************************************************
 * Method: drawSaber
 * Purpose: draws the lightsaber in openGL
 *********************************************************/
void drawSaber(float* po1, float* po2, float r, float g, float b, double trans, float portion){

	// Grab proper width and height
	int width  =  texFrame.size().width;
	int height =  texFrame.size().height;
    

	float *p1;			// smallest point
	float *p2;			// largest point
	float ratio;		// ratio of camera
	double x, y;		// something?
	double aX,aY,aZ;	// coordinates for p1
	double bX,bY,bZ;	// coordinates for p2
	double rad;			// radius for p1 & p2
    
	// Adjust smallest and largest
	if (po1[2] < po2[2]){ p1 = po1; p2 = po2; }
	else{ p1 = po2; p2 = po1; }
    

	ratio = width*1.0/height;		// calculate ratio
    x = ratio*p2[0]/width;			// calculate x for p2
	y = p2[1]/height;				// calculate y for p2
	aX = ratio*p1[0]/(width);		// calculate aX
	aY = (height - p1[1])/height;	// calculate xY
	aZ = 0;							// start aZ off at 0
	bZ = 1.2*(((p2[2]/width)/		// approximate bZ
		(p1[2]/width))/
		(p1[2]/width))/100;	


	// Move x and y closer to camera eye depending on bZ
	bX = (ratio*p2[0]/width) - (x - ((.65 - x) * bZ + x));
	bY = ((height - p2[1])/height) + (y - ((.5 - y) * bZ + y));
		
    			
	glPushMatrix();

		// Draw all cylinder layers
		for (int i = 0; i < cylinderLayers; i++){			
        
			// Calculate radius 
			rad = (.28 +  (1.1*i*i*i*i*i)/(1.0*cylinderLayers*cylinderLayers*cylinderLayers*cylinderLayers*cylinderLayers))*p1[2]/width;
			
			// If p1 is lower than p2
			if(p1[1] > p2[1]){
						
				// Only move bx,by,bz out by portion
				bZ *= portion;
				bX = (ratio*p2[0]/width -aX)*portion + aX - (x - ((.65 - x) * bZ + x));
				bY = ((height - p2[1])/height - aY)*portion +aY + (y - ((.5 - y) * bZ + y));
			}

			// If p2 is lower than p1
			else {

				// Only move ax,ay,az out by portion
				aX += (bX - aX)*(1 - portion);
				aY += (bY - aY)*(1 - portion);
				aZ = bZ*(1-portion);
			}
			
			// Solid white middle
			if (i == 0) glColor4f(1*trans,1*trans,1*trans,1);
			
			// Saber color multiplied by transparency (white = transparent)
			else glColor4f(r*(1-(1.0*i)/cylinderLayers)*trans, g*(1-(1.0*i)/cylinderLayers)*trans, b*(1-(1.0*i)/cylinderLayers)*trans, 1);
        
			// Render the cylinder
			renderCylinder(aX,aY,aZ,bX,bY,bZ,rad,10);
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
    
	// Enabling blending (important for saber appearance)
	glEnable( GL_BLEND );
	glBlendFunc(GL_ONE, GL_ONE);
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
			
			// Store current time
			if(!saber1) lastTime_1 = glutGet( GLUT_ELAPSED_TIME );

			portionDrawn_1 = 0;		// start portion at 0
			saber1 = !saber1;		// toggle saber 1

			break;
		}

		case '2':{

			// Store current time
			if(!saber2) lastTime_2 = glutGet( GLUT_ELAPSED_TIME );

			portionDrawn_2 = 0;		// start portion at 0
			saber2 = !saber2;		// toggle saber 2

			break;
		}

		case '!':{

			if(!showThres1){
                
				// Create a window
				cv::namedWindow("Saber - 1", CV_WINDOW_NORMAL);
                
				// Settings page
				cv::createTrackbar("c1", "Saber - 1", &saber1_c1, 255, NULL);
				cv::createTrackbar("c2", "Saber - 1", &saber1_c2, 255, NULL);
				cv::createTrackbar("c3", "Saber - 1", &saber1_c3, 255, NULL);
				cv::createTrackbar("cm1", "Saber - 1", &saber1_cm1, 255, NULL);
				cv::createTrackbar("cm2", "Saber - 1", &saber1_cm2, 255, NULL);
				cv::createTrackbar("cm3", "Saber - 1", &saber1_cm3, 255, NULL);
				cv::createTrackbar("Color", "Saber - 1", &saber1_color,11, updateColor1);
				
			}
			else {
				
				// Destroy windows
				cvDestroyWindow("Saber - 1");
				cvDestroyWindow("Threshold - 1");
			}
            
			showThres1 = !showThres1;
			break;
		}
		case '@':{
            
			if(!showThres2){
                
				// Create window
				cv::namedWindow("Saber - 2", CV_WINDOW_NORMAL);
                
				// Settings page
				cv::createTrackbar("c1", "Saber - 2", &saber2_c1, 255, NULL);
				cv::createTrackbar("c2", "Saber - 2", &saber2_c2, 255, NULL);
				cv::createTrackbar("c3", "Saber - 2", &saber2_c3, 255, NULL);
				cv::createTrackbar("cm1", "Saber - 2", &saber2_cm1, 255, NULL);
				cv::createTrackbar("cm2", "Saber - 2", &saber2_cm2, 255, NULL);
				cv::createTrackbar("cm3", "Saber - 2", &saber2_cm3, 255, NULL);
				cv::createTrackbar("Color", "Saber - 2", &saber2_color, 11, updateColor2);
			}

			else {

				// Destory windows
				cvDestroyWindow("Saber - 2");
				cvDestroyWindow("Threshold - 2");
			}
            
			showThres2 = !showThres2;
			break;
		}

		case 'q': case 27:{

			// Quit
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

void RenderToDisplay()
{
    int l,lenghOfQuote, i;

	int UpwardsScrollVelocity = -500*(openerTime/openTime);

    glTranslatef(15, -75 + 0*(openerTime/openTime), UpwardsScrollVelocity);
    glRotatef(-80, 1.0, 0.0, 0.0);
    glScalef(0.1, 0.1, 0.1);


    for(  l=0;l<numberOfQuotes;l++)
    {
        lenghOfQuote = (int)strlen(quote[l]);
        glPushMatrix();
        glTranslatef(-(30*30), -(l*200), 0.0);

        for (i = 0; i < lenghOfQuote; i++)
        {
			glPushMatrix();
			glTranslatef((2.8*30*30.0*(i*1.0/lenghOfQuote)), 0, 0.0);
            glColor3f((UpwardsScrollVelocity/10)+300+(l*10),(UpwardsScrollVelocity/10)+300+(l*10),0.0);
            glutStrokeCharacter(GLUT_STROKE_ROMAN, quote[l][i]);
			glPopMatrix();
        }
        glPopMatrix();
    }

}


/*********************************************************
 * Method: display
 * Purpose: function called for screen updating
 *********************************************************/
void display()
{

	if(openerTime < openTime){
		  glDisable(GL_DEPTH_TEST);
			glViewport( 0, 0, (GLsizei) windowWidth, (GLsizei) windowHeight );
		  glMatrixMode(GL_PROJECTION);
		  glLoadIdentity();
		  gluPerspective(60, 1.0, 1.0, 3200);
		  glMatrixMode(GL_MODELVIEW);
	
		 glClear(GL_COLOR_BUFFER_BIT);
		  glLoadIdentity();
		  gluLookAt(0.0, 30.0, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		glClearColor(0.0, 0.0, 0.0, 1.0);
    glLineWidth(3);

		  RenderToDisplay();
		  glEnable(GL_DEPTH_TEST);
	}


	else{
    
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
	if( texFrame.data) glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, texFrame.size().width, texFrame.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, texFrame.data );
    
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
    
	// Disable texturing & enable blending
	glDisable( GL_TEXTURE_2D );
    glEnable(GL_BLEND);
    
	int currentTime = glutGet( GLUT_ELAPSED_TIME );

	// Validation for sabers
	bool saber1_Val = saber1_b1 != NULL && saber1_b2 != NULL && saber1 && portionDrawn_1 > .1 && (currentTime - saber1_time) < 300;
	bool saber2_Val = saber2_b1 != NULL && saber2_b2 != NULL && saber2 && portionDrawn_2 > .1 && (currentTime - saber2_time) < 300;
		
	// Bluring enabled for sabers?
	bool saber1_Blur = saber1_Val && (abs(s1_b1_dX) > .3 || abs(s1_b1_dY) > .3);
	bool saber2_Blur = saber2_Val && (abs(s2_b1_dX) > .3 || abs(s2_b1_dY) > .3);

	// Draw saber 1 and 2
	if(saber1_Val) drawSaber(saber1_b1, saber1_b2, saber1_r, saber1_g, saber1_b, 1, portionDrawn_1);
	if(saber2_Val) drawSaber(saber2_b1, saber2_b2, saber2_r, saber2_g, saber2_b, 1, portionDrawn_2);

	// Draw blur
    for (int i = 0; i < blurCount*blurCount; i++)
    {
		// Only draw if change dx or dy is significant
        if(saber1_Blur) drawSaber(saber1_b1_inter[i], saber1_b2_inter[i], saber1_r, saber1_g, saber1_b, 1-(i*1.0/(blurCount*blurCount)), portionDrawn_1);
        if(saber2_Blur) drawSaber(saber2_b1_inter[i], saber2_b2_inter[i], saber2_r, saber2_g, saber2_b, 1-(i*1.0/(blurCount*blurCount)), portionDrawn_2);
    }

    glDisable(GL_BLEND);
    
	}

	// Double buffering
	glutSwapBuffers();
}


/*********************************************************
 * Method: idle
 * Purpose: callback for system idling (no events)
 *********************************************************/
void idle()
{	

	if(openerTime < openTime){
		
		openerTime = glutGet( GLUT_ELAPSED_TIME ) - lastOpenerTime;
		printf("%d\n", openerTime);
	}
	else{
	// Saber 1 Animation On
	if(saber1 && portionDrawn_1 < 1){
		
		// Calculate portion
		int currTime = glutGet( GLUT_ELAPSED_TIME );
		portionDrawn_1 = (currTime - lastTime_1)/aniTime;
		
		// Portion max is 1
		if(portionDrawn_1 > 1) portionDrawn_1 = 1;
	}

	// Saber 2 Animation On
	if(saber2 && portionDrawn_2 < 1){
		
		// Calculate portion
		int currTime = glutGet( GLUT_ELAPSED_TIME );
		portionDrawn_2 = (currTime - lastTime_2)/aniTime;
		
		// Portion max is 1
		if (portionDrawn_2 > 1) portionDrawn_2 = 1;
	}

	// Get one frame, exit on error
	frame = cvQueryFrame( capture );
	if( !frame ){ fprintf( stderr, "ERROR: frame is null...\n" );  exit(-1); }
    
	// Covert color space to HSV
	cvCvtColor(frame, hsv_frame, CV_BGR2HSV);


	/******************************************
	 * Saber 1 Detection
     *****************************************/
    
	// Saber 1 ranges
	CvScalar hsv_min = cvScalar(saber1_c1, saber1_c2, saber1_c3);
	CvScalar hsv_max = cvScalar(saber1_cm1, saber1_cm2, saber1_cm3);
    
	// Filter out colors which are out of range
	cvInRangeS(hsv_frame, hsv_min, hsv_max, thresholded_1);
    
	// Smooth image	
	cvSmooth( thresholded_1, thresholded_1, CV_MEDIAN, 3,3 );
	
	// Display smoothed image if window exist
	if(showThres1) cvShowImage( "Threshold - 1", thresholded_1 );

	// Erode -> Dilate -> Smooth
	cvErode(thresholded_1,thresholded_1,NULL,1);
    cvDilate( thresholded_1, thresholded_1, NULL, 9 );
	cvSmooth( thresholded_1, thresholded_1, CV_MEDIAN, 9, 9 );

	// Convert to material
	cv::Mat thresholdImg = thresholded_1;
    
	// Find contours
	cv::findContours( thresholdImg, contours_1, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1 );
    
	// Contours refinement
	contoursPoly_1.resize( contours_1.size() );
	center_1.resize( contours_1.size() );
	radius_1.resize( contours_1.size() );
    
	// Refine contours and compute BB and min enclosing circle.
	for( size_t i = 0; i < contours_1.size(); i++ ) {
        
		// Approximates the contours
		cv::approxPolyDP( cv::Mat( contours_1[i] ), contoursPoly_1[i], 3.0, true );

		// Finds a circle with min area enclosing a 2D point set.
		cv::minEnclosingCircle( cv::Mat( contoursPoly_1[i] ), center_1[i], radius_1[i] );
    }
    
	texFrame= frame;		// convert frame to material
	int count = 0;			// start count at 0
	int index1 = -1;		// null index1
	int index2 = -1;		// null index2

	// Check contours
	for( size_t i = 0; i< contours_1.size(); i++ ) {
        
		if(contoursPoly_1[i].size() > minPoints){
			if(count == 0){
				index1 = i;
				count++;
			}
			else if(count == 1){
				index2 = i;
				count++;
			}
			/***************************************************************************
			 *                       Draw the circles detected                         *
			cv::Scalar color = cv::Scalar( 255, 0, 255 );
			cv::drawContours( texFrame, contoursPoly_1, i, color, 1, 8, 
				std::vector<cv::Vec4i>(), 0, cv::Point() );
			cv::rectangle( texFrame, bb_1[i].tl(), bb_1[i].br(), color, 2, 8, 0 );
			cv::circle( texFrame, center_1[i], (int)radius_1[i], color, 2, 8, 0 );
			***************************************************************************/
		}
	}

	// If two circles are found
	if(index1 >= 0 && index2 >= 0){

		saber1_time = glutGet( GLUT_ELAPSED_TIME );

		// Null sabers ?
		if(saber1_b1 != NULL && saber1_b2 != NULL){
			
			// Calculate all distances
			double distance1_1 = distance(saber1_b1, center_1[index1].x, center_1[index1].y);
			double distance2_1 = distance(saber1_b2, center_1[index1].x, center_1[index1].y);
			double distance1_2 = distance(saber1_b1, center_1[index2].x, center_1[index2].y);
			double distance2_2 = distance(saber1_b2, center_1[index2].x, center_1[index2].y);

			// Determine if 1_2 or 2_1 are the min
			bool min_1_2_or_2_1 = (distance2_1 <= distance1_2 && distance2_1 <= distance1_1 &&
				distance2_1 <= distance2_2) || (distance1_2 <= distance2_1 && distance1_2 <= 
				distance1_1 && distance1_2 <= distance2_2);

			// If 1_2 or 2_1 then switch balls
			if(min_1_2_or_2_1){
				saber1_b1[0] = center_1[index2].x;
				saber1_b1[1] = center_1[index2].y;
				saber1_b1[2] = radius_1[index2];

				saber1_b2[0] = center_1[index1].x;
				saber1_b2[1] = center_1[index1].y;
				saber1_b2[2] = radius_1[index1];	
			}

			// Otherwise set balls as usual
			else{
				saber1_b1[0] = center_1[index1].x;
				saber1_b1[1] = center_1[index1].y;
				saber1_b1[2] = radius_1[index1];

				saber1_b2[0] = center_1[index2].x;
				saber1_b2[1] = center_1[index2].y;
				saber1_b2[2] = radius_1[index2];	
			}
		}

		// Since null sabers, set balls as usual
		else {
			saber1_b1[0] = center_1[index1].x;
			saber1_b1[1] = center_1[index1].y;
			saber1_b1[2] = radius_1[index1];
			
			saber1_b2[0] = center_1[index2].x;
			saber1_b2[1] = center_1[index2].y;
			saber1_b2[2] = radius_1[index2];	
		}
	}

    // Move all past points by 1
    for (int i = 3 - 1; i >= 1; i--){

		// Swap #1
        saber1_b1_past[i][0] = saber1_b1_past[i-1][0];
        saber1_b1_past[i][1] = saber1_b1_past[i-1][1];
        saber1_b1_past[i][2] = saber1_b1_past[i-1][2];
        
		// Swap #2
        saber1_b2_past[i][0] = saber1_b2_past[i-1][0];
        saber1_b2_past[i][1] = saber1_b2_past[i-1][1];
        saber1_b2_past[i][2] = saber1_b2_past[i-1][2];
    }
    
	// Set new saber
    saber1_b1_past[0][0] = saber1_b1[0];
    saber1_b1_past[0][1] = saber1_b1[1];
    saber1_b1_past[0][2] = saber1_b1[2];
    
    saber1_b2_past[0][0] = saber1_b2[0];
    saber1_b2_past[0][1] = saber1_b2[1];
    saber1_b2_past[0][2] = saber1_b2[2];
    
	// Start distances at 0
	s1_b1_dX = 0;
	s1_b1_dY = 0;

	// For every pair of past points
	for (int i = 0; (i + 1) < 3; i += 2){

		// Calculate distances
		double dX_b1 = (saber1_b1_past[i + 1][0] - saber1_b1_past[i][0])/(1.0 * ((blurCount*blurCount)/2.0));
		double dY_b1 = (saber1_b1_past[i + 1][1] - saber1_b1_past[i][1])/(1.0 * ((blurCount*blurCount)/2.0));
		double dZ_b1 = (saber1_b1_past[i + 1][2] - saber1_b1_past[i][2])/(1.0 * ((blurCount*blurCount)/2.0));
		double dX_b2 = (saber1_b2_past[i + 1][0] - saber1_b2_past[i][0])/(1.0 * ((blurCount*blurCount)/2.0));
		double dY_b2 = (saber1_b2_past[i + 1][1] - saber1_b2_past[i][1])/(1.0 * ((blurCount*blurCount)/2.0));
		double dZ_b2 = (saber1_b2_past[i + 1][2] - saber1_b2_past[i][2])/(1.0 * ((blurCount*blurCount)/2.0));
		
		// Add distances
		s1_b1_dX += dX_b1;
		s1_b1_dY += dY_b1;

		// Interpolate points
		for (int k = i*(blurCount*blurCount)/2; k < ((i*(blurCount*blurCount)/2) + (blurCount*blurCount)/2); k++){

			// Ball 1
			saber1_b1_inter[i + k][0] = saber1_b1_past[i][0] + dX_b1*(k);
			saber1_b1_inter[i + k][1] = saber1_b1_past[i][1] + dY_b1*(k);
			saber1_b1_inter[i + k][2] = saber1_b1_past[i][2] + dZ_b1*(k);
			
			// Ball 2
			saber1_b2_inter[i + k][0] = saber1_b2_past[i][0] + dX_b2*(k);
			saber1_b2_inter[i + k][1] = saber1_b2_past[i][1] + dY_b2*(k);
			saber1_b2_inter[i + k][2] = saber1_b2_past[i][2] + dZ_b2*(k);
		}
	}


	/******************************************
	 * Saber 2 Detection
     *****************************************/

	// Saber 2 ranges
	CvScalar hsv_min_2 = cvScalar(saber2_c1, saber2_c2, saber2_c3);
	CvScalar hsv_max_2 = cvScalar(saber2_cm1, saber2_cm2, saber2_cm3);
    
	// Filter out colors which are out of range
	cvInRangeS(hsv_frame, hsv_min_2, hsv_max_2, thresholded_2);

	// DSmooth image
	cvSmooth( thresholded_2, thresholded_2, CV_MEDIAN, 3, 3 );
	
	// Display smoothed image if window exist
	if(cvGetWindowHandle("Saber - 2")) cvShowImage( "Threshold - 2", thresholded_2 );
	
	// Erode -> Dilate -> Smooth
    cvErode(thresholded_2,thresholded_2,NULL,1);
    cvDilate( thresholded_2, thresholded_2, NULL, 9 );
	cvSmooth( thresholded_2, thresholded_2, CV_MEDIAN, 9, 9 );

	// Convert to material
	cv::Mat thresholdImg_2 = thresholded_2;
    
	// Find contours
	cv::findContours( thresholdImg_2, contours_2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1 );
    
	// Contours refinement
	contoursPoly_2.resize( contours_2.size() );
	center_2.resize( contours_2.size() );
	radius_2.resize( contours_2.size() );
    
	// Refine contours and compute BB and min enclosing circle.
	for( size_t i = 0; i < contours_2.size(); i++ ) {
        
		// Approximates the contours
		cv::approxPolyDP( cv::Mat( contours_2[i] ), contoursPoly_2[i], 3.0, true );
        
		// Finds a circle with min area enclosing a 2D point set.
		cv::minEnclosingCircle( cv::Mat( contoursPoly_2[i] ), center_2[i], radius_2[i] );
    }

	texFrame= frame;		// convert frame to material
	count = 0;				// start count at 0
	index1 = -1;			// null index1
	index2 = -1;			// null index2


	// Check contours
	for( size_t i = 0; i< contours_2.size(); i++ ) {
        
		if(contoursPoly_2[i].size() > minPoints){
			if(count == 0){
				index1 = i;
				count++;
			}
			else if(count == 1){
				index2 = i;
				count++;
			}
			/***************************************************************************
			 *                       Draw the circles detected                         *
			cv::Scalar color = cv::Scalar( 255, 0, 255 );
			cv::drawContours( texFrame, contoursPoly_2, i, color, 1, 8, 
				std::vector<cv::Vec4i>(), 0, cv::Point() );
			cv::rectangle( texFrame, bb_2[i].tl(), bb_2[i].br(), color, 2, 8, 0 );
			cv::circle( texFrame, center_2[i], (int)radius_2[i], color, 2, 8, 0 );
			***************************************************************************/
		}
	}

	// If two circles are found
	if(index1 >= 0 && index2 >= 0){

		saber2_time = glutGet( GLUT_ELAPSED_TIME );

		// Null sabers ?
		if(saber2_b1 != NULL && saber2_b2 != NULL){

			// Calculate all distances
			double distance1_1 = distance(saber2_b1, center_2[index1].x, center_2[index1].y);
			double distance2_1 = distance(saber2_b2, center_2[index1].x, center_2[index1].y);
			double distance1_2 = distance(saber2_b1, center_2[index2].x, center_2[index2].y);
			double distance2_2 = distance(saber2_b2, center_2[index2].x, center_2[index2].y);
			
			// Determine if 1_2 or 2_1 are the min
			bool min_1_2_or_2_1 = (distance2_1 <= distance1_2 && distance2_1 <= distance1_1 &&
				distance2_1 <= distance2_2) || (distance1_2 <= distance2_1 && distance1_2 <= 
				distance1_1 && distance1_2 <= distance2_2);

			// If 1_2 or 2_1 then switch balls
			if(min_1_2_or_2_1){
				saber2_b1[0] = center_2[index2].x;
				saber2_b1[1] = center_2[index2].y;
				saber2_b1[2] = radius_2[index2];

				saber2_b2[0] = center_2[index1].x;
				saber2_b2[1] = center_2[index1].y;
				saber2_b2[2] = radius_2[index1];	
			}
			
			// Otherwise set balls as usual
			else{
				saber2_b1[0] = center_2[index1].x;
				saber2_b1[1] = center_2[index1].y;
				saber2_b1[2] = radius_2[index1];

				saber2_b2[0] = center_2[index2].x;
				saber2_b2[1] = center_2[index2].y;
				saber2_b2[2] = radius_2[index2];	
			}
		}

		// Since null sabers, set balls as usual
		else {
			saber2_b1[0] = center_2[index1].x;
			saber2_b1[1] = center_2[index1].y;
			saber2_b1[2] = radius_2[index1];
			
			saber2_b2[0] = center_2[index2].x;
			saber2_b2[1] = center_2[index2].y;
			saber2_b2[2] = radius_2[index2];	
		}
	}
    
	// Move all past points by 1
    for (int i = 3 - 1; i >= 1; i--){

		// Swap #1
        saber2_b1_past[i][0] = saber2_b1_past[i-1][0];
        saber2_b1_past[i][1] = saber2_b1_past[i-1][1];
        saber2_b1_past[i][2] = saber2_b1_past[i-1][2];
        
		// Swap #2
        saber2_b2_past[i][0] = saber2_b2_past[i-1][0];
        saber2_b2_past[i][1] = saber2_b2_past[i-1][1];
        saber2_b2_past[i][2] = saber2_b2_past[i-1][2];
    }
    
	// Set new saber
    saber2_b1_past[0][0] = saber2_b1[0];
    saber2_b1_past[0][1] = saber2_b1[1];
    saber2_b1_past[0][2] = saber2_b1[2];
    
    saber2_b2_past[0][0] = saber2_b2[0];
    saber2_b2_past[0][1] = saber2_b2[1];
    saber2_b2_past[0][2] = saber2_b2[2];
    
	// Start distances at 0
	s2_b1_dX = 0;
	s2_b1_dY = 0;

	// For every pair of past points
	for (int i = 0; (i + 1) < 3; i += 2){

		// Calculate distances
		double dX_b1 = (saber2_b1_past[i + 1][0] - saber2_b1_past[i][0])/(1.0 * ((blurCount*blurCount)/2.0));
		double dY_b1 = (saber2_b1_past[i + 1][1] - saber2_b1_past[i][1])/(1.0 * ((blurCount*blurCount)/2.0));
		double dZ_b1 = (saber2_b1_past[i + 1][2] - saber2_b1_past[i][2])/(1.0 * ((blurCount*blurCount)/2.0));
		double dX_b2 = (saber2_b2_past[i + 1][0] - saber2_b2_past[i][0])/(1.0 * ((blurCount*blurCount)/2.0));
		double dY_b2 = (saber2_b2_past[i + 1][1] - saber2_b2_past[i][1])/(1.0 * ((blurCount*blurCount)/2.0));
		double dZ_b2 = (saber2_b2_past[i + 1][2] - saber2_b2_past[i][2])/(1.0 * ((blurCount*blurCount)/2.0));
		
		// Add distances
		s2_b1_dX += dX_b1;
		s2_b1_dY += dY_b1;

		// Interpolate points
		for (int k = i*(blurCount*blurCount)/2; k < ((i*(blurCount*blurCount)/2) +(blurCount*blurCount)/2); k++){

			// Ball 1
			saber2_b1_inter[i + k][0] = saber2_b1_past[i][0] + dX_b1*(k);
			saber2_b1_inter[i + k][1] = saber2_b1_past[i][1] + dY_b1*(k);
			saber2_b1_inter[i + k][2] = saber2_b1_past[i][2] + dZ_b1*(k);
			
			// Ball 2
			saber2_b2_inter[i + k][0] = saber2_b2_past[i][0] + dX_b2*(k);
			saber2_b2_inter[i + k][1] = saber2_b2_past[i][1] + dY_b2*(k);
			saber2_b2_inter[i + k][2] = saber2_b2_past[i][2] + dZ_b2*(k);
		}
	}
	}
    
    glutPostRedisplay();
}


/*********************************************************
 * Method: main
 * Purpose: main program
 *********************************************************/
int main(int argc, char** argv){

	/*
	
	It is a period of summer 2013. Garrick Brazil, Devin Holland,
	Ed Cana, Jeffrey Ring have completed their first OpenGL and
	OpenCV application.
	
	This is it...
	*/
	strcpy(quote[0],"It is a period of summer 2013");
    strcpy(quote[1],"Garrick Brazil, Devin Holland,");
    strcpy(quote[2],"Ed Cana, Jefferey Ring have");
    strcpy(quote[3],"completed their first OpenGL and");
    strcpy(quote[4],"OpenCV application.");
    numberOfQuotes=5;
	glClearColor(0.0, 0.0, 0.0, 1.0);
    glLineWidth(3);

	// Open capture device, exit on error
	capture = cvCaptureFromCAM( 1 );
	if( !capture ){ fprintf( stderr, "ERROR: capture is NULL \n" ); return -1; }
    
	// Get one frame, exit on error
	frame = cvQueryFrame( capture );
	if( !frame ){ fprintf( stderr, "ERROR: frame is null...\n" ); exit(-1); }
    
    
	// Modified images, based on first frame size
	hsv_frame = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 3);
	thresholded_1 = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 1);
	thresholded_2 = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 1);
    
	// Setup the glut window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_ALPHA);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowX, windowY);
    glutCreateWindow("CS-420 - Light Saber");
    init();

	// Callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle);
    
	lastOpenerTime = glutGet( GLUT_ELAPSED_TIME );
	openerTime = 0;

	// Start loop
    glutMainLoop();
    
	// Release & destroy
    cvReleaseCapture( &capture );
	cvReleaseImage(&hsv_frame);
	cvReleaseImage(&frame);
	cvReleaseImage(&thresholded_1);
	cvReleaseImage(&thresholded_2);
	texFrame.release();
	cvDestroyWindow("CS-420 - Light Saber");

    return 0;
}