#include <pthread.h>
#include <signal.h>
#include <memory.h>
#include "TransportManager/ITransportManager.hpp"
#include "ProtocolHandler/ISessionObserver.h"
#include "ProtocolHandler/IProtocolObserver.h"
#include "ProtocolHandler/ProtocolHandler.h"

using namespace NsProtocolHandler;

log4cplus::Logger ProtocolHandler::mLogger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("ProtocolHandler"));

ProtocolHandler::ProtocolHandler( NsAppLink::NsTransportManager::ITransportManager * transportManager ) :
 mProtocolObserver( 0 )
,mSessionObserver( 0 )
,mTransportManager( transportManager )
{
    pthread_create( &mHandleMessagesFromMobileApp, NULL, &ProtocolHandler::handleMessagesFromMobileApp, (void *)this );
    pthread_create( &mHandleMessagesToMobileApp, NULL, &ProtocolHandler::handleMessagesToMobileApp, (void *)this );
}

ProtocolHandler::~ProtocolHandler()
{
    pthread_kill( mHandleMessagesFromMobileApp, 1 );
    pthread_kill( mHandleMessagesToMobileApp, 1 );
    mProtocolObserver = 0;
    mSessionObserver = 0;
    mTransportManager = 0;
}

void ProtocolHandler::setProtocolObserver( IProtocolObserver * observer )
{
    if ( !observer )
    {
        LOG4CPLUS_ERROR(mLogger, "Invalid (NULL) pointer to IProtocolObserver.");
        return;
    }

    mProtocolObserver = observer;
}
        
void ProtocolHandler::setSessionObserver( ISessionObserver * observer )
{
    if ( !observer )
    {
        LOG4CPLUS_ERROR(mLogger, "Invalid (NULL) pointer to ISessionObserver.");
        return;
    }

    mSessionObserver = observer;
}

void ProtocolHandler::sendEndSessionAck( NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
              unsigned int sessionID, 
              unsigned int hashCode )
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    unsigned char versionFlag = PROTOCOL_VERSION_1;
    if (0 != hashCode)
    {
        versionFlag = PROTOCOL_VERSION_2;
    }

    ProtocolPacket packet(versionFlag,
                                COMPRESS_OFF,
                                FRAME_TYPE_CONTROL,
                                SERVICE_TYPE_RPC,
                                FRAME_DATA_END_SESSION,
                                sessionID,
                                0,
                                hashCode);

    if (RESULT_OK == sendFrame(connectionHandle, packet))
    {
        LOG4CPLUS_INFO(mLogger, "sendStartSessionAck() - BT write OK");
    }
    else
    {
        LOG4CPLUS_ERROR(mLogger, "sendStartSessionAck() - BT write FAIL");
    }
}
        
void ProtocolHandler::sendEndSessionNAck( NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
              unsigned int sessionID )
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    ProtocolPacket packet(PROTOCOL_VERSION_2,
                                COMPRESS_OFF,
                                FRAME_TYPE_CONTROL,
                                SERVICE_TYPE_RPC,
                                FRAME_DATA_END_SESSION_NACK,
                                sessionID,
                                0,
                                0);

    if (RESULT_OK == sendFrame(connectionHandle, packet))
    {
        LOG4CPLUS_INFO(mLogger, "sendStartSessionAck() - BT write OK");
    }
    else
    {
        LOG4CPLUS_ERROR(mLogger, "sendStartSessionAck() - BT write FAIL");
    }
}

void ProtocolHandler::sendStartSessionAck( NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
              unsigned char sessionID,
              unsigned int hashCode )
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    unsigned char versionFlag = PROTOCOL_VERSION_1;
    if (0 != hashCode)
    {
        versionFlag = PROTOCOL_VERSION_2;
    }

    ProtocolPacket packet(versionFlag,
                                COMPRESS_OFF,
                                FRAME_TYPE_CONTROL,
                                SERVICE_TYPE_RPC,
                                FRAME_DATA_START_SESSION_ACK,
                                sessionID,
                                0,
                                hashCode);

    if (RESULT_OK == sendFrame(connectionHandle, packet))
    {
        LOG4CPLUS_INFO(mLogger, "sendStartSessionAck() - BT write OK");
    }
    else
    {
        LOG4CPLUS_ERROR(mLogger, "sendStartSessionAck() - BT write FAIL");
    }
}

