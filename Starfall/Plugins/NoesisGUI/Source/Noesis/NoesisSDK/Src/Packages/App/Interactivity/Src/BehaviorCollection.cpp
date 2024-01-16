////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/BehaviorCollection.h>


using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
BehaviorCollection::BehaviorCollection()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
BehaviorCollection::~BehaviorCollection()
{
    if (GetAssociatedObject() != 0)
    {
        int numBehaviors = Count();
        for (int i = 0; i < numBehaviors; ++i)
        {
            Get(i)->Detach();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Ptr<BehaviorCollection> BehaviorCollection::Clone() const
{
    return Noesis::StaticPtrCast<BehaviorCollection>(Freezable::Clone());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Ptr<BehaviorCollection> BehaviorCollection::CloneCurrentValue() const
{
    return Noesis::StaticPtrCast<BehaviorCollection>(Freezable::CloneCurrentValue());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Ptr<Noesis::Freezable> BehaviorCollection::CreateInstanceCore() const
{
    return *new BehaviorCollection();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
bool BehaviorCollection::FreezeCore(bool)
{
    return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorCollection::OnAttached()
{
    DependencyObject* associatedObject = GetAssociatedObject();

    int numBehaviors = Count();
    for (int i = 0; i < numBehaviors; ++i)
    {
        Get(i)->Attach(associatedObject);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorCollection::OnDetaching()
{
    int numBehaviors = Count();
    for (int i = 0; i < numBehaviors; ++i)
    {
        Get(i)->Detach();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorCollection::ItemAdded(Behavior* item)
{
    DependencyObject* associatedObject = GetAssociatedObject();
    if (associatedObject != 0)
    {
        item->Attach(associatedObject);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void BehaviorCollection::ItemRemoved(Behavior* item)
{
    DependencyObject* associatedObject = GetAssociatedObject();
    if (associatedObject != 0)
    {
        item->Detach();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION_(BehaviorCollection, "NoesisApp.BehaviorCollection")

NS_END_COLD_REGION
