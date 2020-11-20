set ORIGINAL_DIR=%CD%

cd FanControlComponent
rmdir x64 /s /q
cd FanControlComponent
rmdir x64 /s /q

cd %ORIGINAL_DIR%

cd .\FanControlComponent_Lib
rmdir x64 /s /q
cd FanControlComponent
rmdir x64 /s /q

cd %ORIGINAL_DIR%

cd .\FanControlComponent_Gtest
rmdir x64 /s /q
cd FanControlComponentUT
rmdir x64 /s /q

cd %ORIGINAL_DIR%

cd .\FanControlComponent_UI
rmdir x64 /s /q
cd FanControlUI
rmdir x64 /s /q

cd %ORIGINAL_DIR%