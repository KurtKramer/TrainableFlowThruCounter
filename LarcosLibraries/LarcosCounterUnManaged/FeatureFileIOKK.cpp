#include  "FirstIncludes.h"

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <typeinfo> 
#include <vector>
#include <string.h>

#include "MemoryDebug.h"

using namespace std;


#include "KKBaseTypes.h"
#include "DateTime.h"
#include "ImageIO.h"
#include "OSservices.h"
#include "RunLog.h"
#include "KKStr.h"
using namespace KKB;


#include "FileDesc.h"
#include "MLClass.h"
#include "LarcosFeatureVector.h"
#include "LarcosFVProducer.h"

using namespace KKMachineLearning;
           
           
#include "FeatureFileIOKK.h"
using namespace LarcosCounterUnManaged;
           
           
FeatureFileIOKKPtr  FeatureFileIOKK::driver = CreateAndRegisterInstance ();



FeatureFileIOKKPtr  FeatureFileIOKK::CreateAndRegisterInstance ()
{
  FeatureFileIOKKPtr d = new FeatureFileIOKK ();
  FeatureFileIO::RegisterDriver (d);
  return d;
}



FeatureFileIOKK::FeatureFileIOKK ():
   FeatureFileIO ("KK",
                  true,     // true = _canRead,
                  true      // true = _canWrite,
                 )                
{
}



FeatureFileIOKK::~FeatureFileIOKK ()
{
}


void  FeatureFileIOKK::Parse_FEATURE_DATA_FILE_Line (KKStr&   line,
                                                     kkint32&  version,
                                                     kkint32&  numOfFields,
                                                     kkint32&  numOfExamples
                                                    )
{
  KKStr  field = line.ExtractToken (" ,\t\n\r");  // Skip past FEATURE_DATA_FILE field.
  KKStr  fieldValue;
  field = line.ExtractToken (" ,\t\n\r");
  while  (!field.Empty ())
  {
    fieldValue = line.ExtractToken (" ,\t\n\r");

    if  (field.EqualIgnoreCase ("Version"))
    {
      version = atoi (fieldValue.Str ());
    }

    else if  (field.EqualIgnoreCase ("NumOfFeatures"))
    {
      numOfFields = atoi (fieldValue.Str ());
    }

    else if  (field.EqualIgnoreCase ("NumOfExamples"))
    {
      numOfExamples = atoi (fieldValue.Str ());
    }

    field = line.ExtractToken (" ,\t\n\r").ToUpper ();
  }
}  /* Parse_FEATURE_DATA_FILE_Line */




VectorInt  FeatureFileIOKK::CreateIndirectionTable (const VectorKKStr&  fields,
                                                    kkint32             numOfFeatures
                                                   )
{
  VectorInt   indirectionTable;
  kkint32  fieldNum = 0;

  for  (fieldNum = 0;  fieldNum < numOfFeatures;  fieldNum++)
    indirectionTable.push_back (rfFeatureData);

  for  (fieldNum = numOfFeatures;  fieldNum < (kkint32)fields.size ();  fieldNum++)
  {
    KKStr  fieldName = fields[fieldNum].ToLower ();
    if  (fieldName == "classname")
      indirectionTable.push_back (rfClassName);

    else if  (fieldName == "classnameidx")
      indirectionTable.push_back (rfImageClassIDX);

    else if  (fieldName == "filename") 
      indirectionTable.push_back (rfImageFileName);

    else if  (fieldName == "origsize") 
      indirectionTable.push_back (rfOrigSize);

    else if  (fieldName == "edgepixels") 
      indirectionTable.push_back (rfNumOfEdgePixels);

    else if  (fieldName == "probability") 
      indirectionTable.push_back (rfProbability);

    else if  (fieldName == "disttoborder") 
      indirectionTable.push_back (rfUnKnown);

    else if  (fieldName == "centroidrow")
      indirectionTable.push_back (rfCentroidRow);

    else if  (fieldName == "centroidcol")
      indirectionTable.push_back (rfCentroidCol);

    else if  (fieldName == "predictedclass")
      indirectionTable.push_back (rfPredictedClass);

    else if  (fieldName == "predictedclassidx")
      indirectionTable.push_back (rfPredictedClassIDX);

    else if  (fieldName == "breaktie")
      indirectionTable.push_back (rfBreakTie);

    else
      indirectionTable.push_back (rfUnKnown);

  }

  return  indirectionTable;
}  /* CreateIndirectionTable */






