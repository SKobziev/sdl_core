#include "../../include/JSONHandler/ALRPCObjects/Speak_request.h"
#include "Speak_requestMarshaller.h"
#include "../../include/JSONHandler/ALRPCObjects/Marshaller.h"
#include "TTSChunkMarshaller.h"

#define PROTOCOL_VERSION	1


/*
  interface	Ford Sync RAPI
  version	1.2
  date		2011-05-17
  generated at	Thu Oct 25 04:31:05 2012
  source stamp	Wed Oct 24 14:57:16 2012
  author	robok0der
*/


Speak_request::~Speak_request(void)
{
}


Speak_request::Speak_request(const Speak_request& c)
{
  *this=c;
}


bool Speak_request::checkIntegrity(void)
{
  return Speak_requestMarshaller::checkIntegrity(*this);
}


Speak_request::Speak_request(void) : ALRPCRequest(PROTOCOL_VERSION,Marshaller::METHOD_SPEAK_REQUEST)
{
}



bool Speak_request::set_ttsChunks(const std::vector<TTSChunk>& ttsChunks_)
{
  unsigned int i=ttsChunks_.size();
  if(i>100 || i<1)  return false;
  while(i--)
  {
    if(!TTSChunkMarshaller::checkIntegrityConst(ttsChunks_[i]))   return false;
  }
  ttsChunks=ttsChunks_;
  return true;
}




const std::vector<TTSChunk>& Speak_request::get_ttsChunks(void) const 
{
  return ttsChunks;
}

