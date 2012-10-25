#ifndef MENUPARAMSMARSHALLER_INCLUDE
#define MENUPARAMSMARSHALLER_INCLUDE

#include <string>
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

#include "../../include/JSONHandler/ALRPCObjects/MenuParams.h"


/*
  interface	Ford Sync RAPI
  version	1.2
  date		2011-05-17
  generated at	Thu Oct 25 04:31:05 2012
  source stamp	Wed Oct 24 14:57:16 2012
  author	robok0der
*/


struct MenuParamsMarshaller
{
  static bool checkIntegrity(MenuParams& e);
  static bool checkIntegrityConst(const MenuParams& e);

  static bool fromString(const std::string& s,MenuParams& e);
  static const std::string toString(const MenuParams& e);

  static bool fromJSON(const Json::Value& s,MenuParams& e);
  static Json::Value toJSON(const MenuParams& e);
};
#endif
