/*     Copyright 2015-2018 Egor Yusov
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF ANY PROPRIETARY RIGHTS.
 *
 *  In no event and under no legal theory, whether in tort (including negligence), 
 *  contract, or otherwise, unless required by applicable law (such as deliberate 
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental, 
 *  or consequential damages of any character arising as a result of this License or 
 *  out of the use or inability to use the software (including but not limited to damages 
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and 
 *  all other commercial damages or losses), even if such Contributor has been advised 
 *  of the possibility of such damages.
 */

#include <stdio.h>
#include <unistd.h>
#include <cstdio>
#include <CoreFoundation/CoreFoundation.h>

#include "CFObjectWrapper.h"

#include "MacOSFileSystem.h"
#include "Errors.h"
#include "DebugUtilities.h"

namespace
{
    std::string FindResource(std::string FilePath)
    {
        std::string dir, name;
        BasicFileSystem::SplitFilePath(FilePath, &dir, &name);
        auto dotPos = name.find(".");
        std::string type = (dotPos != std::string::npos) ? name.substr(dotPos+1) : "";
        if(dotPos != std::string::npos)
            name.erase(dotPos);

        // get bundle and CFStrings
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        CFStringWrapper cf_resource_path = CFStringCreateWithCString(NULL, dir.c_str(), kCFStringEncodingUTF8);
        CFStringWrapper cf_filename = CFStringCreateWithCString(NULL, name.c_str(), kCFStringEncodingUTF8);
        CFStringWrapper cf_file_type = CFStringCreateWithCString(NULL, type.c_str(), kCFStringEncodingUTF8);
        CFURLWrapper cf_url_resource = CFBundleCopyResourceURL(mainBundle, cf_filename, cf_file_type, cf_resource_path);
        std::string resource_path;
        if(cf_url_resource != NULL)
        {
            CFStringWrapper cf_url_string = CFURLCopyFileSystemPath(cf_url_resource, kCFURLPOSIXPathStyle);
            const char* url_string = CFStringGetCStringPtr(cf_url_string, kCFStringEncodingUTF8);
            resource_path = url_string;
        }
        return resource_path;
    }
}
MacOSFile* MacOSFileSystem::OpenFile( const FileOpenAttribs &OpenAttribs )
{
    // Try to find the file in the bundle first
    auto resource_path = FindResource(OpenAttribs.strFilePath);
    MacOSFile *pFile = nullptr;
    if(!resource_path.empty())
    {
        try
        {
            FileOpenAttribs BundleResourceOpenAttribs = OpenAttribs;
            BundleResourceOpenAttribs.strFilePath = resource_path.c_str();
            pFile = new MacOSFile(BundleResourceOpenAttribs, MacOSFileSystem::GetSlashSymbol());
        }
        catch( const std::runtime_error &err )
        {
        }
    }

    if(pFile == nullptr)
    {
        try
        {
            pFile = new MacOSFile(OpenAttribs, MacOSFileSystem::GetSlashSymbol());
        }
        catch( const std::runtime_error &err )
        {
        }
    }
    return pFile;
}


bool MacOSFileSystem::FileExists( const Diligent::Char *strFilePath )
{
    if(!FindResource(strFilePath).empty())
        return true;

    auto res = access(strFilePath, F_OK);
    return res == 0;
}

bool MacOSFileSystem::PathExists( const Diligent::Char *strPath )
{
    UNSUPPORTED( "Not implemented" );
    return false;
}
    
bool MacOSFileSystem::CreateDirectory( const Diligent::Char *strPath )
{
    UNSUPPORTED( "Not implemented" );
    return false;
}

void MacOSFileSystem::ClearDirectory( const Diligent::Char *strPath )
{
    UNSUPPORTED( "Not implemented" );
}

void MacOSFileSystem::DeleteFile( const Diligent::Char *strPath )
{
    remove(strPath);
}
    
std::vector<std::unique_ptr<FindFileData>> MacOSFileSystem::Search(const Diligent::Char *SearchPattern)
{
    UNSUPPORTED( "Not implemented" );
    return std::vector<std::unique_ptr<FindFileData>>();
}