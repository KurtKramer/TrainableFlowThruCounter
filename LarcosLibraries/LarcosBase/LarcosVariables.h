#if  !defined(_LARCOSVARIABLES_)
#define  _LARCOSVARIABLES_

#include  "KKStr.h"


/**
 *@namespace LarcosBase  LarcosVariables.h
 *@brief Classes and structures that are needed to support LARCOS applications.
 *@details The only class in this namespcace is 'LarcosVariables' it provides global 
 * information to all LARCIOS applications such as the LARCOS hime directory.
 */

namespace  LarcosBase 
{
  /** 
   *@class  Variables
   *@brief Variables that specic to teh Machine Learning Library.
   *@details  All methods in this library are to use these variables to locate files and directories that they need.
   */
  class  LarcosVariables
  {
  public:
    LarcosVariables ();
    ~LarcosVariables ();

    /** @brief Directory where you can find Command Line programs such as "CrossValidation.exe"  */
    static  KKStr         CmdLineProgDir  ();  

    static  KKStr         ConfigurationDir ();

    static  KKStr         DataFilesRootDir ();

    static  kkint32       DebugLevel ();

    /** @Returns back the Root Directory where all Larcos related files are to be placed. */
    static  const KKStr&  LarcosHomeDir ();

    static  KKStr         LoggingDir ();  /**< Directory where logging files can be written to.                             */

    static  KKStr         ReportDir ();

    /**@brief Will return the directory path where Scanner files will be placed. */
    static  KKStr         ScannerFilesDefaultDir ();  


    /** @brief will return back the directory where temporary files can be placed. */
    static  KKStr         TempDir ();


    /** @brief  Directory where training library directories will be under.         */
    static  KKStr         TrainingLibrariesDir ();  


    /** @briefDirectory where all Training Model Configuration files are stored.  */
    static  KKStr         TrainingModelsDir    ();  



    /**
     *@brief  Set the Larcos Home Directory to the default location.
     *@details When this method is called the caller wants to reset the Larcos Home Directory to the default location.
     *         If the Environment variable "LarcosHomeDir" exists it will st it to its value otherwise it will be set 
     *         to "C:\Larcos"  or  "/Larcos"  depending on the OS.
     */
    static  void   SetLarcosHomeDir ();


    /** 
     *@brief  Wil set the Larcos Home Directory to the specifie diractory path in '_larcosHomeDir'.
     *@details  Besides the LarcosHomeDir the "KKMLVariables"  "MachineLearningHomeDir" will be set to 
     *          a subdirectory off "LarcosHomeDir". Specificacly it will be set to "${LarcosHomeDir}/MachineLearning".
     */
    static  void   SetLarcosHomeDir (const KKB::KKStr&  _larcosHomeDir);

  private:
    static  KKStr  larcosHomeDir;

    static  kkint32  debugLevel;


  };  /* LarcosVariables */
}  /* LarcosBase*/
#endif

