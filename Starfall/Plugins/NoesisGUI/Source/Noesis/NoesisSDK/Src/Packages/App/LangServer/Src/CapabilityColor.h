////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_CAPABILITYCOLOR_H
#define __APP_CAPABILITYCOLOR_H


namespace Noesis
{
class BaseString;
struct DocumentContainer;
struct TextPosition;

class CapabilityColor final
{
public:
    static void DocumentColorRequest(int bodyId, DocumentContainer& document,
        BaseString& responseBuffer);
    static void ColorPresentationRequest(int bodyId, float red, float green, float blue,
        float alpha, BaseString& responseBuffer);
private:
    CapabilityColor() = default;
};
}

#endif