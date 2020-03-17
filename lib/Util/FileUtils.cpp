/*
Modified BSD License

This file originates from:
http://sf.net/projects/enjomitchsorbit

Copyright (c) 2012, Szymon "Enjo" Ender
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the <organization> nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>

#include "FileUtils.hpp"
#include "CharManipulations.hpp"
using namespace EnjoLib;

std::vector<std::vector<std::string> > FileUtils::GetConfigSections( const std::string & fileName,
        const std::string & startMarker,
        const std::string & endMarker ) const
{
    std::vector<std::vector<std::string> > sections;
    std::ifstream file(fileName.c_str());
    if ( !file.is_open() )
        return sections;

    std::string line;
    while ( std::getline(file, line) )
    {
        if (CharManipulations().Trim(line) == startMarker)
        {
            std::vector<std::string> section = GetOneSection( file, endMarker );
            sections.push_back(section);
        }
    }
    return sections;
}

std::vector<std::string> FileUtils::GetOneSection( std::istream & file, const std::string & endMarker ) const
{
    std::vector<std::string> section;
    std::string line;
    while ( std::getline(file, line) && CharManipulations().Trim(line) != endMarker )
        section.push_back(line);
    return section;
}

bool FileUtils::FileExists( const std::string & fileName ) const
{
    std::ifstream f(fileName.c_str());
        return f.is_open();
}

bool FileUtils::DirExists( const std::string & dirName ) const
{
    struct stat info;

    if( stat( dirName.c_str(), &info ) != 0 )
        //printf( "cannot access %s\n", pathname );
        return false;
    else if( info.st_mode & S_IFDIR )  // S_ISDIR() doesn't exist on my windows
        //printf( "%s is a directory\n", pathname );
        return true;
    else
        //printf( "%s is no directory\n", pathname );
        return false;
}

void FileUtils::CreateDirIfNotExistsLinux(const std::string & dirName) const
{
    if (DirExists(dirName))
        return;
    const std::string command = "mkdir -p " + dirName;
    if (int err = system(command.c_str()))
    {
        std::cout << "FileUtils::CreateDirIfNotExistsLinux(): err = " << err << std::endl;
    }
}

std::string FileUtils::GetBaseDir(const std::string & fullPath) const
{
    std::size_t found = fullPath.find_last_of("/\\");
    if (found == std::string::npos)
    {
        return "";
    }
    const std::string & baseDir = fullPath.substr(0,found);
    return baseDir;
}

size_t FileUtils::GetNumLinesFile( const std::string & fileName, bool skipHeader ) const
{
    std::ifstream is(fileName.c_str());
    if (!is.is_open())
        throw std::runtime_error("File '" + fileName + "' doesn't exist");
    return GetNumLinesFile(is, skipHeader);
}

size_t FileUtils::GetNumLinesFile( std::istream & is, bool skipHeader ) const
{
    size_t size = 0;
    std::string line;
    if (skipHeader)
        std::getline(is, line);
    while ( std::getline(is, line) )
        ++size;
    return size;
}
