////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "DocumentHelper.h"
#include "JsonObject.h"
#include "LenientXamlParser.h"

#include <NsCore/Log.h>
#include <NsCore/UTF8.h>
#include <NsGui/Uri.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/Fonts.h>

#if defined(NS_PLATFORM_WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#elif defined(NS_PLATFORM_OSX) || defined(NS_PLATFORM_LINUX)
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#endif


namespace
{
#if defined(NS_PLATFORM_WINDOWS) || defined(NS_PLATFORM_OSX) || defined(NS_PLATFORM_LINUX)
    struct FontFindData
    {
        char filename[512];
        char extension[16];
        void* handle;
    };
#endif
}

#if defined(NS_PLATFORM_WINDOWS) || defined(NS_PLATFORM_OSX) || defined(NS_PLATFORM_LINUX)
////////////////////////////////////////////////////////////////////////////////////////////////////
static void FontFindClose(FontFindData& findData)
{
#if defined(NS_PLATFORM_WINDOWS)
    int r = ::FindClose(findData.handle);
    NS_ASSERT(r != 0);
#else
    DIR* dir = (DIR*)findData.handle;
    int r = closedir(dir);
    NS_ASSERT(r == 0);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static bool FontFindNext(FontFindData& findData)
{
#if defined(NS_PLATFORM_WINDOWS)
    WIN32_FIND_DATAW fd;
    int res = FindNextFileW(findData.handle, &fd);

    if (res)
    {
        const int MaxFilename = sizeof(findData.filename);
        uint32_t n = Noesis::UTF8::UTF16To8((uint16_t*)fd.cFileName, findData.filename, MaxFilename);
        NS_ASSERT(n <= MaxFilename);
        return true;
    }

    return false;

#else
    DIR* dir = (DIR*)findData.handle;

    while (true)
    {
        dirent* entry = readdir(dir);

        if (entry != 0)
        {
            if (Noesis::StrCaseEndsWith(entry->d_name, findData.extension))
            {
                Noesis::StrCopy(findData.filename, sizeof(findData.filename), entry->d_name);
                return true;
            }
        }
        else
        {
            return false;
        }
    }
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static bool FontFindFirst(const char* directory, const char* extension, FontFindData& findData)
{
#if defined(NS_PLATFORM_WINDOWS)
    char fullPath[sizeof(findData.filename)];
    Noesis::StrCopy(fullPath, sizeof(fullPath), directory);
    Noesis::StrAppend(fullPath, sizeof(fullPath), "/*");
    Noesis::StrAppend(fullPath, sizeof(fullPath), extension);

    uint16_t u16str[sizeof(fullPath)];
    uint32_t numChars = Noesis::UTF8::UTF8To16(fullPath, u16str, sizeof(fullPath));
    NS_ASSERT(numChars <= sizeof(fullPath));

    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileExW((LPCWSTR)u16str, FindExInfoBasic, &fd, FindExSearchNameMatch, 0, 0);
    if (h != INVALID_HANDLE_VALUE)
    {
        numChars = Noesis::UTF8::UTF16To8((uint16_t*)fd.cFileName, findData.filename, sizeof(fullPath));
        NS_ASSERT(numChars <= sizeof(fullPath));
        Noesis::StrCopy(findData.extension, sizeof(findData.extension), extension);
        findData.handle = h;
        return true;
    }

    return false;

#else
    DIR* dir = opendir(directory);

    if (dir != 0)
    {
        Noesis::StrCopy(findData.extension, sizeof(findData.extension), extension);
        findData.handle = dir;

        if (FontFindNext(findData))
        {
            return true;
        }
        else
        {
            FontFindClose(findData);
            return false;
        }
    }

    return false;
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static bool ScanFolderForFamily(const Noesis::Uri& folder, const char* familyName, const char* ext)
{
    FontFindData findData{};

    Noesis::FixedString<512> path;
    folder.GetPath(path);

    if (!Noesis::StrEndsWith(path.Str(), "/"))
    {
        path.PushBack('/');
    }

    bool found = false;
    if (FontFindFirst(path.Str(), ext, findData))
    {
        do
        {
            Noesis::Ptr<Noesis::Stream> stream = Noesis::OpenFileStream(
                Noesis::FixedString<256>(Noesis::FixedString<256>::VarArgs(),
                    "%s%s", path.Str(), findData.filename).Str());

            if (stream != 0)
            {
                Noesis::Fonts::GetTypefaces(stream, [&](const Noesis::Typeface& typeface)
                {
                    if (Noesis::StrCaseStartsWith(familyName, typeface.familyName))
                    {
                        found = true;
                    }
                });

                stream->Close();
            }
        } while (!found && FontFindNext(findData));

        FontFindClose(findData);
    }
    return found;
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
bool Noesis::DocumentHelper::FolderHasFontFamily(const Uri& folder, const char* familyName)
{
#if defined(NS_PLATFORM_WINDOWS) || defined(NS_PLATFORM_OSX) || defined(NS_PLATFORM_LINUX)
    if (ScanFolderForFamily(folder, familyName, ".ttf"))
    {
        return true;
    }
    if (ScanFolderForFamily(folder, familyName, ".otf"))
    {
        return true;
    }
    if (ScanFolderForFamily(folder, familyName, ".ttc"))
    {
        return true;
    }
#endif
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::DocumentHelper::UriDecode(const char* value, Uri& output)
{
    FixedString<256> uri;
    uint32_t i = 0;
    bool isUntitled = false;
    if (StrStartsWith(value, "untitled:"))
    {
        uri.Append("lsfile:///<untitled>/");
        i = 9;
        isUntitled = true;
    }
    else if (StrStartsWith(value, "file://"))
    {
        uri.Append("lsfile://");
        i = 7;
    }
    else
    {
        NS_LOG_WARNING("Uri does not match expected pattern.");
    }

    const uint32_t length = (uint32_t)strlen(value);

    for (; i < length; ++i)
    {
        const char ch = value[i];

        if (isUntitled && ch == ':')
        {
            uri.PushBack('_');
        }
        else if (ch == '%' && (i + 2) < length)
        {
            String hex(value + i + 1, 2);
            const char dec = static_cast<char>(strtol(hex.Str(), nullptr, 16));
            uri.PushBack(dec);
            i += 2;
        }
        else if (ch == '+')
        {
            uri.PushBack(' ');
        }
        else
        {
            uri.PushBack(ch);
        }
    }

    output = Uri(uri.Str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::DocumentHelper::GenerateLineNumbers(DocumentContainer& document)
{
    document.lineStartPositions.Clear();
    document.lineStartPositions.PushBack(0);
    int lineStartPosition = 0;
    int lineBreakPosition;    
    while ((lineBreakPosition = document.text.Find("\r\n", lineStartPosition)) != -1)
    {
        lineStartPosition = lineBreakPosition + 2;
        document.lineStartPositions.PushBack(lineStartPosition);
    }
    if (document.lineStartPositions.Size() == 1)
    {
        while ((lineBreakPosition = document.text.Find("\n", lineStartPosition)) != -1)
        {
            lineStartPosition = lineBreakPosition + 1;
            document.lineStartPositions.PushBack(lineStartPosition);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::TextRange Noesis::DocumentHelper::RangeFromJson(DocumentContainer& document, JsonObject& rangeJson)
{
    JsonObject start = rangeJson.GetObjectValue("start");
    JsonObject end = rangeJson.GetObjectValue("end");
    return {
        PositionFromJson(document, start),
        PositionFromJson(document, end)
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::TextPosition Noesis::DocumentHelper::PositionFromJson(DocumentContainer& document, JsonObject& positionJson)
{
    TextPosition position{
        positionJson.GetUInt32Value("line"),
        positionJson.GetUInt32Value("character"),
        0
    };

    if (position.line >= document.lineStartPositions.Size())
    {
        NS_LOG_INFO("Position start line is out of bounds\n");
        return {};
    }
    
    PopulateTextPosition(document, position);

    return position;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::DocumentHelper::PopulateTextPosition(DocumentContainer& document, TextPosition& position)
{
    position.textPosition = document.lineStartPositions[position.line];
    const char* it = document.text.Str() + position.textPosition;
    LenientXamlParser::UTF16Advance(it, position.character);
    while (it != document.text.Str() + position.textPosition)
    {
        position.textPosition++;
    }
}