#include "handleframe.h"

#include <QDebug>

ItemHandle::ItemHandle(QGraphicsItem *parent,  Handle corner, int buffer) :
    QGraphicsItem(parent),
    mouseDownX(0),
    mouseDownY(0),
    m_Pen(),
    m_corner(corner),
    m_mouseButtonState(kMouseReleased)
{
    setParentItem(parent);

    m_buffer = buffer;
    m_width = 2 * m_buffer;
    m_height = 2 * m_buffer;
    m_color = QColor(0, 128, 255);

    m_Pen.setWidth(1);
    m_Pen.setColor(m_color);

    QGraphicsDropShadowEffect *m_shadow = new QGraphicsDropShadowEffect();
    m_shadow->setOffset(0,0);
    m_shadow->setColor(QColor(0,0,0));
    m_shadow->setBlurRadius(4);
    this->setGraphicsEffect(m_shadow);

    this->setAcceptHoverEvents(true);
}

/***************************************************
 *
 * Members
 *
 ***************************************************/

void ItemHandle::setMouseState(int s)
{
    m_mouseButtonState = s;
}

int ItemHandle::getMouseState()
{
    return m_mouseButtonState;
}

QRectF ItemHandle::rect() const
{
	return QRectF(boundingRect().x() + 2, boundingRect().y() + 2, boundingRect().width() - 4, boundingRect().height() - 4);
}

QRectF ItemHandle::rectAdjusted() const
{
	return QRectF(rect().x() + 0.5, rect().y() + 0.5 , rect().width() - 1, rect().height() - 1);
}

ItemHandle::Handle ItemHandle::getCorner()
{
    return m_corner;
}

/***************************************************
 *
 * Events
 *
 ***************************************************/

// we have to implement the mouse events to keep the linker happy,
// but just set accepted to false since are not actually handling them

void ItemHandle::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void ItemHandle::mousePressEvent(QGraphicsSceneDragDropEvent *event)
{
    event->setAccepted(false);
}

void ItemHandle::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
}

void ItemHandle::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(false);
}

void ItemHandle::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(false);
}


// change the color on hover events to indicate to the use the object has
// been captured by the mouse

void ItemHandle::hoverLeaveEvent ( QGraphicsSceneHoverEvent * )
{
    //    m_color = m_selectionColor;
    //    this->update(0,0,m_width,m_height);
}

void ItemHandle::hoverEnterEvent ( QGraphicsSceneHoverEvent * )
{
    //    m_color = m_highlightColor;
    //    this->update(0,0,m_width,m_height);
}

QRectF ItemHandle::boundingRect() const
{
    return QRectF(0,0,m_width,m_height);
}


void ItemHandle::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

    painter->setPen(m_Pen);
    painter->fillRect(this->rect(), Qt::white);
	painter->drawRect(this->rectAdjusted());

}

/*******************************************************************************************************************************/

/*******************************
 *
 * Handle Frame
 *
 *******************************/

HandleFrame::HandleFrame(int buffer):
    m_dragStart(0,0),
    m_gridSpace(1),
    m_cornerDragStart(0,0),
    m_buffer(buffer),
    m_resizeOnly(false)
{
    m_isDragging = false;

    m_corners[0] = new ItemHandle(this,ItemHandle::TopLeft, m_buffer);
    m_corners[1] = new ItemHandle(this,ItemHandle::Top, m_buffer);
    m_corners[2] = new ItemHandle(this,ItemHandle::TopRight, m_buffer);
    m_corners[3] = new ItemHandle(this,ItemHandle::Right, m_buffer);
    m_corners[4] = new ItemHandle(this,ItemHandle::BottomRight, m_buffer);
    m_corners[5] = new ItemHandle(this,ItemHandle::Bottom, m_buffer);
    m_corners[6] = new ItemHandle(this,ItemHandle::BottomLeft, m_buffer);
    m_corners[7] = new ItemHandle(this,ItemHandle::Left, m_buffer);

    this->setPen(QPen(QColor(0, 128, 255)));
    this->setBrush(QBrush(Qt::NoBrush));
    this->setVisible(false);
    this->setAcceptHoverEvents(true);
    this->setFlags(QGraphicsItem::ItemDoesntPropagateOpacityToChildren
                   );
}


/***************************************************
 *
 * Properties
 *
 ***************************************************/

int HandleFrame::buffer() const
{
    return m_buffer;
}


