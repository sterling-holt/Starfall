////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


//#undef NS_LOG_TRACE
//#define NS_LOG_TRACE(...) NS_LOG_(NS_LOG_LEVEL_TRACE, __VA_ARGS__)


#include "Rive.h"

#include <NsGui/DrawingContext.h>
#include <NsGui/StreamGeometry.h>
#include <NsGui/StreamGeometryContext.h>
#include <NsGui/MatrixTransform.h>
#include <NsGui/SolidColorBrush.h>
#include <NsGui/LinearGradientBrush.h>
#include <NsGui/RadialGradientBrush.h>
#include <NsGui/GradientStopCollection.h>
#include <NsGui/Pen.h>
#include <NsDrawing/Color.h>
#include <NsDrawing/SVGPath.h>
#include <NsCore/Log.h>

NS_WARNING_PUSH
NS_MSVC_WARNING_DISABLE(4100 4458 4245 4389 5051 4201 4267 4355 4459 4060 4996)
NS_GCC_CLANG_WARNING_DISABLE("-Wshadow")
NS_GCC_WARNING_DISABLE("-Wsign-compare")
NS_GCC_WARNING_DISABLE("-Wattributes")
NS_CLANG_WARNING_DISABLE("-Wshorten-64-to-32")
NS_CLANG_WARNING_DISABLE("-Wcomma")

