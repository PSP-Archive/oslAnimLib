#ifndef OSL_ANIMLIB_H
#define OSL_ANIMLIB_H

/*
               oslAnimLib
     
   This is a library to aid in animating
  multiple images or a sprite sheet in the
  advanced 2D graphics library 'OSlib' for
 the PSP.  Inspired by animLib, same concept
    but for, and in, Lua by Grimfate126.
     
     My only request is that if you
    use this in your project, you give
some credit to myself for this particular
         mechanic (animations)
     
 Let me know if this has been of use to you
       and possibly request another
      useful class you'd like to see
     
                                    ~ SG57
*/


// Check out the sample for correct usage of this library
//
// If your using C, add -lstdc++ to your LIBS makefile line
// and change your source file's extension to .cpp INSTEAD of .c - that's it!
class OSL_ANIMATION
{
  private:
    OSL_IMAGE *sSpriteSheet;   // since oslCreateImageTile references the image, we can't delete it afterwards or have it temporary
    std::vector<OSL_IMAGE*> vFrames; // dynamic array of OSL_IMAGE's
    std::string sFilename;          // file path and name for the images
    int iX, iY;                // coordintes of the animation
    int iWidth, iHeight;       // dimensions of the animation
    int iCenterX, iCenterY;    // the 'pole' the image is rotated around
    int iNumFrames;            // number of frames
    int iCurrentFrame;         // current frame in the animation were on
    int iStartFrame;           // frame that the animation starts on
    int iEndFrame;             // frame that the animation ends on
    int iDelay;                // delay count until we go to the next frame in the animation
    int iDelayCount;           // internally used - this value is incremented til' it reaches iDelay then resets
    float fAngle;              // angle in degrees the animation is rotated
    bool bPlay;                // flag to tell if we should play the animation
    bool bLoop;                // flag to tell if we should loop the animation
    bool bReverse;             // flag to tell if we should play animation reversed

  public:
    // Constructor
    OSL_ANIMATION() : sFilename("/"), iX(0), iY(0), iWidth(0), iHeight(0), iCenterX(0), iCenterY(0), iNumFrames(0), iCurrentFrame(0), iEndFrame(0), iDelay(0),
                      iDelayCount(0), fAngle(0.0f), bPlay(false), bLoop(false), bReverse(false)
    {
      sSpriteSheet = NULL;
      vFrames.clear();
    }
    // Destructor
    ~OSL_ANIMATION()
    {
      for (int i = 0; i < (int)vFrames.size(); i++)
      {
        if (vFrames[i])
          oslDeleteImage(vFrames[i]);

        vFrames[i] = NULL;
      }
      vFrames.clear();

      if (sSpriteSheet)
        oslDeleteImage(sSpriteSheet);

      sSpriteSheet = NULL;
    }

    /*
                               DO
      These functions actually do stuff, not set/get values
    */
    
    /* Description: This loads the animation's frames using a sprite sheet           */
    /* Usage: Call this before any other OSL_ANIMATION call - uses interval + offset */
    /* Example:                                                                      */
    /*       animWalkingLeft.Load("Images/Resources/Huge Explosion",6,0,0,32,32,true)*/
    /*   This will load 6 frames, starting at 0,0 each with the dimensions 32x32     */
    /*                            going across horizontally                          */
    void Load(std::string filename, const int numframes, const int startx, const int starty, const int width, const int height, const bool horizontal)
    {
      // clear all the current frames
      for (int i = 0; i < (int)vFrames.size(); i++)
      {
        if (vFrames[i])
          oslDeleteImage(vFrames[i]);

        vFrames[i] = NULL;
      }
      vFrames.clear();

      sFilename = filename;
      sFilename += ".png"; // add the only supported file format extension

      char temp[500];
      strncpy(temp,sFilename.c_str(), 500);

      sSpriteSheet = oslLoadImageFile(temp, OSL_IN_RAM, OSL_PF_8888); // load it
      oslAssert(sSpriteSheet); // verify it loaded correctly

      int StartX = startx; // these are used for if the sprite sheet row/coloumn end is reached
      int StartY = starty;
      int tmp = 0;   // temporary var used for the rows and coloumns in the animation
      iWidth = width;
      iHeight = height;
      iNumFrames = numframes;

      for (int i = 0; i < numframes; i++)
      {
        vFrames.push_back(oslCreateImageTileSize(sSpriteSheet, StartX + (horizontal ? (tmp * width) : 0), StartY + (horizontal ? 0 : (tmp * height)),
                                                 width, height));
        oslAssert(vFrames[i]);

        tmp++;

        // now we check and fix any sprite sheet sprite row/coloumn endings
        if (horizontal)
        {
          if (StartX + ((tmp) * width) >= sSpriteSheet->stretchX)
          {
            StartY += height; // go down a row
            StartX = startx;
            tmp = 0;
          }
        }
        else
        {
          if (StartY + ((tmp) * height) >= sSpriteSheet->stretchY)
          {
            StartX += width; // move over a coloumn
            StartY = starty;
            tmp = 0;
          }
        }
      }

      sFilename = filename; // set it back to how it was
    }
    
