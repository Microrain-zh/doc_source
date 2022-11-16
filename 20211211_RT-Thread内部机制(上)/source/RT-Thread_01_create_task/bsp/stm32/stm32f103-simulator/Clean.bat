echo off


DEL /F/S/Q *.map
DEL /F/S/Q *.lst

DEL /F/S/Q *.o
DEL /F/S/Q *.axf
DEL /F/S/Q *.Bak
DEL /F/S/Q *.crf
DEL /F/S/Q *.d
DEL /F/S/Q *.dep
DEL /F/S/Q *.htm
DEL /F/S/Q *.lnp
DEL /F/S/Q *.iex
DEL /F/S/Q *.hex

DEL /F/S/Q *.Administrator
DEL /F/S/Q JLinkLog.txt
DEL /F/S/Q JLinkSettings.ini
DEL /F/S/Q *.scvd

DEL /F/S/Q *.ncb
attrib -h *.suo
DEL /F/S/Q *.suo

DEL /F/S/Q *.user