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
#include <map>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <vector>


int main(int argc, char** argv) {

    if (argc == 1) {
        printf("INFO: Arg 1 - Virtual Address\n");
        printf("INFO: Arg 2 - Size of Raw\n");
        printf("INFO: Arg 3 - HLE Cache ini file\n");
        printf("INFO: Arg 4 - Library section txt file\n");
        return 0;
    } else if (argc != 5) {
        printf("ERROR: Please input exact arguments...\n");
        return 0;
    } else if (argv[1][0] == NULL || argv[2][0] == NULL || argv[3][0] == NULL || argv[4][0] == NULL) {
        printf("ERROR: Please input valid arguments...\n");
        return 0;
    }

    printf("Verbose test: %s %s %s %s\n", argv[1], argv[2], argv[3], argv[4]);

    uint32_t minRange = strtol(argv[1], 0, 16);
    uint32_t maxRange = strtol(argv[2], 0, 16);
    maxRange += minRange;

    std::map<std::string, std::string> g_SymbolAddresses;
    char buffer[SHRT_MAX] = { 0 };
    char* bufferPtr = buffer;

    DWORD dwRet = GetPrivateProfileSectionA("Symbols", buffer, sizeof(buffer), argv[3]);
    if (dwRet == SHRT_MAX - 2 || dwRet == 0) {
        printf("ERROR: Unable to read HLE Cache file...\n");
        return 0;
    }

    // Parse the .INI file into the map of symbol addresses
    while (strlen(bufferPtr) > 0) {
        std::string ini_entry(bufferPtr);

        auto separator = ini_entry.find('=');
        std::string key = ini_entry.substr(0, separator);
        std::string value = ini_entry.substr(separator + 1, std::string::npos);
        uint32_t addr = strtol(value.c_str(), 0, 16);

        // Register only specific address range.
        if (minRange <= addr && maxRange >= addr) {
            addr -= minRange;
            std::stringstream cacheAddress;
            cacheAddress << std::uppercase << std::hex << std::setw(8) << std::setfill('0') << addr;

            g_SymbolAddresses[key] = cacheAddress.str();
        }


        bufferPtr += strlen(bufferPtr) + 1;
    }
    dwRet = strlen(argv[3]);
    argv[3][dwRet - 1] = 't';
    argv[3][dwRet - 2] = 'x';
    argv[3][dwRet - 3] = 't';

    // Write the found symbol addresses into the cache file
    for (auto it = g_SymbolAddresses.begin(); it != g_SymbolAddresses.end(); ++it) {
        WritePrivateProfileStringA("Symbols", (*it).first.c_str(), (*it).second.c_str(), argv[3]);
    }

    std::vector<std::string> inFileLineArray;
    std::string line;

    std::ifstream inFile(argv[4]);

    if (!inFile.is_open()) {
        printf("ERROR: Unable to open disassembly Library file...\n");
        return 0;
    }
    printf("Reading disassembly library file...\n");

    // This is better to put them into "newline" array for add/remove stuff.
    while (std::getline(inFile, line)) {
        inFileLineArray.push_back(line);
    }
    inFile.close();

    if (inFileLineArray.empty()) {
        printf("ERROR: Unable to read disassembly Library file...\n");
        return 0;
    }

    printf("Editing and saving disassembly library, please wait...\n");

    dwRet = strlen(argv[4]);
    argv[4][dwRet - 3] = 'a';
    argv[4][dwRet - 2] = 's';
    argv[4][dwRet - 1] = 'm';

    // Save the finalize edited to the file.
    std::ofstream outFile(argv[4]);
    
    if (!outFile.is_open()) {
        printf("ERROR: Unable to create disassembly Library file...\n");
        return 0;
    }

    std::string tempStr;
    std::size_t found;
    std::size_t found2;
    bool addNewLine = true;

    // Now start editing each line.
    for (std::vector<std::string>::iterator nLine = inFileLineArray.begin(); nLine != inFileLineArray.end(); ++nLine) {

        //Clean up crew
        found = nLine->find(" { ");
        if (found != std::string::npos) {
            if (nLine->at(found + 3) != '"') {
                *nLine = nLine->substr(0, found);
            }
        }


        // Check for ret
        found = nLine->find(" - ret ");
        if (found != std::string::npos) {
            outFile << nLine->c_str() << std::endl;
            outFile << "" << std::endl;
            addNewLine = false;
            continue;
        }

        // Then start searching for address matching.
        for (std::map<std::string, std::string>::iterator it = g_SymbolAddresses.begin(); it != g_SymbolAddresses.end(); ++it) {
            found = nLine->find(it->second);
            if (found != std::string::npos) {
                if (found == 0) {
                    if (addNewLine) {
                        outFile << "" << std::endl;
                    }
                    outFile << it->first << std::endl;
                } else {
                    found2 = nLine->find(" call ");
                    if (found2 != std::string::npos) {
                        if (nLine->at(found - 1) == ' ') {
                            nLine->append(" (");
                            nLine->append(it->first);
                            nLine->append(")");
                        }
                    } else {
                        found2 = nLine->find(" jmp ");
                        if (found2 != std::string::npos) {
                            if (nLine->at(found - 1) == ' ') {
                                nLine->append(" (");
                                nLine->append(it->first);
                                nLine->append(")");
                            }
                        }
                    }
                }
            }
        }
        addNewLine = true;
        outFile << nLine->c_str() << std::endl;
    }
    outFile.close();

    printf("Exiting...");
    return 0;
}
