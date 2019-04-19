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
#include "CounterFeatureVector.h"
#include "CounterFVProducer.h"
using namespace KKMLL;
                      
#include "FeatureFileIOKK.h"
using namespace CounterUnManaged;
           
           
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
    indirectionTable.push_back ((int)FieldTypes::FeatureData);

  for  (fieldNum = numOfFeatures;  fieldNum < (kkint32)fields.size ();  fieldNum++)
  {
    KKStr  fieldName = fields[fieldNum].ToLower ();
    if  (fieldName == "classname")
      indirectionTable.push_back ((int)FieldTypes::ClassName);

    else if  (fieldName == "classnameidx")
      indirectionTable.push_back ((int)FieldTypes::ImageClassIDX);

    else if  (fieldName == "filename") 
      indirectionTable.push_back ((int)FieldTypes::ImageFileName);

    else if  (fieldName == "origsize") 
      indirectionTable.push_back ((int)FieldTypes::OrigSize);

    else if  (fieldName == "edgepixels") 
      indirectionTable.push_back ((int)FieldTypes::NumOfEdgePixels);

    else if  (fieldName == "probability") 
      indirectionTable.push_back ((int)FieldTypes::Probability);

    else if  (fieldName == "disttoborder") 
      indirectionTable.push_back ((int)FieldTypes::UnKnown);

    else if  (fieldName == "centroidrow")
      indirectionTable.push_back ((int)FieldTypes::CentroidRow);

    else if  (fieldName == "centroidcol")
      indirectionTable.push_back ((int)FieldTypes::CentroidCol);

    else if  (fieldName == "predictedclass")
      indirectionTable.push_back ((int)FieldTypes::PredictedClass);

    else if  (fieldName == "predictedclassidx")
      indirectionTable.push_back ((int)FieldTypes::PredictedClassIDX);

    else if  (fieldName == "breaktie")
      indirectionTable.push_back ((int)FieldTypes::BreakTie);

    else
      indirectionTable.push_back ((int)FieldTypes::UnKnown);

  }

  return  indirectionTable;
}  /* CreateIndirectionTable */



