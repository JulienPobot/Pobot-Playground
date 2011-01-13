#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Profondeur3DAppApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void Profondeur3DAppApp::setup()
{
}

void Profondeur3DAppApp::mouseDown( MouseEvent event )
{
}

void Profondeur3DAppApp::update()
{
}

void Profondeur3DAppApp::draw()
{
	// clear out the window with black
	gl::clear( Color( sin(, 0, 0 ) ); 
}

CINDER_APP_BASIC( Profondeur3DAppApp, RendererGl )
