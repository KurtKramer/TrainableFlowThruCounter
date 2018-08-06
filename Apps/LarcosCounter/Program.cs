using System;
using System.Collections.Generic;
//using System.Linq;
using System.Windows.Forms;

using LarcosCounterManaged;


namespace LarcosCounter
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
      SqlServerTypes.Utilities.LoadNativeAssemblies(AppDomain.CurrentDomain.BaseDirectory);
      UmiMethods.StartMemoryLeakDetection ();

      LarcosCounterManaged.UmiVariables.SetLarcosHomeDir ();  // Will set the application frame work variables to there default values.
                                                              // Will set to value pointed to by environment variable "LarcosHomeDir".  If
                                                              // this variable is not defined will then default to "C:\Larcos".

      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);
      Application.Run (new LarcosCounter ());
    }
  }
}
