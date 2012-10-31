#ifndef UPDATEMODEMARSHALLER_INCLUDE
#define UPDATEMODEMARSHALLER_INCLUDE

#include <string>
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

#include "PerfectHashTable.h"

#include "../../include/JSONHandler/ALRPCObjects/UpdateMode.h"


/*
  interface	Ford Sync RAPI
  version	1.2
  date		2011-05-17
  generated at	Tue Oct 30 08:29:32 2012
  source stamp	Thu Oct 25 06:49:27 2012
  author	robok0der
*/


//! marshalling class for UpdateMode

class UpdateModeMarshaller
{
public:

  static std::string toName(const UpdateMode& e) 	{ return getName(e.mInternal) ?: ""; }

  static bool fromName(UpdateMode& e,const std::string& s)
  { 
    return (e.mInternal=getIndex(s.c_str()))!=UpdateMode::INVALID_ENUM;
  }

  static bool checkIntegrity(UpdateMode& e)		{ return e.mInternal!=UpdateMode::INVALID_ENUM; } 
  static bool checkIntegrityConst(const UpdateMode& e)	{ return e.mInternal!=UpdateMode::INVALID_ENUM; } 

  static bool fromString(const std::string& s,UpdateMode& e);
  static const std::string toString(const UpdateMode& e);

  static bool fromJSON(const Json::Value& s,UpdateMode& e);
  static Json::Value toJSON(const UpdateMode& e);

  static const char* getName(UpdateMode::UpdateModeInternal e)
  {
     return (e>=0 && e<4) ? mHashTable[e].name : NULL;
  }

  static const UpdateMode::UpdateModeInternal getIndex(const char* s);

  static const PerfectHashTable mHashTable[4];
};

#endif
