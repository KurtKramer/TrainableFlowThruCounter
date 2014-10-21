#include "StdAfx.h"
#include "FirstIncludes.h"

#include <stdio.h>
#include <math.h>

#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <map>
#include <vector>
#include "MemoryDebug.h"
using namespace std;


#include "..\KKBase\GoalKeeper.h"
#include "KKBaseTypes.h"
using namespace KKB;


#include "MLClass.h"
using namespace  KKMachineLearning;


#include "UmiClass.h"
#include "UmiClassList.h"
#include "UmiKKStr.h"
using namespace  LarcosCounterManaged;

using namespace System;



UmiClassList::UmiClassList (int x, int y, int z):  
    List<UmiClass^> (),
    directory         (nullptr),
    rootNode          (nullptr)

{
  // Only the creation of 'allClassesInUniverse'  will call this constructor.
  directory = "";
  rootNode = gcnew UmiClass ("AllClasses", "");
  Add (rootNode);
}


UmiClassList::UmiClassList ():
    List<UmiClass^> (),
    directory         (nullptr),
    rootNode          (nullptr)

{
  directory = "";
}




UmiClassList::UmiClassList (String^  _directory):  
  List<UmiClass^> (),
  directory         (_directory),
  rootNode          (nullptr)

{
  rootNode = GetUniqueClass ("AllClasses", "");
  Add (rootNode);
  try  {LoadSubTreeFromDir ();}
  catch (Exception^ e)  
  {
    rootNode = nullptr;
    String^ exceptionStr = "UmiClassList  Could not load Directory[";
    exceptionStr += directory;
    exceptionStr += gcnew String ("]");
    throw  gcnew Exception (exceptionStr, e);
  }
}


UmiClassList::UmiClassList (UmiClassList^  _classes)
{
  directory = _classes->directory;
  rootNode  = _classes->rootNode;

  for each  (UmiClass^  pc in  _classes)
  {
    Add (pc);
  }
}


UmiClassList::UmiClassList (const MLClassListPtr  _classes)
{
  MLClassList::const_iterator  idx;
  for  (idx = _classes->begin ();  idx != _classes->end ();  ++idx)
  {
    MLClassPtr  ic = *idx;
    Add (gcnew UmiClass (UmiKKStr::KKStrToSystenStr (ic->Name ()), UmiKKStr::KKStrToSystenStr (ic->ParentName ())));
  }
}




UmiClassList::~UmiClassList ()
{
  this->!UmiClassList ();
}


UmiClassList::!UmiClassList ()
{
 CleanUpUnmanagedResources ();
}


void  UmiClassList::CleanUpUnmanagedResources ()
{
}




UmiClassList^   UmiClassList::CreateARootedEmptyClassList ()
{
  UmiClassList^  classes = gcnew UmiClassList ();

  UmiClass^  rootNode = GetUniqueClass ("AllClasses", "");
  classes->Add (rootNode);
  classes->RootNode = rootNode;
  return  classes;
}  /* CreateARootedEmptyClassList */



UmiClassList^  UmiClassList::CreateFromCommaDelStr (String^  s)
{
  UmiClassList^ l = gcnew UmiClassList ();
  if  (String::IsNullOrEmpty (s))
    return  l;

  array<String^>^  fields = s->Split (',');
  for  (int  x = 0;  x < fields->Length;  x++)
  {
    String^  className = fields[x]->Trim ();
    if  (className->Length > 0)
      l->Add (GetUniqueClass (className, nullptr));
  }

  return  l;
}  /* CreateFromCommaDelStr */








int  UmiClassList::LookUpIndex (UmiClass^ c)
{
  int  index = 0;
  for  (index = 0;  index < Count;  index++)
  {
    if  (c == (*this)[index])
      return index;
  }
  return -1;
}  /* LookUpIndex */



UmiClass^  UmiClassList::LookUpByName (String^  className)
{
  String^  classNameUpper = className->ToUpper ();
      
  for  (int x = 0;  x < this->Count;  x++)
  {
    UmiClass^  c = (*this)[x];
    if  (c->NameUpper == classNameUpper)
      return  c;
  }

  return nullptr;
}  /* LookUpByName */



