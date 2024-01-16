////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_CAPABILITYCOMPLETION_H
#define __APP_CAPABILITYCOMPLETION_H


namespace Noesis
{
class BaseString;
struct DocumentContainer;
struct TextPosition;

class CapabilityCompletion final
{
public:
    static void CompletionRequest(int bodyId, DocumentContainer& document,
        const TextPosition& position, BaseString& responseBuffer);
    static void AutoInsertCloseRequest(int bodyId, DocumentContainer& document,
                                       const TextPosition& position, BaseString& responseBuffer);
    static void AutoInsertQuotesRequest(int bodyId, DocumentContainer& document,
        const TextPosition& position, BaseString& responseBuffer);
private:
    CapabilityCompletion() = default;
};
}

#endif