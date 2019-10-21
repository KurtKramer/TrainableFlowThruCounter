using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;


namespace CounterFileViewer
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main (string[]  args)
    {
      //UmiMethods.StartMemoryLeakDetection ();

      CounterManaged.UmiVariables.SetCounterHomeDir ();  // Will set the application frame work variables to there default values.
                                                               // Will set to value pointed to by environment variable "CounterHomeDir".  If
                                                               // this variable is not defined will then default to "C:\\Counter".

      AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler
        ((sender, e) => 
          {
            if (e.IsTerminating) 
            {
              object o = e.ExceptionObject;
              MessageBox.Show ("UnhandledException " + o.ToString ());
            }
          }
        );

      Application.EnableVisualStyles ();
      Application.SetCompatibleTextRenderingDefault (false);

      String  fileName    = null;
      if  ((args.Length == 1)  &&  (args[0][0] != '-'))
        fileName = args[0];

      else
      {
        int  x = 0;
        while  (x < args.Length)
        {
          String  fieldName = "";
          String  fieldValue = "";

          if  (args[x][0] != '-')
          {
            if  (String.IsNullOrEmpty (fileName))
              fileName = args[x];
            x++;
          }
          else
          {
            fieldName = args[x];
            x++;

            if  ((x < args.Length)  &&  (args[x][0] != '-'))
            {
              fieldValue = args[x];
              x++;
            }

            fieldName = fieldName.ToUpper ();

            if  ((fieldName == "-NAME")  ||  (fieldName == "-FILENAME"))
              fileName = fieldValue;

            else
            {
              System.Console.WriteLine ("Invalid Parameter[" + fieldName + "]");
              return;
            }
          }
        }
      }
      

      if  (String.IsNullOrEmpty (fileName))
      {
        try
        {
          Application.Run (new CounterFileViewer ());
        }

        catch  (FileNotFoundException  e0)
        {
          MessageBox.Show ("FileNotFoundException FileName [" + e0.FileName + "] Starting CounterFileViewer" + "\n\n" + 
                           e0.StackTrace
                          );
        }
        catch  (FileLoadException  e3)
        {
          MessageBox.Show ("FileLoadException FileName [" + e3.FileName + "] Starting CounterFileViewer" + "\n\n" + 
                           e3.Message  + "\n\n" +
                           e3.StackTrace
                          );
        }
        catch  (EntryPointNotFoundException  e1)
        {
          MessageBox.Show ("EntryPointNotFoundException Starting CounterFileViewer" + "\n\n" + 
                           e1.ToString () + "\n\n"  +
                           e1.Message
                          );
        }
        catch  (Exception e2)
        {
          MessageBox.Show ("Exception Starting CounterFileViewer" + "\n\n" + 
                           e2.ToString () + "\n\n"  +
                           e2.StackTrace
                          );
        }
      }
      else
      {
        try
        {
          Application.Run (new CounterFileViewer (fileName));
        }
        catch  (FileNotFoundException  e0)
        {
          MessageBox.Show ("FileNotFoundException FileName [" + e0.FileName + "] Starting CounterFileViewer" + "\n\n" + 
                           e0.StackTrace
                          );
        }
        catch  (FileLoadException  e3)
        {
          MessageBox.Show ("FileLoadException FileName [" + e3.FileName + "] Starting CounterFileViewer" + "\n\n" + 
                           e3.Message  + "\n\n" +
                           e3.StackTrace
                          );
        }
        catch  (EntryPointNotFoundException  e1)
        {
          MessageBox.Show ("EntryPointNotFoundException Starting CounterFileViewer" + "\n\n" + 
                           e1.ToString () + "\n\n"  +
                           e1.Message
                          );
        }
        catch  (Exception e2)
        {
          MessageBox.Show ("Exception Starting CounterFileViewer" + "\n\n" + 
                           e2.ToString () + "\n\n"  +
                           e2.StackTrace
                          );
        }
      }
    }
  }
}