UmiClass^  UmiClassList::LookUpByUnmanagedClass (MLClassPtr  unmanagedClass)
{
  for each (UmiClass^ pc in (*this))
  {
    if  (pc->UnmanagedImageClass () == unmanagedClass)
      return pc;
  }

  return  nullptr;
}



void  UmiClassList::Save  (String^  _directory)
{
  directory = _directory;
  ClearDirectory ();
  SaveSubTree (rootNode);
}  /* Save */




UmiClassList^  UmiClassList::CreateFromDirectoryStructure (String^  directoryName)
{
  UmiClassList^  list = gcnew UmiClassList ();
      
  UmiClass^  root = list->RootNode;
      
  DirectoryInfo^ di = nullptr;
  try  {di = gcnew DirectoryInfo (directoryName);}
  catch  (Exception^)  {di = nullptr;}
  
  if  (di == nullptr)
    return  list;

  if  (!di->Exists)
    throw gcnew Exception ("UmiClassList.CreateFromDirectoryStructure  Dir[" + directoryName + "] does not exist.");

  list->AddClassesUsingSubdirectoryStructure (di, root);
      
  return  list; 
}  /* CreateFromDirectoryStructure */
  
  
  
UmiClass^  UmiClassList::LoadSingleClass (String^  fileName)
{
  StreamReader^  sr = nullptr;
  try  {sr = gcnew StreamReader (fileName);}
  catch (Exception^)  {sr = nullptr;}
        
  if  (sr == nullptr)
    return  nullptr;
          
  String^  firstLine = sr->ReadLine ()->ToUpper ();
  while  ((firstLine != nullptr)  &&  (firstLine != "[IMAGE_CLASS]"))
    firstLine = sr->ReadLine ()->ToUpper ();
          
  if  (firstLine == nullptr)
  {
    sr->Close();
    delete  sr;
    sr = nullptr;
    return  nullptr;
  }

  String^  className   = "";
  String^  description = "";
  String^  parentName  = "";

  String^  line = sr->ReadLine ();
  while  (line != nullptr)
  {
    array<String^>^  lineParts = line->Split ('=');
    if  (lineParts->Length == 2)
    {
      String^  fieldName  = lineParts->GetValue (0)->ToString ()->ToUpper ();
      String^  fieldValue = lineParts->GetValue (1)->ToString ()->ToUpper ();

      if  (fieldName == "NAME")
        className = fieldValue;
              
      else if  (fieldName == "DESCRIPTION")
        description = fieldValue;
 
      else if  (fieldName == "PARENT")
        parentName = fieldValue;
    }
         
    line = sr->ReadLine ();
  }
      
  sr->Close ();
  delete  sr;
  sr = nullptr;
      
  UmiClass^  newClass = UmiClassList::GetUniqueClass (className, parentName);
  newClass->Description = description;
  return  newClass;
}  /* LoadSingleClass */



