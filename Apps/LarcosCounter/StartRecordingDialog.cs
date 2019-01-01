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
  public partial class StartRecordingDialog : Form
  {
    private  bool  startRecording  = false;
    private  bool  cancelRequested = false;
    private  LarcosCounterManagerWrapper  cameraManager = null;
    private  bool  errorsFound = true;

    private  AdvancedParameters  advParameters = null;

    private  UmiRunLog  runLog = null;

    private  UmiSessionParameters    initialSessionParametrs    = null;
    private  UmiOperatingParameters  initialOperatingParameters = null;

    private  LarcosTrainingModelListManaged  trainModels = null;

    private  String  lastTrainingModelName = "";

    private  ToolTip toolTip = null;

    private  bool    playingBack = false;

    public  String  Comments           {get {return comments.Text;}            set  {comments.Text           = value;}}
    public  String  Destination        {get {return destination.Text;}         set  {destination.Text        = value;}}
    public  String  PersonInCharge     {get {return personInCharge.Text;}      set  {personInCharge.Text     = value;}}
    public  String  SessionDescription {get {return sessionDescription.Text;}  set  {sessionDescription.Text = value;}}
    public  String  ControlNum         {get {return controlNum.Text;}          set  {controlNum.Text         = value;}}
    public  String  Source             {get {return source.Text;}              set  {source.Text             = value;}}

    // Advanced options
    public  int     BackGroundPixelTH  {get {return advParameters.BackGroundPixelTH;}   set {advParameters.BackGroundPixelTH  = value;}}
    public  bool    DataIsToBeCounted  {get {return advParameters.DataIsToBeCounted;}   set {advParameters.DataIsToBeCounted  = value;}}
    public  bool    DataIsToBeRecorded {get {return advParameters.DataIsToBeRecorded;}  set {advParameters.DataIsToBeRecorded = value;}}
    public  int     ErosionStructSize  {get {return advParameters.ErosionStructSize;}   set {advParameters.ErosionStructSize  = value;}}
    public  bool    FlatFieldEnabled   {get {return advParameters.FlatFieldEnabled;}    set {advParameters.FlatFieldEnabled   = value;}}
    public  float   FlowRateFactor     {get {return advParameters.FlowRateFactor;}      set {advParameters.FlowRateFactor     = value;}}
    public  int     MinSizeThreshold   {get {return advParameters.MinSizeThreshold;}    set {advParameters.MinSizeThreshold   = value;}}
    public  bool    SaveParticleImages {get {return advParameters.SaveParticleImages;}  set {advParameters.SaveParticleImages = value;}}
    public  float   ScanRate           {get {return advParameters.ScanRate;}            set {advParameters.ScanRate           = value;}}

    public  UmiScannerFile.Format FileFormat
                                       {get {return advParameters.FileFormat;}          set {advParameters.FileFormat         = value;}}

    public  bool    StartRecording     {get {return startRecording;}}
    public  bool    CancelRequested    {get {return cancelRequested;}}

    private  bool  subjectSuspend      = false;
    private  bool  subjectSizeSuspend  = false;
    private  bool  waterQualitySuspend = false;



    private  void  PopulateSubjects ()
    {
      bool  subjectSuspendOrig = subjectSuspend;
      subjectSuspend = true;

      if  (trainModels == null)
      {
        trainModels = LarcosTrainingModelListManaged.BuildFromTrainigModelDir ();
        if  (trainModels == null)
          trainModels = new LarcosTrainingModelListManaged ();
      }

      subject.DataSource = trainModels.GetListOfSubjects ();
      subjectSuspend = subjectSuspendOrig;
    }



    public  String  TrainingModelName
    {
      get  {return   subject.Text + "_" + subjectSize.Text + "_" + waterQuality.Text;}
      set
      {
        String[] fields = UmiOSservices.GetRootName (value).Split ('_');
        if  (fields.Length < 3)
          return;

        bool  subjectSuspendOrig = subjectSuspend;
        bool  subjectSizeSuspendOrig = subjectSizeSuspend;
        bool  waterQualitySuspendOrig = waterQualitySuspend;

        subjectSuspend      = true;
        subjectSizeSuspend  = true;
        waterQualitySuspend = true;

        if  ((subject.Items == null)  ||  (subject.Items.Count < 1))
          PopulateSubjects ();

        subject.SelectedItem      = "";
        subject.Text              = "";
        subjectSize.SelectedItem  = "";
        subjectSize.Text          = "";
        waterQuality.SelectedItem = "";
        waterQuality.Text         = "";

        subject.SelectedItem = fields[0];

        String[]  sizes = trainModels.GetListOfSizes (fields[0]);
        if  ((sizes != null) &&  (sizes.Length > 0))
        {
          subject.SelectedItem = fields[0];

          subjectSize.DataSource = sizes;

          String[] waterQualities = trainModels.GetListOfWaterQualities (fields[0], fields[1]);
          if  ((waterQualities != null)  &&  (waterQualities.Length > 0))
          {
            subjectSize.SelectedItem = fields[1];

            waterQuality.DataSource = waterQualities;
            if  (trainModels.InList (fields[0], fields[1], fields[2]))
            {
              waterQuality.SelectedItem = fields[2];
              ReFreshFromSelectedTrainingModel ();
            }
          }
        }

        subjectSuspend      = subjectSuspendOrig;
        subjectSizeSuspend  = subjectSizeSuspendOrig;
        waterQualitySuspend = waterQualitySuspendOrig;
      }
    }



    /// <summary>
    /// Constructor for 'StartRecordingDialog' dialog class.  This dialog box prompts the user for required parameters 
    /// need to perform a counting session.  It is to be used for both reco9rding and Playback sessions.
    /// </summary>
    /// <param name="_cameraManager">Reference to LarcosCounterManager instance.</param>
    /// <param name="_initialSessionParametrs"></param>
    /// <param name="_initialOperatingParameters"></param>
    /// <param name="_playingBack"></param>
    /// <param name="_runLog">Messages written to this RunLog instance will be sent to the log-file in "${CounterHomeDir}\LoggingFiles\LarcosCounter"
    /// and appear in the dialog panel.</param>
    public StartRecordingDialog (LarcosCounterManagerWrapper  _cameraManager,
                                 UmiSessionParameters         _initialSessionParametrs,
                                 UmiOperatingParameters       _initialOperatingParameters,
                                 bool                         _playingBack,
                                 UmiRunLog                    _runLog
                                )
    {
      cameraManager = _cameraManager;
      playingBack   = _playingBack;

      runLog = _runLog;
      if  (runLog == null)
        runLog  = cameraManager.GetUmiRunLogInstance ();
  
      if  (runLog == null)
        runLog = new UmiRunLog ();

      initialSessionParametrs = _initialSessionParametrs;
      if  (initialSessionParametrs == null)
        initialSessionParametrs = new UmiSessionParameters ();
    
      initialOperatingParameters = _initialOperatingParameters;
      if  (initialOperatingParameters == null)
        initialOperatingParameters = new UmiOperatingParameters ();

      advParameters = new AdvancedParameters (initialOperatingParameters);
      InitializeComponent ();

      //lastTrainingModelName = initialSessionParametrs.TrainingModelName;

      SetUpToolTips ();
    }



    private void StartRecordingDialog_Load (object sender, EventArgs e)
    {
      if  (playingBack)
        Text = "Replay Counting Session";
      else
        Text = "Start Counting Session";

      sessionDescription.Text = initialSessionParametrs.SessionDescription;
      personInCharge.Text     = initialSessionParametrs.PersonInCharge;
      source.Text             = initialSessionParametrs.Source;
      destination.Text        = initialSessionParametrs.Destination;
      comments.Text           = initialSessionParametrs.Comments;

      if  (playingBack)
        controlNum.Text = initialSessionParametrs.ControlNum;
      else
        controlNum.Text = cameraManager.GetNextControlNumber ();

      PopulateSubjects ();

      TrainingModelName = initialSessionParametrs.TrainingModelName;

      if  (cameraManager.OperatingMode () == LarcosOperatingModes.Advanced)
      {
        AdvancedOptionsButton.Enabled     = true;
        AdvancedOptionsButton.Visible     = true;
        SetCameraParametersButton.Enabled = true;
        SetCameraParametersButton.Visible = true;
      }
      else
      {
        AdvancedOptionsButton.Enabled     = false;
        AdvancedOptionsButton.Visible     = false;
        SetCameraParametersButton.Enabled = false;
        SetCameraParametersButton.Visible = false;
      }
    }



    private  void  SetUpToolTips ()
    {
      toolTip = new ToolTip ();
      toolTip.SetToolTip (subject ,              "Specify from the drop down list what you are going to count.");
      toolTip.SetToolTip (subjectSize,           "Specify from the drop down list the Size of what you are going to count.");
      toolTip.SetToolTip (waterQuality,          "Specify from the drop down list Water Quality.");
      toolTip.SetToolTip (controlNum,            "Use the provided control number or enter your own control number.");
      toolTip.SetToolTip (sessionDescription,    "Provide a short description of this session.");
      toolTip.SetToolTip (personInCharge,        "Enter who is responsible for this session.");
      toolTip.SetToolTip (source,                "Where are you moving the animals from?");
      toolTip.SetToolTip (destination,           "Where are you moving the animals to?");
      toolTip.SetToolTip (comments,              "Enter any other comments you have about this session.");
      toolTip.SetToolTip (StarButton,            "When you are ready to start moving animals press this button.");
      toolTip.SetToolTip (AdvancedOptionsButton, "To override advanced settings; be very careful when using this option!");
      toolTip.SetToolTip (CancelButton,          "Press this button if you decide not to run a session now.");
    }



    /// <summary>
    /// Call this method after the user accepted the Dialog Screen to retrieve UmiSessionParameters to be used for next session.
    /// </summary>
    /// <returns>Instance of UmiSessionParameters </returns>
    public  UmiSessionParameters  GetSessionParameters ()
    {
      UmiSessionParameters  sessionParameters = new UmiSessionParameters (initialSessionParametrs);
      sessionParameters.TrainingModelName  = TrainingModelName;
      sessionParameters.Comments           = comments.Text;
      sessionParameters.Destination        = destination.Text;
      sessionParameters.PersonInCharge     = personInCharge.Text;
      sessionParameters.SessionDescription = sessionDescription.Text;
      sessionParameters.ControlNum         = controlNum.Text;
      sessionParameters.Source             = source.Text;
      return  sessionParameters;
    }



    public  UmiOperatingParameters  GetOperatingParameters ()
    {
      return  advParameters.GetOperatingParameters ();
    }


    
    private  void  ReFreshFromSelectedTrainingModel ()
    {
      String  newTrainingModelName = TrainingModelName;
      if  (newTrainingModelName == lastTrainingModelName)
      {
        runLog.WriteLn (40, "ReFreshFromSelectedTrainingModel  TrainingModelName has not changed: " + newTrainingModelName);
        return;
      }

      runLog.WriteLn (20, "ReFreshFromSelectedTrainingModel  configFileName: " + newTrainingModelName);
      UmiTrainingConfiguration  config = new UmiTrainingConfiguration (newTrainingModelName, initialOperatingParameters, runLog);
      if  (config.Valid ())
      {
        UmiOperatingParameters  opParms = config.GetOperatingParameters ();
        if  (opParms != null)
        {
          if  (playingBack)
          {
            opParms.RequestedScanRate  = initialOperatingParameters.RequestedScanRate;
            opParms.DataIsToBeRecorded = false;
          }

          opParms.PlayingBack        = playingBack;
          initialOperatingParameters = opParms;
          advParameters.SetOperatingParameters (opParms);
        }
      }

      lastTrainingModelName = newTrainingModelName;
    }



    private  void  ValidateTrainingModel ()
    {
      errorProvider1.SetError (subject,      "");
      errorProvider1.SetError (subjectSize,  "");
      errorProvider1.SetError (waterQuality, "");

      String  configFileName = subject.Text + "_" + subjectSize.Text + "_" + waterQuality.Text + ".cfg";

      UmiTrainingConfiguration  config = new UmiTrainingConfiguration (configFileName, null, runLog);
      if  (!config.Valid ())
      {
        errorsFound = true;
        String  errMsg = "Training Model \"" + configFileName + "\" is not valid.";
        errorProvider1.SetError (subject,      errMsg);
        errorProvider1.SetError (subjectSize,  errMsg);
        errorProvider1.SetError (waterQuality, errMsg);
      }
    }



    private  void  ValidateFields ()
    {
      errorsFound = false;
      errorProvider1.SetError (controlNum, "");
      if  (String.IsNullOrEmpty (controlNum.Text))
      {
        errorsFound = true;
        errorProvider1.SetError (controlNum, "You must provide a Control Number.");
      }

      else
      {
        String  controlNumErrMsg = "";
        cameraManager.ControlNumValidityCheck (controlNum.Text, ref controlNumErrMsg);
        if  (!String.IsNullOrEmpty (controlNumErrMsg))
        {
          errorsFound = true;
          errorProvider1.SetError (controlNum, controlNumErrMsg);
        }
      }

      ValidateTrainingModel ();
    }



    private void subject_SelectedIndexChanged (object sender, EventArgs e)
    {
      if  (subjectSuspend)
        return;
      subjectSuspend = true;

      bool  subjectSizeSuspendOrig = subjectSizeSuspend;
      subjectSizeSuspend = true;

      String  s = (String)subject.SelectedItem;
      String  origSize = (String)subjectSize.SelectedItem;
      String  origQuality = (String)waterQuality.SelectedItem;

      String[]  listOfSizes = trainModels.GetListOfSizes (s);
      if  ((listOfSizes != null)  &&  (listOfSizes.Length > 0))
      {
        subjectSize.DataSource = listOfSizes;
        subjectSize.SelectedItem = origSize;
      }

      subjectSizeSuspend = subjectSizeSuspendOrig;
      subjectSuspend = false;
    }



    private void size_SelectedIndexChanged (object sender, EventArgs e)
    {
      if  (subjectSizeSuspend)
        return;
      subjectSizeSuspend = true;

      bool  waterQualitySuspendOrig = waterQualitySuspend;
      waterQualitySuspend = true;

      String  s = (String)subjectSize.SelectedItem;
      String  origQuality = (String)waterQuality.SelectedItem;
      waterQuality.DataSource = trainModels.GetListOfWaterQualities (subject.Text, s);
      waterQuality.SelectedItem = origQuality;

      waterQualitySuspend = waterQualitySuspendOrig;
      subjectSizeSuspend = false;
    }



    private void   waterQuality_SelectedIndexChanged (object sender, EventArgs e)
    {
      if  (waterQualitySuspend)
        return;
    }



    private void AdvancedOptionsButton_Click (object sender, EventArgs e)
    {
      ValidateFields ();
      if  (!errorsFound)
      {
        // If user has selected a different training model we want to retrieve the parameters specified in that training model.
        ReFreshFromSelectedTrainingModel ();

        advParameters.ShowDialog (this);
        if  (advParameters.UserAcceptedScreen)
          initialOperatingParameters = advParameters.GetOperatingParameters ();
      }
    }



    private void StarButton_Click (object sender, EventArgs e)
    {
      ValidateFields ();
      if  (errorsFound)
        return;

      ReFreshFromSelectedTrainingModel ();

      runLog.WriteLn (10, "StartRecordingDialog.StarButton_Click");
      startRecording = true;
      Close ();
    }



    private void CancelButton_Click (object sender, EventArgs e)
    {
      startRecording  = false;
      cancelRequested = true;
      Close ();
    }



    private void SetCameraParametersButton_Click (object sender, EventArgs e)
    {
      ValidateFields ();
      if  (!errorsFound)
      {
        ReFreshFromSelectedTrainingModel ();

        String  modelName = TrainingModelName;

        cameraManager.RequestedScanRate (initialOperatingParameters.RequestedScanRate);

        VisibilityDialog  vd = new VisibilityDialog (cameraManager);
        vd.ShowDialog ();

        UmiTrainingConfiguration  config = new UmiTrainingConfiguration (modelName, initialOperatingParameters, runLog);
        initialOperatingParameters.RequestedAnalogGain  = vd.GetRequestedAnalogGain ();
        initialOperatingParameters.RequestedDigitalGain = vd.GetRequestedDigitalGain ();
        config.SetOperatingParms (initialOperatingParameters);
        config.Save (config.FullFileName ());
        advParameters.SetOperatingParameters (initialOperatingParameters);
      }
    }
  }
}
