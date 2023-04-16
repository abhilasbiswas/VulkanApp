@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

SET includes=/Isrc /I%VULKAN_SDK%/Include /ID:/_Dev/_cpp/vcpkg/installed/x64-windows/include
SET links=/link /LIBPATH:%VULKAN_SDK%/Lib  /LIBPATH:D:/_Dev/_cpp/vcpkg/installed/x64-windows/lib vulkan-1.lib glfw3dll.lib
SET defines=/D DEBUG

echo "Building"

cl /EHsc %includes% %defines% src/main.cpp %links%

main.exe