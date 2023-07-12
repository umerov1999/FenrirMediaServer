@echo off
CALL "%ProgramFiles%\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars32.bat"

cd ./
nmake /E CC="cl /D_WIN32_WINNT=0x0600" /f Makefile.vc mode=static RTLIBCFG=static DEBUG=no MACHINE=x86 GEN_PDB=no WITH_SSL=static WITH_DEVEL=deps ENABLE_IDN=no ENABLE_WINSSL=no ENABLE_SSPI=no ENABLE_SCHANNEL=no ENABLE_SYMBOL_HIDING=yes ENABLE_OPTIMIZE=yes ENABLE_TELNET=no ENABLE_TFTP=no ENABLE_POP3=no ENABLE_IMAP=no ENABLE_GOPHER=no ENABLE_MANUAL=no ENABLE_SMTP=yes
pause