FileDescPtr  FeatureFileIOKK::GetFileDesc (const KKStr&    _fileName,
                                           istream&        _in,
                                           MLClassListPtr  _classes,
                                           kkint32&        _estSize,
                                           KKStr&          _errorMessage,
                                           RunLog&         _log
                                          )
{
  char  buff[102400];

  _errorMessage = "";
  
  _in.getline (buff, sizeof (buff) - 1, '\n');
  KKStr  firstLine (buff);
  firstLine.Upper ();

  if  (firstLine.SubStrPart (0, 16) != "FEATURE_DATA_FILE")
  {
    // We are looking at a very old RAW file,  need to scan for number of fields.
    // Lets count the number of fields in each lime,  
    _log.Level (-1) << endl << endl 
                    << "FeatureFileIOKK::GetFileDesc    ***ERROR***    the first line must contain" << endl
                    << "                                                  'FEATURE_DATA_FILE'" << endl
                    << endl;
    _errorMessage = "The first line must contain FEATURE_DATA_FILE.";
    return NULL;
  }


  kkint32  version       = -1;
  kkint32  numOfFeatures = -1;
  kkint32  numOfExamples = -1;
  Parse_FEATURE_DATA_FILE_Line (firstLine, version, numOfFeatures, numOfExamples);
   if  (version < 39)
  {
    _log.Level (-1) << endl << endl 
                    << "FeatureFileIOKK::GetFileDesc    ***ERROR***"  << endl
                    << endl
                    << "                     Version Number[" << version << "]" << endl
                    << "                     Can not load versions prior to 39." << endl
                    << endl;
    _errorMessage << "Version[" << version << "]  is prior to '39'.";
    return NULL;
  }

  _in.getline (buff, sizeof (buff) - 1);
  KKStr  fieldDescLn (buff);
  VectorKKStr  fields = fieldDescLn.Split ("\n\r\t");
  if  (fields.size () < (kkuint32)numOfFeatures)
  {
    _log.Level (-1) << endl << endl << endl
                    << "FeatureFileIOKK::GetFileDesc     **** Error ***"  << endl
                    << endl
                    << "               Num fields[" << (kkuint32)fields.size () << "]  is less than the number specified." << endl
                    << endl
                    << endl;
    _errorMessage << "Num fields[" << (kkuint32)fields.size () << "]  is less than the number specified." << endl;
    return NULL;
  }
  
   
  FileDescPtr  fd = new FileDesc ();
  fd->Version (version);


  bool  alreadyExists;
  kkint32  featureNum = 0;
  for  (featureNum = 0;  featureNum < numOfFeatures;  featureNum++)
  {
    KKStr&  fieldName = fields[featureNum];
    fd->AddAAttribute (fieldName, NumericAttribute, alreadyExists);
    if  (alreadyExists)
    {
      _log.Level (-1) << endl << endl 
                      << "FeatureFileIOKK::GetFileDesc    ***ERROR***"  << endl
                      << endl
                      << "                     Duplicate Attribute Name[" << fieldName << "]" << endl
                      << endl;
      _errorMessage << "Duplicate Attribute Name[" << fieldName << "]";

      // Can not delete an instance of a 'FileDesc' class once it has been created.
      // delete  fd;
      return NULL;
    }
  }

  return  fd;
}  /* GetFileDesc */





