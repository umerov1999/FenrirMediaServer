@echo off
set path=C:\Program Files\NASM;%path%
C:
cd C:/
set OPENSSL_VERSION=3.0.5
set SEVENZIP="C:\Program Files\7-Zip\7z.exe"
set VS2022_AMD64="C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat"


IF NOT EXIST "C:\openssl-src-win64" GOTO NO_WIN64_SOURCE
DEL "C:\openssl-src-win64" /Q /F /S
RMDIR /S /Q "C:\openssl-src-win64"
:NO_WIN64_SOURCE

del openssl-%OPENSSL_VERSION%.tar
%SEVENZIP% e openssl-%OPENSSL_VERSION%.tar.gz
%SEVENZIP% x -y openssl-%OPENSSL_VERSION%.tar
ren openssl-%OPENSSL_VERSION% openssl-src-win64-VS2022
DEL pax_global_header


CALL %VS2022_AMD64%

C:

cd \openssl-src-win64-VS2022

powershell -Command "(gc Configurations/10-main.conf) -replace '/Zi /Fdossl_static.pdb', '' | Out-File -Encoding ascii Configurations/10-main.conf"
powershell -Command "(gc Configurations/10-main.conf) -replace '\"-g\"', '\"\"' | Out-File -Encoding ascii Configurations/10-main.conf"

perl Configure VC-WIN64A --release no-shared no-engine no-module no-crypto-mdebug no-tests no-external-tests no-legacy no-weak-ssl-ciphers --prefix=C:\openssl-%OPENSSL_VERSION%-64bit-release-static-VS2022 -D_WIN32_WINNT=0x0600
nmake -f Makefile install

cd ../

%SEVENZIP% a -r openssl-%OPENSSL_VERSION%-64bit-release-static-VS2022.7z openssl-%OPENSSL_VERSION%-64bit-release-static-VS2022\*

DEL openssl-%OPENSSL_VERSION%-64bit-release-static-VS2022 /Q /F /S

RMDIR /S /Q openssl-%OPENSSL_VERSION%-64bit-release-static-VS2022
RMDIR /S /Q openssl-src-win64-VS2022
del openssl-%OPENSSL_VERSION%.tar
pause