//********************************************************************************************
//*  A class that will consist of static methods only.  This is where we will keep track of  *
//*  the current active training libraries.  This way we will not need to implement the      *
//*  code in each separate application.                                                      *
//********************************************************************************************

using  System;
using  System.Collections.Generic;
using  System.IO;
using  System.Text;
using  System.Threading;
using  System.Windows.Forms;

using LarcosCounterManaged;

//
//-------------------------------------------------------------------------------------------------------------------
//  Once a model is loaded via  "SetModel1"  or  "SetModel2"  they will not be unloaded except by the Memory Manager.
//  this way if a process is using a specific model that was retrieved by "Model1" or "Model2" they can continue to 
//  use it even if a new model is set by "SetModel1"  or  "SetModel2".
//-------------------------------------------------------------------------------------------------------------------


namespace LarcosManagedRoutines
{
  /// <summary>
  /// A class that will consist of static methods only.  This is where we will keep track of
  /// the current active training libraries.  This way we will not need to implement the
  /// code in each separate application. 
  /// 
  ///  Once a model is loaded via  "SetModel1"  or  "SetModel2"  they will not be unloaded 
  ///  except by the Memory Manager. This way if a process is using a specific model that 
  ///  was retrieved by "Model1" or "Model2" they can continue to use it even if a new model 
  ///  is set by "SetModel1"  or  "SetModel2".
  /// </summary>
  public  class ActiveTrainingLibraries
  {
    public  delegate  void  DoneLoadingEvent (UmiTrainingModel2  model);

    private  static  String               configFileName         = "";
    private  static  Boolean              configLoaded           = false;

    private  static  UmiTrainingModel2    model1                 = null;
    private  static  String               model1Name             = "";
    private  static  DoneLoadingEvent     doneLoadingModel1Event = null;
    private  static  TrainingModelStatus  model1LoadStatus       = null;


    private  static  UmiTrainingModel2    model2                 = null;
    private  static  String               model2Name             = "";
    private  static  DoneLoadingEvent     doneLoadingModel2Event = null;
    private  static  TrainingModelStatus  model2LoadStatus       = null;

    private  static  UmiGoalKeeper      blocker                = new UmiGoalKeeper ("ActiveTrainingLibraries");



    /// <summary>
    /// Will cancel the loading of any models still loading.
    /// </summary>
    /// <remarks>
    /// Call this method to stop any models that are currently loading.  It does this by
    /// setting the cancel flag for each of them and then returns.
    /// <see cref="TrainingModelStatus.CancelLoad"/>
    /// </remarks>
    public  static  void  CancelLoads ()
    {
      BlockStart ();
      if  (model1LoadStatus != null)
        model1LoadStatus.CancelLoad ();
        
      if  (model2LoadStatus != null)
        model2LoadStatus.CancelLoad ();
        
      BlockEnd ();
    }  /* CancelLoads */



    public  static  void  LoadModels (DoneLoadingEvent  loadDoneEvent)
    {
      BlockStart ();
      if  (!configLoaded)
        LoadConfiguration ();

      bool loadModel1 = ((model1 == null)  &&  (!String.IsNullOrEmpty (model1Name))  &&  (model1LoadStatus == null));
      bool loadModel2 = ((model2 == null)  &&  (!String.IsNullOrEmpty (model2Name))  &&  (model2LoadStatus == null));

      bool  pauseForASec = loadModel1 && loadModel2;

      if  (loadModel1)
        SetModel1 (model1Name, false, loadDoneEvent);

      if  (pauseForASec)
        Thread.Sleep (1000);

      if  (loadModel2)
        SetModel2 (model2Name, false, loadDoneEvent);

      BlockEnd ();
    }  /* LoadModels */



