SET DLL_DIR=D:\VS2010\tutorials\bin
SET TARGET_BUILD=Debug

SET PATH=%DLL_DIR%;%PATH%
cd build
%TARGET_BUILD%\MedievalTown.exe %*
