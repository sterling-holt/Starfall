////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/RiveControl.h>
#include <NsApp/RiveInput.h>
#include <NsGui/Uri.h>
#include <NsGui/Stream.h>
#include <NsGui/Enums.h>
#include <NsGui/Viewbox.h>
#include <NsGui/IView.h>
#include <NsGui/UICollection.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/UIElementData.h>
#include <NsGui/MatrixTransform.h>
#include <NsGui/FrameworkPropertyMetadata.h>
#include <NsGui/ContentPropertyMetaData.h>
#include <NsCore/ReflectionImplement.h>
#include <NsCore/HighResTimer.h>
#include <NsCore/TypeConverter.h>
#include <NsCore/CpuProfiler.h>

#include "Rive.h"


using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
RiveControl::RiveControl(): mTicks(Noesis::HighResTimer::Ticks())
{
    mRiveFactory = rivestd::make_unique<NoesisApp::RiveFactory>();
    mRiveRenderer = rivestd::make_unique<NoesisApp::RiveRenderer>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
RiveControl::~RiveControl()
{
    UnregisterInputs();
    DisconnectNode(mInputs, this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const Noesis::Uri& RiveControl::GetSource() const
{
    return GetValue<Noesis::Uri>(SourceProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::SetSource(const Noesis::Uri& value)
{
    SetValue<Noesis::Uri>(SourceProperty, value);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const char* RiveControl::GetState() const
{
    return GetValue<Noesis::String>(StateProperty).Str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Stretch RiveControl::GetStretch() const
{
    return GetValue<Noesis::Stretch>(StretchProperty);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::SetStretch(Noesis::Stretch stretch)
{
    SetValue<Noesis::Stretch>(StretchProperty, stretch);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
RiveInputCollection* RiveControl::GetInputs() const
{
    const_cast<RiveControl*>(this)->EnsureInputs();
    return mInputs;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::SetInputValue(const char* name, Noesis::BaseComponent* value)
{
    if (mRiveScene != nullptr)
    {
        size_t numInputs = mRiveScene->inputCount();
        for (size_t i = 0; i < numInputs; ++i)
        {
            rive::SMIInput* input = mRiveScene->input(i);
            if (input->name() == name)
            {
                if (input->input()->is<rive::StateMachineBool>())
                {
                    rive::SMIBool* boolInput = (rive::SMIBool*)input;
                    if (Noesis::Boxing::CanUnbox<bool>(value))
                    {
                        boolInput->value(Noesis::Boxing::Unbox<bool>(value));
                    }
                    else
                    {
                        Noesis::Ptr<Noesis::BaseComponent> result;
                        const Noesis::Type* type = Noesis::TypeOf<bool>();
                        Noesis::TypeConverter* converter = Noesis::TypeConverter::Get(type);
                        if (converter != nullptr && converter->TryConvertFrom(value, result))
                        {
                            boolInput->value(Noesis::Boxing::Unbox<bool>(result));
                        }
                    }
                }
                else if (input->input()->is<rive::StateMachineNumber>())
                {
                    rive::SMINumber* numberInput = (rive::SMINumber*)input;
                    if (Noesis::Boxing::CanUnbox<float>(value))
                    {
                        numberInput->value(Noesis::Boxing::Unbox<float>(value));
                    }
                    else
                    {
                        Noesis::Ptr<Noesis::BaseComponent> result;
                        const Noesis::Type* type = Noesis::TypeOf<float>();
                        Noesis::TypeConverter* converter = Noesis::TypeConverter::Get(type);
                        if (converter != nullptr && converter->TryConvertFrom(value, result))
                        {
                            numberInput->value(Noesis::Boxing::Unbox<float>(result));
                        }
                    }
                }
                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::FireInputTrigger(const char* name)
{
    if (mRiveScene != nullptr)
    {
        size_t numInputs = mRiveScene->inputCount();
        for (size_t i = 0; i < numInputs; ++i)
        {
            rive::SMIInput* input = mRiveScene->input(i);
            if (input->name() == name)
            {
                if (input->input()->is<rive::StateMachineTrigger>())
                {
                    ((rive::SMITrigger*)input)->fire();
                }
                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t RiveControl::GetSourceInputCount() const
{
    return mRiveScene != nullptr ? (uint32_t)mRiveScene->inputCount() : 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const char* RiveControl::GetSourceInput(uint32_t index, RiveSourceInputType& type) const
{
    NS_ASSERT(index < GetSourceInputCount());
    rive::SMIInput* input = mRiveScene->input(index);
    if (input->input()->is<rive::StateMachineBool>())
    {
        type = RiveSourceInputType_Bool;
    }
    else if (input->input()->is<rive::StateMachineNumber>())
    {
        type = RiveSourceInputType_Number;
    }
    else if (input->input()->is<rive::StateMachineTrigger>())
    {
        type = RiveSourceInputType_Trigger;
    }
    else
    {
        NS_ASSERT_UNREACHABLE;
    }
    return input->name().c_str();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::OnInit()
{
    ParentClass::OnInit();

    if (mInputs != nullptr)
    {
        mInputs->Init();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::OnConnectToView(Noesis::IView* view)
{
    ParentClass::OnConnectToView(view);
    view->Rendering() += Noesis::MakeDelegate(this, &RiveControl::Advance);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::OnDisconnectFromView()
{
    GetView()->Rendering() -= Noesis::MakeDelegate(this, &RiveControl::Advance);
    ParentClass::OnDisconnectFromView();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static rive::Mat2D GetStretchMatrix(Noesis::Stretch stretch, const Noesis::Size& size,
    const rive::AABB& content)
{
    rive::Fit fits[] = { rive::Fit::none, rive::Fit::fill, rive::Fit::contain, rive::Fit::cover };
    rive::Fit fit = fits[(std::underlying_type<Noesis::Stretch>::type)stretch];
    rive::AABB frame(0, 0, size.width, size.height);
    return rive::computeAlignment(fit, rive::Alignment::center, frame, content);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::OnMouseMove(const Noesis::MouseEventArgs& e)
{
    if (mRiveScene != nullptr)
    {
        Noesis::Point pos = PointFromScreen(e.position);
        pos = Noesis::PointTransform(pos, mToRiveScene);
        mRiveScene->pointerMove(rive::Vec2D(pos.x, pos.y));
        e.handled = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::OnMouseDown(const Noesis::MouseButtonEventArgs& e)
{
    if (mRiveScene != nullptr && e.changedButton == Noesis::MouseButton_Left)
    {
        Noesis::Point pos = PointFromScreen(e.position);
        pos = Noesis::PointTransform(pos, mToRiveScene);
        mRiveScene->pointerDown(rive::Vec2D(pos.x, pos.y));
        e.handled = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::OnMouseUp(const Noesis::MouseButtonEventArgs& e)
{
    if (mRiveScene != nullptr && e.changedButton == Noesis::MouseButton_Left)
    {
        Noesis::Point pos = PointFromScreen(e.position);
        pos = Noesis::PointTransform(pos, mToRiveScene);
        mRiveScene->pointerUp(rive::Vec2D(pos.x, pos.y));
        e.handled = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::OnRender(Noesis::DrawingContext* context)
{
    NS_PROFILE_CPU("Rive/Draw");

    if (mRiveScene != nullptr)
    {
        mRiveRenderer->Reset(context);
        mRiveRenderer->save();
        mRiveRenderer->transform(GetStretchMatrix(GetStretch(), GetRenderSize(), mRiveScene->bounds()));
        mRiveScene->draw(mRiveRenderer.get());
        mRiveRenderer->restore();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static Noesis::Size StretchSize(Noesis::Stretch stretch, rive::Scene* scene,
    const Noesis::Size& slot)
{
    Noesis::Size riveSize = scene != nullptr ? Noesis::Size(scene->width(), scene->height()) :
        Noesis::Size::Zero();

    if (stretch == Noesis::Stretch_None)
    {
        return riveSize;
    }
    else
    {
        Noesis::Point scale = Noesis::Viewbox::GetStretchScale(riveSize, slot, stretch);
        return Noesis::Size(riveSize.width * scale.x, riveSize.height * scale.y);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Size RiveControl::MeasureOverride(const Noesis::Size& availableSize)
{
    return StretchSize(GetStretch(), mRiveScene.get(), availableSize);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Size RiveControl::ArrangeOverride(const Noesis::Size& finalSize)
{
    Noesis::Size renderSize = StretchSize(GetStretch(), mRiveScene.get(), finalSize);

    if (mRiveScene != nullptr)
    {
        rive::Mat2D mtx = GetStretchMatrix(GetStretch(), renderSize, mRiveScene->bounds());
        mToRiveScene = Noesis::Inverse(Noesis::Transform2(mtx.values()));
    }

    return renderSize;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::CloneOverride(Noesis::FrameworkElement* clone, Noesis::FrameworkTemplate*) const
{
    RiveControl* rive = (RiveControl*)clone;
    int numInputs = mInputs != nullptr ? mInputs->Count() : 0;
    for (int i = 0; i < numInputs; ++i)
    {
        RiveInput* input = mInputs->Get(i);
        rive->GetInputs()->Add(Noesis::StaticPtrCast<RiveInput>(input->Clone()));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::EnsureInputs()
{
    if (NS_UNLIKELY(mInputs == nullptr))
    {
        mInputs = *new RiveInputCollection();
        ConnectNode(mInputs, this);
        InitComponent(mInputs, IsInitialized());

        mInputs->CollectionChanged() += MakeDelegate(this, &RiveControl::OnInputsChanged);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::OnInputsChanged(Noesis::BaseComponent*,
    const Noesis::NotifyCollectionChangedEventArgs& e)
{
    switch (e.action)
    {
        case Noesis::NotifyCollectionChangedAction_Add:
        {
            RiveInput* input = (RiveInput*)e.newValue;
            input->ValueChanged() += Noesis::MakeDelegate(this, &RiveControl::SetInputValue);
            SetInputValue(input->GetInputName(), input->GetInputValue());
            break;
        }
        case Noesis::NotifyCollectionChangedAction_Remove:
        {
            RiveInput* input = (RiveInput*)e.oldValue;
            input->ValueChanged() -= Noesis::MakeDelegate(this, &RiveControl::SetInputValue);
            break;
        }
        case Noesis::NotifyCollectionChangedAction_Replace:
        {
            RiveInput* iOld = (RiveInput*)e.oldValue;
            iOld->ValueChanged() -= Noesis::MakeDelegate(this, &RiveControl::SetInputValue);
            RiveInput* iNew = (RiveInput*)e.newValue;
            iNew->ValueChanged() += Noesis::MakeDelegate(this, &RiveControl::SetInputValue);
            SetInputValue(iNew->GetInputName(), iNew->GetInputValue());
            break;
        }
        case Noesis::NotifyCollectionChangedAction_Move:
        {
            break;
        }
        case Noesis::NotifyCollectionChangedAction_Reset:
        {
            break;
        }
        case Noesis::NotifyCollectionChangedAction_PreReset:
        {
            UnregisterInputs();
            break;
        }
        default: NS_ASSERT_UNREACHABLE;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::UnregisterInputs()
{
    int numInputs = mInputs != nullptr ? mInputs->Count() : 0;
    for (int i = 0; i < numInputs; ++i)
    {
        RiveInput* input = mInputs->Get(i);
        input->ValueChanged() -= Noesis::MakeDelegate(this, &RiveControl::SetInputValue);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::LoadSource(const Noesis::Uri& uri)
{
    if (!Noesis::StrIsEmpty(uri.Str()))
    {
        Noesis::Ptr<Noesis::Stream> stream = Noesis::GUI::LoadXamlResource(uri);
        if (stream != nullptr)
        {
            uint32_t streamLength = stream->GetLength();
            uint8_t* streamMemory = (uint8_t*)stream->GetMemoryBase();
            if (streamMemory != nullptr)
            {
                mRiveFile = rive::File::import(rive::Span<uint8_t>(streamMemory, streamLength),
                    mRiveFactory.get());
            }
            else
            {
                streamMemory = (uint8_t*)Noesis::Alloc(streamLength);
                stream->Read(streamMemory, streamLength);
                mRiveFile = rive::File::import(rive::Span<uint8_t>(streamMemory, streamLength),
                    mRiveFactory.get());
                Noesis::Dealloc(streamMemory);
            }

            mRiveArtboard = mRiveFile->artboardDefault();
            mRiveArtboard->advance(0.0f);
            mRiveScene = mRiveArtboard->defaultScene();

            // TODO: Hook to state machine "OnStateChanged"

            // Update Rive scene inputs from the Inputs collection
            int numInputs = mInputs != nullptr ? mInputs->Count() : 0;
            for (int i = 0; i < numInputs; ++i)
            {
                RiveInput* input = mInputs->Get(i);
                SetInputValue(input->GetInputName(), input->GetInputValue());
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::OnStateChanged(const char* state)
{
    SetReadOnlyProperty<Noesis::String>(StateProperty, state);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveControl::Advance(Noesis::IView*)
{
    NS_PROFILE_CPU("Rive/Advance");

    // Update current time
    uint64_t t = Noesis::HighResTimer::Ticks();
    float elapsed = (float)Noesis::HighResTimer::Seconds(t - mTicks);
    mTicks = t;

    // Advance Rive scene
    if (mRiveScene != nullptr)
    {
        mRiveScene->advanceAndApply(elapsed);
    }

    // Invalidate control for OnRender
    InvalidateVisual();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION(RiveControl, "NoesisGUIExtensions.RiveControl")
{
    NsMeta<Noesis::ContentPropertyMetaData>("Inputs");

    NsProp("Inputs", &RiveControl::GetInputs);

    auto OnSourceChanged = [](Noesis::DependencyObject* d, const Noesis::DependencyPropertyChangedEventArgs& e)
    {
        RiveControl* rive = Noesis::DynamicCast<RiveControl*>(d);
        if (rive != nullptr)
        {
            const Noesis::Uri& source = e.NewValue<Noesis::Uri>();
            rive->LoadSource(source);
        }
    };

    Noesis::UIElementData* data = NsMeta<Noesis::UIElementData>(Noesis::TypeOf<SelfClass>());
    data->RegisterProperty<Noesis::Uri>(SourceProperty, "Source",
        Noesis::FrameworkPropertyMetadata::Create(Noesis::Uri(),
            Noesis::FrameworkPropertyMetadataOptions_AffectsMeasure |
            Noesis::FrameworkPropertyMetadataOptions_AffectsRender,
            Noesis::PropertyChangedCallback(OnSourceChanged)));
    data->RegisterPropertyRO<Noesis::String>(StateProperty, "State",
        Noesis::FrameworkPropertyMetadata::Create(Noesis::String()));
    data->RegisterProperty<Noesis::Stretch>(StretchProperty, "Stretch",
        Noesis::FrameworkPropertyMetadata::Create(Noesis::Stretch_Uniform,
            Noesis::FrameworkPropertyMetadataOptions_AffectsMeasure |
            Noesis::FrameworkPropertyMetadataOptions_AffectsArrange));
}

NS_END_COLD_REGION

////////////////////////////////////////////////////////////////////////////////////////////////////
const Noesis::DependencyProperty* RiveControl::SourceProperty;
const Noesis::DependencyProperty* RiveControl::StateProperty;
const Noesis::DependencyProperty* RiveControl::StretchProperty;
