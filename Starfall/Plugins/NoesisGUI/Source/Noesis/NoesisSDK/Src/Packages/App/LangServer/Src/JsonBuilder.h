////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_JSONBUILDER_H
#define __APP_JSONBUILDER_H


#include <NsCore/String.h>
#include <NsCore/StringFwd.h>


namespace Noesis
{
class JsonBuilder final
{
public:
    JsonBuilder() = default;
    void StartObject();
    void EndObject();
    void StartArray();
    void EndArray();
    void WritePropertyName(const char* propertyName);
    void WriteValue(const char* value);
    void WriteValue(bool value);
    void WriteValue(uint32_t value);
    void WriteValue(int32_t value);
    void WriteValue(double value);
    void WriteValue(float value);
    void WriteRawValue(const char* value);
    uint32_t Size() const;
    const char* Str() const;
private:    
    String mJson;
};
}

#endif