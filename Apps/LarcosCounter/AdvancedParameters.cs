using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using LarcosManagedRoutines;
using LarcosCounterManaged;


namespace LarcosCounter
{
  public partial class AdvancedParameters : Form
  {
    private  bool  userAcceptedScreen = false;

    private  UmiOperatingParameters  initialOperatingParameters = null;

    public  int    BackGroundPixelTH  {get {return (int)BackGroundPixelTHField.Value;}               set {BackGroundPixelTHField.Value            = (decimal)value;}}
    public  bool   DataIsToBeCounted  {get {return DataIsToBeCountedField.Checked;}                  set {DataIsToBeCountedField.Checked          = value;}}
    public  bool   DataIsToBeRecorded {get {return DataIsToBeRecordedField.Checked;}                 set {DataIsToBeRecordedField.Checked         = value;}}
    public  int    ErosionStructSize  {get {return (int)ErosionStructSizeField.Value;}               set {ErosionStructSizeField.Value            = (decimal)value;}}
    public  bool   FlatFieldEnabled   {get {return FlatFieldCorrectionEnabledField.Checked;}         set {FlatFieldCorrectionEnabledField.Checked = value;}}
    public  float  FlowRateFactor     {get {return (float)flowRateFactorField.Value;}                set {flowRateFactorField.Value               = (decimal)value;}}
    public  bool   SaveParticleImages {get {return SaveParticleImagesField.Checked;}                 set {SaveParticleImagesField.Checked         = value;}}
    public  float  ScanRate           {get {return (float)RequestedScanRateField.Value;}             set {RequestedScanRateField.Value            = (decimal)value;}}
    public  int    MinSizeThreshold   {get {return UmiKKStr.StrToInt (MinSizeThresholdField.Text);}  set {MinSizeThresholdField.Text              = value.ToString ();}}

    public  bool   UserAcceptedScreen {get {return userAcceptedScreen;}}

    public  UmiScannerFile.Format FileFormat   
                    {
                      get {return UmiScannerFile.ScannerFileFormatFromStr (FileFormatField.Text);} 
                      set {FileFormatField.SelectedItem = UmiScannerFile.ScannerFileFormatToStr (value);}
                    }

    
     public AdvancedParameters (UmiOperatingParameters  _initialOperatingParameters)
    {
      initialOperatingParameters = _initialOperatingParameters;
      if  (initialOperatingParameters == null)
        initialOperatingParameters = new UmiOperatingParameters ();
      userAcceptedScreen = false;
      InitializeComponent ();
    }


    private  void  UpdateOperatingParametersFromScreenFields (UmiOperatingParameters  opParms)
    {
      opParms.BackGroundPixelTH      = (int)BackGroundPixelTHField.Value;
      opParms.ConnectedComponentDist = initialOperatingParameters.ConnectedComponentDist;
      opParms.DataIsToBeCounted      = DataIsToBeCountedField.Checked;
      opParms.DataIsToBeRecorded     = DataIsToBeRecordedField.Checked;
      opParms.DestScannerFileFormat  = UmiScannerFile.ScannerFileFormatFromStr (FileFormatField.Text);
      opParms.ErosionStructSize      = (int)ErosionStructSizeField.Value;
      opParms.FlatFieldEnabled       = FlatFieldCorrectionEnabledField.Checked;
      opParms.FlowRateFactor         = (float)flowRateFactorField.Value;
      opParms.MinSizeThreshold       = UmiKKStr.StrToInt (MinSizeThresholdField.Text);
      opParms.PlayingBack            = initialOperatingParameters.PlayingBack;
      opParms.SaveParticleImages     = SaveParticleImagesField.Checked;
      opParms.RequestedScanRate      = (float)RequestedScanRateField.Value;
    }


    public  UmiOperatingParameters  GetOperatingParameters ()
    {
      UmiOperatingParameters  opParms = null;

      if  (!userAcceptedScreen)
      {
        opParms = new UmiOperatingParameters (initialOperatingParameters);
      }
      else
      {
        opParms = new UmiOperatingParameters (initialOperatingParameters);
        UpdateOperatingParametersFromScreenFields (opParms);
      }

      return  opParms;
    }



    public  void  SetOperatingParameters (UmiOperatingParameters  opParms)
    {
      initialOperatingParameters = opParms;
      UpdateScreenFieldsFromOperatingParameters (opParms);
    }



    private void AdvancedParameters_Load (object sender, EventArgs e)
    {
      userAcceptedScreen = false;
      FileFormatField.DataSource      = UmiScannerFile.FileFormatOptions ();
      UpdateScreenFieldsFromOperatingParameters (initialOperatingParameters);
    }



    private  void  UpdateScreenFieldsFromOperatingParameters (UmiOperatingParameters  opParms)
    {
      DataIsToBeCountedField.Checked  = opParms.DataIsToBeCounted;
      SaveParticleImagesField.Checked = opParms.SaveParticleImages;
      DataIsToBeRecordedField.Checked = opParms.DataIsToBeRecorded;
      FlatFieldCorrectionEnabledField.Checked = opParms.FlatFieldEnabled;
      String  fileFormatStr = opParms.DestScannerFileFormatStr;
      FileFormatField.SelectedItem = fileFormatStr;
      FileFormatField.Text         = fileFormatStr;

      try  {flowRateFactorField.Value    = (Decimal)opParms.FlowRateFactor;}       catch  (Exception)  {}
      try  {RequestedScanRateField.Value = (Decimal)opParms.RequestedScanRate;}    catch  (Exception)  {}
      try  {ErosionStructSizeField.Value = (Decimal)opParms.ErosionStructSize;}    catch  (Exception)  {}
      try  {MinSizeThresholdField.Text   = opParms.MinSizeThreshold.ToString ();}  catch  (Exception)  {}
      try  {BackGroundPixelTHField.Value = (Decimal)opParms.BackGroundPixelTH;}    catch  (Exception)  {}
    }


    private void OkButton_Click (object sender, EventArgs e)
    {
      userAcceptedScreen = true;
      UpdateOperatingParametersFromScreenFields (initialOperatingParameters);
      Close ();
    }



    private void CancelButton_Click (object sender, EventArgs e)
    {
      Close ();
    }
  }
}
