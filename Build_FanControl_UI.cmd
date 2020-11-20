MSBuild.exe FanControlComponent_UI\FanControlComponent.sln /p:Configuration=Release /t:Rebuild
MSBuild.exe FanControlComponent_UI\FanControlComponent.sln /p:Configuration=Debug /t:Rebuild

copy FanControlComponent_UI\x64\Release\FanControlUI.exe FanControlUI_Exe\ /Y