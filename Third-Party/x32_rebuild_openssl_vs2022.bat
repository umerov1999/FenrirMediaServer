@echo off
set OPENSSL_VERSION=3.3.0

SET CurrentDir=%cd%
set path=%ProgramFiles%\NASM;%path%
set SEVENZIP="%ProgramFiles%\7-Zip\7z.exe"
set VS2022="%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars32.bat"
CALL %VS2022%

cd /d "%CurrentDir%"

IF NOT EXIST openssl-src-win32-VS2022 GOTO NO_WIN32_SOURCE
DEL openssl-src-win32-VS2022 /Q /F /S
RMDIR /S /Q openssl-src-win32-VS2022
:NO_WIN32_SOURCE

del openssl-%OPENSSL_VERSION%.tar
%SEVENZIP% e openssl-%OPENSSL_VERSION%.tar.gz
%SEVENZIP% x -y openssl-%OPENSSL_VERSION%.tar
ren openssl-%OPENSSL_VERSION% openssl-src-win32-VS2022
IF EXIST pax_global_header DEL pax_global_header


cd /d "%CurrentDir%\openssl-src-win32-VS2022"

powershell -Command "(gc Configurations/10-main.conf) -replace '/Zi /Fdossl_static.pdb', '' | Out-File -Encoding ascii Configurations/10-main.conf"
powershell -Command "(gc Configurations/10-main.conf) -replace '\"-g\"', '\"\"' | Out-File -Encoding ascii Configurations/10-main.conf"
powershell -Command "(gc Configurations/windows-makefile.tmpl) -replace '\"\$\(PERL\)\" \"\$\(SRCDIR\)\\util\\copy.pl\" ossl_static.pdb \"\$\(libdir\)\"', 'echo \"test\"' | Out-File -Encoding ascii Configurations/windows-makefile.tmpl"
powershell -Command "(gc Configurations/windows-makefile.tmpl) -replace 'install_runtime\: install_programs', 'install_runtime\: install_runtime_libs' | Out-File -Encoding ascii Configurations/windows-makefile.tmpl"

perl Configure VC-WIN32 --release no-shared no-engine no-module no-crypto-mdebug no-tests no-external-tests no-legacy no-weak-ssl-ciphers --openssldir=%CurrentDir%\openssl-src-win32-VS2022/TargetInstall --prefix=%CurrentDir%\openssl-%OPENSSL_VERSION%-32bit-release-static-VS2022 -D_WIN32_WINNT=0x0600
nmake -f Makefile install

cd ../

%SEVENZIP% a -r openssl-%OPENSSL_VERSION%-32bit-release-static-VS2022.7z openssl-%OPENSSL_VERSION%-32bit-release-static-VS2022\*

DEL openssl-%OPENSSL_VERSION%-32bit-release-static-VS2022 /Q /F /S

RMDIR /S /Q openssl-%OPENSSL_VERSION%-32bit-release-static-VS2022
RMDIR /S /Q openssl-src-win32-VS2022
del openssl-%OPENSSL_VERSION%.tar
pause
