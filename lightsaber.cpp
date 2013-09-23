/***************************************************************
 *	Garrick Brazil, Edward Cana, Devin Holland, Jeffrey Ring
 *	CS-420: Lightsaber Fight
 *	Dr. Giuseppe Turini
 *	Created on 8/28/2013
/****************************************************************/


#include <gl/glew.h>
#include <Windows.h>
#include <gl/freeglut.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "vector_3d.h"
#include "image_tga.h"


using namespace cv;

// Images and textures.
ImageTGA img1;
GLuint tex1;

cv::Size size = cv::Size(400,300);

char quote[8][80];
int numberOfQuotes=0,i;
double openTime = 24000;
int openerTime, lastOpenerTime;
bool opener = false;

// Saber 1 Settings
int saber1_h_Min = 45;
int saber1_s_Min = 84;
int saber1_v_Min = 75;
int saber1_h_Max = 65;
int saber1_s_Max = 167;
int saber1_v_Max = 255;
int saber1_color;

// Saber 1 Color
float saber1_r;
float saber1_g;
float saber1_b;
int saber1_time;

// Saber 2 Settings
int saber2_h_Min = 138;
int saber2_s_Min = 64;
int saber2_v_Min = 22;
int saber2_h_Max = 162;
int saber2_s_Max = 148;
int saber2_v_Max = 71;
int saber2_color;

// Saber 2 Color
float saber2_r;
float saber2_g;
float saber2_b;
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

// Time and FPS.
int lastFPSTime = 0;
int frameCount = 0;
float fps = 0.0f;
char fpsText[ 20 ];

// Sabers enabling
bool saber1 = false;
bool saber2 = false;

// Current sabers
float saber1_b1[3], saber1_b2[3];
float saber2_b1[3], saber2_b2[3];

// Misc
int minPoints = 5;
int cylinderLayers = 60;
const int blurCount = 6;
int camera;

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

// Images used for processing
VideoCapture capture;
GLuint videoTexture;
Mat frame, smallFrame;
IplImage *hsv_frame;
IplImage *thresholded_1, *thresholded_2;

// Containers and images.
std::vector<cv::Vec3f> circles_1, circles_2;
std::vector< std::vector<cv::Point> > contours_1, contours_2;
std::vector< std::vector<cv::Point> > contoursPoly_1, contoursPoly_2;
std::vector<cv::Rect> bb_1, bb_2;
std::vector<cv::Point2f> center_1, center_2;
std::vector<float> radius_1, radius_2;


/*********************************************************
 * Method: trackbarChange
 * Purpose: update the settings file when one of the trackbar changes
 *********************************************************/
void trackbarChange( int , void* )
{
    string filename = "Settings.xml";
    FileStorage fs(filename, FileStorage::WRITE);
    fs.open(filename, FileStorage::WRITE);
    
    fs << "saber1_h_Min" << saber1_h_Min;
    fs << "saber1_s_Min" << saber1_s_Min;
    fs << "saber1_v_Min" << saber1_v_Min;
    fs << "saber1_h_Max" << saber1_h_Max;
    fs << "saber1_s_Max" << saber1_s_Max;
    fs << "saber1_v_Max" << saber1_v_Max;
    fs << "saber1_color" << saber1_color;
    
    fs << "saber2_h_Min" << saber2_h_Min;
    fs << "saber2_s_Min" << saber2_s_Min;
    fs << "saber2_v_Min" << saber2_v_Min;
    fs << "saber2_h_Max" << saber2_h_Max;
    fs << "saber2_s_Max" << saber2_s_Max;
    fs << "saber2_v_Max" << saber2_v_Max;
	fs << "saber2_color" << saber2_color;
    
    fs << "camera" << camera;
    
    fs.release();
}


/*********************************************************
 * Method: calculateFPS
 * Purpose: calculates the FPS of the application
 *********************************************************/
