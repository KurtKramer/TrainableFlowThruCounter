copy Apps\LarcosCounter\bin\Release\*.exe                                        %LarcosHomeDir%\exe\
copy Apps\LarcosCounter\bin\Release\*.dll                                        %LarcosHomeDir%\exe\
copy Apps\ManagedApps\bin\Release\*.exe                                          %LarcosHomeDir%\exe\
copy Apps\ManagedApps\bin\Release\*.dll                                          %LarcosHomeDir%\exe\
copy Apps\LarcosFileViewer\bin\Release\*.exe                                     %LarcosHomeDir%\exe\
copy Apps\LarcosFileViewer\bin\Release\*.dll                                     %LarcosHomeDir%\exe\
copy OutsidePackages\MySQL\lib\opt\libmysql.dll                                  %LarcosHomeDir%\exe\
copy OutsidePackages\MySQL\lib\opt\libmysql.dll                                  %LarcosHomeDir%\exe\


mkdir %LarcosHomeDir%\SetUp\
mkdir %LarcosHomeDir%\SetUp\vcredist_x86\
copy LarcosSetup\Release\*.*                                                     %LarcosHomeDir%\SetUp\
copy LarcosSetup\Release\vcredist_x86\*.*                                        %LarcosHomeDir%\SetUp\vcredist_x86\



