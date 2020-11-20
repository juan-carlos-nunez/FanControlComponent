call clean_x64.cmd

MSBuild.exe FanControlComponent\FanControlComponent.sln /p:Configuration=Release /t:Clean
MSBuild.exe FanControlComponent\FanControlComponent.sln /p:Configuration=Debug /t:Clean
MSBuild.exe FanControlComponent_Gtest\FanControlComponent.sln /p:Configuration=Release /t:Clean
MSBuild.exe FanControlComponent_Gtest\FanControlComponent.sln /p:Configuration=Debug /t:Clean
MSBuild.exe FanControlComponent_UI\FanControlComponent.sln /p:Configuration=Release /t:Clean
MSBuild.exe FanControlComponent_UI\FanControlComponent.sln /p:Configuration=Debug /t:Clean
MSBuild.exe FanControlComponent_Lib\FanControlComponent.sln /p:Configuration=Release /t:Clean
MSBuild.exe FanControlComponent_Lib\FanControlComponent.sln /p:Configuration=Debug /t:Clean

MSBuild.exe FanControlComponent\FanControlComponent.sln /p:Configuration=Release /t:Rebuild
MSBuild.exe FanControlComponent\FanControlComponent.sln /p:Configuration=Debug /t:Rebuild
MSBuild.exe FanControlComponent_Gtest\FanControlComponent.sln /p:Configuration=Release /t:Rebuild
MSBuild.exe FanControlComponent_Gtest\FanControlComponent.sln /p:Configuration=Debug /t:Rebuild
MSBuild.exe FanControlComponent_UI\FanControlComponent.sln /p:Configuration=Release /t:Rebuild
MSBuild.exe FanControlComponent_UI\FanControlComponent.sln /p:Configuration=Debug /t:Rebuild
MSBuild.exe FanControlComponent_Lib\FanControlComponent.sln /p:Configuration=Release /t:Rebuild
MSBuild.exe FanControlComponent_Lib\FanControlComponent.sln /p:Configuration=Debug /t:Rebuild

copy FanControlComponent_UI\x64\Release\FanControlUI.exe FanControlUI_Exe\ /Y