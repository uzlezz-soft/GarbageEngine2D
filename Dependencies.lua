Include = {}

Include["glad"] = "%{wks.location}/GarbageEngine2D/ThirdParty/glad/Source/Public"
Include["spdlog"] = "%{wks.location}/GarbageEngine2D/ThirdParty/spdlog/Source/Public"
Include["GLFW"] = "%{wks.location}/GarbageEngine2D/ThirdParty/GLFW/include"
Include["stb"] = "%{wks.location}/GarbageEngine2D/ThirdParty/stb/Source/Public"
Include["pfd"] = "%{wks.location}/GarbageEditor/ThirdParty/SingleHeaders/portable-file-dialogs/Source/Public"

Library = {}

Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"