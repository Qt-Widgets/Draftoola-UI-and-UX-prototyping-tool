#ifndef ABSTRACTITEMBASE_H
#define ABSTRACTITEMBASE_H

#include <QGraphicsObject>
#include <QPen>
#include <QBrush>
#include <QMap>
#include <QList>

#include "itemstruct.h"
#include "exportlevel.h"

class AbstractItemBase : public QGraphicsObject
{

    friend class ItemBase;
    friend class Artboard;

public:

    AbstractItemBase(const QRectF rect, QGraphicsItem *parent = nullptr);

    // Properties
    void setID(int id);
    int ID() const;

    void setName(QString name);
    QString name() const;

    void setItemType(ItemType type);
    ItemType itemType() const;

    virtual void setShape(QPainterPath itemShape);
    virtual QPainterPath shape() const;

    virtual QRectF rect() const;
    virtual QRectF boundingRect() const;

    void setHighRenderQuality(bool isHighResolution);
    bool highRenderQuality() const;

    void setInvalidateCache(bool invalidate);
    bool invalidateCache() const;

    void setScaleFactor(qreal scaleFactor);
    qreal scaleFactor() const;

    QPointF anchorTopLeft() const;
    QPointF anchorTop() const;
    QPointF anchorTopRight() const;
    QPointF anchorRight() const;
    QPointF anchorBottomRight() const;
    QPointF anchorBottom() const;
    QPointF anchorBottomLeft() const;
    QPointF anchorLeft() const;
    QPointF anchorCenter() const;

    // Members
    void addExportLevel(ExportLevel exportLevel, int index = -1);
    void removeExportLevel(int index);
    void removeExportLevel(ExportLevel exportLevel);
    void updateExportLevel(ExportLevel exportLevel);
    QList<ExportLevel> exportLevels() const;
    ExportLevel exportLevel(int index);

//    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) = 0;

private:
    // Properties
    int m_id;
    QRectF m_rect;
    QRectF m_boundingRect;
    QString m_name;
    bool m_invaliateCache;
    ItemType m_itemType;
    qreal m_scaleFactor;
    bool m_renderQuality;
    QPainterPath m_shape;

    void setBoundingRect(QRectF boundingrect);

    // Members
    QList<ExportLevel>	m_exportFactorList;
};

#endif // ABSTRACTITEMBASE_H
