////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_WORKSPACE_H
#define __APP_WORKSPACE_H


#include <NsCore/String.h>
#include <NsCore/HashMap.h>
#include <NsGui/XamlProvider.h>
#include <NsGui/FontProvider.h>
#include <NsGui/TextureProvider.h>

#include "Document.h"


namespace Noesis
{

class Workspace final
{
public:
    typedef HashMap<String, DocumentContainer> DocumentMap;

    Workspace();

    bool IsInitialized() const;
    void Initialize();

    void Clear();
    
    bool HasDocument(const Uri& uri);
    DocumentContainer& GetDocument(const Uri& uri);
    DocumentMap& GetDocuments();
    bool AddDocument(const Uri& uri, const char* vsCodeUri, const char* text, uint32_t version);
    bool RemoveDocument(const Uri& uri);

private:
    bool mIsInitialized = false;
    DocumentMap mDocuments;
};
}

#endif
