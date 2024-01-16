////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "JsonBuilder.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::StartObject()
{
    if (mJson.Size() > 0 && mJson[mJson.Size() - 1] == '}')
    {
        mJson.PushBack(',');
    }
    mJson.PushBack('{');
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::EndObject()
{
    if (mJson[mJson.Size() - 1] == ',')
    {
        mJson[mJson.Size() - 1] = '}';
    }
    else
    {
        mJson.PushBack('}');
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::StartArray() 
{
    mJson.PushBack('[');
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::EndArray() 
{
    if (mJson[mJson.Size() - 1] == ',')
    {
        mJson[mJson.Size() - 1] = ']';
    }
    else
    {
        mJson.PushBack(']');
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::WritePropertyName(const char* propertyName) 
{
    if (mJson[mJson.Size() - 1] == '}')
    {
        mJson.PushBack(',');
    }
    mJson.AppendFormat("\"%s\":", propertyName);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::WriteValue(const char* value)
{
    mJson.Append("\"");
    const uint32_t length = static_cast<uint32_t>(strlen(value));
    for (uint32_t i = 0; i < length; i++)
    {
        const char c = *(value + i);
        switch (c)
        {
            // quotation mark
        case '\"':
            mJson.Append("\\\"");
            break;
            // reverse solidus
        case '\\':
            mJson.Append("\\\\");
            break;
            // backspace
        case '\b':
            mJson.Append("\\b");
            break;
            // form feed
        case '\f':
            mJson.Append("\\f");
            break;
            // line feed
        case '\n':
            mJson.Append("\\n");
            break;
            // carriage return
        case '\r':
            mJson.Append("\\r");
            break;
            // tab
        case '\t':
            mJson.Append("\\t");
            break;
        default:
            mJson.PushBack(c);
        }
    }
    mJson.Append("\",");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::WriteValue(bool value)
{
    mJson.Append(value ? "true," : "false,");
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::WriteValue(uint32_t value)
{
    mJson.AppendFormat("%u,", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::WriteValue(int32_t value)
{
    mJson.AppendFormat("%d,", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::WriteValue(double value)
{
    mJson.AppendFormat("%f,", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::WriteValue(float value)
{
    mJson.AppendFormat("%f,", value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::JsonBuilder::WriteRawValue(const char* value) 
{
    mJson.Append(value);
    mJson.PushBack(',');
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t Noesis::JsonBuilder::Size() const
{
    return mJson.Size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const char* Noesis::JsonBuilder::Str() const
{
    return mJson.Str();
}
