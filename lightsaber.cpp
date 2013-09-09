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

int bx = 100, by = 100, bz = 100;

// Video and frame
VideoCapture videoCapture;
Mat frame;

// Texture
GLuint videoTexture;

/*********************************************************
 * Method: drawAxis
 * Purpose: draws axis with given length and width
*********************************************************/
void drawAxis(float l, float w, bool negative){

	// Saving OpenGL attributes: color
	glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);

	// Set line width
	glLineWidth(w);

	// Draw axis using the proper colors
	glBegin(GL_LINES);
		glColor3f( 1.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(l, 0.0f, 0.0f);
		glColor3f(0.0f, 10.f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, l, 0.0f);
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, l);
	glEnd();

	if(negative){

		// Draw axis using negative number
		glBegin(GL_LINES);
			glColor3f( 0.5f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(l, 0.0f, 0.0f);
			glColor3f(0.0f, 0.5f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, l, 0.0f);
			glColor3f(0.0f, 0.0f, 0.5f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.0f, l);
		glEnd();
	}

	glPopAttrib();

}


void renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions,GLUquadricObj *quadric){
	float vx = x2-x1;
	float vy = y2-y1;
	float vz = z2-z1;

	//handle the degenerate case of z1 == z2 with an approximation
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
	void renderCylinder_convenient(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions)
	{
	//the same quadric can be re-used for drawing many cylinders
	GLUquadricObj *quadric=gluNewQuadric();
	gluQuadricNormals(quadric, GLU_SMOOTH);
	renderCylinder(x1,y1,z1,x2,y2,z2,radius,subdivisions,quadric);
	gluDeleteQuadric(quadric);
}

/*********************************************************
 * Method: draw
 * Purpose: draws the lightsaber scene
*********************************************************/
void draw(Vec3f po1, Vec3f po2,int width,int height)
{

	Vec3f p1, p2;
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
 * Method: modifyFrame
 * Purpose: modifies frame for lightsaber scene
*********************************************************/
void modifyFrame(Mat inputFrame)
{
	/*
	Mat blurred,hsv,inRangeOutput,afterCopy,afterMult,white;
	Vec3b color;
	int r = 220, g = 15, b = 15;
	int lr = 71, lg = 47, lb = 146;
	int ur = 138, ug=106, ub = 245;
    color[0]=0;
    color[1]=0;
    color[2]=255;

	GaussianBlur(inputFrame, blurred, Size(11,11),0,0);		//reduce noise
    cvtColor(blurred,hsv,CV_BGR2HSV);						//HSV is bette

	inRange(hsv,Scalar(lr,lg,lb),Scalar(ur,ug,ub),inRangeOutput);	//pick red and convert it to a binary mask
	GaussianBlur(inRangeOutput,afterCopy,Size(101,101),0,0);		//glow effect
    cvtColor(afterCopy,afterCopy,CV_GRAY2BGR);
    cvtColor(inRangeOutput,white,CV_GRAY2BGR);
                
	for(int i=0;i<afterCopy.size().height;i++)
            for(int j=0;j<afterCopy.size().width;j++)
            {
                    Vec3b& s=afterCopy.at<Vec3b>(i,j);
                    s[0]=(s[0]/255.0)*color[0];
                    s[1]=(s[1]/255.0)*color[1];
                    s[2]=(s[2]/255.0)*color[2];
            }
    inputFrame +=(afterCopy*5.0);
	inputFrame +=white;
	*/

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

	
	// Setting material properties.
	/*glMaterialfv( GL_FRONT, GL_AMBIENT, matAmbient );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, matDiffuse );
	glMaterialfv( GL_FRONT, GL_SPECULAR, matSpecular );
	glMaterialfv( GL_FRONT, GL_EMISSION, matEmission );
	glMaterialfv( GL_FRONT, GL_SHININESS, matShininess ); */
	// See: http://www.opengl.org/sdk/docs/man2/xhtml/glColorMaterial.xml
	glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	//glEnable( GL_COLOR_MATERIAL );
	// Setting light 0 properties.
	/*glLightfv( GL_LIGHT0, GL_AMBIENT, lAmbient );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, lDiffuse );
	glLightfv( GL_LIGHT0, GL_SPECULAR, lSpecular );
	// Setting light 1 properties.
	glLightfv( GL_LIGHT1, GL_AMBIENT, lAmbient );
	glLightfv( GL_LIGHT1, GL_DIFFUSE, lDiffuse );
	glLightfv( GL_LIGHT1, GL_SPECULAR, lSpecular );
	// Setting lighting model.
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lModelAmbient );
	glLightModelfv( GL_LIGHT_MODEL_LOCAL_VIEWER, lModelLocalViewer );
	
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHT1 );
	// Enabling depht test.
	*/
	glEnable( GL_DEPTH_TEST );
	// Enabling blending.
	glEnable( GL_BLEND );
	// See: http://www.opengl.org/sdk/docs/man/xhtml/glBlendFunc.xml
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

	glViewport( 0, 0, (GLsizei) windowWidth, (GLsizei) windowHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 60.0, (GLfloat) windowWidth / (GLfloat) windowHeight, 
		0.1, 10.0 );

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
    gluLookAt(.5, .5, 1, .5, .5, 0.0, 0.0, 1, 0 );
	drawAxis(2.0f, 2.0f, true);

	// Texturing
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, videoTexture );
	
    modifyFrame(frame);

	 vector<Vec3f> circles;
	 Mat src_gray,modFrame;
	 modFrame=frame.clone();
	 /// Convert it to gray
  cvtColor( modFrame, src_gray, CV_BGR2GRAY);

  /// Reduce the noise so we avoid false circle detection
  GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );
  /// Apply the Hough Transform to find the circles
  HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows/16, 100, 50, 0, 0 );

  /// Draw the circles detected
  for( size_t i = 0; i < circles.size(); i++ )
  {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // circle center
      circle( modFrame, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      circle( modFrame, center, radius, Scalar(0,0,255), 3, 8, 0 );
   }

	if(true){
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc( GL_SRC_ALPHA_SATURATE, GL_ONE );
		if(circles.size() == 2) draw(circles[0], circles[1], frame.size().width, frame.size().height);
	}

	if( modFrame.data)
    {
		glEnable(GL_TEXTURE_2D);
        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, modFrame.size().width, modFrame.size().height, 0, GL_BGR, GL_UNSIGNED_BYTE, modFrame.data );
    }
	
	
	double ratio = modFrame.size().width*1.0/modFrame.size().height;
    glPushMatrix();
	glDisable(GL_BLEND);
	// Configure texture mapping flipping (vertical) the video frame
	glBegin( GL_QUADS );
		glTexCoord2f( 0.0f, 1.0f ); glVertex3f( 0, 0.0f, 0.0f );
		glTexCoord2f( 1.0f, 1.0f ); glVertex3f( ratio, 0.0f, 0.0f );
		glTexCoord2f( 1.0f, 0.0f ); glVertex3f( ratio, 1.0f, 0.0f );
		glTexCoord2f( 0.0f, 0.0f ); glVertex3f( 0, 1.0f, 0.0f );
    glEnd();
	
	

	glDisable( GL_TEXTURE_2D );
    glPopMatrix();

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
	//videoCapture.grab();
	//videoCapture.retrieve(frame);

    glutPostRedisplay();
}

/*********************************************************
 * Method: main
 * Purpose: main program
*********************************************************/
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
	//videoCapture.grab();
	//videoCapture.retrieve(frame);
    frame = imread( "Regular_Circles.jpg", CV_LOAD_IMAGE_COLOR );
	if(frame.data){}
	else printf("BLAH");
	// Set window to frame size
    windowWidth = frame.size().width;
    windowHeight = frame.size().height;
    
    // Setup the glut window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(windowX, windowY);
    glutCreateWindow("CS-420 - Light Saber");
	namedWindow("Settings", CV_WINDOW_AUTOSIZE);
    
	createTrackbar("bx","Settings",&bx,200,NULL);
	createTrackbar("by","Settings",&by,200,NULL);
	createTrackbar("bz","Settings",&bz,200,NULL);
	imshow("Settings", frame);

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