#pragma once


using namespace System;
using namespace System::Collections::Generic;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Diagnostics;

#include "FeatureVector.h"
using namespace  KKMachineLearning;

#include "LarcosFeatureVector.h"
using namespace  LarcosCounterUnManaged;


#include "MLClass.h"
#include "UmiRunLog.h"
#include "UmiFeatureVector.h"


namespace LarcosCounterManaged
{
  public ref class UmiFeatureVectorList: List<UmiFeatureVector^>
  {
  public:
    UmiFeatureVectorList ();

    UmiFeatureVectorList (LarcosFeatureVectorList&  examples);   /**< Will take ownership of "examples"  contents. */

    UmiFeatureVectorList (FeatureVectorList&  examples);  /**< Will take ownership of "examples"  contents  and reduce it to an empty list.
                                                           * will convert contents into instances of 'PostLarvaeFV'.  If current contents 
                                                           * are instances of 'FeatureVector' will delete them; so make sure that they 
                                                           * are not being used anywhere else.
                                                           */


  private:
    ~UmiFeatureVectorList ();

  protected:
    !UmiFeatureVectorList ();

  public:


	  property  kkuint32  NumExamplesWritten   {kkuint32 get (); }

    property  bool  Owner   {bool get () {return owner;}  void  set (bool _owner)  {owner = _owner;} }



    void  AddQueue (UmiFeatureVectorList^ subQueue);


    void  CancelLoad ();


    ///</summary>
    ///  Will create a list of FeatureVectors where the class assignment will reflect the
    ///  specified Hierarchy level specified by 'level'.  The hierarchy of a given class will
    ///  be indicated by underscore characters in the class name.
    ///
    ///  ex:   Level 1:  gelatinous
    ///        Level 2:  gelatinous_hydromedusae
    ///        Level 3:  gelatinous_hydromedusae_solmundella                                      *
    ///</summary>
    UmiFeatureVectorList^   CreateListForAGivenLevel (kkuint32      level,
                                                        UmiRunLog^  runLog
                                                       );

    UmiFeatureVectorList^  ExtractImagesForAGivenClass (UmiClass^  mlClass);

    UmiClassList^  ExtractListOfClasses ();


    void  LoadInSubDirectoryTree (String^     rootDir,
                                  bool        useDirectoryNameForClassName,
                                  UmiRunLog^  log,
                                  bool        rewiteRootFeatureFile
                                 );
    
    UmiFeatureVector^  LookUpByImageFileName (String^  imageFileName);

    void  RandomizeOrder ();

    void  Save (String^     fileName,
                UmiRunLog^  runLog
               );

    void  SortByImageFileName ();

    UmiFeatureVectorList^   StratifyAmoungstClasses (int  numOfFolds);

    // Will create a "FeatureVectorList" object that will contain the unmanaged instances that are in this list.
    // it will not own these instances because the Managed objects that currently own them; and if you delete them it will cause 
    // memory problems.
    FeatureVectorListPtr      ToFeatureVectorList (UmiRunLog^  runLog);  // Creates an unmanaged list of feature vectors.


  private:
    void  CleanUpUnmanagedResources ();


    bool*           cancelFlag;   // the "CancelLoad" method will monitor this flag;  if set to true will terminate.
    MLClassListPtr  classes;
    kkuint32*       numExamplesWritten;  // used by SaveFeatureFile 
    bool            owner;
    bool            sortedByImageFileName;

    ref  class  FeatureVectorComparitorByImageFileName;
  };  /* UmiFeatureVectorList */

}  /* LarcosCounterManaged */
