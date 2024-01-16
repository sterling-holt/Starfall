////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "CapabilityDiagnostics.h"
#include "JsonObject.h"
#include "LenientXamlParser.h"
#include "MessageWriter.h"
#include "Workspace.h"

#include <NsCore/BaseComponent.h>
#include <NsCore/Log.h>
#include <NsCore/UTF8.h>
#include <NsGui/Uri.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/LangServer.h>

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
////////////////////////////////////////////////////////////////////////////////////////////////////
struct Diagnostic
{
    Noesis::Uri uri;
    bool hasPosition;
    uint32_t lineIndex;
    uint32_t columnIndex;
    Noesis::String message;
    uint32_t severity;

    Diagnostic(const char* uri_, bool hasPosition_, uint32_t lineIndex_, uint32_t columnIndex_,
               const char* message_, uint32_t severity_)
    {
        uri = uri_;
        hasPosition = hasPosition_;
        lineIndex = lineIndex_;
        columnIndex = columnIndex_;
        message = message_;
        severity = severity_;
    }
};

typedef Noesis::HashMap<Noesis::String, Noesis::Vector<Diagnostic, 20>> DiagnosticsMap;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::CapabilityDiagnostics::GetDiagnosticsMessage(RenderCallback renderCallback, 
    DocumentContainer& document, Workspace& workspace, int renderWidth, int renderHeight,
    double renderTime, BaseString& messageBuffer)
{
    DiagnosticsMap diagnosticsMap;

    Vector<Diagnostic, 20> diagnostics;
    diagnosticsMap.Insert(document.uri.Str(), diagnostics);

    ErrorHandlerData errorHandler = SetThreadErrorHandler(&diagnosticsMap, [](const char*, uint32_t,
        const char* message, bool, ErrorContext* context, void* user)
        {
            if (context == nullptr)
            {
                return;
            }

            DiagnosticsMap* diagnosticsMap = static_cast<DiagnosticsMap*>(user);

            const auto it = diagnosticsMap->Find(context->uri);
            if (it == diagnosticsMap->End())
            {
                Vector<Diagnostic, 20> diagnostics;
                diagnostics.EmplaceBack(context->uri, true, context->line - 1, context->column - 1,
                    message, 1);
                diagnosticsMap->Insert(context->uri, diagnostics);
            }
            else
            {
                it->value.EmplaceBack(context->uri, true, context->line - 1, context->column - 1,
                    message, 1);
            }
        });
    
    Ptr<BaseComponent> root = GUI::LoadXaml<BaseComponent>(document.uri);

    SetThreadErrorHandler(errorHandler.user, errorHandler.handler);

    UIElement* content = DynamicCast<UIElement*>(root.GetPtr());
    if (renderCallback != nullptr && content != nullptr)
    {
        FixedString<260> tempPath;
      #ifdef NS_PLATFORM_WINDOWS_DESKTOP
        TCHAR tempPathWBuffer[MAX_PATH];
        GetTempPath(MAX_PATH, tempPathWBuffer);
        char tempPathBuffer[MAX_PATH];

        UTF8::UTF16To8((uint16_t*)tempPathWBuffer, tempPathBuffer, MAX_PATH);

        uint32_t length = static_cast<uint32_t>(strlen(tempPathBuffer));
        for (uint32_t i = 0; i < length; i++)
        {
            if (tempPathBuffer[i] == '\\')
            {
                tempPath.PushBack('/');
            }
            else
            {
                tempPath.PushBack(tempPathBuffer[i]);
            }
        }
        tempPath.Append("noesis/");
        CreateDirectoryA(tempPath.Str(), nullptr);
      #else
        tempPath.Append("/tmp/noesis/");
        mkdir(tempPath.Str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
      #endif

        if (tempPath.Size() > 0)
        {
            FixedString<256> fileName;
            FixedString<256> filePath;
            document.uri.GetPath(filePath);
            for (auto c = filePath.Begin(); c != filePath.End(); c++)
            {
                if (*c == '\\' || *c == '/')
                {
                    if (c != filePath.Begin())
                    {
                        fileName.Append("-_");
                    }
                }
                else if (*c == ':')
                {
                    fileName.PushBack('_');
                }
                else if (*c == '<' || *c == '>')
                {
                    fileName.PushBack('z');
                }
                else
                {
                    fileName.PushBack(*c);
                }
            }
            FixedString<256> previewPath = FixedString<256>(FixedString<256>::VarArgs(),
                "%s%s.png", tempPath.Str(), fileName.Str());

            renderCallback(content, renderWidth, renderHeight, renderTime, previewPath.Str());
        }
    }

    LenientXamlParser::Parts parts;
    LenientXamlParser::LinePartIndicies linePartIndices;
    LenientXamlParser::NSDefinitionMap nsDefinitionMap;
    LenientXamlParser::ParseXaml(document, parts, linePartIndices, nsDefinitionMap);

    auto primaryIt = diagnosticsMap.Find(document.uri.Str());
    // Send errors for non-open documents
    for (const auto& it : diagnosticsMap)
    {
        if (workspace.HasDocument(it.key.Str()))
        {
            continue;
        }        

        JsonBuilder diagnosticsBody;
        diagnosticsBody.StartArray();
        for (const Diagnostic& diagnostic : it.value)
        {
            diagnosticsBody.StartObject();
            diagnosticsBody.WritePropertyName("message");
            diagnosticsBody.WriteValue(diagnostic.message.Str());
            diagnosticsBody.WritePropertyName("severity");
            diagnosticsBody.WriteValue(diagnostic.severity);
            diagnosticsBody.WritePropertyName("range");
            diagnosticsBody.StartObject();
            diagnosticsBody.WritePropertyName("start");
            diagnosticsBody.StartObject();
            diagnosticsBody.WritePropertyName("line");
            diagnosticsBody.WriteValue(0);
            diagnosticsBody.WritePropertyName("character");
            diagnosticsBody.WriteValue(0);
            diagnosticsBody.EndObject();
            diagnosticsBody.WritePropertyName("end");
            diagnosticsBody.StartObject();
            diagnosticsBody.WritePropertyName("line");
            diagnosticsBody.WriteValue(0);
            diagnosticsBody.WritePropertyName("character");
            diagnosticsBody.WriteValue(0);
            diagnosticsBody.EndObject();
            diagnosticsBody.EndObject();
            diagnosticsBody.EndObject();
        }
        diagnosticsBody.EndArray();

        JsonBuilder params;
        params.StartObject();
        params.WritePropertyName("uri");
        params.WriteValue(it.key.Begin() + 2); // + 2 to remove "ls" from the uri
        params.WritePropertyName("diagnostics");
        params.WriteRawValue(diagnosticsBody.Str());
        params.EndObject();

        MessageWriter::CreateNotification("textDocument/publishDiagnostics", params.Str(),
            messageBuffer);
    }

    // Send errors for open documents
    JsonBuilder diagnosticsBody;
    diagnosticsBody.StartArray();
    for (const Diagnostic& diagnostic : primaryIt->value)
    {
        if (diagnostic.lineIndex >= document.lineStartPositions.Size())
        {
            NS_LOG_INFO("Diagnostic message line is outside of document bounds.");
            continue;
        }

        uint32_t startLineIndex = 0;
        uint32_t startCharacterIndex = 0;
        uint32_t endLineIndex = 0;
        uint32_t endCharacterIndex = 0;
        
        const char* text = document.text.Str() + document.lineStartPositions[diagnostic.lineIndex];
        uint32_t characterIndex = LenientXamlParser::UTF8ByteLength(text, diagnostic.columnIndex);
        
        XamlPart xamlPart;

        FindXamlPartResult findResult = LenientXamlParser::FindPartAtPosition(parts,
            linePartIndices, diagnostic.lineIndex, characterIndex, false, xamlPart);

        if (findResult == FindXamlPartResult::None)
        {
            findResult = LenientXamlParser::FindPartAtPosition(parts,
                linePartIndices, diagnostic.lineIndex, characterIndex, true, xamlPart);

            startLineIndex = diagnostic.lineIndex;
            startCharacterIndex = characterIndex;

            if (findResult == FindXamlPartResult::None)
            {
                if (parts.Empty())
                {
                    endLineIndex = document.lineStartPositions.Size() - 1;
                    endCharacterIndex = document.text.Size()
                        - document.lineStartPositions[endLineIndex];
                }
                else
                {
                    const XamlPart& nextPart = parts[0];
                    if (nextPart.startCharacterIndex > 0)
                    {
                        endLineIndex = nextPart.startLineIndex;
                        endCharacterIndex = nextPart.startCharacterIndex - 1;
                    }
                    else
                    {
                        endLineIndex = nextPart.startLineIndex - 1;
                        endCharacterIndex = document.lineStartPositions[nextPart.startLineIndex]
                            - document.lineStartPositions[nextPart.startLineIndex - 1] - 1;
                    }
                }
            }
            else
            {
                if (xamlPart.partIndex == (int32_t)parts.Size() - 1)
                {
                    endLineIndex = document.lineStartPositions.Size() - 1;
                    endCharacterIndex = document.text.Size()
                        - document.lineStartPositions[endLineIndex];
                }
                else
                {
                    const XamlPart& nextPart = parts[xamlPart.partIndex + 1];
                    if (nextPart.startCharacterIndex > 0)
                    {
                        endLineIndex = nextPart.startLineIndex;
                        endCharacterIndex = nextPart.startCharacterIndex - 1;
                    }
                    else
                    {
                        endLineIndex = nextPart.startLineIndex - 1;
                        endCharacterIndex = document.lineStartPositions[nextPart.startLineIndex]
                            - document.lineStartPositions[nextPart.startLineIndex - 1] - 1;
                    }
                }
            }
        }
        else
        {
            startLineIndex = xamlPart.startLineIndex;
            startCharacterIndex = xamlPart.startCharacterIndex;
            endLineIndex = xamlPart.endLineIndex;
            endCharacterIndex = xamlPart.endCharacterIndex;

            if (xamlPart.partKind == XamlPartKind_AttributeKey
                && !xamlPart.HasErrorFlag(ErrorFlags_NoAttributeEquals)
                && !xamlPart.HasErrorFlag(ErrorFlags_NoAttributeValue))
            {
                const XamlPart& valuePart = parts[xamlPart.partIndex + 2];
                endLineIndex = valuePart.endLineIndex;
                endCharacterIndex = valuePart.endCharacterIndex;
            }
        }

        startCharacterIndex = LenientXamlParser::UTF16Length(document.text.Begin()
            + document.lineStartPositions[startLineIndex], document.text.Begin()
            + document.lineStartPositions[startLineIndex] + startCharacterIndex);
        endCharacterIndex = LenientXamlParser::UTF16Length(document.text.Begin()
            + document.lineStartPositions[endLineIndex], document.text.Begin()
            + document.lineStartPositions[endLineIndex] + endCharacterIndex + 1);

        diagnosticsBody.StartObject();
        diagnosticsBody.WritePropertyName("message");
        diagnosticsBody.WriteValue(diagnostic.message.Str());
        diagnosticsBody.WritePropertyName("severity");
        diagnosticsBody.WriteValue(diagnostic.severity);
        diagnosticsBody.WritePropertyName("range");
        diagnosticsBody.StartObject();
        diagnosticsBody.WritePropertyName("start");
        diagnosticsBody.StartObject();
        diagnosticsBody.WritePropertyName("line");
        diagnosticsBody.WriteValue(startLineIndex);
        diagnosticsBody.WritePropertyName("character");
        diagnosticsBody.WriteValue(startCharacterIndex);
        diagnosticsBody.EndObject();
        diagnosticsBody.WritePropertyName("end");
        diagnosticsBody.StartObject();
        diagnosticsBody.WritePropertyName("line");
        diagnosticsBody.WriteValue(endLineIndex);
        diagnosticsBody.WritePropertyName("character");
        diagnosticsBody.WriteValue(endCharacterIndex);
        diagnosticsBody.EndObject();
        diagnosticsBody.EndObject();
        diagnosticsBody.EndObject();
    }

    diagnosticsBody.EndArray();

    JsonBuilder params;
    params.StartObject();
    params.WritePropertyName("uri");
    params.WriteValue(document.vsCodeUri.Str());
    params.WritePropertyName("diagnostics");
    params.WriteRawValue(diagnosticsBody.Str());
    params.EndObject();

    MessageWriter::CreateNotification("textDocument/publishDiagnostics", params.Str(),
        messageBuffer);
}