    /// <summary>
    /// Indicates wheather tehre is at least one active Training Model fully loaded.
    /// </summary>
    /// <returns></returns>
    public  static  bool  ModelsAreAvailable ()  
    {
      bool  loadModel1         = false;
      bool  loadModel2         = false;
      bool  modelsAreAvailable = false;

      BlockStart ();
      if  (!configLoaded)
        LoadConfiguration ();

      if  ((model1 == null)  &&  (!String.IsNullOrEmpty (model1Name))  &&  (model1LoadStatus == null))
        loadModel1 = true;

      if  ((model2 == null)  &&  (!String.IsNullOrEmpty (model2Name))  &&  (model2LoadStatus == null))
        loadModel2 = true;

      modelsAreAvailable = ((model1 != null)  ||  (model2 != null));

      BlockEnd ();


      if  (loadModel1)
        SetModel1 (model1Name, false, null);

      if  (loadModel2)
        SetModel2 (model2Name, false, null);

      return  modelsAreAvailable;
    }  /* ModelsAreAvailable */




    private  static  void  BlockStart ()
    {
      blocker.StartBlock ();
    }  /* BlockStart */



    private  static  void  BlockEnd ()
    {
      blocker.EndBlock ();
    }  /* BlockEnd */




    public  static  bool  ModelsAreLoading ()  
    {
      bool  modelsAreLoading = false;
      BlockStart ();
      if  ((model1LoadStatus != null)  ||  (model2LoadStatus != null))
        modelsAreLoading = true;
      BlockEnd ();
      return  modelsAreLoading;
    }  /* ModelsAreAvailable */



    public  static  UmiTrainingModel2  Model1 ()
    {
      UmiTrainingModel2  model = null;
      
      BlockStart ();
      model = model1;
      BlockEnd ();
      return  model;
    }


    public  static  UmiClassList  Model1ListOfClasses ()
    {
      UmiClassList  classes = null;
      BlockStart ();
      if  (model1 != null)  classes = model1.ImageClasses ();  // Will make a copy of the list of classes in training model1.
      BlockEnd ();
      return  classes;
    }


    public  static  UmiTrainingModel2  Model2 ()
    {
      UmiTrainingModel2  model = null;

      BlockStart ();
      model = model2;
      BlockEnd ();
      return  model;
    }




    /// <summary>
    /// Using supplied 'Raster' data it will call each currently active model and return there predictions.
    /// </summary>
    /// <param name="raster">Raster of image to predict. .</param>
    /// <param name="imageFileName">Name of the image file.</param>
    /// <param name="model1Predictions">The model1 predictions.</param>
    /// <param name="model2Predictions">The model2 predictions.</param>
    /// <param name="intermediateImages">If not set to null will return with a list of intermediate images crteated during feature computation</param>
    /// <param name="runLog">The run log.</param>
    public  static  void  MakePredictions (UmiRaster              raster,
                                           String                 imageFileName,
                                           ref UmiPredictionList  model1Predictions,
                                           ref UmiPredictionList  model2Predictions,
                                           bool                   saveDebugImages,
                                           UmiRunLog              runLog 
                                          )
    {
      model1Predictions = null;
      model2Predictions = null;
      if  (!ModelsAreAvailable ())
        return;

      ParsedImageFileName  pifn = UmiFeatureVector.ParseImageFileName (imageFileName);
      String  scannerFileName = pifn.scannerFileName;
      uint    scanLineNum    = pifn.scanLineNum;

      UmiFeatureVector  fv = new UmiFeatureVector (raster, imageFileName, null, saveDebugImages, runLog);

      if  (model1 != null)
        model1Predictions = model1.PredictProbabilities (fv);

      if  (model2 != null)
        model2Predictions = model2.PredictProbabilities (fv);

      fv = null;

      return;
    }  /* MakePredictions */


