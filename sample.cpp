#include <iostream>         // these STLs must go first else you get all sorts of errors...
#include <vector>
#include <string>
#include <oslib/oslib.h>

#include "oslAnimLib.h"

PSP_MODULE_INFO("oslAnimLib Sample", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

void Input(); // Input-only code here
void Think(); // Think-only code here
void Draw();  // Draw-only code here

OSL_ANIMATION animExplosion; // our multiple image explosion
OSL_ANIMATION animEpic;      // our epic animation
OSL_ANIMATION animWalker;    // our sprite sheet

int cursorX, cursorY; // ill use this to mark when to make an explosion and where the cursor is

int main(int argc, char* argv[])
{
	//Initialization
	oslInit(0);					        //The lib
	oslInitGfx(OSL_PF_8888, 1); //Gfx
	oslInitConsole();			      //Text

	//Load the files
  animEpic.Load("Epic/epic", // it's base name and path
                75);         // # of frames

  animExplosion.Load("Explosion/Angular Explosion", // this is it's base name and path
                     14 /* there's 14 images in the animation */); // took this explosion from my Kitten Cannon

  animWalker.Load("Walker", /* since it's the same path as the eboot we just put it's base name */
                  28,       // number of frames
                  0, 0,     // starting coordinate on the sprite sheet (0,0)
                  22, 35,   // width x height for each animation FRAME
                  true);    // the sprites are horizontally animated

	//Configure the joypad
	oslSetKeyAutorepeatInit(40);     // set some autorepeat delays
	oslSetKeyAutorepeatInterval(10);

	//Initialize variables
	animEpic.Initialize(0,152,   // position on screen
                      -1,-1,   // width and height, -1 tells it to use it's native dimensions
                      0,74,    // start, end frames - 75 frames so subtract one for the end frame
                      3,       // delay
                      true, true, false); // play it on start up, loop it and don't reverse it
	
  animExplosion.Initialize(0,0,        // position on screen coord, this gets changed to the cursor when X is pressed
                           -1,-1,      // width and height of each frame - pass -1 to either to use the width or height specified when they were loaded
                           0, 13,      // start, end frames - 14 images in this animation, subtract 1
                           2,          // delay (game specific - no timers)
                           false, false, false); // play - no, loop - no, reverse - no

  animWalker.Initialize(240,150,   // poisition coord
                        -1,-1,     // width and height of each animation FRAME - pass -1 if you'd like, it'll use the width or height you used when Load() is called
                        0, 27,     // start,end frames - 28 images in this animation, subtract 1
                        7,         // delay (game specific - no timers)
                        true, true, false); // play - yes, loop - yes, reverse - no


  cursorX = 240; // initial positions
  cursorY = 136;

	while (!osl_quit)
	{
    Input();
    Think();
    Draw();
	}
	
  animEpic.Unload();       // if were done unload
	animExplosion.Unload();  // "
	animWalker.Unload();     // "

	oslEndGfx();
	oslQuit();

	return 0;
}

void Think()
{
  animExplosion.Update();
  animWalker.Update();
  animEpic.Update();
  
  if (cursorX >= 480) cursorX = 480;
  if (cursorX <= 0) cursorX = 0;
  if (cursorY <= 0) cursorY = 0;
  if (cursorY >= 272) cursorY = 272;
}

void Input()
{
	oslReadKeys();

  // deform the walker
	if (osl_keys->held.down) animWalker.setHeight(animWalker.getHeight() + 1);
	if (osl_keys->held.up) animWalker.setHeight(animWalker.getHeight() - 1);
	if (osl_keys->held.left) animWalker.setWidth(animWalker.getWidth() - 1);
	if (osl_keys->held.right) animWalker.setWidth(animWalker.getWidth() + 1);

  // move cursor
	for (int i=24;i<=120;i+=16)
	{
		if (osl_keys->analogX > i) cursorX++;
		if (osl_keys->analogY > i) cursorY++;
		if (osl_keys->analogX < -i) cursorX--;
		if (osl_keys->analogY < -i) cursorY--;
	}

	// increase/decrease animWalker's animation speed
	if (osl_keys->pressed.square)
	{
    if (animWalker.getDelay() > 0)
    {
      animWalker.setDelay(animWalker.getDelay() - 1);
    }
  }
	if (osl_keys->pressed.circle) animWalker.setDelay(animWalker.getDelay() + 1);

  // creates an explosion
  if (osl_keys->pressed.cross)
  {
    animExplosion.setX(cursorX - (animExplosion.getWidth()/2));   // place the explosion
    animExplosion.setY(cursorY - (animExplosion.getHeight()/2));  // "
    animExplosion.setCurrentFrame(animExplosion.getStartFrame()); // set the current frame to the start
    animExplosion.Play(); // play it
  }

  if (osl_keys->pressed.triangle) animExplosion.setLoop(!animExplosion.isLooped()); // this flips whether the explosion should loop play

  if (osl_keys->pressed.select) animEpic.setReverse(!animEpic.isReversed()); // this flips whether the walker is walking backwards

  // rotate the walker
	if (osl_keys->held.L) animWalker.setRotationAngle(animWalker.getRotationAngle() - 2);

	if (osl_keys->held.R) animWalker.setRotationAngle(animWalker.getRotationAngle() + 2);

}

void Draw()
{
  oslStartDrawing();
  oslDrawGradientRect(0,0,480,272,RGB(0,0,128), RGB(0,0,128), RGB(0,255,255), RGB(0,255,255));

	//set the color of the text
	oslSetTextColor(RGBA(255,255,255,255));
	oslSetBkColor(RGBA(0,0,0,0));

	oslPrintf_xy(1,1,"oslAnimLib Sample");
  oslPrintf_xy(1,15,"By SG57");
	oslPrintf_xy(1,35,"L/R Triggers Rotate animWalker");
	oslPrintf_xy(1,45,"X creates an explosion");
	oslPrintf_xy(1,55,"TRIANGLE toggles looping of the explosion animation");
	oslPrintf_xy(1,65,"[] and O change the delay for the anim walker by 1");
	oslPrintf_xy(1,75,"Select to reverse epic animation");
	oslPrintf_xy(1,85,"D-Pad deforms the walker");
	
	animEpic.Draw();
	animWalker.Draw(); // draw the walking guy below the explosion for fun
	
	// draw cursor
	oslDrawLine(cursorX-5,cursorY-5,cursorX+5,cursorY+5,RGB(0,0,0));
	oslDrawLine(cursorX-5,cursorY+5,cursorX+5,cursorY-5,RGB(0,0,0));
	
	animExplosion.Draw(); // draw explosion above the cursor
	
	oslEndDrawing();
	oslSyncFrame();
}
