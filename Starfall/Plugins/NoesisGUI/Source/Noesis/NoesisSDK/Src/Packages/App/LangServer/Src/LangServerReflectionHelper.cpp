////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include "LangServerReflectionHelper.h"

#include <NsGui/DependencyData.h>


NS_DECLARE_SYMBOL(Name)


////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::LangServerReflectionHelper::GetTypePropertyData(const TypeClass* typeClass, 
    TypePropertyMap& typePropertyMap, DependencyPropertyMap& dependencyPropertyMap)
{
    const uint32_t propertyCount = typeClass->GetNumProperties();
    for (uint32_t i = 0; i < propertyCount; i++)
    {
        const TypeProperty* typeProperty = typeClass->GetProperty(i);
        const Symbol name = typeProperty->GetName();
        if (typePropertyMap.Find(name) == typePropertyMap.End())
        {
            typePropertyMap.Insert(name, typeProperty);
        }
    }

    TypeMetaData* typeMeta = typeClass->FindMeta(Noesis::TypeOf<DependencyData>());
    if (typeMeta != nullptr)
    {
        const DependencyData* dependencyData = Noesis::DynamicCast<const DependencyData*>(typeMeta);

        dependencyData->EnumProperties([](const DependencyProperty* dp, void* user)
            {
                HashMap<Symbol, const DependencyProperty*>* dependencyProperties =
                    static_cast<HashMap<Symbol, const DependencyProperty*>*>(user);
                const Symbol name = dp->GetName();
                if (name != NSS(Name)
                    && dependencyProperties->Find(name) == dependencyProperties->End())
                {
                    dependencyProperties->Insert(name, dp);
                }
            }, &dependencyPropertyMap);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Noesis::LangServerReflectionHelper::GetTypeAncestorPropertyData(const Noesis::TypeClass* typeClass, TypePropertyMap& typePropertyMap,
    DependencyPropertyMap& dependencyPropertyMap)
{
    while (typeClass != nullptr)
    {
        GetTypePropertyData(typeClass, typePropertyMap, dependencyPropertyMap);
        typeClass = typeClass->GetBase();
    }
}
