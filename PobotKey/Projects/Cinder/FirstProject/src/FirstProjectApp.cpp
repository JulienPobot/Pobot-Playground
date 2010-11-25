#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class FirstProjectAppApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void FirstProjectAppApp::setup()
{
}

void FirstProjectAppApp::mouseDown( MouseEvent event )
{
}

void FirstProjectAppApp::update()
{
}

void FirstProjectAppApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_BASIC( FirstProjectAppApp, RendererGl )