void HandleFrame::setHost(ItemBase *host)
{
    m_host = host;
}


bool HandleFrame::isDragging()
{
    return m_isDragging;
}

void HandleFrame::setIsResize(bool resizeOnly)
{
    m_resizeOnly = resizeOnly;
}

bool HandleFrame::isResize()
{
    return m_resizeOnly;
}

/***************************************************
 *
 * Members
 *
 ***************************************************/

void HandleFrame::adjustSize(int x, int y)
{
    this->setRect(
                this->rect().x(),
                this->rect().y(),
                this->rect().width() + x,
                this->rect().height() + y
                );
}


void HandleFrame::mapToHost()
{
        QPointF mapScene = m_host->mapFromScene(this->scenePos());
        QPointF mapItem = m_host->mapToParent(mapScene);
        QPointF offset(m_buffer, m_buffer);

        m_host->setPos(mapItem + offset);
}


void HandleFrame::setCornerPositions()
{
    qreal posX = rect().x();
    qreal posY = rect().y();
    qreal width = rect().width();
    qreal height = rect().height();

    m_corners[0]->setPos(posX, posY); // TopLeft
    m_corners[1]->setPos(posX + width / 2 - m_buffer, posY); // Top
    m_corners[2]->setPos(posX + width - 2 * m_buffer,  posY); // TopRight
    m_corners[3]->setPos(posX + width - 2 * m_buffer,  posY + height / 2 - m_buffer); // Right
    m_corners[4]->setPos(posX + width - 2 * m_buffer, posY + height - 2 * m_buffer); // BottomRight
    m_corners[5]->setPos(posX + width / 2 - m_buffer, posY + height - 2 * m_buffer); // Bottom
    m_corners[6]->setPos(posX, posY + height - 2 * m_buffer); // BottomLeft
    m_corners[7]->setPos(posX, posY + height / 2 - m_buffer); // Left

    if(!m_host) return;

    QRectF drawableRect(posX, posY, width - 2 * m_buffer, height - 2 * m_buffer);
    m_host->setRect(drawableRect);

    mapToHost();

//    qDebug() << "---------------------- " ;
//    qDebug() << "HandleFrame: HandleFrame Scene Pos: " << scenePos();
//    qDebug() << "HandleFrame: Host Scene Pos: " << m_host->scenePos();
//    qDebug() << "HandleFrame: Handle InnerFramePos: " << drawablePoint << "/" << pos();
//    qDebug() << "HandleFrame: Handle InnerFrameSize: " << drawableRect << "/" << boundingRect();

}


void HandleFrame::installFilter()
{
    m_corners[0]->installSceneEventFilter(this);
    m_corners[1]->installSceneEventFilter(this);
    m_corners[2]->installSceneEventFilter(this);
    m_corners[3]->installSceneEventFilter(this);
    m_corners[4]->installSceneEventFilter(this);
    m_corners[5]->installSceneEventFilter(this);
    m_corners[6]->installSceneEventFilter(this);
    m_corners[7]->installSceneEventFilter(this);
}

/***************************************************
 *
 * Events
 *
 ***************************************************/

