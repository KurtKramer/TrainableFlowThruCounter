#if  !defined(_LARCOSVARIABLES_)
#define  _LARCOSVARIABLES_

#include "KKStr.h"


/**
 *@namespace CounterBase  CounterVariables.h
 *@brief Classes and structures that are needed to support LARCOS applications.
 *@details The only class in this namespcace is 'CounterVariables' it provides global 
 * information to all LARCIOS applications such as the Counter home directory.
 */

namespace  CounterBase 
{
  /** 
   *@class  Variables
   *@brief Variables that specific to the Machine Learning Library.
   *@details  All methods in this library are to use these variables to locate files and directories that they need.
   */
  class  CounterVariables
  {
  public:
    CounterVariables ();
    ~CounterVariables ();

    /** @brief Directory where you can find Command Line programs such as "CrossValidation.exe"  */
    static  KKStr         CmdLineProgDir  ();  

    static  KKStr         ConfigurationDir ();

    static  KKStr         DataFilesRootDir ();

    static  kkint32       DebugLevel ();

    /** @Returns back the Root Directory where all Counter related files are to be placed. */
    static  KKStr         CounterHomeDir ();

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
     *@brief  Set the Counter Home Directory to the default location.
     *@details When this method is called the caller wants to reset the Counter Home Directory to the default location.
     *         If the Environment variable "CounterHomeDir" exists it will st it to its value otherwise it will be set 
     *         to "C:\Larcos"  or  "/Larcos"  depending on the OS.
     */
    static  void   SetCounterHomeDir ();


    /** 
     *@brief  Sets the Home Directory to the specific directory path in '_counterHomeDir'.
     *@details  Besides the CounterHomeDir the "KKMLVariables"  "MachineLearningHomeDir" will be set to 
     *          a subdirectory off "CounterHomeDir". Specifically it will be set to "${CounterHomeDir}/MachineLearning".
     */
    static  void   SetCounterHomeDir (const KKB::KKStr&  _counterHomeDir);

  private:
    static  KKStr    counterHomeDir;

    static  kkint32  debugLevel;


  };  /* CounterVariables */
}  /* CounterBase*/
#endif