void calculateFPS()
{
	// Increase frame count
	frameCount++;
	
	// Get the number of milliseconds in currentTime
	int currTime = glutGet( GLUT_ELAPSED_TIME );
	
	// Calculate time passed.
	int timeInterval = currTime - lastFPSTime;
	
	if( timeInterval > 1000 ){
		// Compute the number of frames per second.
		fps = (float) frameCount / ( (float) timeInterval / 1000.0f );
		
		// Set time.
		lastFPSTime = currTime;
		
		// Reset frame count.
		frameCount = 0;
		
		// Store frames per second in a string.
		sprintf_s( fpsText, 20, "%.3f fps", fps ); 
	}
}


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

	trackbarChange(color,nothing);
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

	trackbarChange(color,nothing);
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
	int width  =  smallFrame.size().width;
	int height =  smallFrame.size().height;
    

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
    			
	glPushMatrix();

		// Draw all cylinder layers
		for (int i = 0; i < cylinderLayers; i++){			
        
			// Calculate radius 
			rad = (.28 +  (1.1*i*i*i*i*i)/(1.0*cylinderLayers*cylinderLayers*cylinderLayers*cylinderLayers*cylinderLayers))*p1[2]/width;

			// Solid white middle
			if (i == 0) glColor4f(1*trans,1*trans,1*trans,1*trans);

			// Saber color multiplied by transparency (white = transparent)
			else glColor4f(r*(1-(1.0*i)/cylinderLayers)*trans, g*(1-(1.0*i)/cylinderLayers)*trans, b*(1-(1.0*i)/cylinderLayers)*trans, 1*trans);
        
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
	
	glGenTextures( 1, &tex1 );
	glBindTexture(GL_TEXTURE_2D, tex1);
	// See: http://www.opengl.org/sdk/docs/man/xhtml/glTexParameter.xml
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	// See: http://www.opengl.org/sdk/docs/man2/xhtml/glTexEnv.xml
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
	// See: http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml
	glTexImage2D( GL_TEXTURE_2D, 0, img1.format(), img1.width(), img1.height(), 0, img1.format(), GL_UNSIGNED_BYTE, img1.data() );

	
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
		case ' ':{
			openerTime = 50000;
			capture.grab();
			if( !capture.retrieve(frame) ){ fprintf( stderr, "ERROR: frame is null...\n" );  exit(-1); }
		
			break;
		}
		case '1':{

			// Store current time
			lastTime_1 = glutGet( GLUT_ELAPSED_TIME );
			
			if (!saber1 && (lastTime_1 - saber1_time) < 300){ saber1 = !saber1; PlaySound(TEXT("TurnOn.WAV"), NULL, SND_ASYNC); portionDrawn_1 = 0;}
			else if((lastTime_1 - saber1_time) < 300){ saber1 = !saber1; PlaySound(TEXT("TurnOff.WAV"), NULL, SND_ASYNC); portionDrawn_1 = 1;}
			break;
		}

		case '2':{
			
			// Store current time
			lastTime_2 = glutGet( GLUT_ELAPSED_TIME );
			
			if (!saber2 && (lastTime_2 - saber2_time) < 300){ saber2 = !saber2; PlaySound(TEXT("TurnOn.WAV"), NULL, SND_ASYNC); portionDrawn_2 = 0;}
			else if((lastTime_2 - saber2_time) < 300){ saber2 = !saber2; PlaySound(TEXT("TurnOff.WAV"), NULL, SND_ASYNC); portionDrawn_2 = 1;}
			break;
		}

		case '!':{

			if(!showThres1){
                
				// Create a window
				cv::namedWindow("Saber - 1", CV_WINDOW_NORMAL);
                
				// Settings page
				cv::createTrackbar("H Min", "Saber - 1", &saber1_h_Min, 255, trackbarChange);
				cv::createTrackbar("S Min", "Saber - 1", &saber1_s_Min, 255, trackbarChange);
				cv::createTrackbar("V Min", "Saber - 1", &saber1_v_Min, 255, trackbarChange);
				cv::createTrackbar("H Max", "Saber - 1", &saber1_h_Max, 255, trackbarChange);
				cv::createTrackbar("S Max", "Saber - 1", &saber1_s_Max, 255, trackbarChange);
				cv::createTrackbar("V Max", "Saber - 1", &saber1_v_Max, 255, trackbarChange);
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
				cv::createTrackbar("H Min", "Saber - 2", &saber2_h_Min, 255, trackbarChange);
				cv::createTrackbar("S Min", "Saber - 2", &saber2_s_Min, 255, trackbarChange);
				cv::createTrackbar("V Min", "Saber - 2", &saber2_v_Min, 255, trackbarChange);
				cv::createTrackbar("H Max", "Saber - 2", &saber2_h_Max, 255, trackbarChange);
				cv::createTrackbar("S Max", "Saber - 2", &saber2_s_Max, 255, trackbarChange);
				cv::createTrackbar("V Max", "Saber - 2", &saber2_v_Max, 255, trackbarChange);
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

/*********************************************************
 * Method: IntroText
 * Purpose: displays opening text sequence
 *********************************************************/
void IntroText()
{
    int l,lenghOfQuote, i;

	int UpwardsScrollVelocity = -400*(pow((openerTime - 10000),.9)/(pow((openTime - 10000),.9)));

    glTranslatef(-20, -75, UpwardsScrollVelocity);
    glRotatef(-80, 1.0, 0.0, 0.0);
    glScalef(0.08, 0.12, 0.12);


    for(  l=0;l<numberOfQuotes;l++)
    {
        lenghOfQuote = (int)strlen(quote[l]);
        glPushMatrix();
        glTranslatef(-(30*30), -(l*200), 0.0);
		int factor = 0;

        for (i = 0; i < lenghOfQuote; i++)
        {
			glPushMatrix();

			if(l != 5 && l != 7)glTranslatef((2.8*30*27.0*((i+factor)*1.0/lenghOfQuote)), 0, 0.0);
			else if(l == 5) glTranslatef((2.8*35*(i+factor)), 0, 0.0);
			else if(l == 7) glTranslatef((2.8*35*(i+factor)), 0, 0.0);

			if(quote[l][i] == 'm') factor++;

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
		  glLineWidth(3);
		  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		  glViewport( 0, 0, (GLsizei) windowWidth, (GLsizei) windowHeight );
		  glMatrixMode(GL_PROJECTION);
		  glLoadIdentity();
		  gluPerspective(60, 1.0, 1.0, 3200);
		  glMatrixMode(GL_MODELVIEW);

		 glLoadIdentity();
		 gluLookAt(0.0, 30, 100.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		 glClearColor(0.0, 0.0, 0.0, 1.0);
		 float logoSize = 35;

		 // From 0 -> 4000
		 if(openerTime <= 4000){

			 char opener[2][80];
			 strcpy(opener[0], "A couple days ago in room");
			 strcpy(opener[1], "number AB-3509...");

			 int l,lenghOfQuote, i;

			int UpwardsScrollVelocity = -150;

			glTranslatef(-10, 10 + 0*(openerTime)/2000.0, UpwardsScrollVelocity);
			glScalef(0.085, 0.085, 0.085);


			for(  l=0; l < 2; l++)
			{
				lenghOfQuote = (int)strlen(opener[l]);
				glPushMatrix();
				glTranslatef(-(30*35), -(l*200), 0.0);
				int factor = 0;
				for (i = 0; i < lenghOfQuote; i++)
				{
					glPushMatrix();
					if(l != 1) glTranslatef((2.8*30*30.0*((i + factor)*1.0/lenghOfQuote)), 0, 0.0);
					else glTranslatef((2.8*42*(i + factor)), 0, 0.0);
					
					if (opener[l][i] == 'm') factor++;
					glColor3f((UpwardsScrollVelocity/10)+300+(l*10),(UpwardsScrollVelocity/10)+300+(l*10),0.0);
					glutStrokeCharacter(GLUT_STROKE_ROMAN, opener[l][i]);
					glPopMatrix();
				}
				glPopMatrix();
			}
		 }

		 // From 4000 -> 10000
		 else if(openerTime <=10000){
			 
			 if(!opener){
				 opener = true;
				 PlaySound(TEXT("Intro.WAV"), NULL, SND_ASYNC);
			 }

			 //Texture
			 glEnable( GL_TEXTURE_2D );
			 glBindTexture( GL_TEXTURE_2D, tex1 );
			 glColor3f( 1.0f, 0.0f, 0.0f );

			 glTexImage2D( GL_TEXTURE_2D, 0, img1.format(), img1.width(), img1.height(), 0, img1.format(), GL_UNSIGNED_BYTE, img1.data() );
		 
			 glPushMatrix();
			 glTranslatef(0,-150*(openerTime-4000)/(6000.0),-750*(openerTime - 4000)/6000.0);
			 glBegin( GL_QUADS );
				glTexCoord2f( 0.0f, 0.0f ); glVertex3f( -logoSize, -logoSize, 0.0f );
				glTexCoord2f( 1.0f, 0.0f ); glVertex3f( logoSize, -logoSize, 0.0f );
				glTexCoord2f( 1.0f, 1.0f ); glVertex3f( logoSize, logoSize, 0.0f );
				glTexCoord2f( 0.0f, 1.0f ); glVertex3f( -logoSize, logoSize, 0.0f );		 
			glEnd();
			glPopMatrix();
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_TEXTURE_2D);
		}
		else{
			IntroText();
		}
	}


	else{
    
	calculateFPS();

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
	if( frame.data) glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, frame.size().width, frame.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.data );
    
    glPushMatrix();
    
		// Calculate frame aspect ratio
		double ratio = frame.size().width*1.0/frame.size().height;
		
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

	// Validation for saberss
	bool saber1_Val = saber1_b1 != NULL && saber1_b2 != NULL && (saber1 ||(!saber1 && portionDrawn_1 != 0)) && portionDrawn_1 > .1 && (currentTime - saber1_time) < 300;
	bool saber2_Val = saber2_b1 != NULL && saber2_b2 != NULL && (saber2 ||(!saber2 && portionDrawn_2 != 0)) && portionDrawn_2 > .1 && (currentTime - saber2_time) < 300;

	// Bluring enabled for sabers?
	bool saber1_Blur = saber1_Val && (abs(s1_b1_dX) > .75*fps/7.0 || abs(s1_b1_dY) > .75*fps/7.0);
	bool saber2_Blur = saber2_Val && (abs(s2_b1_dX) > .75*fps/7.0 || abs(s2_b1_dY) > .75*fps/7.0);

	// Draw saber 1 and 2
	if(saber1_Val) drawSaber(saber1_b1, saber1_b2, saber1_r, saber1_g, saber1_b, 1, portionDrawn_1);
	if(saber2_Val) drawSaber(saber2_b1, saber2_b2, saber2_r, saber2_g, saber2_b, 1, portionDrawn_2);
	
	// Draw blur
    for (int i = 0; i < blurCount*blurCount; i++)
    {
		// Only draw if change dx or dy is significant
        if(saber1_Blur) drawSaber(saber1_b1_inter[i], saber1_b2_inter[i], saber1_r, saber1_g, saber1_b, (1-(i*1.0/(blurCount*blurCount)))*.04, portionDrawn_1);
        if(saber2_Blur) drawSaber(saber2_b1_inter[i], saber2_b2_inter[i], saber2_r, saber2_g, saber2_b, (1-(i*1.0/(blurCount*blurCount)))*.04, portionDrawn_2);
    }

	// Show framerate
	glPushMatrix();
		glColor3ub(0,255,0);
		glRasterPos2f(1.15,1.035);
		for( size_t i = 0; i < 20; ++i ) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, fpsText[i]);
	glPopMatrix();
	
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
		//printf("%d\n", openerTime);
	}

	else if(opener){
	
		capture.grab();
		if( !capture.retrieve(frame) ){ fprintf( stderr, "ERROR: frame is null...\n" );  exit(-1); }
		PlaySound(NULL, NULL, SND_ASYNC);
		opener = false;
	}
	
	else{
	
	capture.grab();
	if( !capture.retrieve(frame) ){ fprintf( stderr, "ERROR: frame is null...\n" );  exit(-1); }
		
	//pyrDown(frame, smallFrame);
	resize(frame,smallFrame, size);

	// Saber 1 Animation On
	if(saber1 && portionDrawn_1 < 1){

		// Calculate portion
		int currTime = glutGet( GLUT_ELAPSED_TIME );
		portionDrawn_1 = (currTime - lastTime_1)/aniTime;

		// Portion max is 1
		if(portionDrawn_1 > 1) portionDrawn_1 = 1;
	}
	else if(!saber1 && portionDrawn_1 > 0){
		
		// Calculate portion
		int currTime = glutGet( GLUT_ELAPSED_TIME );
		portionDrawn_1 = 1 - (currTime - lastTime_1)/aniTime;

		// Portion max is 1
		if(portionDrawn_1 < 0) portionDrawn_1 = 0;
	}
	
	// Saber 2 Animation On
	if(saber2 && portionDrawn_2 < 1){

		// Calculate portion
		int currTime = glutGet( GLUT_ELAPSED_TIME );
		portionDrawn_2 = (currTime - lastTime_2)/aniTime;

		// Portion max is 1
		if (portionDrawn_2 > 1) portionDrawn_2 = 1;
	}

	else if(!saber2 && portionDrawn_2 > 0){
		
		// Calculate portion
		int currTime = glutGet( GLUT_ELAPSED_TIME );
		portionDrawn_2 = 1 - (currTime - lastTime_2)/aniTime;

		// Portion max is 1
		if(portionDrawn_2 < 0) portionDrawn_2 = 0;
	}

	IplImage temp = smallFrame;

	// Covert color space to HSV
	cvCvtColor(&temp, hsv_frame, CV_BGR2HSV);


	/******************************************
	 * Saber 1 Detection
     *****************************************/
    
	// Saber 1 ranges
	CvScalar hsv_min = cvScalar(saber1_h_Min, saber1_s_Min, saber1_v_Min);
	CvScalar hsv_max = cvScalar(saber1_h_Max, saber1_s_Max, saber1_v_Max);
    
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
    
	int count = 0;			// start count at 0
	int index1 = -1;		// null index1
	int index2 = -1;		// null index2

	// Refine contours and min enclosing circle.
	for( size_t i = 0; i < contours_1.size() && count < 2; i++ ) {
        
		// Approximates the contours
		cv::approxPolyDP( cv::Mat( contours_1[i] ), contoursPoly_1[i], 3.0, true );

		// Finds a circle with min area enclosing a 2D point set.
		cv::minEnclosingCircle( cv::Mat( contoursPoly_1[i] ), center_1[i], radius_1[i] );

		// Check valid points
		if(contoursPoly_1[i].size() > minPoints){
			if(count == 0){
				index1 = i;
				count++;
			}
			else if(count == 1){
				index2 = i;
				count++;
			}
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

	double factorDiv = 1.0 * ((blurCount*blurCount)/2.0);

	// For every pair of past points
	for (int i = 0; (i + 1) < 3; i += 2){

		// Calculate distances
		double dX_b1 = (saber1_b1_past[i + 1][0] - saber1_b1_past[i][0])/factorDiv;
		double dY_b1 = (saber1_b1_past[i + 1][1] - saber1_b1_past[i][1])/factorDiv;
		double dZ_b1 = (saber1_b1_past[i + 1][2] - saber1_b1_past[i][2])/factorDiv;
		double dX_b2 = (saber1_b2_past[i + 1][0] - saber1_b2_past[i][0])/factorDiv;
		double dY_b2 = (saber1_b2_past[i + 1][1] - saber1_b2_past[i][1])/factorDiv;
		double dZ_b2 = (saber1_b2_past[i + 1][2] - saber1_b2_past[i][2])/factorDiv;

		// Add distances
		s1_b1_dX += dX_b1;
		s1_b1_dY += dY_b1;

		// Interpolate points
		for (int k = i*factorDiv; k < (i + 1)*factorDiv; k++){

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
	CvScalar hsv_min_2 = cvScalar(saber2_h_Min, saber2_s_Min, saber2_v_Min);
	CvScalar hsv_max_2 = cvScalar(saber2_h_Max, saber2_s_Max, saber2_v_Max);
    
	// Filter out colors which are out of range
	cvInRangeS(hsv_frame, hsv_min_2, hsv_max_2, thresholded_2);

	// Smooth image
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
    
	count = 0;				// start count at 0
	index1 = -1;			// null index1
	index2 = -1;			// null index2


	// Refine contours and compute BB and min enclosing circle.
	for( size_t i = 0; i < contours_2.size() && count < 2; i++ ) {
        
		// Approximates the contours
		cv::approxPolyDP( cv::Mat( contours_2[i] ), contoursPoly_2[i], 3.0, true );
        
		// Finds a circle with min area enclosing a 2D point set.
		cv::minEnclosingCircle( cv::Mat( contoursPoly_2[i] ), center_2[i], radius_2[i] );

		// Check if valid contour
		if(contoursPoly_2[i].size() > minPoints){
			if(count == 0){
				index1 = i;
				count++;
			}
			else if(count == 1){
				index2 = i;
				count++;
			}
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
		double dX_b1 = (saber2_b1_past[i + 1][0] - saber2_b1_past[i][0])/factorDiv;
		double dY_b1 = (saber2_b1_past[i + 1][1] - saber2_b1_past[i][1])/factorDiv;
		double dZ_b1 = (saber2_b1_past[i + 1][2] - saber2_b1_past[i][2])/factorDiv;
		double dX_b2 = (saber2_b2_past[i + 1][0] - saber2_b2_past[i][0])/factorDiv;
		double dY_b2 = (saber2_b2_past[i + 1][1] - saber2_b2_past[i][1])/factorDiv;
		double dZ_b2 = (saber2_b2_past[i + 1][2] - saber2_b2_past[i][2])/factorDiv;

		// Add distances
		s2_b1_dX += dX_b1;
		s2_b1_dY += dY_b1;

		// Interpolate points
		for (int k = i*factorDiv; k < (i*factorDiv) + factorDiv; k++){

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
    
	
	img1.load( "Logo.tga" );
	strcpy(quote[0],"It is a period of summer");
    strcpy(quote[1],"2013. Garrick Brazil, Devin");
    strcpy(quote[2],"Holland, Ed Cana, and");
    strcpy(quote[3],"Jeffrey Ring have completed");
    strcpy(quote[4],"their first OpenGL and");
	strcpy(quote[5],"OpenCV application.");
	strcpy(quote[6],"");
	strcpy(quote[7],"This is it.");
    numberOfQuotes=8;
	glClearColor(0.0, 0.0, 0.0, 1.0);
    glLineWidth(4);


    string filename = "Settings.xml";
    FileStorage fs(filename, FileStorage::READ);
    fs.open(filename, FileStorage::READ);
    
	fs["saber1_h_Min"] >> saber1_h_Min;
    fs["saber1_s_Min"] >> saber1_s_Min;
    fs["saber1_v_Min"] >> saber1_v_Min;
    fs["saber1_h_Max"] >> saber1_h_Max;
    fs["saber1_s_Max"] >> saber1_s_Max;
    fs["saber1_v_Max"] >> saber1_v_Max;
    fs["saber1_color"] >> saber1_color;
    
    fs["saber2_h_Min"] >> saber2_h_Min;
    fs["saber2_s_Min"] >> saber2_s_Min;
    fs["saber2_v_Min"] >> saber2_v_Min;
    fs["saber2_h_Max"] >> saber2_h_Max;
    fs["saber2_s_Max"] >> saber2_s_Max;
    fs["saber2_v_Max"] >> saber2_v_Max;
    fs["saber2_color"] >> saber2_color;

	fs["camera"] >> camera;

    fs.release();

	// Update colors
	updateColor1(saber1_color, NULL);
	updateColor2(saber2_color, NULL);

	// Open capture device, exit on error
	capture.open(camera);
	//capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    //capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480); 

	if( !capture.isOpened() ){ fprintf( stderr, "ERROR: capture is NULL \n" ); return -1; }
    
	// Get one frame, exit on error
	capture.grab();
	if( !capture.retrieve(frame) ){ fprintf( stderr, "ERROR: frame is null...\n" ); exit(-1); }
    

	smallFrame = Mat(size, frame.depth(), frame.channels());

	// Modified images, based on first frame size
	hsv_frame = cvCreateImage(cvSize(smallFrame.size().width, smallFrame.size().height), IPL_DEPTH_8U, 3);
	thresholded_1 = cvCreateImage(cvSize(smallFrame.size().width, smallFrame.size().height), IPL_DEPTH_8U, 1);
	thresholded_2 = cvCreateImage(cvSize(smallFrame.size().width, smallFrame.size().height), IPL_DEPTH_8U, 1);
    
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
    
	// Start loop
    glutMainLoop();
    
	// Release & destroy
    capture.release();
	cvReleaseImage(&hsv_frame);
	frame.release();
	cvReleaseImage(&thresholded_1);
	cvReleaseImage(&thresholded_2);
	cvDestroyWindow("CS-420 - Light Saber");

    return 0;
}