LarcosFeatureVectorListPtr  FeatureFileIOKK::LoadFile (const KKStr&      _fileName,
                                                       const FileDescPtr _fileDesc,
                                                       MLClassList&      _classes, 
                                                       istream&          _in,
                                                       kkint32           _maxCount,    // Maximum # images to load,  less than '0'  indicates all.
                                                       VolConstBool&     _cancelFlag,
                                                       bool&             _changesMade,
                                                       KKStr&            _errorMessage,
                                                       RunLog&           _log
                                                      )
{
  _log.Level (10) << "FeatureFileIOKK::LoadFile   Loading file[" << _fileName << "]." << endl;
  _log.Flush ();


  VectorInt  featureFieldIndexTable;

  char buff[20480];

  _in.getline (buff, sizeof (buff) - 1);
  KKStr  firstLine (buff);

  firstLine.Upper ();

  if  (firstLine.SubStrPart (0, 16) != "FEATURE_DATA_FILE")
  {
    _log.Level (-1) << endl << endl << endl 
                    << "FeatureFileIOKK::LoadFile     **** Error missing 'FEATURE_DATA_FILE' line ****    can not load" << endl
                    << endl;
    _errorMessage = "Error missing 'FEATURE_DATA_FILE' line";
    return  NULL;
  }

  kkint32  version         = -1;
  kkint32  numOfFeatures   = -1;
  kkint32  numOfExamples   = -1;
  Parse_FEATURE_DATA_FILE_Line (firstLine, version, numOfFeatures, numOfExamples);
  if  (version < 38)
  {
    _log.Level (-1) << endl << endl << endl
                   << "FeatureFileIOKK::LoadFile     ***Error***    File version[" << version << "]  prior to 38.   can not load."
                   << endl
                   << endl;
    _errorMessage << "File version[" << version << "]  prior to 38.   can not load.";
    return  NULL;
  }
  

  // The second line will have a list of fields;  we will use ths line to build
  // an indirection table.
  _in.getline (buff, sizeof (buff) - 1);
  KKStr  fieldDescLine (buff);
  VectorKKStr  fields = fieldDescLine.Parse ("\n\r\t");
  if  ((kkint32)fields.size () < numOfFeatures)
  {
    _log.Level (-1) << endl << endl << endl
                   << "FeatureFileIOKK::LoadFile     **** Error ***"  << endl
                   << endl
                   << "               Num fields[" << (kkuint32)fields.size () << "]  is less than the number specified." << endl
                   << endl
                   << endl;
    _errorMessage << "Num fields[" << (kkuint32)fields.size () << "]  is less than the number specified." << endl;
    return  NULL;
  }


  VectorInt   indirectionTable = CreateIndirectionTable (fields, numOfFeatures);

  LarcosFeatureVectorListPtr  examples = new LarcosFeatureVectorList (_fileDesc, 
                                                                      true,          // true=Owner  examples will own the LarcosFeatureVector instances it contains.
                                                                      _log
                                                                      );

  examples->Version (version);

  kkint32  fieldNum = 0;

  bool  eof = false;
  bool  eol = false;

  KKStr  className;
  char   firstChar;
  kkint32  lineCount = 0;

  float  value;

  kkint32  lineNum = 0;

  eof = false;
  eol = false;

  KKStr field (128);

  GetToken (_in, ",\t", field, eof, eol);
  while  ((!eof)   &&  (!_cancelFlag)   &&  (((kkint32)examples->size () < _maxCount)  ||  (_maxCount < 0)))
  {
    if  (eol)
    {
      // We must have a blank line.  There is nothing to do in this case.
    }

    else if  (field.SubStrPart (0, 1) == "//")
    {
      // We are looking at a comment line.  Will skip to end of line
      while  ((!eol)  &&  (!eof))
        GetToken (_in, ",\t", field, eof, eol);
    }
    else
    {
      // We have an example.
      fieldNum  = 0;
      LarcosFeatureVectorPtr  example = new LarcosFeatureVector (numOfFeatures);
      example->Version (version);

      while  ((!eol)  &&  (!eof))
      {
        field.TrimRight ();

        if  (fieldNum >= (kkint32)indirectionTable.size ())
        {
          _log.Level (-1) << endl << endl << endl
                          << "FeatureFileIOKK::LoadFile     **** Error ***"  << endl
                          << endl
                          << "               LineNum[" << lineNum << "]  has to many fields defined." << endl
                          << endl
                          << endl;
          _errorMessage << "LineNum[" << lineNum << "]  has to many fields defined.";
          delete  examples;
          delete  example;
          return NULL;
        }
      
        switch  (indirectionTable[fieldNum])
        {
        case rfFeatureData:
          firstChar = field.FirstChar ();
          if  (((firstChar < '0')  ||  (firstChar > '9'))  &&  (firstChar != '-'))
          {
            KKStr  temp = field.ToUpper ();
            if  ((temp == "INFINITY")  || (temp == "NAN"))
            {
              example->AddFeatureData (fieldNum,  KKB::FloatMax);
            }
            else
            {
              _log.Level (-1) << endl << endl << endl
                              << "FeatureFileIOKK::LoadFile     **** Error ***"  << endl
                              << endl
                              << "               Invalid Feature Field[" << fieldNum << "]  on LineNum[" << lineNum << "]." << endl
                              << endl
                              << endl;
              _errorMessage << "Invalid Feature Field[" << fieldNum << "]  on LineNum[" << lineNum << "].";
              delete  examples;
              delete  example;
              return NULL;
            }
          }

          else
          {
            value = (float)atof (field.Str ());
            example->AddFeatureData (fieldNum,  value);
          }
          break;


        case rfClassName: 
          className = field;
          if  (className.Empty ())
             className = "UnKnown";
          example->MLClass (_classes.GetMLClassPtr (className));
          break;

        case rfImageClassIDX:
          // Ignore this field.
          break;

        case rfImageFileName: 
          example->ImageFileName (field);
          break;

        case rfOrigSize: 
          example->OrigSize ((float)atof (field.Str ()));
          break;

        case rfNumOfEdgePixels: 
          example->NumOfEdgePixels (atoi (field.Str ()));
          break;

        case rfProbability: 
          example->Probability ((float)atof (field.Str ()));
          break;

        case rfCentroidRow: 
          example->CentroidRow((float)atof (field.Str ()));
          break;

        case rfCentroidCol: 
          example->CentroidCol ((float)atof (field.Str ()));
          break;

        case rfPredictedClass: 
          if  (!field.Empty ())
          example->PredictedClass (_classes.GetMLClassPtr (field));
          break;
  
        case rfPredictedClassIDX:
          // We ignore this field
          break;

        case rfBreakTie: 
          example->BreakTie  ((float)atof (field.Str ()));
          break;

        case rfUnKnown:
          // ignore this field.
          break;
                  
        default:
          _log.Level(-1) << endl << endl << endl
                         << "FeatureFileIOKK::LoadFile   *** Undefined File Format Specified." << endl
                         << endl;
          break;
        }

        GetToken (_in, ",\t", field, eof, eol);
        fieldNum++;
      }

      if  (example->OrigSize () <= 0)
        example->OrigSize (example->FeatureData (0));

      if  (example->ImageFileName ().Empty ())
      {
        KKStr  imageFileName = _fileName + "_" + StrFormatInt (lineCount, "ZZZZZZ0");
        example->ImageFileName (imageFileName);
      }

      if  (example->ImageFileName ().Empty ())
        example->ImageFileName (osGetRootName (_fileName) + "_" + StrFormatInt (lineCount, "000000"));


      if  (example->PredictedClass () == NULL)
        example->PredictedClass (example->MLClass ());

      examples->PushOnBack (example);

      lineNum++;
    }

    GetToken (_in, ",\t", field, eof, eol);
  }

  _log.Level (10) << "LoadFile - Loaded [" << (kkint32)examples->size () << "] images from file[" << _fileName << "]." << endl;

  return  examples;
}  /* LoadFile */




