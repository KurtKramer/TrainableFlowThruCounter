using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using LarcosCounterManaged;

namespace LarcosManagedRoutines
{
  /// <summary>
  /// Object is responsible for maintaining a list of all Ground Truth locations.   
  /// in a ScannerFile.
  /// </summary>
  public  class GroundTruthData
  {
    public class  Entry
    {
      public  Entry (long  _scanLine,
                     int   _col
                    )
      {
        scanLine = _scanLine;
        col      = _col;
      }

      public long  scanLine;
      public int   col;
    }  /* Entry */


    private  List<Entry>   data     = null;
    private  StreamWriter  sw       = null;
    private  String        fileName = null;


    public  GroundTruthData (String  _fileName)
    {
      fileName = _fileName;
      data = new List<Entry> ();
      GroundTruthFileLoad ();
    }


    private  void  GroundTruthFileLoad ()
    {
      if  (data == null)
        data = new List<Entry> ();
      else
        data.Clear ();

      StreamReader sr = null;
      try  {sr = new StreamReader (fileName);}
      catch  (Exception)  {sr = null;}

      if  (sr == null)
        return;

      String  txtLine = sr.ReadLine ();
      while  (txtLine != null)
      {
        if  (String.IsNullOrEmpty (txtLine)  ||  (txtLine.StartsWith ("//")))
          continue;

        String[]  fields = txtLine.Split ('\t');
        if  (fields.Length <  3)
        continue;

        String  rowType = fields[0].ToLower ().Trim ();
        long  scanLine = UmiKKStr.StrToLong (fields[1]);
        int   col      = UmiKKStr.StrToLong (fields[2]);

        if  (rowType == "add")
        {
          data.Add (new Entry (scanLine, col));
        }

        else if  (rowType == "delete")
        {
          int  idx = data.FindIndex (delegate (Entry e) {return (e.scanLine == scanLine)  &&  (e.col == col);});
          if  (idx >= 0)
            data.RemoveAt (idx);
        }
      }

      sr.Close ();
    }  /* GroundTruthFileLoad */


    private class  ScanLineComparer: IComparer<Entry>
    {
      public int Compare(Entry x, Entry y)
      {
        if  (x.scanLine < y.scanLine)
          return -1;

        else if  (x.scanLine > y.scanLine)
          return  1;

        else 
          return  x.col.CompareTo (y.col);
      }
    }  /* ScanLineComparer */


    private  void  GroundTruthFileSave ()
    {
      if  (sw != null)
      {
        sw.Close ();
        sw = null;
      }

      DateTime  d = DateTime.Now;

      String  extensiomn   = UmiOSservices.GetFileExtension (fileName);
      String  tempFileName = UmiOSservices.RemoveExtension (fileName) + "." + d.ToString ("yyyyMMddHHmmss") +".gt";

      sw = null;
      try  {sw = new StreamWriter (tempFileName);}
      catch  {sw = null;}

      if  (sw == null)
        return;

      data.Sort (new ScanLineComparer ());
      sw.WriteLine ("//GroundTruth Data");
      sw.WriteLine ("ScannerFile" + "\t" + fileName);
      sw.WriteLine ("DateTime"    + "\t" + d.ToString ("yyyy/MM/dd HH:mm:ss"));
      sw.WriteLine ("//");

      foreach (Entry entry in data)
      {
        sw.WriteLine ("Add" + "\t" + entry.scanLine + "\t" +entry.col);
      }

      sw.WriteLine ("// Done Writing File");
      sw.Close ();
      sw = null;

      File.Delete (fileName);
      UmiOSservices.RenameFile (tempFileName, fileName);
    } /* GroundTruthFileSave*/


    
    public  void  AddEntry (long  _scanLine,
                            int   _col
                           )
    {
      data.Add (new Entry (_scanLine, _col));
      if  (sw == null)
      {
        try  {sw = new StreamWriter (fileName);}
        catch  (Exception)  {sw = null;}
      }

      if  (sw != null)
      {
        sw.WriteLine ("Add" + "\t" + _scanLine + "\t" + _col);
        sw.Flush ();
      }
    }  /* AddEntry */



    public  void  DeleteEntry (long  _scanLine,
                               int   _col
                              )
    {
      int  idx = data.FindIndex (delegate (Entry entry) {return ((entry.scanLine == _scanLine)  &&  (entry.col == _col));});
      if  (idx >= 0)
        data.RemoveAt (idx);

      if  (sw == null)
      {
        try  {sw = new StreamWriter (fileName);}
        catch  (Exception)  {sw = null;}
      }

      if  (sw != null)
      {
        sw.WriteLine ("Delete" + "\t" + _scanLine + "\t" + _col);
        sw.Flush ();
      }
    }  /* DeleteEntry */



    private  long  DistanceSqr (long scanLine1, int col1,
                                long scanLine2, int col2
                               )
    {
      long  delta1 = scanLine1 - scanLine2;
      long  delta2 = col1 - col2;
      return  delta1 * delta1 + delta2 * delta2;
    }  /* DistnaceSqr */
                       


    public  Entry  NearestGroundTruthEntry (long _scanLine,
                                            int  _col,
                                            long _maxDist
                                           )
    {
      List<Entry> candidates = data.FindAll (delegate (Entry e) {return (e.scanLine >= (_scanLine - _maxDist))  &&  (e.scanLine <= (_scanLine + _maxDist));});
      Entry  nearest = null;
      long  nearestDistSqr = 9999999999;

      long  maxDistSqr = _maxDist * _maxDist;

      foreach (Entry candidate in candidates)
      {
        long  candidateDistSqr = DistanceSqr (_scanLine, _col, candidate.scanLine, candidate.col);
        if  (candidateDistSqr > maxDistSqr)
          continue;
        if  (candidateDistSqr < nearestDistSqr)
        {
          nearestDistSqr = candidateDistSqr;
          nearest = candidate;
        }
      }
      return  nearest;
    }  /* NearestGroundTruthEntry */




  }  /* GroundTruthData */
}
