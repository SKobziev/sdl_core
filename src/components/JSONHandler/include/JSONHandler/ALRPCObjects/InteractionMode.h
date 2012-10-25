#ifndef INTERACTIONMODE_INCLUDE
#define INTERACTIONMODE_INCLUDE


/*
  interface	Ford Sync RAPI
  version	1.2
  date		2011-05-17
  generated at	Thu Oct 25 04:31:05 2012
  source stamp	Wed Oct 24 14:57:16 2012
  author	robok0der
*/


/**
     For application-requested interactions, this mode indicates the method in which the user is notified and uses the interaction.
     This mode causes the interaction to only occur on the display, meaning the choices are provided only via the display.
     Selections are made with the OK and Seek Right and Left, Tune Up and Down buttons.
     This mode causes the interaction to only occur using V4.
     Selections are made by saying the command.
     This mode causes both a VR and display selection option for an interaction.
     Selections can be made either from the menu display or by speaking the command.
*/

class InteractionMode
{
public:
  enum InteractionModeInternal
  {
    INVALID_ENUM=-1,
    MANUAL_ONLY=0,
    VR_ONLY=1,
    BOTH=2
  };

  InteractionMode() : mInternal(INVALID_ENUM)				{}
  InteractionMode(InteractionModeInternal e) : mInternal(e)		{}

  InteractionModeInternal get(void) const	{ return mInternal; }
  void set(InteractionModeInternal e)		{ mInternal=e; }

private:
  InteractionModeInternal mInternal;
  friend class InteractionModeMarshaller;
};

#endif
