#ifndef _UMICLASS_
#define _UMICLASS_


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Diagnostics;


#include "MLClass.h"
#include "PostLarvaeFV.h"
#include "UmiRunLog.h"
using namespace  KKB;
using namespace  KKMLL;


namespace LarcosCounterManaged 
{
  /**
   *@author  Kurt A. Kramer
   *@class  UmiClass
   *@brief  Represents a single 'Class' in the machine learning world.
   *@details 
   * Machine Learning Class.  Keep in mind that when we use the word 'class' in these 
   * comments we are referring the Machine Learning concept of 'class' and not the c++ 
   * or c# idea.  The idea is that for any image or example(more general) a class can 
   * be assigned to them.  This class is to indicate a grouping which will allow us to 
   * group like images/examples together.
   *
   * UmiClass is the Managed Version of the unmanaged class 'KKMLL::MLClass'.  
   * For every instance of this class there will be a matching instance of a 'MLClass' 
   * object.  Also just like 'MLClass' there can only be one instance of a class with
   * the same name.  This helps simplify other programming because we can compare pointers 
   * or handles directly to know if we are talking about the same class.  To accomplish 
   * this, the creation of new Classes is done through a static method called 
   * UmiClassList::GetUniqueClass.
   */

  public ref  class UmiClass
  {
  public:
    UmiClass (String^       _name,
              UmiClass^   _parent
             );

    UmiClass (String^  _name,
              String^  _parentName
             );

    property  int                 ClassId          {int              get ();}
    property  float               CountFactor      {float            get ();}
    property  String^             Description      {String^          get () {return description;};  void set (String^ _description);}
    property  String^             Name             {String^          get () {return name;}}
    property  String^             NameUpper        {String^          get () {return nameUpper;}}
    property  UmiClass^           Parent           {UmiClass^        get () {return parent;}        void set (UmiClass^ _parent);}
    property  String^             ParentName       {String^          get ();}
    property  List<UmiClass^>^    Children         {List<UmiClass^>^ get ();}
    property  bool                StoredOnDataBase {bool             get ();                        void set (bool _StoredOnDataBase);}  


    void    AddAChild  (UmiClass^  child);

    bool    ChangeNameOfClass (String^ newName);  /**< Will only change the name if no other instance of 'MLClass' is using the new name */

    void    CreateAUnManagedClass ();

    void    DeleteFromDisk (String^  directory);

    UmiClass^  MLClassForGivenHierarchialLevel (kkuint32 level);

    bool    IsEqual (UmiClass^ c);

    bool    IsAncestor (UmiClass^  candidate);

    bool    IsDecendent (UmiClass^  candidate);

    bool    IsInMySubTree (UmiClass^  candidate);

    /**
     *@brief  Determines the number of generations between this class and the candidate class.  
     *@details We search all our children to determine if Candidate is one our decedents. If 
     *         it is not we return -1 otherwise we return the number of generations below us.
     *         Our immediate children will be considered 1 generation apart.
     *@param[in] Candidate Class that we are searching for amongst out children.
     *@return The number of generations between us and the candidate or -1 if not one of our decedents.
     */
    int     NumGenerations (UmiClass^  candidate);

    int     NumOfChildren ();

    bool    ParentLoopCheck (UmiClass^ startingClass);

    void    RemoveAChild (UmiClass^ child);
      
    void    Save (String^  directory);

    UmiClass^  SearchTree (String^  _nameUpper);

    UmiClass^  SearchFamilyTree (String^    _nameUpper,
                                 UmiClass^  _nodeSearchingFrom
                                );

    /** @brief Returns a pointer to the unmanaged version of this Class. */
    MLClassPtr  UnmanagedImageClass ();


    static String^  GetClassNameFromDirName (String^  subDir);

  private:
    String^        description;
    String^        name;
    String^        nameUpper;
    UmiClass^      parent;
    MLClassPtr     unmanagedImageClass;  // Our link to the unmanaged version of this class;

    List<UmiClass^>^  children;
  
  };  /* UmiClass */

}  /* LarcosCounterManaged */

#endif
