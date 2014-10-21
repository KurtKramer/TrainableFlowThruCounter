#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>
#include "MemoryDebug.h"
using namespace std;

#include "MemoryDebug.h"
#include "KKBaseTypes.h"
#include "..\KKBase\GoalKeeper.h"
using namespace  KKB;


#include "MLClass.h"
using namespace  KKMachineLearning;


#include "UmiOSservices.h"
#include "UmiClass.h"
#include "UmiClassList.h"
#include "UmiKKStr.h"
using namespace  LarcosCounterManaged;


using namespace System;
using namespace System::Windows::Forms;




String^  UmiClass::GetClassNameFromDirName (String^  subDir)
{
  if  (String::IsNullOrEmpty (subDir))
    return  String::Empty;
  return  UmiKKStr::KKStrToSystenStr (MLClass::GetClassNameFromDirName (UmiKKStr::SystemStringToKKStr (subDir)));
}





UmiClass::UmiClass (String^     _name,
                    UmiClass^   _parent
                   ):
     children            (gcnew List<UmiClass^> ()),
     description         (String::Empty),
     unmanagedImageClass (NULL),
     name                (_name),
     nameUpper           (_name->ToUpper ()),
     parent              (_parent)

{
  unmanagedImageClass = MLClass::CreateNewMLClass (UmiKKStr::SystemStringToKKStr (name));
  if  (_parent != nullptr)
  {
    MLClassPtr  unmanagedParentClass = _parent->UnmanagedImageClass ();
    unmanagedImageClass->Parent (unmanagedParentClass);
  }
}



UmiClass::UmiClass (String^  _name,
                    String^  _parentName
                   ):

     children            (gcnew List<UmiClass^> ()),
     description         (String::Empty),
     unmanagedImageClass (NULL),
     name                (_name),
     nameUpper           (_name->ToUpper ()),
     parent              (nullptr)
{
  unmanagedImageClass = MLClass::CreateNewMLClass (UmiKKStr::SystemStringToKKStr (name));
  if  (!String::IsNullOrEmpty (_parentName))
  {
    parent = UmiClassList::GetUniqueClass (_parentName, "");
    unmanagedImageClass->Parent (parent->UnmanagedImageClass ());
  }
}



MLClassPtr  UmiClass::UnmanagedImageClass ()
{
  if  (unmanagedImageClass)
    return  unmanagedImageClass;

  unmanagedImageClass = MLClass::CreateNewMLClass (UmiKKStr::SystemStringToKKStr (name));
  return  unmanagedImageClass;
}



int  UmiClass::ClassId::get () 
{
  return UnmanagedImageClass ()->ClassId ();
}


float  UmiClass::CountFactor::get ()
{
  return  UnmanagedImageClass ()->CountFactor ();
}


void  UmiClass::Description::set (String^ _description)
{
  description = _description;
  if  (unmanagedImageClass)
    unmanagedImageClass->Description (UmiKKStr::SystemStringToKKStr (description));
}




bool  UmiClass::ChangeNameOfClass (String^ _newName)  
{
  KKStr  newName = UmiKKStr::SystemStringToKKStr (_newName);
  bool  changeSuccessful = false;
  MLClass::ChangeNameOfClass (unmanagedImageClass, newName, changeSuccessful);
  if  (changeSuccessful)
  {
    name = _newName;
    nameUpper = name->ToUpper ();
  }

  return  changeSuccessful;
}  /* ChangeNameOfClass */





String^  UmiClass::ParentName::get ()
{
  if   (parent != nullptr)
    return  parent->Name;
  else
    return  String::Empty;
}



void  UmiClass::CreateAUnManagedClass ()
{
  if  (!unmanagedImageClass)
  {
    unmanagedImageClass = MLClass::CreateNewMLClass (UmiKKStr::SystemStringToKKStr (name));
    unmanagedImageClass->Description (UmiKKStr::SystemStringToKKStr (description));
    if  (parent != nullptr)
      unmanagedImageClass->Parent (parent->UnmanagedImageClass ());
  }
}  /* CreateAUnManagedClass*/







