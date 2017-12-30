#include "artboard.h"
#include <QDebug>
#include "stroke.h"

/*********************
 *
 * Artboard Label
 *
 *********************/

ArtboardLabel::ArtboardLabel(QString name, QGraphicsItem *parent) : QGraphicsSimpleTextItem(name, parent)
{
    this->setFlags(QGraphicsItem::ItemIsSelectable |
                   QGraphicsItem::ItemIsFocusable
                   );
}


/*******************************************************************************************************************************/

/*********************
 *
 * Artboard
 *
 *********************/

Artboard::Artboard(QString name, QGraphicsItem *parent) : Artboard(name, 0,0,375,667, parent){}
Artboard::Artboard(QString name, qreal x, qreal y, qreal w, qreal h, QGraphicsItem *parent) : Artboard(name, QRectF(x,y,w,h), parent){}
Artboard::Artboard(QString name, QRectF rect, QGraphicsItem *parent) : ItemBase(parent),
    m_offset(20),
    m_buffer(4),
    m_rect(rect)
{

	Stroke stroke("ArtboardNoneStroke", Qt::transparent, 0);

    m_shadow = new QGraphicsDropShadowEffect();
    m_shadow->setOffset(0,0);
    m_shadow->setColor(QColor(0,0,0));
    m_shadow->setBlurRadius(m_buffer);

    m_artboard = new ItemBase(m_rect);
    m_artboard->setGraphicsEffect(m_shadow);
    m_artboard->setFlag(QGraphicsItem::ItemIsSelectable, false);
    m_artboard->setFlag(QGraphicsItem::ItemIsFocusable, false);
	m_artboard->setStroke(stroke);
    m_artboard->setParentItem(this);

    m_label = new ArtboardLabel(name, this);
    m_label->setPos(boundingRect().x(), boundingRect().y());

    this->setFlag(QGraphicsItem::ItemIsPanel, true);
    this->setAcceptHoverEvents(true);
}

/***************************************************
 *
 * Properties
 *
 ***************************************************/

QGraphicsItem * Artboard::canvas() const
{
    return m_artboard;
}

void Artboard::setRect(QRectF rect)
{
    m_rect = rect;
    update();
    m_artboard->setRect(rect);
    m_artboard->update();

}

QRectF Artboard::rect() const
{
    return m_rect;
}

QRectF Artboard::boundingRect() const
{
    return QRectF(rect().x() - m_buffer, rect().y() - m_offset - m_buffer, rect().width() + 2 * m_buffer, rect().height() + m_offset + 2 * m_buffer);
}


/***************************************************
 *
 * Events
 *
 ***************************************************/

void Artboard::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);


}

