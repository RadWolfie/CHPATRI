/*
MIT License

Copyright (c) 2017 CHPATRI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

#include <Windows.h>
#include <WinBase.h>
#include <stdlib.h>
#include <limits.h>
#include <conio.h>
#include <map>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>

#include "third-party\iniFile\iniFile.hpp"

#ifdef _UNICODE
typedef std::wfstream tfstream;
typedef std::wstring tstring;
#define tprintf wprintf
#define tstricmp _wcsicmp
#define tstrtol wcstol
#define tstrlen wcslen
typedef std::wstringstream tstringstream;
#else
typedef std::fstream tfstream;
typedef std::string tstring;
#define tprintf printf
#define tstricmp stricmp
#define tstrtol strtol
#define tstrlen strlen
typedef std::stringstream tstringstream;
#endif

#define readSettings \
    if (!settings.keyValue[_T("arg1")].empty()) { \
        minRange = tstrtol(settings.keyValue[_T("arg1")].c_str(), 0, 16); \
    } \
    if (!settings.keyValue[_T("arg2")].empty()) { \
        maxRange = tstrtol(settings.keyValue[_T("arg2")].c_str(), 0, 16); \
    } \
    if (!settings.keyValue[_T("arg3")].empty()) { \
        hleCacheIn = settings.keyValue[_T("arg3")]; \
    } \
    if (!settings.keyValue[_T("arg4")].empty()) { \
        libraryIn = settings.keyValue[_T("arg4")]; \
    } \
    if (!settings.keyValue[_T("arg5")].empty()) { \
        hleCacheOut = settings.keyValue[_T("arg5")]; \
    } \
    if (!settings.keyValue[_T("arg6")].empty()) { \
        libraryOut = settings.keyValue[_T("arg6")]; \
    }

int _tmain(int argc, TCHAR** argv) {

    iniFile settings;

    uint32_t minRange;
    uint32_t maxRange;
    tstring  hleCacheIn;
    tstring  hleCacheOut;
    tstring  libraryIn;
    tstring  libraryOut;
    tstring  hleCacheLog;

    std::map<tstring, tstring>  symbolAddresses;
    std::map<tstring, uint32_t> usedAddresses;
    std::vector<tstring>        detectAddresses;
    std::vector<tstring>::iterator  iDetectAddress;

    TCHAR buffer[SHRT_MAX] = { 0 };
    TCHAR* bufferPtr = buffer;

    DWORD dwRet;

    tfstream fileBuffer;

    std::vector<tstring> inFileLineArray;
    tstring line;

    std::string tempStr;
    std::size_t found;
    std::size_t found2;
    bool addNewLine = true;

    // Output help info.
    if (argc == 2 && tstricmp(argv[1], _T("-h")) == 0) {
        tprintf(_T("INFO: Arg 1 - Virtual Address\n"));
        tprintf(_T("INFO: Arg 2 - Size of Raw\n"));
        tprintf(_T("INFO: Arg 3 - HLE Cache ini file (Input)\n"));
        tprintf(_T("INFO: Arg 4 - Library section txt file (Input)\n"));
        tprintf(_T("INFO: Arg 5 - HLE Cache ini file (Output, optional)\n"));
        tprintf(_T("INFO: Arg 6 - Library section txt file (Output, optional)\n"));
        return 0;
    }

    // Read global ini file.
    if (settings.m_open_file(_T("CHPATRI.ini"))) {

        tprintf(_T("INFO: Reading CHPATRI.ini file.\n"));
        readSettings;

        settings.m_close();
        settings.m_clear();
    } else {
        tprintf(_T("INFO: CHPATRI.ini file does not exist, skipping.\n"));
    }

    // Check arguments count, then determine best place to start.
    switch (argc) {
        // TODO: Need to add support for "interactive" mode.
        case 1: {
            // Don't need to check maxrange here.
            if (hleCacheIn.empty() && libraryIn.empty()) {
                tprintf(_T("INFO: Interactive mode is not supportive at the moment.\n"));
                return 0;
            }
            break;
        }
        // Either output help info or read individual ini file by user's drag and drop.
        case 2: {
            // Read individual ini file.
            if (settings.m_open_file(argv[1])) {

                tprintf(_T("INFO: Reading %s file.\n"), argv[1]);
                readSettings;

                settings.m_close();
                settings.m_clear();
            } else {
                tprintf(_T("ERROR: %s file does not exist.\n"), argv[1]);
                return 0;
            }
            break;
        }
        case 6: {
            libraryOut = argv[6];
        }
        case 5: {
            hleCacheOut = argv[5];
            break;
        }
        default: {
            if (argc < 5 || argc > 7) {
                tprintf(_T("ERROR: Please input within range of 4 to 6 arguments requirement...\n"));
                return 0;
            } 
        }
    }

    if (maxRange == NULL || hleCacheIn.empty() || libraryIn.empty()) {
        tprintf(_T("ERROR: Please input valid arguments...\n"));
        return 0;
    }

    // Use same path as input.
    if (hleCacheOut.empty()) {
        hleCacheOut = hleCacheIn.c_str();
        hleCacheOut.append(_T(".txt"));

    }
    if (libraryOut.empty()) {
        libraryOut = libraryIn.c_str();
        libraryOut.append(_T(".asm"));
    }

    hleCacheLog = hleCacheOut.substr(0, hleCacheOut.length()-4) + _T(".log");

    tprintf(_T("Arg 1: %8X\nArg 2: %8X\nArg 3: %s\nArg 4: %s\nArg 5: %s\nArg 6: %s\n\n"),
            minRange, maxRange, hleCacheIn.c_str(), libraryIn.c_str(), hleCacheOut.c_str(), libraryOut.c_str());

    maxRange += minRange;

    dwRet = GetPrivateProfileSection(_T("Symbols"), buffer, sizeof(buffer), hleCacheIn.c_str());
    if (dwRet == SHRT_MAX - 2 || dwRet == 0) {
        printf("ERROR: Unable to read HLE Cache file...\n");
        return 0;
    }

    // Parse the .INI file into the map of symbol addresses
    while (tstrlen(bufferPtr) > 0) {
        tstring ini_entry(bufferPtr);

        auto separator = ini_entry.find('=');
        tstring key = ini_entry.substr(0, separator);
        tstring value = ini_entry.substr(separator + 1, std::string::npos);
        uint32_t addr = tstrtol(value.c_str(), 0, 16);

        // Register only specific address range.
        if (minRange <= addr && maxRange >= addr) {
            addr -= minRange;
            tstringstream cacheAddress;
            cacheAddress << std::uppercase << std::hex << std::setw(8) << std::setfill(_T('0')) << addr;

            symbolAddresses[key] = cacheAddress.str();
        }


        bufferPtr += tstrlen(bufferPtr) + 1;
    }

    fileBuffer.open(hleCacheOut.c_str(), tfstream::out);

    // Write the found symbol addresses into the cache file
    for (auto it = symbolAddresses.begin(); it != symbolAddresses.end(); ++it) {
        fileBuffer << (*it).first.c_str() << "=" << (*it).second.c_str() << std::endl;
    }

    fileBuffer.close();

    fileBuffer.open(libraryIn.c_str(), tfstream::in);

    if (!fileBuffer.is_open()) {
        printf("ERROR: Unable to open disassembly Library file...\n");
        return 0;
    }
    printf("Reading disassembly library file...\n");

    // This is better to put them into "newline" array for add/remove stuff.
    while (std::getline(fileBuffer, line)) {
        inFileLineArray.push_back(line);
    }
    fileBuffer.close();

    if (inFileLineArray.empty()) {
        printf("ERROR: Unable to read disassembly Library file...\n");
        return 0;
    }

    printf("Editing and saving disassembly library, please wait...\n");

    // Save the finalize edited to the file.
    fileBuffer.open(libraryOut.c_str(), tfstream::out);

    if (!fileBuffer.is_open()) {
        printf("ERROR: Unable to create disassembly Library file...\n");
        return 0;
    }

    // Now start editing each line.
    for (std::vector<tstring>::iterator nLine = inFileLineArray.begin(); nLine != inFileLineArray.end(); ++nLine) {

        //Clean up crew
        found = nLine->find(_T(" { "));
        if (found != tstring::npos) {
            if (nLine->at(found + 3) != _T('"')) {
                *nLine = nLine->substr(0, found);
            }
        }


        // Check for ret
        found = nLine->find(_T(" - ret "));
        if (found != tstring::npos) {
            fileBuffer << nLine->c_str() << std::endl;
            fileBuffer << std::endl;
            addNewLine = false;
            continue;
        }

        // Then start searching for address matching.
        for (std::map<tstring, tstring>::iterator it = symbolAddresses.begin(); it != symbolAddresses.end(); ++it) {
            found = nLine->find(it->second);
            if (found != tstring::npos) {
                if (found == 0) {
                    if (addNewLine) {
                        fileBuffer << std::endl;
                    }
                    fileBuffer << it->first << std::endl;
                    detectAddresses.push_back(it->first);
                    continue;
                }
                found2 = nLine->find(_T(" call "));
                if (found2 != std::string::npos) {
                    if (nLine->at(found - 1) == _T(' ')) {
                        nLine->append(_T(" ("));
                        nLine->append(it->first);
                        nLine->append(_T(")"));
                        usedAddresses[it->first] += 1;
                    }
                    continue;
                } 
                found2 = nLine->find(_T(" jmp "));
                if (found2 != std::string::npos) {
                    if (nLine->at(found - 1) == _T(' ')) {
                        nLine->append(_T(" ("));
                        nLine->append(it->first);
                        nLine->append(_T(")"));
                        usedAddresses[it->first] += 1;
                    }
                    continue;
                }
                // Add more asm op code finding here
            }
        }
        addNewLine = true;
        fileBuffer << nLine->c_str() << std::endl;
    }
    fileBuffer.close();

    tprintf(_T("Saving log file, please wait...\n"));

    // Generate a log report.
    fileBuffer.open(hleCacheLog.c_str(), tfstream::out);

    if (!fileBuffer.is_open()) {
        printf("ERROR: Unable to create log file...\n");
        return 0;
    }

    std::sort (detectAddresses.begin(), detectAddresses.end());
    iDetectAddress = detectAddresses.begin();

    // Report error for not found detect addresses.
    for (std::map<tstring, tstring>::iterator iSymbolAddress = symbolAddresses.begin(); iSymbolAddress != symbolAddresses.end(); iSymbolAddress++) {
        if (iDetectAddress == detectAddresses.end()) {
            fileBuffer << _T("ERROR: Not Found ") << iSymbolAddress->first.c_str() << std::endl;
            continue;
        }
        if (iSymbolAddress->first.compare(*iDetectAddress) == 0) {
            iDetectAddress = detectAddresses.erase(iDetectAddress);
        } else {
            fileBuffer << _T("ERROR: Not Found ") << iSymbolAddress->first.c_str() << std::endl;
        }
    }

    // Report count of used address
    for (std::map<tstring, uint32_t>::iterator iUsedAddress = usedAddresses.begin(); iUsedAddress != usedAddresses.end(); iUsedAddress++) {
        fileBuffer << _T("INFO: ") << std::setw(4) << iUsedAddress->second << _T("x ") << iUsedAddress->first.c_str() << std::endl;
    }
    fileBuffer.close();

    tprintf(_T("Completed!\n\nPress any key to exit.\n"));
    _getch();
    return 0;
}