    /// <summary>
    /// Using supplied 'Raster' data it will call each currently active model and return there predictions.
    /// </summary>
    /// <param name="raster">Raster of image to predict. .</param>
    /// <param name="imageFileName">Name of the image file.</param>
    /// <param name="model1Predictions">The model1 predictions.</param>
    /// <param name="runLog">The run log.</param>
    public  static  void  MakePredictions (UmiRaster              raster,
                                           String                 imageFileName,
                                           ref UmiPredictionList  model1Predictions,
                                           bool                   saveDebugImages,
                                           UmiRunLog              runLog 
                                          )
    {
      model1Predictions = null;
      if  (!ModelsAreAvailable ())
        return;

      ParsedImageFileName  pifn = UmiFeatureVector.ParseImageFileName (imageFileName);
      String  scannerFileName = pifn.scannerFileName;
      uint    scanLineNum     = pifn.scanLineNum;

      UmiFeatureVector  fv = new UmiFeatureVector (raster, imageFileName, null, saveDebugImages, runLog);

      if  (model1 != null)
        model1Predictions = model1.PredictProbabilities (fv);

      fv = null;

      return;
    }  /* MakePredictions */




    public  static  void  MakePredictions (UmiFeatureVector       featureVector,
                                           ref UmiPredictionList  model1Predictions,
                                           ref UmiPredictionList  model2Predictions,
                                           UmiRunLog              runLog 
                                          )
    {
      model1Predictions = null;
      model2Predictions = null;
      if  ((!ModelsAreAvailable ())  ||  (featureVector == null))
        return;

      if  (model1 != null)
        model1Predictions = model1.PredictProbabilities (featureVector);

      if  (model2 != null)
        model2Predictions = model2.PredictProbabilities (featureVector);

      return;
    }  /* MakePredictions */




    public  static  String  Model1Name
    {
      get 
      {
        String  modelName = "";
        BlockStart ();
        if  (!configLoaded)
          LoadConfiguration ();
        modelName = model1Name;
        BlockEnd ();
        return  modelName;
      }
    }


    public  static  String  Model2Name
    {
      get 
      {
        String  modelName = "";
        BlockStart ();
        if  (!configLoaded)
          LoadConfiguration ();
        modelName = model2Name;
        BlockEnd ();
        return  modelName;
      }
    }




    private  static  void  SetConfigurationFileName ()
    {
      configFileName = UmiOSservices.AddSlash (UmiVariables.ConfigurationDir ()) + "ActiveTrainingLibraries.cfg";
    }  /* SetConfigurationFileName */



    public  static  void  LoadConfiguration ()
    {
      BlockStart ();

      SetConfigurationFileName ();

      model1 = null;
      model2 = null;

      StreamReader  sr = null;
      try  {sr = new StreamReader (configFileName);}  catch  (Exception)  {sr = null;}
      if  (sr != null)
      {
        String  line = "";
      
        while  ((line = sr.ReadLine ()) != null)
        {
          String[]  fields = line.Split ('\t', '=');
          if  (fields.Length < 2)
            continue;

          String fieldName = fields[0].ToLower ();
          if  (fieldName == "model1name")
            model1Name = fields[1];

          else if  (fieldName == "model2name")
            model2Name = fields[1];
        }

        sr.Close ();
        sr = null;

        //if  (!String.IsNullOrEmpty (model1Name))
        //  SetModel1 (model1Name, false, null);

        //if  (!String.IsNullOrEmpty (model2Name))
        //  SetModel2 (model2Name, false, null);
      }
      
      configLoaded = true;
      
      BlockEnd ();
    }  /* LoadConfiguration */
    





    public  static  void  SaveConfiguration ()
    {
      BlockStart ();

      SetConfigurationFileName ();

      StreamWriter  sw = null;
      try  {sw = new StreamWriter (configFileName);}  catch  (Exception)  {sw = null;}
      if  (sw == null)
        return;

      sw.WriteLine ("Model1Name" + "\t" + model1Name);
      sw.WriteLine ("Model2Name" + "\t" + model2Name);

      sw.Close ();
      sw = null;

      BlockEnd ();
    }  /* LoadConfiguration */
    




