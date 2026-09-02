@echo off
set PATH=C:\Program Files\JetBrains\CLion 2025.2\bin\mingw\bin;%PATH%
g++ -std=c++17 -Wall -Wextra -static Document.cpp BracketChecker.cpp BookmarkManager.cpp WordIndex.cpp main.cpp -o editor.exe > build.log 2>&1
echo EXIT_CODE=%ERRORLEVEL% >> build.log