    /* Description: This loads the animation's frames using a multiple images        */
    /* Usage: Call this before any other OSL_ANIMATION call                          */
    /* Example:                                                                      */
    /*       animWalkingLeft.Load("Images/Resources/Huge Explosion",6)               */
    /*   This will load 6 frames named consecutively located at                      */
    /*              ImagesResources/Huge Explosion[0 - 6 here].png                   */
    void Load(std::string filename, const int numframes)
    {
      // delete the sprite sheet, we don't need it
      if (sSpriteSheet)
        oslDeleteImage(sSpriteSheet);
        
      sSpriteSheet = NULL;
      
      // clear all the current frames
      for (int i = 0; i < (int)vFrames.size(); i++)
      {
        if (vFrames[i])
          oslDeleteImage(vFrames[i]);

        vFrames[i] = NULL;
      }
      vFrames.clear();
      
      iNumFrames = numframes;
      
      char tempFilename[500];

      for (int i = 0; i < iNumFrames; i++)
      {
        snprintf(tempFilename, 500, "%s%i.png",filename.c_str(),i+1);
        
        vFrames.push_back(oslLoadImageFile(tempFilename, OSL_IN_RAM, OSL_PF_8888)); // add it
        oslAssert(vFrames[i]);
      }
      
      iWidth = vFrames[0]->stretchX;
      iHeight = vFrames[0]->stretchY;
      sFilename = filename; // just set it back to how it was...
    }
    
    /* Description: This unloads the animation's frames                              */
    /* Usage: Call this when leaving a state, exiting the game, etc.                 */
    void Unload()
    {
      for (int i = 0; i < (int)vFrames.size(); i++)
      {
        if (vFrames[i])
          oslDeleteImage(vFrames[i]);

        vFrames[i] = NULL;
      }
      vFrames.clear();

      if (sSpriteSheet)
        oslDeleteImage(sSpriteSheet);
        
      sSpriteSheet = NULL;
    }
    
    /* Description: This initializes alot of members                                 */
    /* Usage: Call this after you load the animation or to reset the animation       */
    /* Example:                                                                      */
    /*         animWalkingLeft.Initialize(0,0,32,32,0,10,20,false,true,false)        */
    /*                                                                               */
    /* This makes animWalkingLeft X and Y at (0,0).  It's width and height 32, it's  */
    /* starting frame at 0, ending frame at 10. The animation delay at 20, it's angle*/
    /* at 0 degrees, it won't play after being initialized, it will loop when played */
    /* and it won't play reversed.                                                   */
    void Initialize(const int x, const int y, const int width, const int height, const int startFrame, const int endFrame,
                    const int delay, const bool play, const bool loop, const bool reverse)
    {
      iX = x;
      iY = y;

      if (width != -1) iWidth = width;
      if (height != -1) iHeight = height;

      iCenterX = iWidth/2; // middle of image
      iCenterY = iHeight/2;// "

      iStartFrame = startFrame;
      iEndFrame = endFrame;

      iDelay = delay;
      iDelayCount = 0;

      fAngle = 0.0f;

      bPlay = play;
      bLoop = loop;
      bReverse = reverse;

      if (bReverse)
        iCurrentFrame = iEndFrame - 1;
      else
        iCurrentFrame = iStartFrame;
    }
    
