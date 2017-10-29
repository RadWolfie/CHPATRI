/*
MIT License

Copyright (c) 2017 iniFile

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

#pragma once

#include <tchar.h>
#include <windef.h>
#include <stdlib.h>
#include <fstream>
#include <string>

typedef enum commentChar {
    pound = _T('#'),
    semiColon = _T(';')
} commentChar;

class iniFile {

#ifdef _UNICODE
typedef std::wfstream tfstream;
typedef std::wstring tstring;
#define tprintf wprintf
#else
typedef std::fstream tfstream;
typedef std::string tstring;
#define tprintf printf
#endif

    private:
        tstring fileName;
        tfstream fileStream;
    public:
        std::map<tstring, tstring> keyValue;
        /// <summary>
        /// To initialize iniFile.
        /// </summary>
        iniFile(void) {
        };
        /// <summary>
        /// To release iniFile, cannot be re-used after release.
        /// </summary>
        ~iniFile(void) {
        };
        /// <summary>
        /// To close a file.
        /// </summary>
        /// <returns>Does not return a value.</returns>
        void m_close(void) {
            fileStream.close();
        };
        /// <summary>
        /// To open a file.
        /// </summary>
        /// <param name="fileName">Name of a file to open.</param>
        /// <returns>Return true or false.</returns>
        bool m_open_file(const TCHAR* newFileName) {

            if (newFileName != nullptr) {
                fileName = newFileName;
            }

            if (fileStream.is_open()) {
                fileStream.close();
                keyValue.clear();
            }

            fileStream.open(fileName, tfstream::in);

            if (!fileStream.is_open()) {
                tprintf(_T("ERROR: Unable to open %s file...\n"), fileName.c_str());
                return false;
            }

            tstring lineStr;
            while (std::getline(fileStream, lineStr)) {
                size_t separator = lineStr.find(_T('='));
                if (separator == tstring::npos) {
                    continue;
                }

                tstring key = lineStr.substr(0, separator);
                tstring value = lineStr.substr(separator + 1, tstring::npos);
                keyValue[key] = value;
            }
            return true;
        };
        /// <summary>
        /// To create a file.
        /// </summary>
        /// <param name="fileName">Name of a file to create.</param>
        /// <returns>Return true or false.</returns>
        bool m_create_file(const TCHAR* newFileName) {

            if (newFileName != nullptr) {
                fileName = newFileName;
            }

            if (fileStream.is_open()) {
                fileStream.close();
                keyValue.clear();
            }

            fileStream.open(fileName, tfstream::in);

            if (!fileStream.is_open()) {
                tprintf(_T("ERROR: Unable to create %s file...\n"), fileName.c_str());
                return false;
            }
            return true;
        };
        /// <summary>
        /// To save data buffer into file content.
        /// </summary>
        /// <returns>Return true or false.</returns>
        bool m_save(void) {
            if (!fileStream.is_open()) {
                return 0;
            }
            fileStream.close();
            fileStream.open(fileName, tfstream::out | tfstream::trunc);

            for (auto it = keyValue.begin(); it != keyValue.end(); ++it) {
                fileStream << it->first << _T("=") << it->second;
            }
            fileStream.close();
            fileStream.open(fileName, tfstream::in);
            return true;
        };
        /// <summary>
        /// To clear the content and data buffer. (NOTICE: If you want to clear everything in a file, you must save it after call m_clear.)
        /// </summary>
        /// <returns>Does not return a value.</returns>
        void m_clear(void) {
            keyValue.clear();
        };
};