void  UmiClassList::LoadSubTreeFromDir ()
{
  DirectoryInfo^  di = nullptr;
  try  {di = gcnew DirectoryInfo (directory);}
  catch  (Exception^ e){throw gcnew Exception ("Error getting directory info for DirName[" + directory + "]", e);}
      
  if  (!di->Exists)
    throw gcnew Exception ("ImageList.LoadSubTreeFromDir  Directory[" + directory + "] does not exist.");
      
   array<FileInfo^>^ files = di->GetFiles ("*.txt");

   if  (files == nullptr)
     return;
      
   int  x;

   for  (x = 0;  x < files->Length;  x++)
   {
     FileInfo^  fi = (FileInfo^)(files->GetValue (x));

     UmiClass^  c = LoadSingleClass (directory + fi->Name);
     if  (c == nullptr)
       continue;

     if  (c->NameUpper == "ALLCLASSES")
     {
       c = nullptr;
     }
     else
     {
       Add (c);          
     }
   }        

   for each (UmiClass^ c in *this)
   {
     if  (c != rootNode)
     {
       UmiClass^  parent = nullptr;
        
       if  (String::IsNullOrEmpty (c->ParentName))
         parent = rootNode;
       else
       {
         parent =  LookUpByName (c->ParentName);
         if  (parent == nullptr)
           parent = rootNode;
       }
          
       parent->AddAChild (c);
     }
   }
      
   files = nullptr;
   di = nullptr;
 }  /* LoadSubTreeFromDir */



 void  UmiClassList::ClearDirectory ()
 {
   DirectoryInfo^  di = nullptr;
   try  {di = gcnew DirectoryInfo (directory);}
   catch  (Exception^ e)  
   { 
     di = nullptr;
     throw gcnew Exception ("UmiClassList.ClearDirectory   Could not get directory info[" + directory + "]", e);
   }

   if  (!di->Exists)
   {
     try  {di->Create ();}
     catch  (Exception^ e2)  {throw gcnew Exception ("Could not create Directory[" + directory + "]", e2); }
   }
      
   array<FileInfo^>^ files = di->GetFiles ("*.txt");
   for  (int x = 0;  x < files->Length;  x++)
   {
     FileInfo^ fi = (FileInfo^)(files->GetValue (x));
     fi->Delete ();
   }

   delete  files;
   files = nullptr;

   delete  di;
   di = nullptr;

} /* ClearDirectory */



void  UmiClassList::SaveSubTree (UmiClass^  subTree)
{
  subTree->Save (directory);

  List<UmiClass^>^  children = subTree->Children;
  if  (children != nullptr)
  {
    for  (int x = 0;  x < children->Count;  x++)
    {
      UmiClass^ c = (*children)[x];
      SaveSubTree (c);
    }
  }
}  /* SaveSubTree */
    
    
  
void  UmiClassList::AddClassesUsingSubdirectoryStructure (DirectoryInfo^  di,
                                                            UmiClass^     subTree
                                                           )
{
  array<DirectoryInfo^>^  subDirectories = di->GetDirectories ();

  for  (int idx = 0;  idx < subDirectories->Length;  idx++)
  {
    DirectoryInfo^ subDirectory = (DirectoryInfo^)subDirectories->GetValue (idx);
    if  ((subDirectory->Name  == ".")  ||  (subDirectory->Name == ".."))
      continue;

    UmiClass^  c = UmiClassList::GetUniqueClass (subDirectory->Name, "");
    c->Parent = subTree;
    subTree->AddAChild (c);
    Add (c);
        
    AddClassesUsingSubdirectoryStructure (subDirectory, c);
  }
}  /* AddClassesUsingSubdirectoryStructure */
  
  

ref class  UmiClassList::ClassNameComparitor : public System::Collections::Generic::IComparer<UmiClass^>
{
public:
   virtual  int  Compare (UmiClass^ a, UmiClass^ b)
  {
    return  a->NameUpper->CompareTo (b->NameUpper);
  }
};
			

void  UmiClassList::SortByName ()
{
  ClassNameComparitor^ c = gcnew ClassNameComparitor ();
  Sort (c);
}




array<String^>^  UmiClassList::ToNameArray ()
{
  array<String^>^  results = gcnew array<String^> (Count);

  for  (int x = 0;  x < Count;  ++x)
    results[x] = (*this)[x]->Name;
  return results;
}






UmiClassList^  UmiClassList::operator+ (UmiClassList^  left,
                                        UmiClassList^  right
                                       )
{
  UmiClassList^  results = gcnew UmiClassList (left);
  for each  (UmiClass^ c in right)
  {
    if  (results->LookUpByUnmanagedClass (c->UnmanagedImageClass ()) == nullptr)
      results->Add (c);
  }
  results->SortByName ();
  return  results;
}