/**
 * @brief  Used to quickly locate Index in MLClassList object; needed by RawFile format.
 * @param[out] classIdx   Map template, from MLClass to Index in a MLClassList object.
 * @param[out] mlClass  Image class that the Index of is needed for.
 * @return[out] Index indicated by 'classIdx' that mlClass is at.
*/
kkint32  GetClassIdx (const map<MLClassPtr, kkint32>&  classIdx,
                      const MLClassPtr                 mlClass
                     )
{
  if  (!mlClass)
    return -1;

  map<MLClassPtr, kkint32>::const_iterator p;
  p = classIdx.find (mlClass);
  if (p == classIdx.end ())
    return -1;
  return p->second;
}  /* GetClassIdx */




void   FeatureFileIOKK::SaveFile (FeatureVectorList&     _data,
                                  const KKStr&           _fileName,
                                  const FeatureNumList&  _selFeatures,
                                  ostream&               _out,
                                  kkuint32&              _numExamplesWritten,
                                  VolConstBool&          _cancelFlag,
                                  bool&                  _successful,
                                  KKStr&                 _errorMessage,
                                  RunLog&                _log
                                 )
{
  LarcosFeatureVectorListPtr  examples  = NULL;

  _numExamplesWritten = 0;

  bool  weOwnImages = false;

  const type_info& _dataTI     = typeid(_data);
  const type_info& _larcosFVTI = typeid(LarcosFeatureVectorList);

  const type_info* _dataTIPtr    =  &(typeid(_data));
  const type_info* _larcosFVTIPtr = &(typeid(LarcosFeatureVectorList));

  if  (typeid (_data) == typeid (LarcosFeatureVectorList))
  {
    examples = dynamic_cast<LarcosFeatureVectorListPtr>(&_data);
  }
  else
  {
    weOwnImages = true;
    examples = new LarcosFeatureVectorList (_data, true);
    examples->Version (_data.Version ());
  }

  const FileDescPtr  fileDesc = _data.FileDesc ();

  ClassStatisticListPtr  classStatistics = examples->GetClassStatistics ();

  kkint32  fileVersionNum = examples->Version ();
  if  (fileVersionNum < 1)
  {
    fileVersionNum = _LarcosFVProducer_VersionNum_;
    examples->Version (fileVersionNum);
  }

  map<MLClassPtr, kkint32>  classIdx;

  _out << "FEATURE_DATA_FILE"                                                << "\t"  
       << "Version"        << "\t" << StrFormatInt (fileVersionNum, "ZZZ0")  << "\t" 
       << "NumOfFeatures"  << "\t" << _selFeatures.NumOfFeatures ()          << "\t" 
       << "NumOfExamples"  << "\t" << examples->QueueSize ()
       << endl;

  {
    kkint32  idx;
    for  (idx = 0;  idx < _selFeatures.NumOfFeatures ();  idx++)
    { 
      kkint32  featureNum = _selFeatures[idx];
      _out << fileDesc->FieldName (featureNum) << "\t";
    }
 
    _out << "ClassName"         << "\t"
         << "ClassNameIdx"      << "\t"
         << "FileName"          << "\t"
         << "OrigSize"          << "\t"
         << "EdgePixels"        << "\t"
         << "Probability"       << "\t"
         << "CentroidRow"       << "\t"
         << "CentroidCol"       << "\t"
         << "PredictedClass"    << "\t"
         << "PredictedClassIdx" << "\t"
         << "BreakTie"
         << endl;
  }

  {
    // Write out Class Statistics
    kkint32  seqNum = 0;
    ClassStatisticList::iterator  idx;
    for  (idx = classStatistics->begin ();  idx != classStatistics->end ();  idx++)
    {
      ClassStatisticPtr  cs = *idx;
      _out << "//" << cs->Name () << "\t" 
           <<         seqNum      << "\t"
           <<         cs->Count ()
           << endl;

      classIdx.insert (pair<MLClassPtr, kkint32> (cs->MLClass (), seqNum));
      seqNum++;
    }
  }

  {
    // Write out the actual examples.
    LarcosFeatureVectorPtr   example = NULL;
    for  (kkint32 idx = 0; idx < (kkint32)examples->size (); idx++)
    {
      example = examples->IdxToPtr (idx);

      kkint32  x;  
      for  (x = 0; x < _selFeatures.NumOfFeatures (); x++)
      {
        kkint32  featureNum = _selFeatures[x];
        _out << example->FeatureData (featureNum) << "\t";
      }

      {
        kkint32  imageClassIdx = GetClassIdx (classIdx, example->MLClass ());
        kkint32  predClassIdx  = GetClassIdx (classIdx, example->PredictedClass ());
        kkint32  p = (kkint32)_out.precision();
        _out.precision (11);
        _out << example->ClassName          () << "\t" 
             << imageClassIdx                  << "\t"
             << example->ImageFileName      () << "\t" 
             << example->OrigSize           () << "\t"
             << example->NumOfEdgePixels    () << "\t"
             << example->Probability        () << "\t"
             << example->CentroidRow        () << "\t"
             << example->CentroidCol        () << "\t"
             << example->PredictedClassName () << "\t"
             << predClassIdx                   << "\t"
             << example->BreakTie           ()
             << endl;
        _out.precision (p);
      }
      _numExamplesWritten++;
    }
  }

  delete  classStatistics;
  if  (weOwnImages)
  {
    delete  examples;
    examples = NULL;
  }

  _successful = true;
  return;

}  /* SaveFile */


