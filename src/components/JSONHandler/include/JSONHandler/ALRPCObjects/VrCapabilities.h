#ifndef VRCAPABILITIES_INCLUDE
#define VRCAPABILITIES_INCLUDE


/*
  interface	Ford Sync RAPI
  version	1.2
  date		2011-05-17
  generated at	Tue Oct 30 08:29:32 2012
  source stamp	Thu Oct 25 06:49:27 2012
  author	robok0der
*/


///  Contains information about the VR capabilities.

class VrCapabilities
{
public:
  enum VrCapabilitiesInternal
  {
    INVALID_ENUM=-1,
    Text=0
  };

  VrCapabilities() : mInternal(INVALID_ENUM)				{}
  VrCapabilities(VrCapabilitiesInternal e) : mInternal(e)		{}

  VrCapabilitiesInternal get(void) const	{ return mInternal; }
  void set(VrCapabilitiesInternal e)		{ mInternal=e; }

private:
  VrCapabilitiesInternal mInternal;
  friend class VrCapabilitiesMarshaller;
};

#endif