FileDescConstPtr  FeatureFileIOKK::GetFileDesc (const KKStr&    _fileName,
                                                istream&        _in,
                                                MLClassListPtr  _classes,
                                                kkint32&        _estSize,
                                                KKStr&          _errorMessage,
                                                RunLog&         _log
                                               )
{
  _errorMessage = "";
  
  bool  eof = false;
  KKStr  firstLine = osReadRestOfLine2 (_in, eof);
  firstLine.Upper ();

  if  (!firstLine.StartsWith ("FEATURE_DATA_FILE"))
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
    _log.Level (-1) << endl << "FeatureFileIOKK::GetFileDesc   ***ERROR***   Version Number[" << version << "]  Can not load versions prior to 39." << endl << endl;
    _errorMessage << "Version[" << version << "]  is prior to '39'.";
    return NULL;
  }

  KKStr  fieldDescLn = osReadRestOfLine2 (_in, eof);
  VectorKKStr  fields = fieldDescLn.Split ("\n\r\t");
  if  (fields.size () < (kkuint32)numOfFeatures)
  {
    _log.Level (-1) << endl << endl << endl
                    << "FeatureFileIOKK::GetFileDesc     **** Error ***"  << endl
                    << endl
                    << "               Number fields[" << (kkuint32)fields.size () << "]  is less than the number specified." << endl
                    << endl
                    << endl;
    _errorMessage << "Number fields[" << (kkuint32)fields.size () << "]  is less than the number specified." << endl;
    return NULL;
  }
   
  FileDescPtr  fd = new FileDesc ();
  fd->Version (version);

  bool  alreadyExists;
  kkint32  featureNum = 0;
  for  (featureNum = 0;  featureNum < numOfFeatures;  featureNum++)
  {
    KKStr&  fieldName = fields[featureNum];
    fd->AddAAttribute (fieldName, AttributeType::Numeric, alreadyExists);
    if  (alreadyExists)
    {
      _log.Level (-1) << endl << endl 
                      << "FeatureFileIOKK::GetFileDesc   ***ERROR***"  << endl
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



CounterFeatureVectorListPtr  FeatureFileIOKK::LoadFile (const KKStr&     _fileName,
                                                       FileDescConstPtr  _fileDesc,
                                                       MLClassList&      _classes, 
                                                       istream&          _in,
                                                       OptionUInt32      _maxCount,
                                                       VolConstBool&     _cancelFlag,
                                                       bool&             _changesMade,
                                                       KKStr&            _errorMessage,
                                                       RunLog&           _log
                                                      )
{
  _log.Level (10) << "FeatureFileIOKK::LoadFile   Loading file[" << _fileName << "]." << endl;
  _log.Flush ();
  
  VectorInt  featureFieldIndexTable;

  bool eof = false;
  bool  eol = false;
  KKStr  firstLine = osReadRestOfLine2 (_in, eof);
  firstLine.Upper ();
  if  (!firstLine.StartsWith ("FEATURE_DATA_FILE"))
  {
    _log.Level (-1) << endl << "FeatureFileIOKK::LoadFile   ***ERROR***    missing 'FEATURE_DATA_FILE' line ****" << endl << endl;
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
  
  // The second line will have a list of fields;  we will use this line to build
  // an indirection table.
  KKStr  fieldDescLine = osReadRestOfLine2 (_in, eof);
  VectorKKStr  fields = fieldDescLine.Parse ("\n\r\t");
  if  ((kkint32)fields.size () < numOfFeatures)
  {
    _log.Level (-1) << endl << "FeatureFileIOKK::LoadFile   ***ERROR***   Number fields[" << (kkuint32)fields.size () << "]  is less than the number specified." << endl << endl;
    _errorMessage << "Number fields[" << (kkuint32)fields.size () << "]  is less than the number specified.";
    return  NULL;
  }

  VectorInt   indirectionTable = CreateIndirectionTable (fields, numOfFeatures);

  CounterFeatureVectorListPtr  examples = new CounterFeatureVectorList (_fileDesc, true);

  examples->Version (version);

  kkint32  fieldNum = 0;

  KKStr  className;
  char   firstChar;
  kkint32  lineCount = 0;

  float  value;

  kkint32  lineNum = 0;

  eof = false;
  eol = false;

  KKStr field (128);

  kkuint32 maxToLoad = _maxCount ? _maxCount.value () : uint32_max;

  GetToken (_in, ",\t", field, eof, eol);
  while  ((!eof)  &&  (!_cancelFlag)  &&  (examples->QueueSize () < maxToLoad))
  {
    if  (eol)
    {
      // We must have a blank line.  There is nothing to do in this case.
    }

    else if  (field.StartsWith ("//"))
    {
      // We are looking at a comment line.  Will skip to end of line
      while  ((!eol)  &&  (!eof))
        GetToken (_in, ",\t", field, eof, eol);
    }
    else
    {
      // We have an example.
      fieldNum  = 0;
      CounterFeatureVectorPtr  example = new CounterFeatureVector (numOfFeatures);
      example->Version (version);

      while  ((!eol)  &&  (!eof))
      {
        field.TrimRight ();

        if  (fieldNum >= (kkint32)indirectionTable.size ())
        {
          _log.Level (-1) << endl << "FeatureFileIOKK::LoadFile   ***ERROR***   LineNum[" << lineNum << "]  has to many fields defined." << endl << endl;
          _errorMessage << "LineNum[" << lineNum << "]  has to many fields defined.";
          delete  examples;
          delete  example;
          return NULL;
        }
      
        switch  (indirectionTable[fieldNum])
        {
        case FieldTypes::FeatureData:
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

        case FieldTypes::ClassName: 
          className = field;
          if  (className.Empty ())
             className = "UnKnown";
          example->MLClass (_classes.GetMLClassPtr (className));
          break;

        case FieldTypes::ImageClassIDX:
          // Ignore this field.
          break;

        case FieldTypes::ImageFileName: 
          example->ExampleFileName (field);
          break;

        case FieldTypes::OrigSize: 
          example->OrigSize ((float)atof (field.Str ()));
          break;

        case FieldTypes::NumOfEdgePixels: 
          example->NumOfEdgePixels (atoi (field.Str ()));
          break;

        case FieldTypes::Probability: 
          example->Probability ((float)atof (field.Str ()));
          break;

        case FieldTypes::CentroidRow: 
          example->CentroidRow((float)atof (field.Str ()));
          break;

        case FieldTypes::CentroidCol: 
          example->CentroidCol ((float)atof (field.Str ()));
          break;

        case FieldTypes::PredictedClass: 
          if  (!field.Empty ())
          example->PredictedClass (_classes.GetMLClassPtr (field));
          break;
  
        case FieldTypes::PredictedClassIDX:
          // We ignore this field
          break;

        case FieldTypes::BreakTie: 
          example->BreakTie  ((float)atof (field.Str ()));
          break;

        case FieldTypes::UnKnown:
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

      if  (example->ExampleFileName ().Empty ())
      {
        KKStr  imageFileName = _fileName + "_" + StrFormatInt (lineCount, "ZZZZZZ0");
        example->ExampleFileName (imageFileName);
      }

      if  (example->ExampleFileName ().Empty ())
        example->ExampleFileName (osGetRootName (_fileName) + "_" + StrFormatInt (lineCount, "000000"));


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



void   FeatureFileIOKK::SaveFile (FeatureVectorList&    _data,
                                  const KKStr&          _fileName,
                                  FeatureNumListConst&  _selFeatures,
                                  ostream&              _out,
                                  kkuint32&             _numExamplesWritten,
                                  VolConstBool&         _cancelFlag,
                                  bool&                 _successful,
                                  KKStr&                _errorMessage,
                                  RunLog&               _log
                                 )
{
  CounterFeatureVectorListPtr  examples  = NULL;

  _numExamplesWritten = 0;

  bool  weOwnImages = false;

  const type_info& _dataTI     = typeid(_data);
  const type_info& _larcosFVTI = typeid(CounterFeatureVectorList);

  const type_info* _dataTIPtr    =  &(typeid(_data));
  const type_info* _larcosFVTIPtr = &(typeid(CounterFeatureVectorList));

  if  (typeid (_data) == typeid (CounterFeatureVectorList))
  {
    examples = dynamic_cast<CounterFeatureVectorListPtr>(&_data);
  }
  else
  {
    weOwnImages = true;
    examples = new CounterFeatureVectorList (_data, true);
    examples->Version (_data.Version ());
  }

  const FileDescConstPtr  fileDesc = _data.FileDesc ();

  ClassStatisticListPtr  classStatistics = examples->GetClassStatistics ();

  kkint32  fileVersionNum = examples->Version ();
  if  (fileVersionNum < 1)
  {
    fileVersionNum = _CounterFVProducer_VersionNum_;
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
    CounterFeatureVectorPtr   example = NULL;
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
        _out << example->MLClassName        () << "\t" 
             << imageClassIdx                  << "\t"
             << example->ExampleFileName    () << "\t" 
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
