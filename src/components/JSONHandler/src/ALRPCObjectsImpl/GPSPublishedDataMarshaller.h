#ifndef GPSPUBLISHEDDATAMARSHALLER_INCLUDE
#define GPSPUBLISHEDDATAMARSHALLER_INCLUDE

#include <string>
#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

#include "../../include/JSONHandler/ALRPCObjects/GPSPublishedData.h"


/*
  interface	Ford Sync RAPI
  version	1.2
  date		2011-05-17
  generated at	Thu Oct 25 04:31:05 2012
  source stamp	Wed Oct 24 14:57:16 2012
  author	robok0der
*/


struct GPSPublishedDataMarshaller
{
  static bool checkIntegrity(GPSPublishedData& e);
  static bool checkIntegrityConst(const GPSPublishedData& e);

  static bool fromString(const std::string& s,GPSPublishedData& e);
  static const std::string toString(const GPSPublishedData& e);

  static bool fromJSON(const Json::Value& s,GPSPublishedData& e);
  static Json::Value toJSON(const GPSPublishedData& e);
};
#endif