    /* Description: This does all the drawing for the animationn                     */
    /* Usage: Call this in your main loop for when you want the animation to be drawn*/
    /* Example:                                                                      */
    /*            if ((bool) bWalkingLeft)                                           */
    /*            {                                                                  */
    /*              animWalkingLeft.Draw();                                          */
    /*            }                                                                  */
    void Draw()
    {
      if (bPlay)
      {
        if (!vFrames[iCurrentFrame])
          oslDebug("%s\nvFrames[%i] not loaded!",sFilename.c_str(),iCurrentFrame);

        oslDrawImage(vFrames[iCurrentFrame]);
      }
    }
    
    /* Description: This does all the thinking for the animation                     */
    /* Usage: Call this in your main loop for when you want the animation to update  */
    /* Example:                                                                      */
    /*            if ((bool) bWalkingLeft)                                           */
    /*            {                                                                  */
    /*              animWalkingLeft.Update();                                        */
    /*            }                                                                  */
    void Update()
    {
      if (bPlay)
      {
        // if the frames are out of whack :S
        if (iEndFrame < iStartFrame)
          oslDebug("When updating an animation, it appears it's end frame (%i) is LESS THEN it's start frame (%i)\n\nwtf is wrong with you?", iEndFrame, iStartFrame);

        for (int i = iStartFrame; i < iEndFrame; i++)
        {
          if (!vFrames[i])
            oslDebug("%s\nvFrames[%i] not loaded!",sFilename.c_str(),i);

          // update animation frames' value's
          vFrames[i]->x = iX;
          vFrames[i]->y = iY;
          vFrames[i]->stretchX = iWidth;
          vFrames[i]->stretchY = iHeight;
          vFrames[i]->centerX = iCenterX;
          vFrames[i]->centerY = iCenterY;
          vFrames[i]->angle = fAngle;
        }

        // delay stuff
        iDelayCount++;

        if (iDelayCount >= iDelay)
        {
          if (bReverse)
          {
            iCurrentFrame--;

            if (iCurrentFrame < iStartFrame)
            {
              iCurrentFrame = iEndFrame - 1;

              if (!bLoop)
              {
                bPlay = false;
              }
            }
          }
          else
          {
            iCurrentFrame++;

            if (iCurrentFrame >= iEndFrame)
            {
              iCurrentFrame = iStartFrame;

              if (!bLoop)
              {
                bPlay = false;
              }
            }
          }

          iDelayCount = 0;
        }
      }
    }

    /*
                          SET
      These functions set private members' values
    */
    
    /* Description: This sets the bPlay boolean to true meaning play the animation   */
    /* Usage: Pass this when an event occurs and you want to play the animation      */
    /* Example: myAnimation.setFilename("Resources/Animations/Huge Explosion")       */
    inline void Play()
    {
      bPlay = true;
    }

    /* Description: This sets the file path and base file name, doesn't reload though*/
    /*                                               use Unload() and Load() for that*/
    /* Usage: Pass this a string containing the path to the animation (no extension) */
    /* Example: myAnimation.setFilename("Resources/Animations/Huge Explosion")       */
    inline void setFilename(const std::string filename)
    {
      sFilename = filename;
    }
    
    /* Description: This sets y-coordinate as to where to draw the animation         */
    /* Usage: Pass this an integer for where to draw the animation on the y-axis     */
    /* Example: myAnimation.setY(272)                                                */
    inline void setY(const int y)
    {
      iY = y;
    }

    /* Description: This sets x-coordinate as to where to draw the animation         */
    /* Usage: Pass this an integer for where to draw the animation on the x-axis     */
    /* Example: myAnimation.setY(480)                                                */
    inline void setX(const int x)
    {
      iX = x;
    }

    /* Description: This sets the width of the animation                             */
    /* Usage: Pass this an integer that the animation's width will be stretched to   */
    /* Example: myAnimation.setWidth(256)                                            */
    inline void setWidth(const int width)
    {
      iWidth = width;
    }
    
