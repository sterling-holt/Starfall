////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_DOCUMENTHELPER_H
#define __APP_DOCUMENTHELPER_H


#include "Document.h"


namespace Noesis
{
class JsonObject;
struct DocumentContainer;
class Workspace;

class DocumentHelper final
{
public:
    static bool FolderHasFontFamily(const Uri& folder, const char* familyName);
    static void UriDecode(const char* value, Uri& output);
    static void GenerateLineNumbers(DocumentContainer& document);
    static TextRange RangeFromJson(DocumentContainer& document, JsonObject& rangeJson);
    static TextPosition PositionFromJson(DocumentContainer& document, JsonObject& positionJson);
    static void PopulateTextPosition(DocumentContainer& document, TextPosition& position);
private:
    DocumentHelper() = default;
};
}

#endif
