/*************************************************************************************

   Draftoola - UI and UX prototyping tool for designing static and animated layouts.

   Copyright (C) 2019 Martin Reininger <nitramr>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

**************************************************************************************/

#ifndef FILLS_H
#define FILLS_H

#include <QBrush>
#include <QGradient>
#include <QPixmap>
#include <QColor>
#include <QPainter>

#include <itemstruct.h>
#include <abstractitemproperty.h>
#include <gradient.h>
#include <color.h>

class Fills : public AbstractItemProperty
{

     Q_CLASSINFO("Version", "1.0.0")

public:

    enum FillMode {
        Fill = 0,
        Fit = 1,
        Stretch = 2,
        Tile = 3
    };


    // Constructor
    Fills();
    Fills(const QString name);
    Fills(const QString name, const Color & color);
    Fills(const QString name, const QPixmap & pixmap, const FillMode fillMode = FillMode::Fill);
    Fills(const QString name, const QImage & image, const FillMode fillMode = FillMode::Fill);
    Fills(const QString name, const Gradient & gradient);
    Fills(const Fills &other);
    virtual ~Fills(){}


    // Properties
    void setFillType(FillType filltype);
    FillType fillType() const;

    void setFillMode(FillMode fillMode);
    FillMode fillMode() const;

    void setGradient(Gradient gradient);
    virtual Gradient gradient() const;

    void setColor(Color color);
    Color color() const;

    void setPixmap(QPixmap pixmap);
    QPixmap pixmap() const;

    void setOpacity(int opacity);
    int opacity() const;


    bool operator==( const Fills & other ) const;
    inline bool operator!=(const Fills &fill) const { return !(operator==(fill)); }
    friend QDataStream &operator<<(QDataStream &out, const Fills &obj);
    friend QDataStream &operator>>(QDataStream &in, Fills &obj);

#ifndef QT_NO_DEBUG_STREAM
    friend QDebug operator<<(QDebug dbg, const Fills &obj);
#endif

private:

    FillType m_fillType = FillType::Color;
    FillMode m_fillMode = FillMode::Fill;
    Gradient m_gradient;
    Color m_color;
    QPixmap m_pixmap;
    int m_opacity;

    void fromObject(AbstractItemProperty object);

};
Q_DECLARE_METATYPE(Fills)
Q_DECLARE_METATYPE(Fills::FillMode)

#endif // FILLS_H
