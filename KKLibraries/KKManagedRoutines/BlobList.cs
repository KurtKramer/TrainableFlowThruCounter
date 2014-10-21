using System;
using System.Collections.Generic;
using System.Text;

namespace KKManagedRoutines
{
  public  class  BlobList:  List<Blob> 
  {
    private  int  nextBlobId = 0;
    private  bool owner      = false;

    public  BlobList (bool  _owner)
    {
      owner = _owner;
      nextBlobId = 0;
    }


    public  Blob  LookUpByBlobId (int  blobId)
    {
      if  ((blobId < 0)  ||  (blobId >= nextBlobId))
      {
        throw new Exception ("BlobList::LookUpByBlobId    Invalid BlobID[" + blobId.ToString () + "]");
      }

      return  this[blobId];
    }  /* LookUpByBlobId */



    public  void  Dispose ()
    {
      if  (owner)
      {
        int  idx = 0;
        for  (idx = 0;  idx < Count;  idx++)
        {
          if  (this[idx] != null)
          {
            this[idx].Dispose ();
            this[idx] = null;
          }
        }
      }
    }



    
    public  Blob  NewBlob (int  rowTop, 
                           int  colLeft
                          )
    {
      Blob  blob = new Blob (nextBlobId);
      blob.RowTop   = rowTop;
      blob.RowBot   = rowTop;
      blob.ColLeft  = colLeft;
      blob.ColRight = colLeft; 
      nextBlobId++;
      this.Add (blob);
      return  blob;
    }


    
    public  void  MergeBlobIds (Blob     blob,
                                int      blobId,
                                int[][]  blobIds
                               )
    {                               
      int  newId = blob.Id;
  
      Blob  blobToMerge = LookUpByBlobId (blobId);
      if  (blobToMerge == null)
         return;

      int  col;
      int  row;

      int  rowBot   = blobToMerge.RowBot;
      int  rowTop   = blobToMerge.RowTop;

      int  colLeft = blobToMerge.ColLeft;
      int  colRight = blobToMerge.ColRight;

      for  (row = rowBot;  row <= rowTop;  row++)
      {
        for  (col = colLeft;  col <= colRight;  col++)
        {
          if  (blobIds[row][col] == blobId)
             blobIds[row][col] = newId;
        }
      }

      blob.RowTopMaybe   = blobToMerge.RowTop;
      blob.RowBotMaybe   = blobToMerge.RowBot;
      blob.ColLeftMaybe  = blobToMerge.ColLeft;
      blob.ColRightMaybe = blobToMerge.ColRight;
      blob.PixelCount    = blob.PixelCount + blobToMerge.PixelCount;

      DeleteEntry (blobToMerge.Id);
      blobToMerge.Dispose ();
    }  /* MergeBlobIds */


      
    public  void  DeleteEntry (int blobId)
    {
      this[blobId] = null;
    }



    public  Blob  LocateLargestBlob ()
    {
      int       idx;
      int       qSize       = Count;
      Blob      blob        = null;
      Blob      tempBlob    = null;
      int       largestSize = 0;

      for  (idx = 0;  idx < qSize;  idx++)
      {
        tempBlob = this[idx];
        if  (tempBlob == null)
          continue;

        if  (tempBlob.PixelCount > largestSize)
        {
          largestSize = tempBlob.PixelCount;
          blob = tempBlob;
        }
      }

      return  blob;
    } /* LocateLargestBlob */

  }  /* BlobList */
}