void ProtocolHandler::sendStartSessionNAck( NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
              unsigned char sessionID )
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    unsigned char versionFlag = PROTOCOL_VERSION_1;
    
    ProtocolPacket packet(versionFlag,
                                COMPRESS_OFF,
                                FRAME_TYPE_CONTROL,
                                SERVICE_TYPE_RPC,
                                FRAME_DATA_START_SESSION_NACK,
                                sessionID,
                                0,
                                0);

    if (RESULT_OK == sendFrame(connectionHandle, packet))
    {
        LOG4CPLUS_INFO(mLogger, "sendStartSessionAck() - BT write OK");
    }
    else
    {
        LOG4CPLUS_ERROR(mLogger, "sendStartSessionAck() - BT write FAIL");
    }

}

void ProtocolHandler::sendData(const AppLinkRawMessage * message)
{
    if ( !message )
    {
        LOG4CPLUS_ERROR(mLogger, "Invalid message for sending to mobile app is received.");
        return;
    }
    mMessagesToMobileApp.push(message);    
}

void ProtocolHandler::onFrameReceived(NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
    const uint8_t * data, size_t dataSize)
{
    if (connectionHandle && dataSize > 0 && data )
    {
        IncomingMessage message;
        message.mData = data;
        message.mDataSize = dataSize;
        message.mConnectionHandle = connectionHandle;
        mMessagesFromMobileApp.push( message );
    }
    else 
    {
        LOG4CPLUS_ERROR( mLogger, "Invalid incoming message received in ProtocolHandler from Transport Manager." );
    }
}

RESULT_CODE ProtocolHandler::sendFrame( NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
        const ProtocolPacket & packet )
{
    if ( !packet.getPacket() )
    {
        LOG4CPLUS_ERROR(mLogger, "Failed to create packet.");
        return RESULT_FAIL;
    }

    if (mTransportManager)
    {
        mTransportManager->sendFrame(connectionHandle, packet.getPacket(), packet.getPacketSize() );
    }
    else
    {
        LOG4CPLUS_WARN(mLogger, "No Transport Manager found.");
        return RESULT_FAIL;
    }

    return RESULT_OK;
}

RESULT_CODE ProtocolHandler::sendSingleFrameMessage(NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
                                      const unsigned char sessionID,
                                      unsigned int protocolVersion,
                                      const unsigned char servType,
                                      const unsigned int dataSize,
                                      const unsigned char *data,
                                      const bool compress)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    unsigned char versionF = PROTOCOL_VERSION_1;
    if (2 == protocolVersion)
    {
        versionF = PROTOCOL_VERSION_2;
    }

    ProtocolPacket packet(versionF,
                        compress,
                        FRAME_TYPE_SINGLE,
                        servType,
                        0,
                        sessionID,
                        dataSize,
                        mMessageCounters[sessionID]++,
                        data);

    return sendFrame( connectionHandle, packet );   
}
        
