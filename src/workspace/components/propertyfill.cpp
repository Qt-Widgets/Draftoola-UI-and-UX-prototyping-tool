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

#include "propertyfill.h"
#include "ui_propertyfill.h"

#include <QPainter>
#include <QDebug>
#include <utilities.h>
#include <popupmenu.h>


PropertyFill::PropertyFill(QWidget *parent) : PropertyFill(Fills(), parent){}

PropertyFill::PropertyFill(Fills fill, QWidget *parent) : QWidget(parent),
ui(new Ui::propertyFill)
{
    ui->setupUi(this);

    m_colorDialog = new ColorDialog;

    PopupMenu * m_colorMenu = new PopupMenu(m_colorDialog);

    QPixmap pixmap(ui->btn_color->iconSize());
    pixmap.fill(fill.color());
    ui->btn_color->setIcon(pixmap);
    ui->btn_color->setMenu(m_colorMenu);

    // https://doc.qt.io/qt-5/qpainter.html#CompositionMode-enum
    ui->combo_blending->addItem(tr("Normal"), QVariant(QPainter::CompositionMode_SourceOver)); //0
    ui->combo_blending->insertSeparator(ui->combo_blending->count());
    ui->combo_blending->addItem(tr("Darken"), QVariant(QPainter::CompositionMode_Darken)); //16
    ui->combo_blending->addItem(tr("Multiply"), QVariant(QPainter::CompositionMode_Multiply)); //13
    ui->combo_blending->addItem(tr("Color Burn"), QVariant(QPainter::CompositionMode_ColorBurn)); //19
    ui->combo_blending->insertSeparator(ui->combo_blending->count());
    ui->combo_blending->addItem(tr("Lighten"), QVariant(QPainter::CompositionMode_Lighten)); //17
    ui->combo_blending->addItem(tr("Screen"), QVariant(QPainter::CompositionMode_Screen)); //14
    ui->combo_blending->addItem(tr("Color Dodge"), QVariant(QPainter::CompositionMode_ColorDodge)); //18
    ui->combo_blending->insertSeparator(ui->combo_blending->count());
    ui->combo_blending->addItem(tr("Overlay"), QVariant(QPainter::CompositionMode_Overlay)); //15
    ui->combo_blending->addItem(tr("Soft Light"), QVariant(QPainter::CompositionMode_SoftLight)); //21
    ui->combo_blending->addItem(tr("Hard Light"), QVariant(QPainter::CompositionMode_HardLight)); //20
    ui->combo_blending->addItem(tr("Difference"), QVariant(QPainter::CompositionMode_Difference)); //22
    ui->combo_blending->addItem(tr("Exclusion"), QVariant(QPainter::CompositionMode_Exclusion)); //23
    ui->combo_blending->insertSeparator(ui->combo_blending->count());
    ui->combo_blending->addItem(tr("Plus"), QVariant(QPainter::CompositionMode_Plus)); //12

    setFill(fill);
}

PropertyFill::~PropertyFill()
{
    delete ui;
}

void PropertyFill::setFill(Fills fill)
{
    disconnectSlots();

    m_fill = fill;

    ui->cb_active->setChecked(fill.isOn());
    ui->sb_opacity->setValue(fill.opacity()*100);
    m_colorDialog->setProperty(&m_fill);

    int indexMode = 0;
    for(int i = 0; i < ui->combo_blending->count(); i++){
        if(fill.blendMode() == static_cast<QPainter::CompositionMode>(ui->combo_blending->itemData(i).toInt())){
            indexMode = i;
            break;
        }
    }

    ui->combo_blending->setCurrentIndex(indexMode);

    drawFill(fill);

    connectSlots();
}

Fills PropertyFill::fill() const
{
    return m_fill;
}

void PropertyFill::drawFill(Fills fill)
{
    QPixmap pixmap(ui->btn_color->iconSize());
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setOpacity(fill.opacity());

    switch (fill.fillType()) {
    case FillType::Color:{
      //  painter.setOpacity(1.0); // opacity is definied by color alpha channel
        painter.fillRect(pixmap.rect(), QBrush(fill.color()));
        break;
    }
    case FillType::RadialGradient:
        painter.fillRect(pixmap.rect(), QBrush(fill.gradient().radial(pixmap.rect())));
        break;
    case FillType::ConicalGradient:
        painter.fillRect(pixmap.rect(), QBrush(fill.gradient().conical(pixmap.rect())));
        break;
    case FillType::LinearGradient:
        painter.fillRect(pixmap.rect(), QBrush(fill.gradient().linear()));
        break;
    case FillType::Image:
        painter.drawPixmap(pixmap.rect(), fill.pixmap(), fill.pixmap().rect());
        break;
    case FillType::Pattern:
        break;
    default:
        break;
    }

    ui->btn_color->setIcon(pixmap);
}

void PropertyFill::connectSlots()
{
    connect(ui->btnDelete, &QToolButton::clicked, this, &PropertyFill::removeClick);
    connect(ui->cb_active, &QCheckBox::clicked, this, &PropertyFill::updateFill);
    connect(ui->sb_opacity, QOverload<int>::of(&QSpinBox::valueChanged), this, &PropertyFill::updateOpacity);
    connect(ui->combo_blending, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PropertyFill::updateFill);
    connect(m_colorDialog, &ColorDialog::propertyChanged, this, &PropertyFill::updateColor);
}

void PropertyFill::disconnectSlots()
{
    disconnect(ui->btnDelete, &QToolButton::clicked, this, &PropertyFill::removeClick);
    disconnect(ui->cb_active, &QCheckBox::clicked, this, &PropertyFill::updateFill);
    disconnect(ui->sb_opacity, QOverload<int>::of(&QSpinBox::valueChanged), this, &PropertyFill::updateOpacity);
    disconnect(ui->combo_blending, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PropertyFill::updateFill);
    disconnect(m_colorDialog, &ColorDialog::propertyChanged, this, &PropertyFill::updateColor);
}

void PropertyFill::updateFill()
{
    m_fill.setIsOn(ui->cb_active->isChecked());
    m_fill.setBlendMode(static_cast<QPainter::CompositionMode>(ui->combo_blending->currentData().toInt()) );

//    qDebug() << "PropertyFill::updateFill()" << ui->sb_opacity->value()/100.0;
//    qDebug() << "PropertyFill opacity" << m_fill.opacity();

//    disconnectSlots();
//    m_colorDialog->setProperty(&m_fill);
//    connectSlots();

    // update preview
    drawFill(m_fill);

    emit hasChanged(true);
}

void PropertyFill::updateColor()
{
    m_fill = *m_colorDialog->fill();

    disconnectSlots();
    ui->sb_opacity->setValue( qRound(m_fill.opacity() * 100) );
    connectSlots();

    updateFill();
}

void PropertyFill::updateOpacity()
{

    m_fill.setOpacity(ui->sb_opacity->value()/100.0);
    m_colorDialog->setProperty(&m_fill);

    updateFill();
}

void PropertyFill::removeClick()
{
    emit remove(this);
}