UmiClassList^  UmiClassList::operator* (UmiClassList^  left,
                                        UmiClassList^  right
                                       )
{
  UmiClassList^  results = gcnew UmiClassList ();
  for each  (UmiClass^ c in left)
  {
    if  (right->LookUpByUnmanagedClass (c->UnmanagedImageClass ()) != nullptr)
      results->Add (c);
  }
  results->SortByName ();
  return  results;
}



//********************************************************************************
//*  Supports the idea that there is only one instance of a class with the same  *
//*  name in memory.                                                             *
//********************************************************************************
//UmiClassList^  UmiClassList::allClassesInUniverse = nullptr;

UmiClass^  UmiClassList::GetUniqueClass (String^  _name,
                                         String^  _parentName
                                        )
{
  getUniqueClassBlocker->StartBlock ();

  if  (allClassesInUniverse == nullptr)
  {
    allClassesInUniverse               = gcnew UmiClassList (0, 0, 0);
    allClassesInUniverseUnManagedIndex = gcnew ManagedClassLocator ();
    allClassesInUniverseNameIndex      = gcnew NameIndex ();
  }

  UmiClass^  existingScsClass = nullptr;
  String^  upperName = _name->ToUpper ();


  int  pcIDX = allClassesInUniverseNameIndex->IndexOfKey (upperName);
  if  (pcIDX >= 0)
  {
    existingScsClass = allClassesInUniverseNameIndex->Values[pcIDX];
  }
  else
  {
    UmiClass^  newClass = gcnew UmiClass (_name, _parentName);
    allClassesInUniverse->Add (newClass);
    allClassesInUniverseNameIndex->Add (upperName, newClass);
    allClassesInUniverseUnManagedIndex->Add (newClass->UnmanagedImageClass (), newClass);
    existingScsClass = newClass;
  }

  getUniqueClassBlocker->EndBlock ();

  return  existingScsClass;
}  /* GetUniqueClass */





//********************************************************************************
//*  Supports the idea that there is only one instance of a class with the same  *
//*  name in memory.                                                             *
//********************************************************************************
UmiClass^  UmiClassList::GetUniqueClass (MLClassPtr  mlClass)
{
  if  (allClassesInUniverse == nullptr)
  {
    getUniqueClassBlocker->StartBlock ();
    if  (allClassesInUniverse == nullptr)
	{
      allClassesInUniverse               = gcnew UmiClassList (0, 0, 0);
      allClassesInUniverseUnManagedIndex = gcnew ManagedClassLocator ();
      allClassesInUniverseNameIndex      = gcnew NameIndex ();
      getUniqueClassBlocker->EndBlock ();
    }
  }

  UmiClass^  c = allClassesInUniverseUnManagedIndex->LookUp (mlClass);

  if  (c == nullptr)
  {
    c = GetUniqueClass (UmiKKStr::KKStrToSystenStr (mlClass->Name ()), UmiKKStr::KKStrToSystenStr (mlClass->ParentName ()));
  }

  return  c;

}  /* GetUniqueClass */



UmiClass^  UmiClassList::GetUnKnownClassStatic ()
{
  return  GetUniqueClass (MLClass::GetUnKnownClassStatic ());
}




/**
 *@brief Static method that is meant to only work on 'allClassesInUniverse'.
 */
void  UmiClassList::UpdateParent (UmiClass^  srcClass,
                                    UmiClass^  destClass
                                   )
{
  if  (allClassesInUniverse == nullptr)
    return;

  for each  (UmiClass^  pc  in allClassesInUniverse)
  {
    if  (pc->Parent == srcClass)
      pc->Parent = destClass;
  }
}  /* MergeClasses */