void  UmiClass::Parent::set (UmiClass^ _parent)  
{
  if  (_parent == parent)
    return;

  parent = _parent;
  CreateAUnManagedClass ();
  if  (parent == nullptr)
    unmanagedImageClass->Parent (NULL);
  else
    unmanagedImageClass->Parent (parent->UnmanagedImageClass ());
}  /* Parent::set */




List<LarcosCounterManaged::UmiClass^>^  LarcosCounterManaged::UmiClass::Children::get ()
{
  return  children;
}



bool    UmiClass::StoredOnDataBase::get ()
{
  if  (unmanagedImageClass)
    return  unmanagedImageClass->StoredOnDataBase ();
  else
    return  false;
}



void  UmiClass::StoredOnDataBase::set (bool _storedOnDataBase)
{
  if  (!unmanagedImageClass)
    CreateAUnManagedClass ();
  unmanagedImageClass->StoredOnDataBase (_storedOnDataBase);
}




bool  UmiClass::IsEqual (UmiClass^ c)
{
  return  ((name        == c->name)        &&
           (nameUpper   == c->nameUpper)   &&
           (description == c->description)
          );
}



int  UmiClass::NumOfChildren ()
{
  if  (children == nullptr)
     return 0;

  return  children->Count;
}



void    UmiClass::AddAChild  (UmiClass^  child)
{
  if  (children == nullptr)
    children = gcnew List<UmiClass^> ();
  child->Parent = this;
      
  // Make sure we are not already in the children list.
  bool  alreadyInChildrenList = false;
  for  (int x = 0;  x < children->Count;  x++)
  {
    UmiClass^ c = (*children)[x];
    if  (c == child)
    {
      alreadyInChildrenList = true;
      break;
    }
  }
      
  if  (!alreadyInChildrenList)
    children->Add (child);
}  /* AddAChild */





void  UmiClass::RemoveAChild (UmiClass^ child)
{
  if  (children == nullptr)
    return;
        
  int  x = 0;
  for  (x = 0;  x < children->Count;  x++)
  {
    if  ((*children)[x] == child)
    {
      children->RemoveAt (x);
      break;
    }  
  }
      
  if  (children->Count == 0)
  {
    children = nullptr;
  }
}  /* RemoveAChild */




bool  UmiClass::ParentLoopCheck (UmiClass^ startingClass)
{
  if  (startingClass == this)
    return true;
        
  if  (parent == nullptr)
    return false;
      
  return  parent->ParentLoopCheck (startingClass);
}




UmiClass^  UmiClass::SearchTree (String^  _nameUpper)
{ 
  if  (_nameUpper == nameUpper)
    return this;
      
  if  (children == nullptr)
    return nullptr;
        
  for  (int x = 0;  x < children->Count;  x++)
  {
    UmiClass^  c = (*children)[x];
    UmiClass^  aDecendentNode = c->SearchTree (_nameUpper);
    if  (aDecendentNode != nullptr)
      return  aDecendentNode;
  }
      
  return  nullptr;
}  /* SearchTree */




UmiClass^  UmiClass::SearchFamilyTree (String^      _nameUpper,
                                           UmiClass^  _nodeSearchingFrom
                                          )
{
  if  (_nameUpper == nameUpper)
    return  this;
        
  if  ((parent != nullptr)  &&  (parent != _nodeSearchingFrom))
  {
    UmiClass^  relatedToParent = parent->SearchFamilyTree (_nameUpper, this);
    if  (relatedToParent != nullptr)
      return  relatedToParent;
  }
      
  for  (int x = 0;  x < children->Count;  x++)
  {
    UmiClass^  child = (*children)[x];
    if  (child  != _nodeSearchingFrom)
    {
      UmiClass^  relatedToChild = child->SearchFamilyTree (_nameUpper, this);
      if  (relatedToChild != nullptr)
        return  relatedToChild;
    }
  }
      
  return  nullptr;
}  /* SearchFamilyTree */




