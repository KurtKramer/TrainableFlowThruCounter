﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using CounterManaged;

namespace CounterManagedRoutines
{
  public partial class FeatureDataDisplay :Form
  {
    public FeatureDataDisplay (String            imageFileName,
                               UmiFeatureVector  calcFV,
                               UmiFeatureVector  dbFV
                              )
    {
      InitializeComponent ();

      uint  numFeatures = 0;

      UmiFeatureVector  descFV = null;

      if  (calcFV != null)
      {
        numFeatures = calcFV.NumFeatures;
        descFV = calcFV;
      }

      if  ((dbFV != null)  &&  (dbFV.NumFeatures < numFeatures))
      {
        numFeatures = dbFV.NumFeatures;
        descFV = dbFV;
      }

      if  ((descFV == null)  ||  (numFeatures < 1))
        return;

      for  (uint x = 0;  x < numFeatures;  ++x)
      {
        ListViewItem  lvi = new ListViewItem (descFV.FeatureName (x));

        double  fvValueCalc = 0.0;
        double  fvValueDb   = 0.0;

        if  (calcFV != null)
          fvValueCalc = calcFV.FeatureValue (x);

        if  (dbFV != null)
          fvValueDb = dbFV.FeatureValue (x);

        double  delta = fvValueCalc - fvValueDb;

        String  formatStr = "f6";

        if  (Math.Max (Math.Abs (fvValueCalc), Math.Abs (fvValueDb)) < 1.0)
          formatStr = "e4";

        lvi.SubItems.Add (fvValueCalc.ToString (formatStr));
        lvi.SubItems.Add (fvValueDb.ToString (formatStr));
        if  (delta != 0)
        {
          lvi.SubItems.Add (delta.ToString (formatStr));
          lvi.SubItems[lvi.SubItems.Count - 1].ForeColor = Color.Red;
        }

        FeatureDataList.Items.Add (lvi);
      }
    }
  }  /* FeatureDataDisplay */
}