RESULT_CODE ProtocolHandler::sendMultiFrameMessage(NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
                                         const unsigned char sessionID,
                                         unsigned int protocolVersion,
                                         const unsigned char servType,
                                         const unsigned int dataSize,
                                         const unsigned char *data,
                                         const bool compress,
                                         const unsigned int maxDataSize)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    RESULT_CODE retVal = RESULT_OK;

    unsigned char versionF = PROTOCOL_VERSION_1;
    if (2 == protocolVersion)
    {
        versionF = PROTOCOL_VERSION_2;
    }

    int numOfFrames = 0;
    int lastDataSize = 0;

    if (dataSize % maxDataSize)
    {
        numOfFrames = (dataSize / maxDataSize) + 1;
        lastDataSize = dataSize % maxDataSize;
    }
    else
        numOfFrames = dataSize / maxDataSize;

    unsigned char *outDataFirstFrame = new unsigned char[FIRST_FRAME_DATA_SIZE];
    outDataFirstFrame[0] = dataSize >> 24;
    outDataFirstFrame[1] = dataSize >> 16;
    outDataFirstFrame[2] = dataSize >> 8;
    outDataFirstFrame[3] = dataSize;

    outDataFirstFrame[4] = numOfFrames >> 24;
    outDataFirstFrame[5] = numOfFrames >> 16;
    outDataFirstFrame[6] = numOfFrames >> 8;
    outDataFirstFrame[7] = numOfFrames;

    ProtocolPacket firstPacket(versionF,
                                     compress,
                                     FRAME_TYPE_FIRST,
                                     servType,
                                     0,
                                     sessionID,
                                     FIRST_FRAME_DATA_SIZE,
                                     mMessageCounters[sessionID],
                                     outDataFirstFrame);  

    retVal = sendFrame( connectionHandle, firstPacket );

    delete [] outDataFirstFrame;

    unsigned char *outDataFrame = new unsigned char[maxDataSize];

    for (unsigned int i = 0 ; i < numOfFrames ; i++)
    {
        if (i != (numOfFrames - 1) )
        {
            memcpy(outDataFrame, data + (maxDataSize * i), maxDataSize);

            ProtocolPacket packet(versionF,
                                        compress,
                                        FRAME_TYPE_CONSECUTIVE,
                                        servType,
                                        ( (i % FRAME_DATA_MAX_VALUE) + 1),
                                        sessionID,
                                        maxDataSize,
                                        mMessageCounters[sessionID],
                                        outDataFrame);            

            retVal = sendFrame( connectionHandle, packet );
            if ( RESULT_FAIL == retVal)
            {
                break;
            }
        }
        else
        {
            memcpy(outDataFrame, data + (maxDataSize * i), lastDataSize);

            ProtocolPacket packet(versionF,
                                        compress,
                                        FRAME_TYPE_CONSECUTIVE,
                                        servType,
                                        0x0,
                                        sessionID,
                                        lastDataSize,
                                        mMessageCounters[sessionID]++,
                                        outDataFrame);            

            retVal = sendFrame( connectionHandle, packet );
        }
    }

    delete [] outDataFrame;

    return retVal;
}

RESULT_CODE ProtocolHandler::handleMessage( NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
                    ProtocolPacket * packet )
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    switch (packet -> getFrameType())
    {
        case FRAME_TYPE_CONTROL:
        {
            LOG4CPLUS_INFO(mLogger, "handleMessage() - case FRAME_TYPE_CONTROL");

            return handleControlMessage( connectionHandle, packet );
        }
        case FRAME_TYPE_SINGLE:
        {
            LOG4CPLUS_INFO(mLogger, "handleMessage() - case FRAME_TYPE_SINGLE");

            if ( !mSessionObserver )
            {
                LOG4CPLUS_ERROR(mLogger, "Cannot handle message from Transport Manager: ISessionObserver doesn't exist.");
                return RESULT_FAIL;
            }

            int connectionKey = mSessionObserver -> keyFromPair( connectionHandle,
                                        packet -> getSessionId() );
            
            AppLinkRawMessage * rawMessage = new AppLinkRawMessage( connectionKey,
                                        packet -> getVersion(),
                                        packet -> getData(),
                                        packet -> getDataSize() );

            if (mProtocolObserver)
                mProtocolObserver->onDataReceivedCallback(rawMessage);

            break;
        }
        case FRAME_TYPE_FIRST:
        case FRAME_TYPE_CONSECUTIVE:
        {
            LOG4CPLUS_INFO(mLogger, "handleMessage() - case FRAME_TYPE_CONSECUTIVE");

            return handleMultiFrameMessage( connectionHandle, packet ); 
        }
        default:
        {
            LOG4CPLUS_WARN(mLogger, "handleMessage() - case default!!!");
        }
    }

    return RESULT_OK;
}

