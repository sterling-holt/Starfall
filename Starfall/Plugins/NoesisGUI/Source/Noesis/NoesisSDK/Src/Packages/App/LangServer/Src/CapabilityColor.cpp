////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "CapabilityColor.h"
#include "LSPErrorCodes.h"
#include "MessageWriter.h"
#include "LenientXamlParser.h"
#include "LangServerReflectionHelper.h"

#include <NsCore/String.h>
#include <NsGui/UIElement.h>
#include <NsGui/DependencyData.h>
#include <NsGui/Brush.h>
#include <NsGui/ResourceDictionary.h>
#include <NsDrawing/Color.h>


////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::CapabilityColor::ColorPresentationRequest(int bodyId, float red, float green,
    float blue, float alpha, BaseString& responseBuffer)
{
    Color color{ red, green, blue, alpha };

    JsonBuilder result;

    result.StartArray();

    result.StartObject();
    result.WritePropertyName("label");
    result.WriteValue(
        Noesis::FixedString<12>(Noesis::FixedString<12>::VarArgs(), "%s",
            color.ToString().Str()).Str());
    result.EndObject();

    result.EndArray();

    MessageWriter::CreateResponse(bodyId, result.Str(), responseBuffer);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void GenerateColorRangeResult(Noesis::DocumentContainer& document, const Noesis::XamlPart& part,
                              Noesis::JsonBuilder& result)
{
    Noesis::Color color;
    int index;

    Noesis::Color::TryParse(part.content.Str(), color, index);

    uint32_t startPosition = part.startCharacterIndex;
    uint32_t endPosition = part.endCharacterIndex + 1;

    if (part.partKind == Noesis::XamlPartKind_EndTag)
    {
        endPosition--;
    }
    else if (part.partKind == Noesis::XamlPartKind_AttributeValue)
    {
        startPosition++;
        endPosition--;
    }
    
    uint32_t startCharacterIndex = Noesis::LenientXamlParser::UTF16Length(document.text.Str() +
        document.lineStartPositions[part.startLineIndex], document.text.Str()
        + document.lineStartPositions[part.startLineIndex] + startPosition);
    uint32_t endCharacterIndex = Noesis::LenientXamlParser::UTF16Length(document.text.Str() +
        document.lineStartPositions[part.endLineIndex], document.text.Str()
        + document.lineStartPositions[part.endLineIndex] + endPosition);

    result.StartObject();

    result.WritePropertyName("range");
    result.StartObject();
        result.WritePropertyName("start");
        result.StartObject();
            result.WritePropertyName("line");
            result.WriteValue(part.startLineIndex);
            result.WritePropertyName("character");
            result.WriteValue(startCharacterIndex);
        result.EndObject();
        result.WritePropertyName("end");
        result.StartObject();
            result.WritePropertyName("line");
            result.WriteValue(part.endLineIndex);
            result.WritePropertyName("character");
            result.WriteValue(endCharacterIndex);
        result.EndObject();
    result.EndObject();

    result.WritePropertyName("color");
    result.StartObject();
        result.WritePropertyName("red");
        result.WriteValue(color.r);
        result.WritePropertyName("green");
        result.WriteValue(color.g);
        result.WritePropertyName("blue");
        result.WriteValue(color.b);
        result.WritePropertyName("alpha");
        result.WriteValue(color.a);
    result.EndObject();

    result.EndObject();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::CapabilityColor::DocumentColorRequest(int bodyId, DocumentContainer& document,
    BaseString& responseBuffer)
{
    LenientXamlParser::Parts parts;
    LenientXamlParser::LinePartIndicies linePartIndices;
    LenientXamlParser::NSDefinitionMap nsDefinitionMap;
    LenientXamlParser::ParseXaml(document, parts, linePartIndices, nsDefinitionMap);

    if (parts.Size() == 0)
    {
        MessageWriter::CreateErrorResponse(bodyId, LSPErrorCodes::RequestCancelled,
            "RequestCancelled: Invalid completion position.", responseBuffer);
        return;
    }

    const TypeClass* brushType = TypeOf<Brush>();
    const TypeClass* colorType = TypeOf<Color>();

    Noesis::TypePropertyMap typeProperties;
    Noesis::DependencyPropertyMap dependencyProperties;
    Symbol currentTypeId;
    
    JsonBuilder result;
    
    result.StartArray();
    
    for (uint32_t i = 0; i < parts.Size(); i++)
    {
        XamlPart& part = parts[i];

        if (part.errorFlags > 0)
        {
            continue;
        }

        if (part.partKind == XamlPartKind_TagContent)
        {
            const XamlPart& parentPart = parts[part.parentPartIndex];
            if (parentPart.partKind != XamlPartKind_StartTagBegin
                || parentPart.errorFlags >= ErrorFlags_Error)
            {
                continue;
            }

            if (parentPart.typeId != currentTypeId)
            {
                const TypeClass* typeClass =
                    DynamicCast<const TypeClass*>(Reflection::GetType(parentPart.typeId));
                if (typeClass == nullptr)
                {
                    continue;
                }
                LangServerReflectionHelper::GetTypeAncestorPropertyData(typeClass, typeProperties,
                    dependencyProperties);
                currentTypeId = parentPart.typeId;
            }
            auto tpIt = typeProperties.Find(parentPart.propertyId);
            if (tpIt != typeProperties.End()
                && (brushType->IsAssignableFrom(tpIt->value->GetContentType())
                    || colorType->IsAssignableFrom(tpIt->value->GetContentType())))
            {
                GenerateColorRangeResult(document, part, result);
            }
            auto dpIt = dependencyProperties.Find(parentPart.propertyId);
            if (dpIt != dependencyProperties.End()
                && (brushType->IsAssignableFrom(dpIt->value->GetType())
                    || colorType->IsAssignableFrom(dpIt->value->GetType())))
            {
                GenerateColorRangeResult(document, part, result);
            }
        }
        else if (part.partKind == XamlPartKind_AttributeValue)
        {
            XamlPart& parentPart = parts[part.parentPartIndex];
            if (parentPart.partKind != XamlPartKind_AttributeKey 
                || parentPart.errorFlags > 0 || parentPart.HasFlag(PartFlags_NSDefinition))
            {
                continue;
            }
            if (parentPart.typeId != currentTypeId)
            {
                const TypeClass* typeClass =
                    DynamicCast<const TypeClass*>(Reflection::GetType(parentPart.typeId));
                if (typeClass == nullptr)
                {
                    continue;
                }
                LangServerReflectionHelper::GetTypeAncestorPropertyData(typeClass, typeProperties,
                    dependencyProperties);
                currentTypeId = parentPart.typeId;
            }
            auto tpIt = typeProperties.Find(parentPart.propertyId);
            if (tpIt != typeProperties.End()
                && (brushType->IsAssignableFrom(tpIt->value->GetContentType())
                || colorType->IsAssignableFrom(tpIt->value->GetContentType())))
            {
                GenerateColorRangeResult(document, part, result);
            }
            auto dpIt = dependencyProperties.Find(parentPart.propertyId);
            if (dpIt != dependencyProperties.End()
                && (brushType->IsAssignableFrom(dpIt->value->GetType())
                || colorType->IsAssignableFrom(dpIt->value->GetType())))
            {
                GenerateColorRangeResult(document, part, result);
            }
        }
    }

    result.EndArray();

    MessageWriter::CreateResponse(bodyId, result.Str(), responseBuffer);
}