#include "rive/src/artboard.cpp"
#include "rive/src/component.cpp"
#include "rive/src/dependency_sorter.cpp"
#include "rive/src/drawable.cpp"
#include "rive/src/draw_rules.cpp"
#include "rive/src/draw_target.cpp"
#include "rive/src/factory.cpp"
#include "rive/src/file.cpp"
#include "rive/src/hittest_command_path.cpp"
#include "rive/src/layout.cpp"
#include "rive/src/nested_artboard.cpp"
#include "rive/src/node.cpp"
#include "rive/src/renderer.cpp"
#include "rive/src/rive_counter.cpp"
#include "rive/src/scene.cpp"
#include "rive/src/simple_array.cpp"
#include "rive/src/transform_component.cpp"
#include "rive/src/world_transform_component.cpp"
#include "rive/src/animation/animation_state.cpp"
#include "rive/src/animation/animation_state_instance.cpp"
#include "rive/src/animation/blend_animation.cpp"
#include "rive/src/animation/blend_animation_1d.cpp"
#include "rive/src/animation/blend_animation_direct.cpp"
#include "rive/src/animation/blend_state.cpp"
#include "rive/src/animation/blend_state_1d.cpp"
#include "rive/src/animation/blend_state_1d_instance.cpp"
#include "rive/src/animation/blend_state_direct.cpp"
#include "rive/src/animation/blend_state_direct_instance.cpp"
#include "rive/src/animation/blend_state_transition.cpp"
#include "rive/src/animation/cubic_interpolator.cpp"
#include "rive/src/animation/keyed_object.cpp"
#include "rive/src/animation/keyed_property.cpp"
#include "rive/src/animation/keyframe.cpp"
#include "rive/src/animation/keyframe_bool.cpp"
#include "rive/src/animation/keyframe_color.cpp"
#include "rive/src/animation/keyframe_double.cpp"
#include "rive/src/animation/keyframe_id.cpp"
#include "rive/src/animation/layer_state.cpp"
#include "rive/src/animation/linear_animation.cpp"
#include "rive/src/animation/linear_animation_instance.cpp"
#include "rive/src/animation/listener_action.cpp"
#include "rive/src/animation/listener_align_target.cpp"
#include "rive/src/animation/listener_bool_change.cpp"
#include "rive/src/animation/listener_input_change.cpp"
#include "rive/src/animation/listener_number_change.cpp"
#include "rive/src/animation/listener_trigger_change.cpp"
#include "rive/src/animation/nested_animation.cpp"
#include "rive/src/animation/nested_linear_animation.cpp"
#include "rive/src/animation/nested_remap_animation.cpp"
#include "rive/src/animation/nested_simple_animation.cpp"
#include "rive/src/animation/nested_state_machine.cpp"
#include "rive/src/animation/state_instance.cpp"
#include "rive/src/animation/state_machine.cpp"
#include "rive/src/animation/state_machine_input.cpp"
#include "rive/src/animation/state_machine_input_instance.cpp"
#include "rive/src/animation/state_machine_instance.cpp"
#include "rive/src/animation/state_machine_layer.cpp"
#include "rive/src/animation/state_machine_listener.cpp"
#include "rive/src/animation/state_transition.cpp"
#include "rive/src/animation/system_state_instance.cpp"
#include "rive/src/animation/transition_bool_condition.cpp"
#include "rive/src/animation/transition_condition.cpp"
#include "rive/src/animation/transition_number_condition.cpp"
#include "rive/src/animation/transition_trigger_condition.cpp"
#include "rive/src/assets/file_asset.cpp"
#include "rive/src/assets/file_asset_contents.cpp"
#include "rive/src/assets/image_asset.cpp"
#include "rive/src/bones/bone.cpp"
#include "rive/src/bones/root_bone.cpp"
#include "rive/src/bones/skin.cpp"
#include "rive/src/bones/skinnable.cpp"
#include "rive/src/bones/tendon.cpp"
#include "rive/src/bones/weight.cpp"
#include "rive/src/constraints/constraint.cpp"
#include "rive/src/constraints/distance_constraint.cpp"
#include "rive/src/constraints/ik_constraint.cpp"
#include "rive/src/constraints/rotation_constraint.cpp"
#include "rive/src/constraints/scale_constraint.cpp"
#include "rive/src/constraints/targeted_constraint.cpp"
#include "rive/src/constraints/transform_constraint.cpp"
#include "rive/src/constraints/translation_constraint.cpp"
#include "rive/src/core/binary_reader.cpp"
#include "rive/src/core/field_types/core_bool_type.cpp"
#include "rive/src/core/field_types/core_bytes_type.cpp"
#include "rive/src/core/field_types/core_color_type.cpp"
#include "rive/src/core/field_types/core_double_type.cpp"
#include "rive/src/core/field_types/core_string_type.cpp"
#include "rive/src/core/field_types/core_uint_type.cpp"
#include "rive/src/generated/artboard_base.cpp"
#include "rive/src/generated/backboard_base.cpp"
#include "rive/src/generated/draw_rules_base.cpp"
#include "rive/src/generated/draw_target_base.cpp"
#include "rive/src/generated/nested_artboard_base.cpp"
#include "rive/src/generated/node_base.cpp"
#include "rive/src/generated/animation/animation_base.cpp"
#include "rive/src/generated/animation/animation_state_base.cpp"
#include "rive/src/generated/animation/any_state_base.cpp"
#include "rive/src/generated/animation/blend_animation_1d_base.cpp"
#include "rive/src/generated/animation/blend_animation_direct_base.cpp"
#include "rive/src/generated/animation/blend_state_1d_base.cpp"
#include "rive/src/generated/animation/blend_state_direct_base.cpp"
#include "rive/src/generated/animation/blend_state_transition_base.cpp"
#include "rive/src/generated/animation/cubic_interpolator_base.cpp"
#include "rive/src/generated/animation/entry_state_base.cpp"
#include "rive/src/generated/animation/exit_state_base.cpp"
#include "rive/src/generated/animation/keyed_object_base.cpp"
#include "rive/src/generated/animation/keyed_property_base.cpp"
#include "rive/src/generated/animation/keyframe_bool_base.cpp"
#include "rive/src/generated/animation/keyframe_color_base.cpp"
#include "rive/src/generated/animation/keyframe_double_base.cpp"
#include "rive/src/generated/animation/keyframe_id_base.cpp"
#include "rive/src/generated/animation/linear_animation_base.cpp"
#include "rive/src/generated/animation/listener_align_target_base.cpp"
#include "rive/src/generated/animation/listener_bool_change_base.cpp"
#include "rive/src/generated/animation/listener_number_change_base.cpp"
#include "rive/src/generated/animation/listener_trigger_change_base.cpp"
#include "rive/src/generated/animation/nested_bool_base.cpp"
#include "rive/src/generated/animation/nested_number_base.cpp"
#include "rive/src/generated/animation/nested_remap_animation_base.cpp"
#include "rive/src/generated/animation/nested_simple_animation_base.cpp"
#include "rive/src/generated/animation/nested_state_machine_base.cpp"
#include "rive/src/generated/animation/nested_trigger_base.cpp"
#include "rive/src/generated/animation/state_machine_base.cpp"
#include "rive/src/generated/animation/state_machine_bool_base.cpp"
#include "rive/src/generated/animation/state_machine_layer_base.cpp"
#include "rive/src/generated/animation/state_machine_listener_base.cpp"
#include "rive/src/generated/animation/state_machine_number_base.cpp"
#include "rive/src/generated/animation/state_machine_trigger_base.cpp"
#include "rive/src/generated/animation/state_transition_base.cpp"
#include "rive/src/generated/animation/transition_bool_condition_base.cpp"
#include "rive/src/generated/animation/transition_number_condition_base.cpp"
#include "rive/src/generated/animation/transition_trigger_condition_base.cpp"
#include "rive/src/generated/assets/file_asset_contents_base.cpp"
#include "rive/src/generated/assets/folder_base.cpp"
#include "rive/src/generated/assets/image_asset_base.cpp"
#include "rive/src/generated/bones/bone_base.cpp"
#include "rive/src/generated/bones/cubic_weight_base.cpp"
#include "rive/src/generated/bones/root_bone_base.cpp"
#include "rive/src/generated/bones/skin_base.cpp"
#include "rive/src/generated/bones/tendon_base.cpp"
#include "rive/src/generated/bones/weight_base.cpp"
#include "rive/src/generated/constraints/distance_constraint_base.cpp"
#include "rive/src/generated/constraints/ik_constraint_base.cpp"
#include "rive/src/generated/constraints/rotation_constraint_base.cpp"
#include "rive/src/generated/constraints/scale_constraint_base.cpp"
#include "rive/src/generated/constraints/transform_constraint_base.cpp"
#include "rive/src/generated/constraints/translation_constraint_base.cpp"
#include "rive/src/generated/shapes/clipping_shape_base.cpp"
#include "rive/src/generated/shapes/contour_mesh_vertex_base.cpp"
#include "rive/src/generated/shapes/cubic_asymmetric_vertex_base.cpp"
#include "rive/src/generated/shapes/cubic_detached_vertex_base.cpp"
#include "rive/src/generated/shapes/cubic_mirrored_vertex_base.cpp"
#include "rive/src/generated/shapes/ellipse_base.cpp"
#include "rive/src/generated/shapes/image_base.cpp"
#include "rive/src/generated/shapes/mesh_base.cpp"
#include "rive/src/generated/shapes/mesh_vertex_base.cpp"
#include "rive/src/generated/shapes/points_path_base.cpp"
#include "rive/src/generated/shapes/polygon_base.cpp"
#include "rive/src/generated/shapes/rectangle_base.cpp"
#include "rive/src/generated/shapes/shape_base.cpp"
#include "rive/src/generated/shapes/star_base.cpp"
#include "rive/src/generated/shapes/straight_vertex_base.cpp"
#include "rive/src/generated/shapes/triangle_base.cpp"
#include "rive/src/generated/shapes/paint/fill_base.cpp"
#include "rive/src/generated/shapes/paint/gradient_stop_base.cpp"
#include "rive/src/generated/shapes/paint/linear_gradient_base.cpp"
#include "rive/src/generated/shapes/paint/radial_gradient_base.cpp"
#include "rive/src/generated/shapes/paint/solid_color_base.cpp"
#include "rive/src/generated/shapes/paint/stroke_base.cpp"
#include "rive/src/generated/shapes/paint/trim_path_base.cpp"
#include "rive/src/importers/artboard_importer.cpp"
#include "rive/src/importers/backboard_importer.cpp"
#include "rive/src/importers/file_asset_importer.cpp"
#include "rive/src/importers/keyed_object_importer.cpp"
#include "rive/src/importers/keyed_property_importer.cpp"
#include "rive/src/importers/layer_state_importer.cpp"
#include "rive/src/importers/linear_animation_importer.cpp"
#include "rive/src/importers/state_machine_importer.cpp"
#include "rive/src/importers/state_machine_layer_importer.cpp"
#include "rive/src/importers/state_machine_listener_importer.cpp"
#include "rive/src/importers/state_transition_importer.cpp"
#include "rive/src/math/aabb.cpp"
#include "rive/src/math/contour_measure.cpp"
#include "rive/src/math/hit_test.cpp"
#include "rive/src/math/mat2d.cpp"
#include "rive/src/math/mat2d_find_max_scale.cpp"
#include "rive/src/math/raw_path.cpp"
#include "rive/src/math/raw_path_utils.cpp"
#include "rive/src/math/vec2d.cpp"
#include "rive/src/shapes/clipping_shape.cpp"
#include "rive/src/shapes/cubic_asymmetric_vertex.cpp"
#include "rive/src/shapes/cubic_detached_vertex.cpp"
#include "rive/src/shapes/cubic_mirrored_vertex.cpp"
#include "rive/src/shapes/cubic_vertex.cpp"
#include "rive/src/shapes/ellipse.cpp"
#include "rive/src/shapes/image.cpp"
#include "rive/src/shapes/mesh.cpp"
#include "rive/src/shapes/mesh_vertex.cpp"
#include "rive/src/shapes/metrics_path.cpp"
#include "rive/src/shapes/parametric_path.cpp"
#include "rive/src/shapes/path.cpp"
#include "rive/src/shapes/path_composer.cpp"
#include "rive/src/shapes/path_vertex.cpp"
#include "rive/src/shapes/points_path.cpp"
#include "rive/src/shapes/polygon.cpp"
#include "rive/src/shapes/rectangle.cpp"
#include "rive/src/shapes/shape.cpp"
#include "rive/src/shapes/shape_paint_container.cpp"
#include "rive/src/shapes/star.cpp"
#include "rive/src/shapes/straight_vertex.cpp"
#include "rive/src/shapes/triangle.cpp"
#include "rive/src/shapes/vertex.cpp"
#include "rive/src/shapes/paint/color.cpp"
#include "rive/src/shapes/paint/fill.cpp"
#include "rive/src/shapes/paint/gradient_stop.cpp"
#include "rive/src/shapes/paint/linear_gradient.cpp"
#include "rive/src/shapes/paint/radial_gradient.cpp"
#include "rive/src/shapes/paint/shape_paint.cpp"
#include "rive/src/shapes/paint/shape_paint_mutator.cpp"
#include "rive/src/shapes/paint/solid_color.cpp"
#include "rive/src/shapes/paint/stroke.cpp"
#include "rive/src/shapes/paint/trim_path.cpp"
#include "rive/src/text/font_hb.cpp"
#include "rive/src/text/line_breaker.cpp"

