// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

//
// To receive images using PvPipeline
//


#include  "FirstIncludes.h"


#include  <fstream>
#include  <iostream>
#include  <map>
#include  <ostream>
#include  <vector>
using namespace  std;


#ifdef WIN32
#include <windows.h>
#include <conio.h>
#include <process.h>
#endif // WIN32

#include <stdio.h>

#ifdef _UNIX_

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int _kbhit(void)
{
  struct termios oldt, newt;
  int ch;
  int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }

  return 0;
}

#define _getch getchar

#endif // _UNIX_


#include <PvDeviceFinderWnd.h>
#include <PvDevice.h>
#include <PvPipeline.h>
#include <PvBuffer.h>
#include <PvStream.h>
#include <PvStreamRaw.h>
#include <PvSystem.h>

#include "MemoryDebug.h"
#include "KKBaseTypes.h"
#include "OSservices.h"
#include "KKStr.h"
using namespace  KKB;


#include "ScannerFile3BitEncoded.h"
#include "Variables.h"
using namespace  KKLSC;


//
// Shows how to use a PvPipeline object to acquire images from a device
//

bool AcquireImages()
{
  RunLog  log;

  PvSystem  pvSystem;

  PvResult  pvResult = pvSystem.Find ();
  if  (!pvResult.IsOK ())
    return  false;

  PvUInt32  interfaceCount = pvSystem.GetInterfaceCount ();

  for  (PvUInt32 x = 0;  x < interfaceCount;  ++x)
  {
    PvInterface*   pvInterface   = pvSystem.GetInterface (x);
    if  (pvInterface == NULL)
      continue;

    PvString  interfaceDescription = pvInterface->GetDescription ();
   
    PvString  interfaceId =  pvInterface->GetID ();
    PvString  interfaceIpAddress =  pvInterface->GetIPAddress ();
    PvInterfaceType interfaceType = pvInterface->GetType ();
    PvString  interfaceMacAddress = pvInterface->GetMACAddress ();

    cout << "InterfaceDescription : " << interfaceDescription.GetAscii () << endl;
    cout << "InterfaceId          : " << interfaceIpAddress.GetAscii   () << endl;
    cout << "InterfaceIpAddress   : " << interfaceIpAddress.GetAscii   () << endl;
    cout << "InterfaceType        : " << interfaceType                    << endl;
    cout << "InterfaceMacAddress  : " << interfaceMacAddress.GetAscii  () << endl;

    PvUInt32  deviceCount = pvInterface->GetDeviceCount ();
    for  (PvUInt32  y = 0; y < deviceCount;  ++y)
    {
      PvDeviceInfo*  pvDeviceInfo = pvInterface->GetDeviceInfo (y);
      PvString  deviceId = pvDeviceInfo->GetID ();
      cout  << "DeviceID: " << deviceId.GetAscii ()  << endl;

      PvString  deviceMacAddress = pvDeviceInfo->GetMACAddress ();
      cout << "Device Mac Address  : " << deviceMacAddress.GetAscii () << endl;
    }
    cout << endl;
  }

  // Create a GEV Device finder dialog
  PvDeviceFinderWnd lDeviceFinderWnd;


  // Prompt the user to select a GEV Device
  lDeviceFinderWnd.ShowModal();

  // Get the connectivity information for the selected GEV Device
  PvDeviceInfo* lDeviceInfo = lDeviceFinderWnd.GetSelected();

  // If no device is selected, abort
  if( lDeviceInfo == NULL )
  {
    printf( "No device selected.\n" );
    return false;
  }

  // Connect to the GEV Device
  PvDevice lDevice;
  printf( "Connecting to %s\n", lDeviceInfo->GetMACAddress().GetAscii() );
  if ( !lDevice.Connect( lDeviceInfo ).IsOK() )
  {
     printf( "Unable to connect to %s\n", lDeviceInfo->GetMACAddress().GetAscii() );
     return false;
  }
  printf( "Successfully connected to %s\n", lDeviceInfo->GetMACAddress().GetAscii() );
  printf( "\n" );

  // Get device parameters need to control streaming
  PvGenParameterArray *lDeviceParams = lDevice.GetGenParameters();
  PvGenInteger *lTLLocked = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "TLParamsLocked" ) );
  PvGenInteger *lPayloadSize = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "PayloadSize" ) );
  PvGenCommand *lStart = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStart" ) );
  PvGenCommand *lStop = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "AcquisitionStop" ) );

  
  PvGenInteger *lHeight = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Height" ) );
  PvGenInteger *lWidth  = dynamic_cast<PvGenInteger *>( lDeviceParams->Get( "Width" ) );

  PvInt64  frameHeight = 480;
  PvInt64  frameWidth  = 2048;

  if  (lHeight)
    lHeight->GetValue (frameHeight);

  if  (lWidth)
    lWidth->GetValue (frameWidth);

  // Negotiate streaming packet size
  lDevice.NegotiatePacketSize();

  // Create the PvStream object
  PvStream lStream;

  // Open stream - have the PvDevice do it for us
  printf( "Opening stream to device\n" );
  lStream.Open( lDeviceInfo->GetIPAddress() );

  // Create the PvPipeline object
  PvPipeline lPipeline( &lStream );
    
  // Reading payload size from device
  PvInt64 lSize = 0;
  lPayloadSize->GetValue( lSize );

  // Set the Buffer size and the Buffer count
  lPipeline.SetBufferSize( static_cast<PvUInt32>( lSize ) );
  //lPipeline.SetBufferCount( 16 ); // Increase for high frame rate without missing block IDs
  lPipeline.SetBufferCount( 128 ); // Increase for high frame rate without missing block IDs
  //lPipeline.SetBufferCount( 1 ); // Increase for high frame rate without missing block IDs

  // Have to set the Device IP destination to the Stream
  lDevice.SetStreamDestination( lStream.GetLocalIPAddress(), lStream.GetLocalPort() );

  // IMPORTANT: the pipeline needs to be "armed", or started before 
  // we instruct the device to send us images
  printf( "Starting pipeline\n" );
  lPipeline.Start();

  // Get stream parameters/stats
  PvGenParameterArray *lStreamParams = lStream.GetParameters();
  PvGenInteger *lCount     = dynamic_cast<PvGenInteger *>( lStreamParams->Get( "ImagesCount" ) );
  PvGenFloat   *lFrameRate = dynamic_cast<PvGenFloat *>  ( lStreamParams->Get( "AcquisitionRateAverage" ) );
  PvGenFloat   *lBandwidth = dynamic_cast<PvGenFloat *>  ( lStreamParams->Get( "BandwidthAverage" ) );


  // TLParamsLocked is optional but when present, it MUST be set to 1
  // before sending the AcquisitionStart command
  if  (lTLLocked != NULL)
  {
    printf( "Setting TLParamsLocked to 1\n" );
    lTLLocked->SetValue( 1 );
  }

  printf( "Resetting timestamp counter...\n" );
  PvGenCommand *lResetTimestamp = dynamic_cast<PvGenCommand *>( lDeviceParams->Get( "GevTimestampControlReset" ) );
  lResetTimestamp->Execute();

  // The pipeline is already "armed", we just have to tell the device
  // to start sending us images
  printf( "Sending StartAcquisition command to device\n" );
  PvResult lResult = lStart->Execute();

  char lDoodle[] = "|\\-|-/";
  int lDoodleIndex = 0;
  PvInt64 lImageCountVal = 0;
  double lFrameRateVal = 0.0;
  double lBandwidthVal = 0.0;

  PvUInt64  lastBlockId = 0;

  KKStr  scannerFileName = osAddSlash (Variables::ScannerFilesDefaultDir ()) + "SCS_" + osGetLocalDateTime ().YYYYMMDDHHMMSS () + ".spr";

  ScannerFilePtr  sf = ScannerFile::CreateScannerFileForOutput (scannerFileName, ScannerFile::sf3BitEncoded, (kkint32)frameWidth, (kkint32)frameHeight, log);
  ofstream  sipper3File (scannerFileName.Str (), ios_base::binary);

  int  totalLostPackets = 0;

  // Acquire images until the user instructs us to stop
  printf( "\n<press the enter key to stop streaming>\n" );
  while ( !_kbhit() )
  {
    // Retrieve next buffer		
    PvBuffer *lBuffer = NULL;
    PvResult  lOperationResult;
    int  bufferCount = lPipeline.GetBufferCount ();
    int  outputQueueSize = lPipeline.GetOutputQueueSize ();

    PvResult lResult = lPipeline.RetrieveNextBuffer (&lBuffer, 1000, &lOperationResult );
       
    if ( lResult.IsOK() )
    {
      if ( lOperationResult.IsOK() )
      {
        lCount->GetValue( lImageCountVal );
        lFrameRate->GetValue( lFrameRateVal );
        lBandwidth->GetValue( lBandwidthVal );

        const PvUInt8*  rawData = lBuffer->GetDataPointer        ();
        PvUInt64  blockID       = lBuffer->GetBlockID            ();
        int  lostPacketCount    = lBuffer->GetLostPacketCount    ();
        int  requiredSize       = lBuffer->GetRequiredSize       ();
        int  acquiredSize       = lBuffer->GetAcquiredSize       ();
        int  ignoredPacketCount = lBuffer->GetIgnoredPacketCount ();

        int  height             = lBuffer->GetImage ()->GetHeight             ();
        int  width              = lBuffer->GetImage ()->GetWidth              ();
        int  bitsPerPixel       = lBuffer->GetImage ()->GetBitsPerPixel       ();
        int  effectiveImageSize = lBuffer->GetImage ()->GetEffectiveImageSize ();
        int  offsetX            = lBuffer->GetImage ()->GetOffsetX            ();
        int  offsetY            = lBuffer->GetImage ()->GetOffsetY            ();
        int  imageSize          = lBuffer->GetImage ()->GetImageSize          ();

        totalLostPackets += lostPacketCount;

        printf ("\n bufferCount[%d]  outputQueueSize[%d]\n", 
                bufferCount,
                outputQueueSize
               );
/*
               printf( "%c Timestamp: %016llX BlockID: %04X %.01f FPS %.01f Mb/s  Height[%d]  Width[%d]  BitsPerPixel[%d]  EffectiveImageSize[%d]  BlockID[%d]  ImageSize[%d]  LostPacketCount[%d]  requiredSize[%d]  acquiredSize[%d]  ignoredPacketCount[%d]  offsetX[%d]  offsetY[%d].\n", 
                   lDoodle[ lDoodleIndex ],
                   lBuffer->GetTimestamp(),
                   lBuffer->GetBlockID(),
                   lFrameRateVal,
                   lBandwidthVal / 1024 / 1024,
                   height,
                   width,
                   bitsPerPixel,
                   effectiveImageSize,
                   blockID,
                   imageSize,
                   lostPacketCount,
                   requiredSize,
                   acquiredSize,
                   ignoredPacketCount,
                   offsetX,
                   offsetY
                  );
*/
        if  (blockID % 100 == 0)
        {
          printf ("    BlockId[%d]", 
                  blockID
                 );
        }

        if  ((blockID - lastBlockId) > 0)
        {
          printf ("\n\n bufferCount[%d]  outputQueueSize[%d]  BlockId[%d].\n\n", 
                  bufferCount,
                  outputQueueSize,
                  blockID
                 );
        }
        {
          int  x = 0;
          const PvUInt8*  rawDataNextPixel = rawData;
          //for  (int bufferNum = 0;  bufferNum < bufferCount;  bufferNum++)
          
          {
            uchar  buff[4096];
            uchar*  buffPtr = buff;

            for  (int row = 0;  row < height;  row++)
            {
              buffPtr = buff;
              memset (buff, 0, sizeof (buff));
              buffPtr += 1024;

              for  (x = 0; x < width;  ++x)
              {
                buffPtr[x] = (~rawDataNextPixel[x]) >> 5;
              }
              rawDataNextPixel += width;
              sf->WriteScanLine (buff, sizeof (buff));
            }

            lastBlockId = blockID;
          }
        }
      }
           

      // We have an image - do some processing (...) and VERY IMPORTANT,
      // release the buffer back to the pipeline
      lPipeline.ReleaseBuffer (lBuffer);
    }
    else
    {
      // Timeout
      printf( "%c Timeout\r", lDoodle[ lDoodleIndex ] );
    }

    ++lDoodleIndex %= 6;
  }

  sipper3File.close ();

  cout << endl << endl 
     << "totalLostPackets" << "\t" << totalLostPackets << endl
     << endl;

  _getch(); // Flush key buffer for next stop
  printf( "\n\n" );

  // Tell the device to stop sending images
  printf( "Sending AcquisitionStop command to the device\n" );
  lStop->Execute();

  // If present reset TLParamsLocked to 0. Must be done AFTER the 
  // streaming has been stopped
  if ( lTLLocked != NULL )
  {
    printf( "Resetting TLParamsLocked to 0\n" );
    lTLLocked->SetValue( 0 );
  }

  // We stop the pipeline - letting the object lapse out of 
  // scope would have had the destructor do the same, but we do it anyway
  printf( "Stop pipeline\n" );
  lPipeline.Stop();

  // Now close the stream. Also optionnal but nice to have
  printf( "Closing stream\n" );
  lStream.Close();

  // Finally disconnect the device. Optional, still nice to have
  printf( "Disconnecting device\n" );
  lDevice.Disconnect();

  return true;
}


//
// Main function
//

int main()
{
    // PvPipeline used to acquire images from a device
    printf( "1. PvPipeline sample - image acquisition from a device\n\n" );
    AcquireImages();

    printf( "\n<press the enter key to exit>\n" );
    while ( !_kbhit() );

    return 0;
}