    public  static  void  SetModel1 (String            _model1Name,
                                     bool              _forceRebuild,
                                     DoneLoadingEvent  _doneLoadingEvent
                                    )
    {
      bool  model1CurentlyLoading = false;
      
      BlockStart ();

      if  (model1LoadStatus != null)
      {
        model1CurentlyLoading = true;
      }
      else
      {
        if  ((model1 != null)  &&  (model1Name == _model1Name)  &&  (!_forceRebuild))
        {
          // We already have this model loaded;  there is nothing to do.
          // But we still have to the call back to let it know that the model is loaded.
          if  (_doneLoadingEvent != null)
            _doneLoadingEvent (model1);
        }
        else
        {
          if  (model1 != null)
          {
            model1 = null;
            GC.Collect ();
          }
          GC.Collect ();
          
          Model1LoadStart (_model1Name, _forceRebuild, _doneLoadingEvent);
          SaveConfiguration ();
        }
      }

      BlockEnd ();
        
      if  (model1CurentlyLoading)
      {
        MessageBox.Show ("Can not set a new Model;  one is currenly loading now.");
        return;
      }
    }  /* SetModel1 */








    public  static  void  SetModel2 (String            _model2Name,
                                     bool              _forceRebuild,
                                     DoneLoadingEvent  _doneLoadingEvent
                                    )
    {
      bool  model2CurentlyLoading = false;
      BlockStart ();

      if  (model2LoadStatus != null)
        model2CurentlyLoading = true;
      else
      {
        if  ((model2 != null)  &&  (model2Name == _model2Name)  &&  (!_forceRebuild))
        {
          // We already have this model loaded;  there is nothing to do.
          // But we still have to the call back to let it know that the model is loaded.
          if  (_doneLoadingEvent != null)
            _doneLoadingEvent (model2);
        }
        else
        {
          if  (model2 != null)
          {
            model2 = null;
            GC.Collect ();
          }
          GC.Collect ();
          Model2LoadStart (_model2Name, _forceRebuild, _doneLoadingEvent);
          SaveConfiguration ();
        }
      }  

      BlockEnd ();
        
      if  (model2CurentlyLoading)
      {
        MessageBox.Show ("Can not set a new Model;  one is currently loading now.");
        return;
      }
    }  /* SetModel2 */




   static  private  void  Model1LoadStart (String            _model1Name,
                                           bool              _forceRebuild,
                                           DoneLoadingEvent  _doneLoadingEvent
                                          )
    {
      model1Name = _model1Name;

      if  (String.IsNullOrEmpty (model1Name))
        return;

      doneLoadingModel1Event = _doneLoadingEvent;

      model1LoadStatus = new TrainingModelStatus (_model1Name,  _forceRebuild, Model1LoadCompletion);
      model1LoadStatus.Show ();
    }  /* Model1LoadStart */






   static  private  void  Model2LoadStart (String            _model2Name,
                                           bool              _forceRebuild,
                                           DoneLoadingEvent  _doneLoadingEvent
                                          )
    {
      model2Name = _model2Name;

      if  (String.IsNullOrEmpty (model2Name))
        return;

      doneLoadingModel2Event = _doneLoadingEvent;

      model2LoadStatus = new TrainingModelStatus (_model2Name,  _forceRebuild, Model2LoadCompletion);
      model2LoadStatus.Show ();
    }  /* Model1LoadStart */







    static  private  void  Model1LoadCompletion (TrainingModelStatus  sender)
    {
      if  (!sender.ValidModel ())
        model1 = null;
      else
        model1 = sender.TrainiedModel ();
       
      model1LoadStatus = null;
      if  (doneLoadingModel1Event != null)
        doneLoadingModel1Event (model1);
    }  /* Model1LoadCompletion */

  
  
  
  


    static  private  void  Model2LoadCompletion (TrainingModelStatus  sender)
    {
      if  (!sender.ValidModel ())
        model2 = null;
      else
        model2 = sender.TrainiedModel ();
       
      model2LoadStatus = null;
      if  (doneLoadingModel2Event != null)
        doneLoadingModel2Event (model2);
    }  /* Model2LoadCompletion */

  
  }
}