NS_WARNING_POP


using namespace NoesisApp;


////////////////////////////////////////////////////////////////////////////////////////////////////
static const Noesis::PenLineJoin gLineJoin[3] =
{
    Noesis::PenLineJoin_Miter,
    Noesis::PenLineJoin_Round,
    Noesis::PenLineJoin_Bevel
};

static const Noesis::PenLineCap gLineCap[3] =
{
    Noesis::PenLineCap_Flat,
    Noesis::PenLineCap_Round,
    Noesis::PenLineCap_Square
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct RiveRenderShader: public rive::RenderShader
{
    enum Type { Solid, Linear, Radial };
    Type mType;

    Noesis::Ptr<Noesis::Brush> mBrush;

    RiveRenderShader(Type type, Noesis::Brush* brush): mType(type), mBrush(brush) { }

    /// Redirect allocations to Noesis memory manager
    static void* operator new(size_t size) { return Noesis::Alloc(size); }
    static void operator delete(void* ptr) { Noesis::Dealloc(ptr); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct RiveRenderPaint: public rive::RenderPaint
{
    RiveFactory* mFactory;
    Noesis::Ptr<Noesis::Brush> mBrush;
    Noesis::Ptr<Noesis::Pen> mPen;
    rive::rcp<rive::RenderShader> mShader;
    rive::BlendMode mBlendMode;

    RiveRenderPaint(RiveFactory* factory): mFactory(factory), mBlendMode(rive::BlendMode::srcOver)
    {
    }

    /// Redirect allocations to Noesis memory manager
    static void* operator new(size_t size) { return Noesis::Alloc(size); }
    static void operator delete(void* ptr) { Noesis::Dealloc(ptr); }

    void style(rive::RenderPaintStyle value) override
    {
        if (value == rive::RenderPaintStyle::stroke)
        {
            mPen = *new Noesis::Pen();
        }
    }

    void thickness(float value) override
    {
        NS_ASSERT(mPen != nullptr);
        mPen->SetThickness(value);
    }

    void cap(rive::StrokeCap value) override
    {
        NS_ASSERT(mPen != nullptr);
        Noesis::PenLineCap cap = gLineCap[(std::underlying_type<rive::StrokeCap>::type)value];
        mPen->SetStartLineCap(cap);
        mPen->SetEndLineCap(cap);
        mPen->SetDashCap(cap);
    }

    void join(rive::StrokeJoin value) override
    {
        NS_ASSERT(mPen != nullptr);
        mPen->SetLineJoin(gLineJoin[(std::underlying_type<rive::StrokeJoin>::type)value]);
    }

    void blendMode(rive::BlendMode value) override
    {
        mBlendMode = value;

        TweakOpacity();
    }

    void color(unsigned int value) override
    {
        shader(mFactory->makeSolid(value));
    }

    void shader(rive::rcp<rive::RenderShader> value) override
    {
        mFactory->recycleShader(mShader);
        mShader = value;

        RiveRenderShader* shader = (RiveRenderShader*)value.get();
        if (mPen != nullptr)
        {
            mPen->SetBrush(shader->mBrush);
        }
        else
        {
            mBrush = shader->mBrush;
        }

        TweakOpacity();
    }

    void invalidateStroke() override
    {
    }

    // TODO: This is a hack trying to simulate the blend modes we don't have implemented
    void TweakOpacity()
    {
        switch (mBlendMode)
        {
            case rive::BlendMode::overlay:
            case rive::BlendMode::darken:
            case rive::BlendMode::lighten:
            case rive::BlendMode::hardLight:
            case rive::BlendMode::softLight:
            {
                Noesis::Brush* brush = mPen != nullptr ? mPen->GetBrush() : mBrush;
                if (brush != nullptr)
                {
                    float opacity = brush->GetOpacity();
                    brush->SetOpacity(opacity * 0.5f);
                }
                break;
            }
            case rive::BlendMode::difference:
                NS_LOG_WARNING("BlendMode 'Difference' not supported");
                break;
            case rive::BlendMode::exclusion:
                NS_LOG_WARNING("BlendMode 'Exclusion' not supported");
                break;
            case rive::BlendMode::hue:
                NS_LOG_WARNING("BlendMode 'Hue' not supported");
                break;
            case rive::BlendMode::saturation:
                NS_LOG_WARNING("BlendMode 'Saturation' not supported");
                break;
            case rive::BlendMode::color:
                NS_LOG_WARNING("BlendMode 'Color' not supported");
                break;
            case rive::BlendMode::luminosity:
                NS_LOG_WARNING("BlendMode 'Luminosity' not supported");
                break;
            default: break;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct RiveRenderImage: public rive::RenderImage
{
    // TODO

    RiveRenderImage()
    {
    }

    /// Redirect allocations to Noesis memory manager
    static void* operator new(size_t size) { return Noesis::Alloc(size); }
    static void operator delete(void* ptr) { Noesis::Dealloc(ptr); }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
struct RiveRenderPath: public rive::RenderPath
{
    Noesis::Ptr<Noesis::StreamGeometry> mStreamGeometry;
    Noesis::StreamGeometryContext mContext;
    bool mClosed;

    RiveRenderPath(): mStreamGeometry(*new Noesis::StreamGeometry()),
        mContext(mStreamGeometry->Open()), mClosed(false)
    {
        NS_LOG_TRACE("[%p] RenderPath.ctor", this);
    }

    ~RiveRenderPath()
    {
        NS_LOG_TRACE("[%p] RenderPath.dtor", this);
    }

    /// Redirect allocations to Noesis memory manager
    static void* operator new(size_t size) { return Noesis::Alloc(size); }
    static void operator delete(void* ptr) { Noesis::Dealloc(ptr); }

    void reset() override
    {
        NS_LOG_TRACE("[%p] Reset()", this);

        mContext = mStreamGeometry->Open();
        mClosed = false;
    }

    void fillRule(rive::FillRule value) override
    {
        NS_LOG_TRACE("[%p] FillRule(%s)", this, value == rive::FillRule::nonZero ?
            "NonZero" : "EvenOdd");

        mStreamGeometry->SetFillRule(value == rive::FillRule::nonZero ?
            Noesis::FillRule_Nonzero : Noesis::FillRule_EvenOdd);
    }

    void addRenderPath(rive::RenderPath* path_, const rive::Mat2D& transform_) override
    {
        RiveRenderPath* path = (RiveRenderPath*)path_;
        path->prepareGeometry();

        NS_LOG_TRACE("[%p] AddPath(%p)", this, path);

        // Ignore paths with invalid transform
        Noesis::Transform2 transform(transform_.values());
        float det = Noesis::Determinant(transform.Rotation());
        if (Noesis::IsZero(det) || Noesis::IsNaN(det)) return;

        // Append path commands
        Noesis::PathCommandIterator it = path->mStreamGeometry->GetCommands();
        while (!it.End())
        {
            auto cmd = it.Next();

            switch (cmd.figureCommand)
            {
                case Noesis::SVGPath::FigureCommand_Move:
                {
                    Noesis::Point p(cmd.move.x, cmd.move.y);
                    Noesis::Point pt = p * transform;
                    mContext.BeginFigure(pt, false);
                    break;
                }
                case Noesis::SVGPath::FigureCommand_Line:
                {
                    Noesis::Point p(cmd.line.x, cmd.line.y);
                    Noesis::Point pt = p * transform;
                    mContext.LineTo(pt);
                    break;
                }
                case Noesis::SVGPath::FigureCommand_Cubic:
                {
                    Noesis::Point p(cmd.cubic.x0, cmd.cubic.y0);
                    Noesis::Point pt = p * transform;
                    Noesis::Point q(cmd.cubic.x1, cmd.cubic.y1);
                    Noesis::Point qt = q * transform;
                    Noesis::Point r(cmd.cubic.x2, cmd.cubic.y2);
                    Noesis::Point rt = r * transform;
                    mContext.CubicTo(pt, qt, rt);
                    break;
                }
                case Noesis::SVGPath::FigureCommand_Close:
                {
                    mContext.SetIsClosed(true);
                    break;
                }
                default: NS_ASSERT_UNREACHABLE;
            }
        }
    }

    void moveTo(float x, float y) override
    {
        if (Noesis::IsNaN(x)) x = 0.0f;
        if (Noesis::IsNaN(y)) y = 0.0f;
        mContext.BeginFigure(Noesis::Point(x, y), false);
        NS_LOG_TRACE("[%p] MoveTo(%.4f,%.4f)", this, x, y);
    }

    void lineTo(float x, float y) override
    {
        if (Noesis::IsNaN(x)) x = 0.0f;
        if (Noesis::IsNaN(y)) y = 0.0f;
        mContext.LineTo(Noesis::Point(x, y));
        NS_LOG_TRACE("[%p] LineTo(%.4f,%.4f)", this, x, y);
    }

    void cubicTo(float ox, float oy, float ix, float iy, float x, float y) override
    {
        if (Noesis::IsNaN(ox)) ox = 0.0f;
        if (Noesis::IsNaN(oy)) oy = 0.0f;
        if (Noesis::IsNaN(ix)) ix = 0.0f;
        if (Noesis::IsNaN(iy)) iy = 0.0f;
        if (Noesis::IsNaN(x)) x = 0.0f;
        if (Noesis::IsNaN(y)) y = 0.0f;
        mContext.CubicTo(Noesis::Point(ox, oy), Noesis::Point(ix, iy), Noesis::Point(x, y));
        NS_LOG_TRACE("[%p] CubicTo(%.4f,%.4f;%.4f,%.4f;%.4f,%.4f)", this, ox, oy, ix, iy, x, y);
    }

    void close() override
    {
        mContext.SetIsClosed(true);
        NS_LOG_TRACE("[%p] Close()", this);
    }

    void prepareGeometry()
    {
        if (!mClosed)
        {
            mContext.Close();
            mClosed = true;
        }
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
RiveFactory::RiveFactory()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
RiveFactory::~RiveFactory()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveFactory::recycleShader(const rive::rcp<rive::RenderShader>& shader)
{
    if (shader.get() == nullptr) return;

    RiveRenderShader* shader_ = (RiveRenderShader*)shader.get();
    switch (shader_->mType)
    {
        case RiveRenderShader::Solid:
        {
            mRecycledSolid.PushBack(shader);
            break;
        }
        case RiveRenderShader::Linear:
        {
            mRecycledLinear.PushBack(shader);
            break;
        }
        case RiveRenderShader::Radial:
        {
            mRecycledRadial.PushBack(shader);
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderShader> RiveFactory::makeSolid(unsigned int color)
{
    rive::rcp<rive::RenderShader> shader;

    Noesis::Ptr<Noesis::SolidColorBrush> brush;
    if (mRecycledSolid.Empty())
    {
        brush = *new Noesis::SolidColorBrush();
        shader.reset(new RiveRenderShader(RiveRenderShader::Solid, brush));
    }
    else
    {
        shader = mRecycledSolid.Back();
        mRecycledSolid.PopBack();
        brush.Reset((Noesis::SolidColorBrush*)((RiveRenderShader*)shader.get())->mBrush.GetPtr());
    }

    brush->SetOpacity(1.0f);
    brush->SetColor(Noesis::Color::FromPackedBGRA(color));

    return shader;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
static void UpdateBrushStops(Noesis::GradientStopCollection* brushStops,
    size_t numStops, const float stops[], const rive::ColorInt colors[],
    Noesis::Vector<Noesis::Ptr<Noesis::GradientStop>>& recycledStops)
{
    for (int i = 0; i < brushStops->Count(); ++i)
    {
        recycledStops.EmplaceBack().Reset(brushStops->Get(i));
    }
    brushStops->Clear();

    for (size_t i = 0; i < numStops; ++i)
    {
        Noesis::Ptr<Noesis::GradientStop> stop;
        if (recycledStops.Empty())
        {
            stop = *new Noesis::GradientStop();
        }
        else
        {
            stop = recycledStops.Back();
            recycledStops.PopBack();
        }

        stop->SetColor(Noesis::Color::FromPackedBGRA(colors[i]));
        stop->SetOffset(stops[i]);

        brushStops->Add(stop);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderShader> RiveFactory::makeLinearGradient(float sx, float sy, float ex, float ey,
    const rive::ColorInt colors[], const float stops[], size_t count)
{
    rive::rcp<rive::RenderShader> shader;

    Noesis::Ptr<Noesis::LinearGradientBrush> brush;
    if (mRecycledLinear.Empty())
    {
        brush = *new Noesis::LinearGradientBrush();
        shader.reset(new RiveRenderShader(RiveRenderShader::Linear,brush));
    }
    else
    {
        shader = mRecycledLinear.Back();
        mRecycledLinear.PopBack();
        brush.Reset((Noesis::LinearGradientBrush*)((RiveRenderShader*)shader.get())->mBrush.GetPtr());
    }

    brush->SetOpacity(1.0f);
    brush->SetStartPoint(Noesis::Point(sx, sy));
    brush->SetEndPoint(Noesis::Point(ex, ey));
    brush->SetMappingMode(Noesis::BrushMappingMode_Absolute);
    brush->SetSpreadMethod(Noesis::GradientSpreadMethod_Pad);

    UpdateBrushStops(brush->GetGradientStops(), count, stops, colors, mRecycledStops);

    return shader;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderShader> RiveFactory::makeRadialGradient(float cx, float cy, float radius,
    const rive::ColorInt colors[], const float stops[], size_t count)
{
    rive::rcp<rive::RenderShader> shader;

    Noesis::Ptr<Noesis::RadialGradientBrush> brush;
    if (mRecycledRadial.Empty())
    {
        brush = *new Noesis::RadialGradientBrush();
        shader.reset(new RiveRenderShader(RiveRenderShader::Radial, brush));
    }
    else
    {
        shader = mRecycledRadial.Back();
        mRecycledRadial.PopBack();
        brush.Reset((Noesis::RadialGradientBrush*)((RiveRenderShader*)shader.get())->mBrush.GetPtr());
    }

    brush->SetOpacity(1.0f);
    brush->SetCenter(Noesis::Point(cx, cy));
    brush->SetGradientOrigin(Noesis::Point(cx, cy));
    brush->SetRadiusX(radius);
    brush->SetRadiusY(radius);
    brush->SetMappingMode(Noesis::BrushMappingMode_Absolute);
    brush->SetSpreadMethod(Noesis::GradientSpreadMethod_Pad);

    UpdateBrushStops(brush->GetGradientStops(), count, stops, colors, mRecycledStops);

    return shader;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<rive::RenderPaint> RiveFactory::makeRenderPaint()
{
    return rivestd::make_unique<RiveRenderPaint>(this);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<rive::RenderPath> RiveFactory::makeRenderPath(rive::RawPath& path,
    rive::FillRule /*fillRule*/)
{
    std::unique_ptr<rive::RenderPath> renderPath = rivestd::make_unique<RiveRenderPath>();
    rive::RenderPath* renderPath_ = renderPath.get();

    int i = 0;
    rive::Span<const rive::Vec2D> points = path.points();

    for (auto verb : path.verbs())
    {
        switch (verb)
        {
            case rive::PathVerb::move:
            {
                const rive::Vec2D& p = points[i++];
                renderPath_->moveTo(p.x, p.y);
                NS_LOG_TRACE("[%p] MoveTo(%.4f,%.4f)", renderPath_, p.x, p.y);
                break;
            }
            case rive::PathVerb::line:
            {
                const rive::Vec2D& p = points[i++];
                renderPath_->lineTo(p.x, p.y);
                NS_LOG_TRACE("[%p] LineTo(%.4f,%.4f)", renderPath_, p.x, p.y);
                break;
            }
            case rive::PathVerb::cubic:
            {
                const rive::Vec2D& p1 = points[i++];
                const rive::Vec2D& p2 = points[i++];
                const rive::Vec2D& p3 = points[i++];
                renderPath_->cubicTo(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                NS_LOG_TRACE("[%p] CubicTo(%.4f,%.4f;%.4f,%.4f;%.4f,%.4f)", renderPath_,
                    p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                break;
            }
            case rive::PathVerb::close:
            {
                renderPath_->close();
                NS_LOG_TRACE("[%p] Close()", this);
                break;
            }

            case rive::PathVerb::quad:
            default:
                NS_ASSERT_UNREACHABLE;
        }
    }

    return renderPath;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<rive::RenderPath> RiveFactory::makeEmptyRenderPath()
{
    return rivestd::make_unique<RiveRenderPath>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
std::unique_ptr<rive::RenderImage> RiveFactory::decodeImage(rive::Span<const uint8_t>)
{
    return rivestd::make_unique<RiveRenderImage>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderBuffer> RiveFactory::makeBufferU16(rive::Span<const uint16_t>)
{
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderBuffer> RiveFactory::makeBufferU32(rive::Span<const uint32_t>)
{
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
rive::rcp<rive::RenderBuffer> RiveFactory::makeBufferF32(rive::Span<const float>)
{
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
RiveRenderer::RiveRenderer(): mContext(nullptr), mContextStackDepth(0), mUsedTransforms(0)
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::Reset(Noesis::DrawingContext* context)
{
    mContext = context;
    mContextStackCounts.Clear();
    mContextStackDepth = 0;
    mUsedTransforms = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Noesis::Ptr<Noesis::MatrixTransform> RiveRenderer::AllocateTransform()
{
    if (mUsedTransforms < mTransformPool.Size())
    {
        return mTransformPool[mUsedTransforms++];
    }

    mUsedTransforms++;
    Noesis::Ptr<Noesis::MatrixTransform> transform = Noesis::MakePtr<Noesis::MatrixTransform>();
    mTransformPool.PushBack(transform);
    return transform;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::save()
{
    mContextStackCounts.PushBack(mContextStackDepth);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::restore()
{
    uint32_t savedDepth = mContextStackCounts.Back();
    mContextStackCounts.PopBack();
    uint32_t count = mContextStackDepth - savedDepth;
    for (uint32_t i = 0; i < count; ++i)
    {
        mContext->Pop();
    }
    mContextStackDepth = savedDepth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::transform(const rive::Mat2D& transform)
{
    auto matrixTransform = AllocateTransform();
    matrixTransform->SetMatrix(Noesis::Transform2(transform.values()));
    mContext->PushTransform(matrixTransform);
    ++mContextStackDepth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::drawPath(rive::RenderPath* path_, rive::RenderPaint* paint_)
{
    RiveRenderPaint* paint = (RiveRenderPaint*)paint_;
    Noesis::Brush* brush = paint->mBrush;
    Noesis::Pen* pen = paint->mPen;

    RiveRenderPath* path = (RiveRenderPath*)path_;
    path->prepareGeometry();

    Noesis::Geometry* geometry = path->mStreamGeometry;
    if (!geometry->GetRenderBounds(pen).IsEmpty())
    {
        switch (paint->mBlendMode)
        {
            case rive::BlendMode::srcOver:
            {
                NS_LOG_TRACE("[%p] DrawPath(Brush=%p, Pen=%p, Blend=SrcOver)", path, brush, pen);

                mContext->DrawGeometry(brush, pen, geometry);
                break;
            }
            case rive::BlendMode::screen:
            case rive::BlendMode::colorDodge:
            case rive::BlendMode::lighten:
            case rive::BlendMode::softLight:
            {
                NS_LOG_TRACE("[%p] DrawPath(Brush=%p, Pen=%p, Blend=Screen)", path, brush, pen);

                mContext->PushBlendingMode(Noesis::BlendingMode_Screen);
                mContext->DrawGeometry(brush, pen, geometry);
                mContext->Pop();
                break;
            }
            case rive::BlendMode::multiply:
            case rive::BlendMode::colorBurn:
            case rive::BlendMode::darken:
            {
                NS_LOG_TRACE("[%p] DrawPath(Brush=%p, Pen=%p, Blend=Multiply)", path, brush, pen);

                mContext->PushBlendingMode(Noesis::BlendingMode_Multiply);
                mContext->DrawGeometry(brush, pen, geometry);
                mContext->Pop();
                break;
            }
            case rive::BlendMode::overlay:
            case rive::BlendMode::hardLight:
            {
                NS_LOG_TRACE("[%p] DrawPath(Brush=%p, Pen=%p, Blend=Muliply+Screen)", path, brush, pen);

                mContext->PushBlendingMode(Noesis::BlendingMode_Multiply);
                mContext->DrawGeometry(brush, pen, geometry);
                mContext->Pop();

                mContext->PushBlendingMode(Noesis::BlendingMode_Screen);
                mContext->DrawGeometry(brush, pen, geometry);
                mContext->Pop();

                break;
            }
            default: break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::clipPath(rive::RenderPath* path_)
{
    RiveRenderPath* path = (RiveRenderPath*)path_;
    path->prepareGeometry();
    Noesis::Geometry* geometry = path->mStreamGeometry;

    NS_LOG_TRACE("[%p] ClipPath()", path);

    mContext->PushClip(geometry);
    ++mContextStackDepth;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::drawImage(const rive::RenderImage* /*image*/, rive::BlendMode /*value*/,
    float /*opacity*/)
{
    // TODO
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void RiveRenderer::drawImageMesh(const rive::RenderImage* /*image*/,
    rive::rcp<rive::RenderBuffer> /*vertices*/,
    rive::rcp<rive::RenderBuffer> /*uvCoords*/,
    rive::rcp<rive::RenderBuffer> /*indices*/,
    rive::BlendMode /*blendMode*/,
    float /*opacity*/)
{
    // TODO
}