bool HandleFrame::sceneEventFilter ( QGraphicsItem * watched, QEvent * event )
{
    ItemHandle * corner = dynamic_cast<ItemHandle *>(watched);
    if ( corner == NULL) return false; // not expected to get here

    QGraphicsSceneMouseEvent * mevent = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
    if ( mevent == NULL)
    {
        // this is not one of the mouse events we are interrested in
        return false;
    }


    switch (event->type() )
    {
    // if the mouse went down, record the x,y coords of the press, record it inside the corner object
    case QEvent::GraphicsSceneMousePress:
    {
        corner->setMouseState(ItemHandle::kMouseDown);
        corner->mouseDownX = mevent->pos().x();
        corner->mouseDownY = mevent->pos().y();
    }
        break;

    case QEvent::GraphicsSceneMouseRelease:
    {
        corner->setMouseState(ItemHandle::kMouseReleased);

    }
        break;

    case QEvent::GraphicsSceneMouseMove:
    {
        corner->setMouseState(ItemHandle::kMouseMoving );
    }
        break;

    default:
        // we dont care about the rest of the events
        return false;
        break;
    }


    if ( corner->getMouseState() == ItemHandle::kMouseMoving )
    {

        qreal x = mevent->pos().x(), y = mevent->pos().y();

        // depending on which corner has been grabbed, we want to move the position
        // of the item as it grows/shrinks accordingly. so we need to eitehr add
        // or subtract the offsets based on which corner this is.

        int XaxisSign = 0;
        int YaxisSign = 0;
        switch( corner->getCorner() )
        {
        case ItemHandle::TopLeft:
        {
            XaxisSign = +1;
            YaxisSign = +1;
        }
            break;
        case ItemHandle::Top:
        {
            XaxisSign = 0;
            YaxisSign = +1;
        }
            break;

        case ItemHandle::TopRight:
        {
            XaxisSign = -1;
            YaxisSign = +1;
        }
            break;
        case ItemHandle::Right:
        {
            XaxisSign = -1;
            YaxisSign = 0;
        }
            break;

        case ItemHandle::BottomRight:
        {
            XaxisSign = -1;
            YaxisSign = -1;
        }
            break;
        case ItemHandle::Bottom:
        {
            XaxisSign = 0;
            YaxisSign = -1;
        }
            break;

        case ItemHandle::BottomLeft:
        {
            XaxisSign = +1;
            YaxisSign = -1;
        }
            break;
        case ItemHandle::Left:
        {
            XaxisSign = +1;
            YaxisSign = 0;
        }
            break;

        }

        // if the mouse is being dragged, calculate a new size and also re-position
        // the box to give the appearance of dragging the corner out/in to resize the box

        int xMoved = corner->mouseDownX - x;
        int yMoved = corner->mouseDownY - y;

        int newWidth = this->rect().width() + ( XaxisSign * xMoved);
        if ( newWidth < 40 ) newWidth  = 40;

        int newHeight = this->rect().height() + (YaxisSign * yMoved) ;
        if ( newHeight < 40 ) newHeight = 40;

        int deltaWidth  =   newWidth - this->rect().width() ;
        int deltaHeight =   newHeight - this->rect().height() ;

        adjustSize(  deltaWidth ,   deltaHeight);

        deltaWidth *= (-1);
        deltaHeight *= (-1);

        int newXpos;
        int newYpos;

        switch(corner->getCorner()){
        case ItemHandle::TopLeft:
            newXpos = this->pos().x() + deltaWidth;
            newYpos = this->pos().y() + deltaHeight;
            this->setPos(newXpos, newYpos);

            break;
        case ItemHandle::Top:
        case ItemHandle::TopRight:
            newYpos = this->pos().y() + deltaHeight;
            this->setPos(this->pos().x(), newYpos);

            break;
        case ItemHandle::Left:
        case ItemHandle::BottomLeft:
            newXpos = this->pos().x() + deltaWidth;
            this->setPos(newXpos,this->pos().y());

            break;
        }


        setCornerPositions();

        this->update();
    }

    return true;// true => do not send event to watched - we are finished with this event
}


void HandleFrame::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    m_isDragging = false;
   // this->setPos( ( static_cast<int>(this->x()) / m_gridSpace) * m_gridSpace, (static_cast<int>(this->y()) / m_gridSpace) * m_gridSpace );
}


void HandleFrame::mousePressEvent ( QGraphicsSceneMouseEvent * event )
{
    event->setAccepted(true);
    m_isDragging = true;
    m_dragStart = event->pos();

}


void HandleFrame::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
    if(!m_resizeOnly){
        QPointF newPos = event->pos() ;
        QPointF m_loc = (newPos - m_dragStart);
        QPointF origin(this->pos() + m_loc);
        this->setPos(origin);

        mapToHost();
    }

}


void HandleFrame::hoverLeaveEvent ( QGraphicsSceneHoverEvent * ){}
void HandleFrame::hoverEnterEvent ( QGraphicsSceneHoverEvent * ){}


void HandleFrame::paint (QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{

    int posX = this->rect().x();
    int posY = this->rect().y();

    painter->setPen(this->pen());

	QRectF selection(posX + m_buffer + 0.5, posY + m_buffer + 0.5, this->rect().width() - 2 * m_buffer - 1, this->rect().height() - 2 * m_buffer - 1);
    painter->drawRect(selection);


}


//void HandleFrame::mouseMoveEvent(QGraphicsSceneDragDropEvent *event)
//{
//    event->setAccepted(false);
//}

//void HandleFrame::mousePressEvent(QGraphicsSceneDragDropEvent *event)
//{
//    event->setAccepted(false);

//}
