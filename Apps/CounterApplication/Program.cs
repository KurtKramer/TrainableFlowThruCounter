using System;
using System.Collections.Generic;
//using System.Linq;
using System.Windows.Forms;

using CounterManaged;


namespace CounterApplication
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
      //SqlServerTypes.Utilities.LoadNativeAssemblies(AppDomain.CurrentDomain.BaseDirectory);

#if (DEBUG)
      UmiMethods.StartMemoryLeakDetection ();
#endif

      CounterManaged.UmiVariables.SetCounterHomeDir ();  // Will set the application frame work variables to there default values.

      var homeDir = CounterManaged.UmiVariables.CounterHomeDir();

      if  (!System.IO.Directory.Exists(homeDir))
      {
        MessageBox.Show($"Home Directory {homeDir} does not exists!");
        Application.Exit ();
        return;
      }

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      try
      {
        Application.Run(new CounterApplication());
      }
      catch (Exception e)
      {
        using (var r = new System.IO.StreamWriter(@"C:\Temp\TrainableFlowThruCounter.log", append:true))
        {
          r.WriteLine(DateTime.Now.ToLongTimeString () + "  " + e.ToString ());
          r.WriteLine();
          r.WriteLine(e.StackTrace);
          r.WriteLine();
        }
      }
      System.GC.Collect();
    }
  }
}