    /* Description: This sets the height of the animation                            */
    /* Usage: Pass this an integer that the animation's height will be stretched to  */
    /* Example: myAnimation.setHeight(256)                                           */
    inline void setHeight(const int height)
    {
      iHeight = height;
    }
    
    /* Description: This sets the center X of rotation of the animation              */
    /* Usage: Pass this an integer that the animation's rotation center X will be    */
    /* Example: myAnimation.setCenterX(128)                                          */
    inline void setCenterX(const int x)
    {
      iCenterX = x;
    }
    
    /* Description: This sets the center Y of rotation of the animation              */
    /* Usage: Pass this an integer that the animation's rotation center Y will be    */
    /* Example: myAnimation.setCenterY(128)                                          */
    inline void setCenterY(const int y)
    {
      iCenterY = y;
    }
    
    /* Description: This sets the angle of the animation                             */
    /* Usage: Pass this an integer that the animation will be rotated to (degrees)   */
    /* Example: myAnimation.setRotationAngle(180)                                    */
    inline void setRotationAngle(const float angle)
    {
      fAngle = angle;
    }
    
    /* Description: This sets the delay of the animation                             */
    /* Usage: Pass this an integer that the animation will delay for before going to */
    /*        the next frame in the animation (tweak this til it's right)            */
    /* Example: myAnimation.setDelay(100)                                            */
    inline void setDelay(const int delay)
    {
      iDelay = delay;
    }
    
    /* Description: This sets the current frame of the animation                     */
    /* Usage: Pass this an integer that the current animation frame will be displaed */
    /* Example: myAnimation.setCurrentFrame(2)                                       */
    inline void setCurrentFrame(const int frame)
    {
      if (vFrames[frame])
        iCurrentFrame = frame;
      else
        oslDebug("problem when setting the current frame to %i - it isn't loaded?", frame);
    }
    
    /* Description: This sets the starting frame of the animation                    */
    /* Usage: Pass this an integer that the animation will start on                  */
    /* Example: myAnimation.setStartFrame(3)                                         */
    inline void setStartFrame(const int frame)
    {
      iStartFrame = frame;
    }

    /* Description: This sets the ending frame of the animation                      */
    /* Usage: Pass this an integer that the animation will end on                    */
    /* Example: myAnimation.setEndFrame(7)                                           */
    inline void setEndFrame(const int frame)
    {
      iEndFrame = frame;
    }

    /* Description: This sets whether or not to loop the animation                   */
    /* Usage: Pass this a boolean for if the animation should loop when finished     */
    /* Example: myAnimation.setLoop(true)                                            */
    inline void setLoop(const bool loop)
    {
      bLoop = loop;
    }
    
    
    /* Description: This sets whether or not to play the animation backwards         */
    /* Usage: Pass this a boolean for if the animation should play reversedished     */
    /* Example: myAnimation.setReverse(true)                                         */
    inline void setReverse(const bool reverse)
    {
      bReverse = reverse;
    }

    /*
                     GET
      These functions get private members
      
      Won't demonstrate usage of the obvious ones ;)
    */
    inline const std::vector<OSL_IMAGE*> getFrames()
    {
      return vFrames;
    }
    
    inline const std::string getFilename()
    {
      return sFilename;
    }

    inline const int getY()
    {
      return iY;
    }
    inline const int getX()
    {
      return iX;
    }
    inline const int getWidth()
    {
      return iWidth;
    }
    inline const int getHeight()
    {
      return iHeight;
    }
    inline const int getCenterX()
    {
      return iCenterX;
    }
    inline const int getCenterY()
    {
      return iCenterY;
    }
    inline const float getRotationAngle()
    {
      return fAngle;
    }
    inline const int getDelay()
    {
      return iDelay;
    }
    inline const int getStartFrame()
    {
      return iStartFrame;
    }
    inline const int getEndFrame()
    {
      return iEndFrame;
    }
    inline const int getCurrentFrame()
    {
      return iCurrentFrame;
    }
    inline const int getFrameCount()
    {
      return iNumFrames;
    }
    
    inline const bool isPlaying()
    {
      return bPlay;
    }
    inline const bool isLooped()
    {
      return bLoop;
    }
    inline const bool isReversed()
    {
      return bReverse;
    }
};

#endif
