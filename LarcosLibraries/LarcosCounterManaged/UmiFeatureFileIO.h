#pragma once


namespace LarcosCounterManaged {

  public  ref class UmiFeatureFileIO
  {
  public:
    UmiFeatureFileIO (String^  driverName);

  private:
    ~UmiFeatureFileIO ();

  protected:
    !UmiFeatureFileIO ();

  public:
    static  array<String^>^  RegisteredDriverNames ();
    static  bool             ValidDriverName (String^ _driverName);

    property  bool     CancelLoad         {bool     get ();   void set (bool _cancelLoad); }
    property  bool     CanRead            {bool     get ();}
    property  bool     CanWrite           {bool     get ();}
    property  bool     ChangesMade        {bool     get ();}
    property  String^  DriverName         {String^  get ();} 
    property  kkuint32 NumExamplesWritten {kkuint32 get ();}
    property  bool     Successful         {bool     get ();}
    property  bool     Valid              {bool     get ();}


    void  SaveFeatureFile (String^                  _fileName, 
                           UmiFeatureVectorList^  _examples,
                           UmiRunLog^             _runLog
                          );

    UmiFeatureVectorList^   LoadFeatureFile (String^          _fileName, 
                                               UmiClass^  _classes, 
                                               UmiRunLog^     _runLog
                                              );


  private:
    bool*             cancelLoad;
    bool*             changesMade;
    FeatureFileIOPtr  driver;
    String^           lastError;
    kkuint32*             numExamplesWritten;
    bool*             successful;
  };

}  /* LarcosCounterManaged */
