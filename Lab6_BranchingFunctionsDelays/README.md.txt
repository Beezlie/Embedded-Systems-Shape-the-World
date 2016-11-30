Ran into bug in Keil debugger.  Project compiles and downloads to flash but the debugger closes immediately once opened.

Fix (courtesy of Teas instruments forum):

https://e2e.ti.com/support/microcontrollers/tiva_arm/f/908/t/296452#pi239031350=4

In the Windows registry search for an entry:

HKEY_CURRENT_USER\Software\Microsoft\Windows NT\CurrentVersion\AppCompatFlags\Layers

If there is an entry similar to the following: 

C:\Keil\UV4\UV4.exe           REG_SZ               $ IgnoreFreeLibrary<lmidk-agdi.dll>

Remove the entry.  Reboot and retry the debug session.