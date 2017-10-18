# CHPATRI
* Convert
* HLE Cache File
* Plus
* Asm Section
* To
* Reverse Engineer
* Informative Asm File

### How to Use it
To use it, you will need to input exact 4 arguments in your command prompt (cmd.exe) in order to process the conversion correctly.

```
CHPATRI.exe <Section's Virtual Address> <Section's Size of Raw> <HLE Cache file name> <Section Asm file>
```

For example with Cel Damage title:
```
CHPATRI.exe 0024CF80 0001FF4C 93bf5881.ini DSOUND.txt
```

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
