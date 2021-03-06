/**
 * \file gradient_slider.cpp
 *
 * \author Mattia Basaglia
 *
 * \copyright Copyright (C) 2013-2019 Mattia Basaglia
 * \copyright Copyright (C) 2014 Calle Laakkonen
 * \copyright Copyright (C) 2017 caryoscelus
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "QtColorWidgets/gradient_slider.hpp"

#include <QPainter>
#include <QStyleOptionSlider>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QDebug>
#include <QStylePainter>
//#include <utilities.h>
//#include "QtColorWidgets/color_utils.hpp"


namespace color_widgets {

static const double selectorSize = 6;

class GradientSlider::Private
{
public:
    QLinearGradient gradient;
    QBrush back;

    Private() :
        back(Qt::darkGray, Qt::DiagCrossPattern)
    {        
        back.setTexture(QPixmap(QStringLiteral(":/color_widgets/alphaback.png")));
        gradient.setCoordinateMode(QGradient::StretchToDeviceMode);
        gradient.setSpread(QGradient::RepeatSpread);
    }

    void mouse_event(QMouseEvent *ev, GradientSlider* owner)
    {
        qreal pos = (owner->geometry().width() > selectorSize) ?
            static_cast<qreal>(ev->pos().x() - selectorSize/2) / (owner->geometry().width() - selectorSize) : 0;
        pos = qMax(qMin(pos, 1.0), 0.0);
        owner->setSliderPosition(qRound(owner->minimum() +
            pos * (owner->maximum() - owner->minimum())));
    }

};

GradientSlider::GradientSlider(QWidget *parent) :
    GradientSlider(Qt::Horizontal, parent)
{}

GradientSlider::GradientSlider(Qt::Orientation orientation, QWidget *parent) :
    QSlider(orientation, parent), p(new Private)
{
    setTickPosition(NoTicks);
}

GradientSlider::~GradientSlider()
{
    delete p;
}

void GradientSlider::mousePressEvent(QMouseEvent *ev)
{
    if ( ev->button() == Qt::LeftButton )
    {
        ev->accept();
        setSliderDown(true);
        p->mouse_event(ev, this);
        update();
    }
    else
    {
        QSlider::mousePressEvent(ev);
    }
}

void GradientSlider::mouseMoveEvent(QMouseEvent *ev)
{
    if ( ev->buttons() & Qt::LeftButton )
    {
        ev->accept();
        p->mouse_event(ev, this);
        update();
    }
    else
    {
        QSlider::mouseMoveEvent(ev);
    }
}

void GradientSlider::mouseReleaseEvent(QMouseEvent *ev)
{
    if ( ev->button() == Qt::LeftButton )
    {
        ev->accept();
        setSliderDown(false);
        update();
    }
    else
    {
        QSlider::mousePressEvent(ev);
    }
}

QBrush GradientSlider::background() const
{
    return p->back;
}

void GradientSlider::setBackground(const QBrush &bg)
{
    p->back = bg;
    update();
    Q_EMIT backgroundChanged(bg);
}

QGradientStops GradientSlider::colors() const
{
    return p->gradient.stops();
}

void GradientSlider::setColors(const QGradientStops &colors)
{
    p->gradient.setStops(colors);
    update();
}

QLinearGradient GradientSlider::gradient() const
{
    return p->gradient;
}

void GradientSlider::setGradient(const QLinearGradient &gradient)
{
    p->gradient = gradient;
    update();
}

void GradientSlider::setColors(const QVector<QColor> &colors)
{
    QGradientStops stops;
    stops.reserve(colors.size());

    double c = colors.size() - 1;
    if(c==0) {
        stops.append(QGradientStop(0, colors.at(0)));

    } else {
        for(int i=0;i<colors.size();++i) {
            stops.append(QGradientStop(i/c, colors.at(i)));
        }
    }
    setColors(stops);
}

void GradientSlider::setFirstColor(const QColor &c)
{
    QGradientStops stops = p->gradient.stops();
    if(stops.isEmpty())
        stops.push_back(QGradientStop(0.0, c));
    else
        stops.front().second = c;
    p->gradient.setStops(stops);

    update();
}

void GradientSlider::setLastColor(const QColor &c)
{
    QGradientStops stops = p->gradient.stops();
    if(stops.size()<2)
        stops.push_back(QGradientStop(1.0, c));
    else
        stops.back().second = c;
    p->gradient.setStops(stops);
    update();
}

QColor GradientSlider::firstColor() const
{
    QGradientStops s = colors();
    return s.empty() ? QColor() : s.front().second;
}

QColor GradientSlider::lastColor() const
{
    QGradientStops s = colors();
    return s.empty() ? QColor() : s.back().second;
}

void GradientSlider::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);

    QRect drawRect(selectorSize/2 + 1, selectorSize/2 + 1, geometry().width()-selectorSize -2, geometry().height()-selectorSize -2);

    qreal gradient_direction = invertedAppearance() ? -1 : 1;

    if(orientation() == Qt::Horizontal){
        p->gradient.setFinalStop(gradient_direction, 0);
    }
    else{
        p->gradient.setFinalStop(0, -gradient_direction);
    }

    QStyleOptionFrame panel;
    panel.initFrom(this);
    panel.lineWidth = 1;
    panel.midLineWidth = 0;
    panel.rect = drawRect;
    panel.state |= QStyle::State_Sunken;
    style()->drawPrimitive(QStyle::PE_Frame, &panel, &painter, this);
    QRect r = style()->subElementRect(QStyle::SE_FrameContents, &panel, this);

    painter.setPen(Qt::NoPen);
    painter.fillRect(r, p->back);
    painter.fillRect(r, p->gradient);

    qreal pos = (maximum() != 0) ?
        static_cast<qreal>(value() - minimum()) / maximum() : 0;
    QColor color;
    auto stops = p->gradient.stops();
    int i;
    for (i = 0; i < stops.size(); i++) {
        if (stops[i].first > pos)
            break;
    }
    if (i == 0) {
        color = firstColor();
    } if (i == stops.size()) {
        color = lastColor();
    } else {
        auto &a = stops[i - 1];
        auto &b = stops[i];
        auto c = (b.first - a.first);
        qreal q = (c != 0) ?
            (pos - a.first) / c : 0;
        color = QColor::fromRgbF(b.second.redF() * q + a.second.redF() * (1.0 - q),
            b.second.greenF() * q + a.second.greenF() * (1.0 - q),
            b.second.blueF() * q + a.second.blueF() * (1.0 - q),
            b.second.alphaF() * q + a.second.alphaF() * (1.0 - q));
    }

    pos = pos * (geometry().width() - selectorSize*2) + selectorSize;
//    if (color.valueF() > 0.5 || color.alphaF() < 0.5) {
//        painter.setPen(QPen(Qt::black, hWidth));
//    } else {
//        painter.setPen(QPen(Qt::white, hWidth));
//    }

//    painter.setPen(QPen(Qt::black, hWidth));

//    QPointF p1 = QPointF(2.5, 2.5) + QPointF(pos, 0);
//    QPointF p2 = p1 + QPointF(0, geometry().height() - 5);
//    painter.drawLine(p1, p2);

//    painter.setRenderHint(QPainter::Antialiasing, true);

//    QPen penBorder(Qt::darkGray, hWidth);
//    penBorder.setCapStyle(Qt::RoundCap);
//    painter.setPen(penBorder);
//    painter.drawLine(p1, p2);

//    QPen penFill(Qt::white, hWidth-2);
//    penFill.setCapStyle(Qt::RoundCap);
//    painter.setPen(penFill);
//    painter.drawLine(p1, p2);


//    QStyleOptionSlider handle;
//     handle.init(this);
//     handle.orientation = this->orientation();
//     handle.minimum = this->minimum();
//     handle.maximum = this->maximum();
//     handle.sliderPosition = this->value();
//     handle.sliderValue = this->value();
//     handle.subControls = QStyle::SC_SliderHandle;
//     painter.drawComplexControl(QStyle::CC_Slider, handle);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::darkGray, 1));
    painter.setBrush(QBrush(Qt::white));
    painter.drawEllipse(QPointF(pos, height()/2), selectorSize,selectorSize);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(color));
    painter.drawEllipse(QPointF(pos, height()/2), selectorSize-3,selectorSize-3);



}

} // namespace color_widgets
