////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "MessageReader.h"
#include "CapabilityDiagnostics.h"
#include "CapabilityCompletion.h"
#include "CapabilityColor.h"
#include "LSPErrorCodes.h"
#include "Workspace.h"
#include "DocumentHelper.h"
#include "JsonObject.h"
#include "MessageWriter.h"

#include <NsCore/Log.h>
#include <NsCore/String.h>
#include <NsGui/Uri.h>
#include <NsDrawing/Color.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
bool HandleTextDocumentMessage(const Noesis::JsonObject& body,
    const Noesis::JsonObject& params, Noesis::Workspace& workspace, double currentTime,
    Noesis::BaseString& responseBuffer)
{
    const char* method = body.GetStrValue("method");
    const Noesis::JsonObject textDocument = params.GetObjectValue("textDocument");
    Noesis::Uri uri;
    Noesis::FixedString<256> vsCodeUri = textDocument.GetStrValue("uri");
    Noesis::DocumentHelper::UriDecode(vsCodeUri.Str(), uri);
    if (Noesis::StrEquals(method, "textDocument/didOpen"))
    {
        const char* text = textDocument.GetStrValue("text");
        if (!workspace.AddDocument(uri, vsCodeUri.Str(), text,
            textDocument.GetUInt32Value("version")))
        {
            NS_LOG_INFO("Method '%s', Uri '%s' is already open.\n", method, uri.Str());
            return true;
        }

        Noesis::DocumentContainer& document = workspace.GetDocument(uri);

        document.isDirty = false;
        document.lastUpdateTime = currentTime;

        Noesis::CapabilityDiagnostics::GetDiagnosticsMessage(nullptr, document, workspace, 0,
            0, 0, responseBuffer);
        return true;
    }
    if (!workspace.HasDocument(uri))
    {
        NS_LOG_INFO("Method '%s', Uri '%s' is not open.\n", method, uri.Str());
        return true;
    }
    if (Noesis::StrEquals(method, "textDocument/didClose"))
    {
        workspace.RemoveDocument(uri);
        return true;
    }
    if (Noesis::StrEquals(method, "textDocument/didChange"))
    {
        Noesis::DocumentContainer& document = workspace.GetDocument(uri);

        document.isDirty = true;
        document.lastUpdateTime = currentTime;

        uint32_t count = params.GetValueCount("contentChanges");
        for (uint32_t i = 0; i < count; i++)
        {
            Noesis::JsonObject contentChangeJson = params.GetObjectValue("contentChanges", i);
            Noesis::JsonObject rangeJson = contentChangeJson.GetObjectValue("range");
            const Noesis::TextRange range =
                Noesis::DocumentHelper::RangeFromJson(document, rangeJson);
            const char* text = contentChangeJson.GetStrValue("text");
            
            if (range.start.textPosition > document.text.Size())
            {
                NS_LOG_INFO("Start position is beyond the end of the document.\n");
            }
            else if (range.start.textPosition == document.text.Size())
            {
                document.text.Append(text);
            }
            else
            {
                // Replace does not support growing size, so we need to Erase and Insert
                document.text.Erase(range.start.textPosition, range.end.textPosition 
                    - range.start.textPosition);
                document.text.Insert(range.start.textPosition, text);
            }

            ++document.version;

            Noesis::DocumentHelper::GenerateLineNumbers(document);
        }
        return true;
    }
    if (Noesis::StrEquals(method, "textDocument/documentColor"))
    {
        Noesis::DocumentContainer& document = workspace.GetDocument(uri);
        Noesis::CapabilityColor::DocumentColorRequest(body.GetInt32Value("id"), document,
            responseBuffer);
        return true;
    }
    if (Noesis::StrEquals(method, "textDocument/colorPresentation"))
    {
        const Noesis::JsonObject colorJson = params.GetObjectValue("color");
        Noesis::CapabilityColor::ColorPresentationRequest(body.GetInt32Value("id"),
            colorJson.GetFloatValue("red"), colorJson.GetFloatValue("green"),
            colorJson.GetFloatValue("blue"), colorJson.GetFloatValue("alpha"),
            responseBuffer);

        return true;
    }
    if (Noesis::StrEquals(method, "textDocument/completion") 
        || Noesis::StrEquals(method, "textDocument/hasCompletion"))
    {
        Noesis::DocumentContainer& document = workspace.GetDocument(uri);
        Noesis::JsonObject positionJson = params.GetObjectValue("position");
        Noesis::TextPosition position =
            Noesis::DocumentHelper::PositionFromJson(document, positionJson);

        Noesis::CapabilityCompletion::CompletionRequest(body.GetInt32Value("id"), document,
            position, responseBuffer);
        return true;
    }

    return Noesis::StrEquals(method, "textDocument/didSave");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::MessageReader::HandleMessage(RenderCallback renderCallback, const JsonObject& body,
    Workspace& workspace, double currentTime, BaseString& responseBuffer)
{
    if (!body.HasProperty("method"))
    {
        MessageWriter::CreateErrorResponse(body.GetInt32Value("id"), 
            LSPErrorCodes::ParseError, "The message does not contain a method.",
            responseBuffer);
        return;
    }

    const char* method = body.GetStrValue("method");

    if (StrEquals(method, "initialized") && !workspace.IsInitialized())
    {
        if (body.HasProperty("id"))
        {
            MessageWriter::CreateErrorResponse(body.GetInt32Value("id"), 
                LSPErrorCodes::ServerNotInitialized,
                "Workspace is not initialized.", responseBuffer);
        }
        else
        {
            NS_LOG_INFO("Received notification, but the workspace is not initialized.");
        }
        return;
    }
    if (StrEquals(method, "initialized"))
    {
        return;
    }

    JsonObject params = body.GetObjectValue("params");

    if (StrStartsWith(method, "textDocument/") && HandleTextDocumentMessage(body, params,
        workspace, currentTime, responseBuffer))
    {
        return;
    }
    if (StrEquals(method, "initialize"))
    {
        workspace.Clear();
        workspace.Initialize();

        JsonBuilder result;
        result.StartObject(); {
            result.WritePropertyName("capabilities");
            result.StartObject(); {
                result.WritePropertyName("textDocumentSync");
                result.WriteValue(2);
                result.WritePropertyName("colorProvider");
                result.WriteValue(true);
                //result.WritePropertyName("linkedEditingRangeProvider");
                //result.WriteValue(true);
                result.WritePropertyName("completionProvider");
                result.StartObject(); {
                    result.WritePropertyName("triggerCharacters");
                    result.WriteValue("<");
                    result.WritePropertyName("completionItem");
                    result.StartObject(); {
                        result.WritePropertyName("labelDetailsSupport");
                        result.WriteValue(true);
                    } result.EndObject();
                } result.EndObject();
            } result.EndObject();
        } result.EndObject();
        
        return MessageWriter::CreateResponse(body.GetInt32Value("id"),
            result.Str(), responseBuffer);
    }    
    if (StrEquals(method, "xaml/runDiagnostics"))
    {
        Workspace::DocumentMap& documents = workspace.GetDocuments();
        for (auto& entry : documents)
        {
            entry.value.isDirty = false;
            CapabilityDiagnostics::GetDiagnosticsMessage(renderCallback, entry.value, workspace,
                params.GetInt32Value("previewRenderWidth"),
                params.GetInt32Value("previewRenderHeight"),
                params.GetDoubleValue("previewRenderTime"), responseBuffer);
        }
        return;
    }
    if (StrEquals(method, "xaml/autoInsert"))
    {
        const JsonObject textDocument = params.GetObjectValue("textDocument");
        Uri uri;
        DocumentHelper::UriDecode(textDocument.GetStrValue("uri"), uri);
        DocumentContainer& document = workspace.GetDocument(uri);
        JsonObject positionJson = params.GetObjectValue("position");
        TextPosition position = DocumentHelper::PositionFromJson(document, positionJson);
        
        const char* insertKind = params.GetStrValue("kind");
        if (StrEquals(insertKind, "autoClose"))
        {
            CapabilityCompletion::AutoInsertCloseRequest(body.GetInt32Value("id"), document,
                position, responseBuffer);
        }
        else if(StrEquals(insertKind, "autoQuote"))
        {
            CapabilityCompletion::AutoInsertQuotesRequest(body.GetInt32Value("id"), document,
                position, responseBuffer);
        }
        else
        {
            MessageWriter::CreateErrorResponse(body.GetInt32Value("id"),
                LSPErrorCodes::RequestFailed, "RequestCancelled: Insert kind not supported.",
                responseBuffer);
        }
        return;
    }
    if (StrEquals(method, "completionItem/resolve"))
    {
        const char* label = params.GetStrValue("label");
        
        JsonBuilder result;
        result.StartObject();
        result.WritePropertyName("label");
        result.WriteValue(label);
        result.EndObject();

        MessageWriter::CreateResponse(body.GetInt32Value("id"), result.Str(), responseBuffer);
        return;
    }
    
    if (body.HasProperty("id"))
    {
        MessageWriter::CreateErrorResponse(body.GetInt32Value("id"), LSPErrorCodes::RequestFailed,
            String(String::VarArgs(), "Method '%s' is not supported.", 
            method).Str(), responseBuffer);
    }
    
}
