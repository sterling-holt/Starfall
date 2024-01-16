#include "rive/core/type_conversions.hpp"
#include "rive/shapes/metrics_path.hpp"
#include "rive/renderer.hpp"
#include "rive/math/cubic_utilities.hpp"
#include "rive/math/raw_path.hpp"
#include "rive/math/contour_measure.hpp"

using namespace rive;

void MetricsPath::reset()
{
    m_Paths.clear();
    m_Contour.reset(nullptr);
    m_RawPath = RawPath();
    m_ComputedLengthTransform = Mat2D();
    m_ComputedLength = 0;
}

void MetricsPath::addPath(CommandPath* path, const Mat2D& transform)
{
    MetricsPath* metricsPath = static_cast<MetricsPath*>(path);
    m_ComputedLength += metricsPath->computeLength(transform);
    m_Paths.emplace_back(metricsPath);
}

void MetricsPath::moveTo(float x, float y)
{
    assert(m_RawPath.points().size() == 0);
    m_RawPath.move({x, y});
}

void MetricsPath::lineTo(float x, float y) { m_RawPath.line({x, y}); }

void MetricsPath::cubicTo(float ox, float oy, float ix, float iy, float x, float y)
{
    m_RawPath.cubic({ox, oy}, {ix, iy}, {x, y});
}

void MetricsPath::close()
{
    // Should we pass the close() to our m_RawPath ???
}

float MetricsPath::computeLength(const Mat2D& transform)
{
    // Only compute if our pre-computed length is not valid
    if (!m_Contour || transform != m_ComputedLengthTransform)
    {
        m_ComputedLengthTransform = transform;
        m_Contour = ContourMeasureIter(m_RawPath * transform).next();
        m_ComputedLength = m_Contour ? m_Contour->length() : 0;
    }
    return m_ComputedLength;
}

void MetricsPath::trim(float startLength, float endLength, bool moveTo, RenderPath* result)
{
    assert(endLength >= startLength);
    if (!m_Paths.empty())
    {
        m_Paths.front()->trim(startLength, endLength, moveTo, result);
        return;
    }

    // TODO: if we can change the signature of MetricsPath and/or trim() to speak native
    //       rawpaths, we wouldn't need this temporary copy (since ContourMeasure speaks
    //       native rawpaths).
    RawPath tmp;
    m_Contour->getSegment(startLength, endLength, &tmp, moveTo);
    tmp.addTo(result);
}

RenderMetricsPath::RenderMetricsPath(std::unique_ptr<RenderPath> path) :
    m_RenderPath(std::move(path))
{}

void RenderMetricsPath::addPath(CommandPath* path, const Mat2D& transform)
{
    MetricsPath::addPath(path, transform);
    m_RenderPath->addPath(path->renderPath(), transform);
}

void RenderMetricsPath::reset()
{
    MetricsPath::reset();
    m_RenderPath->reset();
}

void RenderMetricsPath::moveTo(float x, float y)
{
    MetricsPath::moveTo(x, y);
    m_RenderPath->moveTo(x, y);
}

void RenderMetricsPath::lineTo(float x, float y)
{
    MetricsPath::lineTo(x, y);
    m_RenderPath->lineTo(x, y);
}

void RenderMetricsPath::cubicTo(float ox, float oy, float ix, float iy, float x, float y)
{
    MetricsPath::cubicTo(ox, oy, ix, iy, x, y);
    m_RenderPath->cubicTo(ox, oy, ix, iy, x, y);
}

void RenderMetricsPath::close()
{
    MetricsPath::close();
    m_RenderPath->close();
}

void RenderMetricsPath::fillRule(FillRule value) { m_RenderPath->fillRule(value); }
