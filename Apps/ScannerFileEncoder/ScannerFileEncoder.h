#if !defined(_SCANNERFILEENCODER_)
#define  _SCANNERFILEENCODER_

#include "Application.h"
#include "KKQueue.h"
#include "RandomNumGenerator.h"
#include "RunLog.h"
#include "ScannerFile.h"
#include "KKStr.h"


namespace  ScannerFileEncoderApp
{
  class  ScannerFileEncoder: public Application
  {
  public:
    ScannerFileEncoder ();

    ~ScannerFileEncoder ();

    virtual
    void  InitalizeApplication (kkint32 argc,
                                char**  argv
                               );
                                        
    void  Main ();

  private:

    void   AddScanLineToBuffer ();

    void   BuildScannerFile ();

    void   DisplayCommandLineParameters ();

    void   AddScanLine (uchar* scanLine, 
                        int    width
                       );

    void   FlushScanLineBuffer (ulong scanRow);

    RasterPtr  GenerateRandomImage ();

    RasterListPtr  GetImagesInSubDirectory (KKStr  dirName);

    void  PopulateScanLinePositions ();

    virtual
    bool   ProcessCmdLineParameter (char    parmSwitchCode, 
                                    KKStr   parmSwitch, 
                                    KKStr   parmValue
                                   );

    void   ProcessDirectory (KKStr  dirName);

    void   ProcessFile (const KKStr&  fileName);

    void   WritePixelToScannerFile (ulong scanLineRow, 
                                   uint  col,
                                   uchar pixel
                                  );


    struct  ScanLine
    {
      ScanLine (uint  _width)  
      {
        scanLine = new uchar[_width];
        memset (scanLine, 0, _width);
      }

      ScanLine (uchar*  _scanLine)  {scanLine = _scanLine;}
      ~ScanLine ()     {delete  scanLine;  scanLine = NULL;}

      uchar&  operator[] (int  col)  {return  scanLine[col];}
      uchar*  scanLine;
    };

    typedef  ScanLine*  ScanLinePtr;

    vector<ulong>       scanLinePositions;

    KKStr               destFileName;
    int                 imagesPerSec;
    RandomNumGenerator  randGen;
    double              scanRate;
    KKQueue<ScanLine>   scanLineBuffer;
    uint                scanLineBufferStart;  /**< First row in output Scanner file that the scanLineBuffer is for.  */
    uint                scanLineBufferEnd;    /**< Last  row in output Scanner file that the scanLineBuffer is for.   */
    uint                scanLineWidth;
    long                scannerFileLenInSecs;
    long                scannerFileLenInScanLines;
    KKStr               srcDir;
    RasterListPtr       srcImages;
    ScannerFilePtr      scannerFile;
    int                 totalImages;
  };  /* ScannerFileEncoder */
}
#endif
