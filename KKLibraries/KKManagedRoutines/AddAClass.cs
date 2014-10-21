using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using KKManagedRoutines;
using LarcosCounterManaged;



namespace KKManagedRoutines
{
  public partial class  AddAClass : Form
  {
    private  UmiTrainingModel2  trainingModel = null;
    private  bool               validationErrorsFound = false;
    private  UmiParticleEntry   particleEntry    = null;
    private  UmiRunLog          runLog           = null;
    private  bool               newClassWasAdded = false;

    public   bool  NewClassWasAdded  {get {return  newClassWasAdded;}}
    
    public  AddAClass (UmiTrainingModel2 _trainingModel,
                       UmiParticleEntry  _particleEntry,
                       UmiRunLog         _runLog
                      )
    {
      InitializeComponent ();
      particleEntry = _particleEntry;
      runLog = _runLog;
      if  (runLog == null)
        runLog = new UmiRunLog ();
      trainingModel = _trainingModel;
    }



    private  void  ValidateClassName ()
    {
      errorProvider1.SetError (ClassNameField, "");
      String  className = ClassNameField.Text;

      if  (className == null)
        className = "";

      char[]  trimChars = {'\n', '\r', ' ', '\t'};
      className = className.Trim (trimChars);
      if  (String.IsNullOrEmpty (className))
      {
        errorProvider1.SetError (ClassNameField, "Class name can not be left blank.");
        validationErrorsFound = true;
        return;
      }

      for  (int x = 0;  x < className.Length;  ++x)
      {
        char c = className[x];
        if  (((c >= 'A')  &&  (c <= 'Z'))  ||  
             ((c >= 'a')  &&  (c <= 'z'))  ||
             ((c >= '0')  &&  (c <= '9'))  ||
             (c == '_')  ||
             (c == '-')
            )
        {
        }
        else
        {
          errorProvider1.SetError (ClassNameField, "Invalid character[" + c.ToString () + "] at position[" + x.ToString () + "].");
          validationErrorsFound = true;
          return;
        }
      }

      UmiClass  ic = UmiClassList.GetUniqueClass (className, "");

      if  (trainingModel.IncludesClass (ic))
      {
        errorProvider1.SetError (ClassNameField, "Training model already contains this class name.");
        return;
      }
      
      validationErrorsFound = false;
    }



    void  VerifyFields ()
    {
      validationErrorsFound = false;
      ValidateClassName ();
    }



    private void AddNewClassButton_Click (object sender, EventArgs e)
    {
      VerifyFields ();
      if  (validationErrorsFound)
        return;
      AddNewClassToTrainingModel ();
      Close ();
    }



    private  void  AddNewClassToTrainingModel ()
    {
      UmiTrainingConfiguration  config = new UmiTrainingConfiguration (trainingModel.ConfigFileName, null, runLog);
      UmiClass  ic = UmiClassList.GetUniqueClass (ClassNameField.Text, "");

      UmiTrainingClass tc = new UmiTrainingClass (ic, "");
      String  fullFileName = config.FullFileName ();
      tc.CountFactor = (float)CountField.Value;
      config.AddATrainingClass (tc);
      config.Save (fullFileName);
      particleEntry.PredClass = ic;
      newClassWasAdded = true;

      String  classDirPath = UmiOSservices.AddSlash (config.RootDirExpanded ()) + ic.Name;
      UmiOSservices.CreateDirectoryPath (classDirPath);

      Close ();
    }



    private void AddAClass_FormClosing (object sender, FormClosingEventArgs e)
    {
      if  ((!newClassWasAdded)  &&  (!String.IsNullOrEmpty (ClassNameField.Text)))
      {
        DialogResult  dr = MessageBox.Show (this, "The new Class Name is not empty are you sure you want to close the form ?", "Add A Class", MessageBoxButtons.YesNo);
        if  (dr == DialogResult.No)
          e.Cancel = true;
      }
      return;
    }

    private void CancelButton_Click (object sender, EventArgs e)
    {
      Close ();
    }
  }
}
