**NOTE: This tool is design for Cxbx-Reloaded developers to troubleshoot, add, and validate OOVPA signatures.**
# CHPATRI
* Convert
* HLE Cache File
* Plus
* Asm Section
* To
* Reverse Engineer
* Informative Asm File

### Feature
* Input 4 to 6 arguments manually.
* CHPATRI.ini file as global in same directory with CHPATRI.exe file.
* Drag and drop custom ini file on CHPATRI.exe file executable.

### How to Use it
To use it, you will need to input within range of 4 to 6 arguments in your command prompt (cmd.exe) in order to process conversion correctly. Or use your own ini file to automate process your request. See "**Automate Process**" to find out how.

```
CHPATRI.exe <Section's Virtual Address> <Section's Size of Raw> <HLE Cache file name (Input)> <Section Asm file (Input)> [HLE Cache file name (Output, optional)] [Section Asm file (Output, optional)]
```

For example with Cel Damage title:
```
CHPATRI.exe 0024CF80 0001FF4C 93bf5881.ini DSOUND.txt rangeDetect.txt DSOUND.asm
```

### Automate Process
There are up to 6 arguments can be used. Create a global `CHPATRI.ini` file in same directory CHPATRI.exe executable is in. Then add all or a few of these variables from below in CHPATRI.ini file.
* `arg1=` Virtual Address
* `arg2=` Size of Raw
* `arg3=` HLE Cache ini file (Input)
* `arg4=` Library section txt file (Input)
* `arg5=` HLE Cache ini file (Output, optional)
* `arg6=` Library section txt file (Output, optional)

Plus you can also use custom ini file for drag and drop onto CHPATRI.exe file. Be aware of custom ini will overwrite global settings.

**NOTE: arg3 to arg6 does not require any quotes.**

### Steps to take
**NOTE: Does not necessarily follow in that order.**
1. Download alugi's quickbms tool and xbe_dump.bms files. Run it to extract all sections into raw section files in a separate folder.
2. Next, you will need to dump xbe information from Cxbx(-R)'s, tool in order to obtain `Virtual Address` and `Size of Raw` values from specific section.
3. Run the title in Cxbx(-R) to generate HLE Cache file.
4. Run Cheat Engine tool and open any raw section file you chose for Virtual Address and Size of Raw values to be use.
   1. Click on `Memory View` button.
   2. Click on `File`-->`Save disassemble output"` from dropdown menu.
   3. `From` input should be `0` and `To` input should be exactly the same as `Size of Raw` value.
   4. Then click "Save" (Please make sure you save it with an extension at the end.)
5. Now you can use CHPATRI tool, see "How to Use it" instruction above.
6. Finally, open generated section file and start reverse engineer of what's missing and/or verify if location is correct.