RESULT_CODE ProtocolHandler::handleMultiFrameMessage( NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
              ProtocolPacket * packet )
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    if ( !mSessionObserver )
    {
        LOG4CPLUS_ERROR(mLogger, "No ISessionObserver set.");
        return RESULT_FAIL;
    }

    int key = mSessionObserver->keyFromPair(connectionHandle, packet -> getSessionId());

    if (packet -> getFrameType() == FRAME_TYPE_FIRST)
    {
        LOG4CPLUS_INFO(mLogger, "handleMultiFrameMessage() - FRAME_TYPE_FIRST");
        
        //const unsigned char * data = packet -> getData();
        unsigned int totalDataBytes = packet -> getData()[0] << 24;
        totalDataBytes |= packet -> getData()[1] << 16;
        totalDataBytes |= packet -> getData()[2] << 8;
        totalDataBytes |= packet -> getData()[3];

        packet -> setTotalDataBytes( totalDataBytes );

        mIncompleteMultiFrameMessages[key] = packet;
    }
    else
    {
        LOG4CPLUS_INFO(mLogger, "handleMultiFrameMessage() - Consecutive frame");

        std::map<int, ProtocolPacket*>::iterator it = mIncompleteMultiFrameMessages.find(key);

        if ( it == mIncompleteMultiFrameMessages.end() )
        {
            LOG4CPLUS_ERROR(mLogger, "Frame of multiframe message for non-existing session id");
            return RESULT_FAIL;
        }

        if ( it->second->appendData( packet -> getData(), packet -> getDataSize() ) != RESULT_OK )
        {
            LOG4CPLUS_ERROR(mLogger, "Failed to append frame for multiframe message.");
            return RESULT_FAIL;
        }
                
        if ( packet -> getFrameData() == FRAME_DATA_LAST_FRAME )
        {
            if ( !mProtocolObserver )
            {
                LOG4CPLUS_ERROR(mLogger, "Cannot handle multiframe message: no IProtocolObserver is set.");
                return RESULT_FAIL;
            }

            AppLinkRawMessage * rawMessage = new AppLinkRawMessage( key,
                                        packet -> getVersion(),
                                        packet -> getData(),
                                        packet -> getDataSize() );

            mProtocolObserver -> onDataReceivedCallback( rawMessage );

            mIncompleteMultiFrameMessages.erase( it );
        }

    }

    return RESULT_OK;
}

RESULT_CODE ProtocolHandler::handleControlMessage( NsAppLink::NsTransportManager::tConnectionHandle connectionHandle,
              const ProtocolPacket * packet )
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);

    if ( !mSessionObserver )
    {
        LOG4CPLUS_ERROR(mLogger, "ISessionObserver is not set.");
        return RESULT_FAIL;
    }
    
    if (packet -> getFrameData() == FRAME_DATA_END_SESSION)
    {
        LOG4CPLUS_INFO(mLogger, "handleControlMessage() - FRAME_DATA_END_SESSION");

        unsigned char currentSessionID = packet -> getSessionId();

        mSessionObserver -> onSessionEndedCallback( connectionHandle, currentSessionID );
    }

    if (packet -> getFrameData() == FRAME_DATA_START_SESSION)
    {
        LOG4CPLUS_INFO(mLogger, "handleControlMessage() - FRAME_DATA_START_SESSION");

        mSessionObserver -> onSessionStartedCallback( connectionHandle );
    }

    return RESULT_OK;
}

