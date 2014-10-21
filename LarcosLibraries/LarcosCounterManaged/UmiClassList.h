#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;
using namespace System::IO;

//using namespace UmiCSharpClasses;

#include "UmiGoalKeeper.h"
#include "MLClass.h"
#include "UmiClass.h"


namespace LarcosCounterManaged
{

  ref class  ManagedClassLocator;


  public  ref  class  UmiClassList:  public  List<UmiClass^>
  {
  private:
    UmiClassList (int x, int y, int z);

  public:
    UmiClassList ();
    UmiClassList (String^         _directory);
    UmiClassList (UmiClassList^   _classes);
    UmiClassList (MLClassListPtr  _classes);

  protected:
    !UmiClassList ();

  private:
    ~UmiClassList ();

  public:
    property  String^     Directory  {String^    get () {return  directory; }}
    property  UmiClass^   RootNode   {UmiClass^  get () {return  rootNode;  }  void  set (UmiClass^  _rootNode) {rootNode = _rootNode;}}

    static
    UmiClassList^   CreateARootedEmptyClassList ();

    static
    UmiClassList^   CreateFromDirectoryStructure (String^  directoryName);

    static
    UmiClassList^   CreateFromCommaDelStr (String^  s);

    UmiClassList^   ExtractListOfClassesForAGivenHierarchialLevel (kkuint32 level);


    // Will generate an array of 3 String's.  Each String will represent a seperate line to be 
    // uses as a Class name Header.  Classes will be seperated by the tab('\t') character.
    // The underscore('_') character in the Class name wil be used to deliminate what goes 
    // on which line.  Ex:  The class name "Crustacean_Copepod_Oncea"  will have "Crustacean" 
    // on the 1st line, "Copepod" on the second line, and "Oncea" on the third line.
    List<String^>^    ExtractThreeTitleLines ();

    
    // Using the "_name" parameter will search the global ClassList(allClassesInUniverse)
    // for an existing instance of 'UmiClass' that has the same name.  If one is found
    // that is what is retuned to the caller; otherwise a new intance is created and returned 
    // to the caller.  This new instance will also be added to 'allClassesInUniverse'.
    static  UmiClass^  GetUniqueClass (String^  _name,
                                       String^  _parentName
                                      );


    // Will locate a UmiClass instance that has the same unmanaged mlClass reference.
    // if none exists then will create a new one and add to 'allClassesInUniverse'.
    static  UmiClass^  GetUniqueClass (MLClassPtr  mlClass);  

    static  UmiClass^  GetUnKnownClassStatic ();

    /**
     *@brief Static method that is meant to only work on 'allClassesInUniverse'.
     *@details  Example use would be when deleting or merging classes.  We would
     *          want to redirect any class that has the srcClass as a parent
     *          to the destClass.
     */
    static  void  UmiClassList::UpdateParent (UmiClass^  srcClass,
                                                UmiClass^  destClass
                                               );

    UmiClass^  LookUpByName (String^  className);

    UmiClass^  LookUpByUnmanagedClass (MLClassPtr  unmanagedClass);

    int        LookUpIndex (UmiClass^ c);

    void       Save  (String^  _directory);

    void       SortByName ();

    array<String^>^  ToNameArray ();

    
    /**
     *@brief  Union operator.
     */
    static  UmiClassList^  operator+ (UmiClassList^  left,
                                      UmiClassList^  right
                                     );


    /**
     *@brief  Intersection operator.
     */
    static  UmiClassList^  operator* (UmiClassList^  left,
                                      UmiClassList^  right
                                     );


  private:
    void  CleanUpUnmanagedResources ();

    static  LarcosCounterManaged::UmiGoalKeeper^  getUniqueClassBlocker      // Will make sure that two threads don't try to
      = gcnew LarcosCounterManaged::UmiGoalKeeper ("UniqueClassBlocker");    // add the same class name at the same time.

    UmiClass^  LoadSingleClass (String^  fileName);

    void  LoadSubTreeFromDir ();

    void  ClearDirectory ();

    void  SaveSubTree (UmiClass^  subTree);
  
    void  AddClassesUsingSubdirectoryStructure (DirectoryInfo^  di,
                                                UmiClass^       subTree
                                               );

    String^     directory;
    UmiClass^   rootNode;


    //*********************************************************************************
    //*  The following structuures support the Concept of only one instance of a      *
    //*  'UmiClass' object for the same name will be created.  A global list of     *
    //*  already existinmg instances will be maintained by 'allClassesInUniverse'.    *
    //*  The ability to locate an existing instance quickly will be acomplised by     *
    //*  'allClassesInUniverseUnManagedIndex' and 'allClassesInUniverseNameIndex'.    *
    //*                                                                               *
    //*********************************************************************************
    ref  class  Node
    {
    public:
      Node (MLClassPtr _unmanagedClass,
            UmiClass^  _managedClass
           );

      MLClassPtr  unmanagedClass;
      UmiClass^   managedClass;
      Node^       left;
      Node^       right;
    };  /* Node */
  

    ref class  ManagedClassLocator
    {
    public:
      ManagedClassLocator  ();
      Node^  root;

      bool  Add (MLClassPtr  unmanagedClass,
                 UmiClass^   managedClass
                );

      UmiClass^   LookUp (MLClassPtr  unmanagedClass);
    };  /* ManagedClassLocator */


    ref class  ClassNameComparitor;

    typedef  System::Collections::Generic::SortedList<String^, UmiClass^>  NameIndex;

    static  UmiClassList^       allClassesInUniverse;
    static  ManagedClassLocator^  allClassesInUniverseUnManagedIndex;
    static  NameIndex^            allClassesInUniverseNameIndex;

  };  /* UmiClassList */
}