UmiClassList^  UmiClassList::ExtractListOfClassesForAGivenHierarchialLevel (kkuint32 level)
{
  UmiClassList^  newList = gcnew UmiClassList ();
  for each (UmiClass^ c in *this)
  {
    UmiClass^  levelClass = c->MLClassForGivenHierarchialLevel (level);
    if  (newList->LookUpIndex (levelClass) < 0)
      newList->Add (levelClass);
  }
  return  newList;
}  /* ExtractListOfClassesForAGivenHierarchialLevel */





  //**********************************************************************************
  //*  Needed by UmiClassList to help locate instanes of 'UmiClass'  when given  *
  //*  a pointer to a 'MLClass' object.  It implements a simple Binary Tree with  *
  //*  that supports only 2 operations  'Add'  and  'LookUp'.                        *
  //*                                                                                *
  //*  neither 'map<>' or 'SortedList<>' could wuld with both Managed and Unmanaged  *
  //*  objects together.                                                             *
  //*                                                                                *
  //*  This is an UnBalanced tree so some thought into reblancing it needs 2 b done. *
  //**********************************************************************************
UmiClassList::ManagedClassLocator::ManagedClassLocator  ()
{
  root = nullptr;
}


UmiClassList::Node::Node (MLClassPtr _unmanagedClass,
                          UmiClass^  _managedClass
                         ):
       unmanagedClass (_unmanagedClass),
       managedClass   (_managedClass),
       left           (nullptr),
       right          (nullptr)
{}


 
  

bool  UmiClassList::ManagedClassLocator::Add (MLClassPtr   unmanagedClass,
                                              UmiClass^    managedClass
                                             )
{
  if  (root == nullptr)
  {
    root = gcnew Node (unmanagedClass, managedClass);
    return  true;
  }

  Node^  next = root;
  while  (true)
  {
    if  (unmanagedClass == next->unmanagedClass)
    {
      // We have a duplicate entry.
      return  false;  // Not Successful
    }

    if  (unmanagedClass < next->unmanagedClass)
    {
      if  (next->left == nullptr)
      {
        next->left = gcnew Node (unmanagedClass, managedClass);
        return true;
      }
      else
      {
        next = next->left;
      }
    }
    else
    {
      if  (next->right == nullptr)
      {
        next->right = gcnew Node (unmanagedClass, managedClass);
        return true;
      }
      else
      {
        next = next->right;
      }
    }
  }  /* End of while loop */
}  /* Add */



UmiClass^   UmiClassList::ManagedClassLocator::LookUp (MLClassPtr  unmanagedClass)
{
  if  (root == nullptr)
    return nullptr;

  Node^  next = root;
  while  (next != nullptr)
  {
    if  (next->unmanagedClass == unmanagedClass)
      return  next->managedClass;
    if  (unmanagedClass < next->unmanagedClass)
      next = next->left;
    else
      next = next->right;
  }

  return  nullptr;
}





List<String^>^  UmiClassList::ExtractThreeTitleLines ()
{
  KKStr  hd1 (Count * 30);
  KKStr  hd2 (Count * 30);
  KKStr  hd3 (Count * 30);


  int x;
  for  (x = 0;  x < Count;  x++)
  {
    if  (x > 0)
    {
      hd1 << "\t";
      hd2 << "\t";
      hd3 << "\t";
    }

    KKStr  part1 = "";
    KKStr  part2 = "";
    KKStr  part3 = "";

    int  numOfParts = 0;

    KKStr  className = UmiKKStr::SystemStringToKKStr((*this)[x]->Name);
    className.TrimLeft ();
    className.TrimRight ();
   
    numOfParts = 1;
    part1 = className.ExtractToken ("_");
    if  (!className.Empty ())
    {
      numOfParts = 2;
      part2 = className.ExtractToken ("_");

      if  (!className.Empty ())
      {
        numOfParts = 3;
        part3 = className;
      }
    }


    switch  (numOfParts)
    {
      case 1: hd3 << part1;
              break;

      case 2: hd2 << part1;
              hd3 << part2;
              break;

      case 3: hd1 << part1;
              hd2 << part2;
              hd3 << part3;
              break;
    }
  }

  List<String^>^  results = gcnew List<String^> ();
  results->Add (UmiKKStr::KKStrToSystenStr (hd1));
  results->Add (UmiKKStr::KKStrToSystenStr (hd2));
  results->Add (UmiKKStr::KKStrToSystenStr (hd3));

  return  results;
}  /* ExtractThreeTitleLines */