void * ProtocolHandler::handleMessagesFromMobileApp( void * params )
{
    ProtocolHandler * handler = static_cast<ProtocolHandler*> (params);
    if ( !handler )
    {
        pthread_exit( 0 );
    }

    while( 1 )
    {
        while( ! handler -> mMessagesFromMobileApp.empty() )
        {
            IncomingMessage message = handler -> mMessagesFromMobileApp.pop();
            LOG4CPLUS_INFO_EXT(mLogger, "Message from mobile app received of size " << message.mDataSize );

            //@TODO check for ConnectionHandle.
            //@TODO check for data size - crash is possible.
            if ((0 != message.mData) && (0 != message.mDataSize) && (MAXIMUM_FRAME_SIZE >= message.mDataSize))
            {        
                ProtocolPacket * packet = new ProtocolPacket;
                if ( packet -> deserializePacket( message.mData, message.mDataSize ) == RESULT_FAIL )
                {
                    LOG4CPLUS_ERROR(mLogger, "Failed to parse received message.");
                    delete packet;
                }
                else 
                {
                    handler -> handleMessage( message.mConnectionHandle, packet );
                }
            }
            else
            {
                LOG4CPLUS_WARN(mLogger, "handleMessagesFromMobileApp() - incorrect or NULL data");
            }
        }
        handler -> mMessagesFromMobileApp.wait();
    }

    pthread_exit( 0 );
}

void * ProtocolHandler::handleMessagesToMobileApp( void * params )
{
    ProtocolHandler * handler = static_cast<ProtocolHandler*> (params);
    if ( !handler )
    {
        pthread_exit( 0 );
    }

    //TODO: check if continue running condition.
    while( 1 )
    {
        while ( ! handler -> mMessagesToMobileApp.empty() )
        {
            const AppLinkRawMessage * message = handler -> mMessagesToMobileApp.pop();
            LOG4CPLUS_INFO_EXT(mLogger, "Message to mobile app: connection " << message->getConnectionKey()
                    << "; dataSize: " << message->getDataSize() );

            unsigned int maxDataSize = 0;
            if ( PROTOCOL_VERSION_1 == message -> getProtocolVersion() )
                maxDataSize = MAXIMUM_FRAME_SIZE - PROTOCOL_HEADER_V1_SIZE;
            else if ( PROTOCOL_VERSION_2 == message -> getProtocolVersion() )
                maxDataSize = MAXIMUM_FRAME_SIZE - PROTOCOL_HEADER_V2_SIZE;

            NsAppLink::NsTransportManager::tConnectionHandle connectionHandle = 0;
            unsigned char sessionID = 0;

            if ( !handler -> mSessionObserver )
            {
                LOG4CPLUS_ERROR(mLogger, "Cannot handle message to mobile app: ISessionObserver doesn't exist.");
                pthread_exit(0);
            }
            handler -> mSessionObserver -> pairFromKey( message->getConnectionKey(), connectionHandle, sessionID );

            if ( message -> getDataSize() <= maxDataSize )
            {
                if (handler -> sendSingleFrameMessage(connectionHandle,
                                            sessionID, 
                                            message -> getProtocolVersion(),
                                            SERVICE_TYPE_RPC, 
                                            message -> getDataSize(), 
                                            message -> getData(), 
                                            false) != RESULT_OK)
                {
                    LOG4CPLUS_ERROR(mLogger, "ProtocolHandler failed to send single frame message.");
                }                    
            }
            else
            {
                if (handler -> sendMultiFrameMessage(connectionHandle,
                                            sessionID,
                                            message -> getProtocolVersion(),
                                            SERVICE_TYPE_RPC, 
                                            message -> getDataSize(), 
                                            message -> getData(), 
                                            false,
                                            maxDataSize) != RESULT_OK)
                {
                    LOG4CPLUS_ERROR(mLogger, "ProtocolHandler failed to send multi frame messages.");
                }
            }
        }
        handler -> mMessagesToMobileApp.wait();
    }

    pthread_exit( 0 );
}