////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __APP_REFLECTIONHELPER_H
#define __APP_REFLECTIONHELPER_H


#include <NsCore/HashMap.h>
#include <NsCore/TypeProperty.h>
#include <NsCore/Symbol.h>
#include <NsGui/DependencyProperty.h>


namespace Noesis
{
typedef HashMap<Symbol, const TypeProperty*> TypePropertyMap;
typedef HashMap<Symbol, const DependencyProperty*> DependencyPropertyMap;

class TypeClass;

class LangServerReflectionHelper final
{
public:

    static void GetTypePropertyData(const Noesis::TypeClass* typeClass,
        TypePropertyMap& typePropertyMap, DependencyPropertyMap& dependencyPropertyMap);
    static void GetTypeAncestorPropertyData(const Noesis::TypeClass* typeClass,
        TypePropertyMap& typePropertyMap, DependencyPropertyMap& dependencyPropertyMap);
private:
    LangServerReflectionHelper() = default;
};
}

#endif