bool  UmiClass::IsInMySubTree (UmiClass^  candidate)
{
  if  (candidate->nameUpper == nameUpper)
    return  true;
    
  if  (children == nullptr)
    return false;
        
  bool  isInMySubTree = false;
  for  (int x = 0;  x < children->Count;  x++)
  {
    UmiClass^ c = (*children)[x];
    if  (c->IsInMySubTree (candidate))
    {
      isInMySubTree = true;
      break;
    }
  }
      
  return  isInMySubTree;
}  /* IsInMySubTree */
    


bool  UmiClass::IsAncestor (UmiClass^  candidate)
{
  if  (IsEqual (candidate))
    return  true;

  else if  (parent == nullptr)
    return false;

  else
    return  parent->IsAncestor (candidate);
}  /* IsAncestor */





bool  UmiClass::IsDecendent (UmiClass^  candidate)
{
  if  (IsEqual (candidate))
    return  true;

  if  ((children == nullptr)  ||  (children->Count < 1))
    return false;

  for each  (UmiClass^ c in children)
  {
    if  (c->IsDecendent (candidate))
      return true;
  }

  return false;
}  /* IsDecendent */





// Will return the number of generations between this class and the candidate 
// class.  If 'candidate' is not a decendent then will return -1.
int  UmiClass::NumGenerations (UmiClass^  candidate)
{
  for each  (UmiClass^ pc in children)
  {
    if  (pc == candidate)
      return 1;

    int  zed = pc->NumGenerations (candidate);
    if  (zed >= 0)
      return  zed + 1;
  }

  return  -1;
}  /* NumGenerations */




    
    
void  UmiClass::Save (String^  directory)
{
  String^  fileName = UmiOSservices::AddSlash (directory) + name + ".txt";

  StreamWriter^  sw = nullptr;

  try  {StreamWriter^  sw = gcnew StreamWriter (fileName, false);}
  catch  (Exception^ e)  
  {
    sw = nullptr;

    MessageBox::Show ("Error occured trying to create UmiClass Description file[" + fileName + "]" + "\n\n" +
                      e->ToString (),
                      "UmiClass.Save[" + fileName + "]",
                      MessageBoxButtons::OK
                     );
   }
      
   if  (sw != nullptr)
   {
     sw->WriteLine ("[IMAGE_CLASS]");
     sw->WriteLine ("Name="         + name);
     sw->WriteLine ("Description="  + description);
     if  (parent != nullptr)
       sw->WriteLine ("Parent="     + parent->Name);
     sw->Close ();
     delete  sw;
     sw = nullptr;
   }
}  /* Save */




void  UmiClass::DeleteFromDisk (String^  directory)
{
  String^  fileName = UmiOSservices::AddSlash (directory) + name + ".txt";
  FileInfo^  fi = nullptr;

  try  {fi = gcnew FileInfo (fileName);}  catch (Exception^)  {fi = nullptr;}
  if  (fi != nullptr)
  {
    fi->Delete ();
    delete  fi;
    fi= nullptr;
  }
}  /* DeleteFromDisk */



UmiClass^  UmiClass::MLClassForGivenHierarchialLevel (kkuint32 level)
{
  array<String^>^  levelNames = name->Split ('_');
  if  (level < 1)
    level = 1;

  String^  fullLevelName = "";

  int  curLevel = 0;
  while  ((curLevel < (int)level)  &&  (curLevel < levelNames->Length))
  {
    if  (curLevel < 1)
      fullLevelName = levelNames->GetValue (curLevel)->ToString ();
    else
    {
      fullLevelName += "_";
      fullLevelName += levelNames->GetValue (curLevel)->ToString ();
    }

    curLevel++;
  }

  return  UmiClassList::GetUniqueClass (fullLevelName, "");
}
