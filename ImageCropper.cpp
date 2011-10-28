#include "ImageCropper.h"
#include <QPainter>
#include <QRect>
#include <QMouseEvent>

ImageCropper::ImageCropper(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(300,300);
    m_cropper = QRect(QPoint(0, 0),
                      QSize(40, 40));

    setMouseTracking(true);
    m_isMousePressed = false;
    m_isProportionFixed = false;
}

ImageCropper::~ImageCropper()
{

}

void ImageCropper::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;

    //QLabel::setPixmap(pixmap);
    update();
}

void ImageCropper::setProportion(QSizeF proportion)
{
    if (m_proportion != proportion) {
        if ( proportion.width() > proportion.height() ) {
            m_proportion.setWidth( (float)proportion.width() / proportion.height() );
            m_proportion.setHeight( 1 );
        } else {
            m_proportion.setHeight( (float)proportion.height() / proportion.width() );
            m_proportion.setWidth( 1 );
        }
    }

    if ( m_isProportionFixed ) {
        if ( ( (float)m_cropper.width()    / m_cropper.height() ) !=
             ( (float)m_proportion.width() / m_proportion.height() ) ) {
            int minSide = m_cropper.width() < m_cropper.height() ? m_cropper.width() : m_cropper.height();

            m_cropper.setWidth( minSide * m_proportion.width() );
            m_cropper.setHeight( minSide * m_proportion.height() );
            update();
        }
    }

}

void ImageCropper::setIsProportionFixed(bool isFixed)
{
    if ( m_isProportionFixed != isFixed ) {
        m_isProportionFixed = isFixed;
        setProportion(m_proportion);
    }
}

void ImageCropper::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent( event );

    /*if ( m_pixmap.size() != this->size() )*/ {
        QPixmap scaledPixmap = m_pixmap.scaled(this->size(),
                                         Qt::KeepAspectRatio,
                                         Qt::FastTransformation),
                                         //Qt::SmoothTransformation),
                shownPixmap(this->size());
        QPainter p(&shownPixmap);
        if ( this->size().height() == scaledPixmap.height() ) {
            p.drawPixmap( ( this->width() - scaledPixmap.width() ) / 2, 0, scaledPixmap );
        } else {
            p.drawPixmap( 0, ( this->height() - scaledPixmap.height() ) / 2, scaledPixmap );
        }
        p.end();

        QPainter widgetPainter(this);
        widgetPainter.fillRect( this->rect(), Qt::black );
        widgetPainter.drawPixmap(0,0,shownPixmap);
        widgetPainter.end();

    }


    /*if ( m_isNeedPaintCropGrid )*/ {
        QPainter painter(this);

        // затемненный фон
        QPainterPath p;
        p.addRect(m_cropper);
        p.addRect(this->rect());
        painter.setBrush(QBrush(QColor(0,0,0,120)));
        painter.setPen(Qt::transparent);
        painter.drawPath(p);

        ////
        // Рамка и контрольные точки
        painter.setPen(Qt::white);
        // Рамка
        painter.setBrush(QBrush(Qt::transparent));
        painter.drawRect(m_cropper);
        // Контрольные точки
        painter.setBrush(QBrush(Qt::white));
        // Вспомогательные параметры
        // X coordinates
        int leftXCoord   = m_cropper.left() - 2;
        int centerXCoord = m_cropper.center().x() - 3;
        int rightXCoord  = m_cropper.right() - 2;
        // Y coordinates
        int topYCoord    = m_cropper.top() - 2;
        int middleYCoord = m_cropper.center().y() - 3;
        int bottomYCoord = m_cropper.bottom() - 2;

        QSize pointSize( 6, 6 );

        QVector<QRect> points;
        points
               // left side
               << QRect( QPoint(leftXCoord, topYCoord), pointSize )
               << QRect( QPoint(leftXCoord, middleYCoord), pointSize )
               << QRect( QPoint(leftXCoord, bottomYCoord), pointSize )
               // center side
               << QRect( QPoint(centerXCoord, topYCoord), pointSize )
               << QRect( QPoint(centerXCoord, middleYCoord), pointSize )
               << QRect( QPoint(centerXCoord, bottomYCoord), pointSize )
               // right side
               << QRect( QPoint(rightXCoord, topYCoord), pointSize )
               << QRect( QPoint(rightXCoord, middleYCoord), pointSize )
               << QRect( QPoint(rightXCoord, bottomYCoord), pointSize );

        painter.drawRects( points );
        // пунктирные линии
        QPen dashPen(Qt::white);
        dashPen.setStyle(Qt::DashLine);
        painter.setPen(dashPen);
        // вертикальная
        painter.drawLine( QPoint(m_cropper.center().x(), m_cropper.top()),
                          QPoint(m_cropper.center().x(), m_cropper.bottom()) );
        // горизонтальная
        painter.drawLine( QPoint(m_cropper.left(), m_cropper.center().y()),
                          QPoint(m_cropper.right(), m_cropper.center().y()) );

        painter.end();
    }
}


void ImageCropper::mousePressEvent(QMouseEvent *ev)
{

    if (ev->type() == QEvent::MouseButtonDblClick ) {
        cropImage();
    } else {
        m_isMousePressed = true;
        m_startMousePos  = ev->pos();
        m_storedCropper = m_cropper;
    }

    this->setCursor( cursorIcon(m_cropper, ev->pos()) );
}

void ImageCropper::mouseMoveEvent(QMouseEvent *ev)
{

    QPoint mouse_pos = ev->pos(); // relative to widget

    if ( !m_isMousePressed ) {
        m_cursorPosition = cursorPosition(m_cropper, mouse_pos);
        this->setCursor( cursorIcon(m_cropper, mouse_pos) );
    }
    else if ( m_cursorPosition != None )
    {
        QPoint mouseDelta;
        mouseDelta.setX( mouse_pos.x() - m_startMousePos.x() );
        mouseDelta.setY( mouse_pos.y() - m_startMousePos.y() );

        if (m_cursorPosition != Middle) {
            QRect newGeometry;
            if ( m_isProportionFixed )
                newGeometry = calculateGeometry(m_storedCropper,
                                                m_cursorPosition,
                                                mouseDelta,
                                                m_proportion);
            else
                newGeometry = calculateGeometry(m_storedCropper,
                                                m_cursorPosition,
                                                mouseDelta);
            m_cropper = newGeometry ;
        } else {
            m_cropper.moveTo( m_storedCropper.topLeft() + mouseDelta );
        }

        update();
    }
}

void ImageCropper::mouseReleaseEvent(QMouseEvent *ev)
{
    m_isMousePressed = false;

    this->setCursor( cursorIcon(m_cropper, ev->pos()) );
}








inline bool ImageCropper::isPointNearSide ( int sideCoordinate,
                                            int pointCoordinate
                                            ) const
{
    const int indent = 10;
    return sideCoordinate - indent < pointCoordinate &&
            pointCoordinate < sideCoordinate + indent;
}

// =====
// Определение местоположения курсора над виджетом
// =====
// widgetSize - размер виджета
// mousePosition - положение мыши относительно виджета (не глобальное)
CursorPosition ImageCropper::cursorPosition ( const QRect cropRect,
                                              const QPoint mousePosition
                                              )
{
    CursorPosition cursorPosition = None;

    if ( cropRect.contains( mousePosition ) ) {


        // Двухстороннее направление
        if ( isPointNearSide( cropRect.top(), mousePosition.y() ) &&
             isPointNearSide( cropRect.left(), mousePosition.x() ) )
            cursorPosition = TopLeft;
        else if ( isPointNearSide( cropRect.bottom(), mousePosition.y() ) &&
                  isPointNearSide( cropRect.left(), mousePosition.x() ) )
            cursorPosition = BottomLeft;
        else if ( isPointNearSide( cropRect.top(), mousePosition.y() )  &&
                  isPointNearSide( cropRect.right(), mousePosition.x() ) )
            cursorPosition = TopRight;
        else if ( isPointNearSide( cropRect.bottom(), mousePosition.y() ) &&
                  isPointNearSide( cropRect.right(), mousePosition.x() ) )
            cursorPosition = BottomRight;
        // Одностороннее направление
        else if ( isPointNearSide( cropRect.left(), mousePosition.x() ) )   cursorPosition = Left;
        else if ( isPointNearSide( cropRect.right(), mousePosition.x() ) )  cursorPosition = Right;
        else if ( isPointNearSide( cropRect.top(), mousePosition.y() ) )    cursorPosition = Top;
        else if ( isPointNearSide( cropRect.bottom(), mousePosition.y() ) ) cursorPosition = Bottom;
        // Без направления
        else cursorPosition = Middle;
    }

    return cursorPosition;
}

// Получить иконку курсора соответствующую местоположению мыши
QCursor ImageCropper::cursorIcon( const QRect widgetSize,
                                  const QPoint mousePosition )
{

    QCursor cursorIcon;

    switch ( cursorPosition( widgetSize, mousePosition ) )
    {
    case TopRight:
    case BottomLeft:
        cursorIcon = QCursor( Qt::SizeBDiagCursor );
        break;
    case TopLeft:
    case BottomRight:
        cursorIcon = QCursor( Qt::SizeFDiagCursor );
        break;
    case Top:
    case Bottom:
        cursorIcon = QCursor( Qt::SizeVerCursor );
        break;
    case Left:
    case Right:
        cursorIcon = QCursor( Qt::SizeHorCursor );
        break;
    case Middle:
        if ( m_isMousePressed ) cursorIcon = QCursor( Qt::ClosedHandCursor );
        else                    cursorIcon = QCursor( Qt::OpenHandCursor );
        break;
    case None:
    default:
        cursorIcon = QCursor( Qt::ArrowCursor );
        break;
    }

    return cursorIcon;
}





// Получить размер виджета после его изменения мышью
// ======
// mouseDelta - разница между начальным положением перемещения мыши и текущим
// ------
// Контракты:
// 1. Метод должен вызываться, только при зажатой кнопке мыши
//    (т.е. при перемещении или изменении размера виджета)
QRect ImageCropper::calculateGeometry(const QRect sourceGeometry,
                                 const CursorPosition cursorPosition,
                                 const QPoint mouseDelta,
                                 const QSizeF proportions
                                 )
{
    QRect resultGeometry;

    if ( proportions.isValid() )
        resultGeometry = calculateGeometryWithFixedProportions(sourceGeometry,
                                                                   cursorPosition,
                                                                   mouseDelta,
                                                                   proportions);
    else
        resultGeometry = calculateGeometryWithCustomProportions(sourceGeometry,
                                                                    cursorPosition,
                                                                    mouseDelta);
        return resultGeometry;
}


// Получить размер виджета после его изменения мышью
// Метод изменяет виджет не сохраняя начальных пропорций сторон
// ------
// Контракты:
// 1. Метод должен вызываться, только при зажатой кнопке мыши
//    (т.е. при перемещении или изменении размера виджета)
QRect ImageCropper::calculateGeometryWithCustomProportions(const QRect sourceGeometry,
                                                      const CursorPosition cursorPosition,
                                                      const QPoint mouseDelta
                                                      )
{
    QRect resultGeometry = sourceGeometry;

    switch ( cursorPosition )
    {
    case TopLeft:
        resultGeometry.setLeft( sourceGeometry.left() + mouseDelta.x() );
        resultGeometry.setTop ( sourceGeometry.top()  + mouseDelta.y() );
        break;
    case TopRight:
        resultGeometry.setTop  ( sourceGeometry.top()   + mouseDelta.y() );
        resultGeometry.setRight( sourceGeometry.right() + mouseDelta.x() );
        break;
    case BottomLeft:
        resultGeometry.setBottom( sourceGeometry.bottom() + mouseDelta.y() );
        resultGeometry.setLeft  ( sourceGeometry.left()   + mouseDelta.x() );
        break;
    case BottomRight:
        resultGeometry.setBottom( sourceGeometry.bottom() + mouseDelta.y() );
        resultGeometry.setRight ( sourceGeometry.right()  + mouseDelta.x() );
        break;
    case Top:
        resultGeometry.setTop( sourceGeometry.top() + mouseDelta.y() );
        break;
    case Bottom:
        resultGeometry.setBottom( sourceGeometry.bottom() + mouseDelta.y() );
        break;
    case Left:
        resultGeometry.setLeft( sourceGeometry.left() + mouseDelta.x() );
        break;
    case Right:
        resultGeometry.setRight( sourceGeometry.right() + mouseDelta.x() );
        break;
    default:
        break;
    }

    return resultGeometry;
}

// Получить размер виджета после его изменения мышью
// Метод изменяет виджет сохраняя начальные пропорции сторон
// ------
// Контракты:
// 1. Метод должен вызываться, только при зажатой кнопке мыши
//    (т.е. при перемещении или изменении размера виджета)
QRect ImageCropper::calculateGeometryWithFixedProportions(const QRect sourceGeometry,
                                                     const CursorPosition cursorPosition,
                                                     const QPoint mouseDelta,
                                                     const QSizeF proportions
                                                 )
{
    int delta = abs(mouseDelta.x()) >= abs(mouseDelta.y()) ? mouseDelta.x() : mouseDelta.y();

    QRect resultGeometry = sourceGeometry;


    switch ( cursorPosition )
    {
    case TopLeft:
    case Left:
        resultGeometry.setTop  ( sourceGeometry.top()   + proportions.height() * delta );
        resultGeometry.setLeft ( sourceGeometry.left()  + proportions.width()  * delta );
        break;
    case TopRight:
    case Right:
    case Top:
        if (abs(mouseDelta.x()) >= abs(mouseDelta.y())) {
            resultGeometry.setTop  ( sourceGeometry.top()   + proportions.height() * delta * -1 );
            resultGeometry.setRight( sourceGeometry.right() + proportions.width()  * delta );
        } else {
            resultGeometry.setTop  ( sourceGeometry.top()   + proportions.height() * delta );
            resultGeometry.setRight( sourceGeometry.right() + proportions.width()  * delta * -1 );
        }
        break;
    case BottomLeft:
        if (abs(mouseDelta.x()) <= abs(mouseDelta.y())) {
            resultGeometry.setBottom( sourceGeometry.bottom() + proportions.height() * delta );
            resultGeometry.setLeft  ( sourceGeometry.left()   + proportions.width()  * delta * -1);
        } else {
            resultGeometry.setBottom( sourceGeometry.bottom() + proportions.height() * delta * -1 );
            resultGeometry.setLeft  ( sourceGeometry.left()   + proportions.width()  * delta );
        }
        break;
    case BottomRight:
    case Bottom:
        resultGeometry.setBottom( sourceGeometry.bottom() + proportions.height() * delta );
        resultGeometry.setRight ( sourceGeometry.right()  + proportions.width()  * delta );
        break;

    default:
        break;
    }

    return resultGeometry;
}

void ImageCropper::cropImage()
{
    /*
      1 Увеличить/уменьшить область обрезки в соответствии с реальной картинкой
      1.1. определить расстояние от края (лево, верх)
      1.2. определить пропорцию области обрезки по отношению к исходному изображению
      1.3. вырезать область из исходного изображения
      обновить оригинальную картинку
      обновить виджет
      */
    QPixmap scaledPixmap = m_pixmap.scaled(this->size(),
                                     Qt::KeepAspectRatio,
                                     Qt::FastTransformation);
    int leftDelta = 0,
        topDelta = 0;

    if ( this->size().height() == scaledPixmap.height() )
        leftDelta = ( this->width() - scaledPixmap.width() ) / 2;
    else
        topDelta = ( this->height() - scaledPixmap.height() ) / 2;

    float xScale = (float)m_pixmap.width()  / scaledPixmap.width(),
          yScale = (float)m_pixmap.height() / scaledPixmap.height();

    QRect realSizeRect( QPoint(m_cropper.left() - leftDelta, m_cropper.top() - topDelta),
                        m_cropper.size() );

    // Top left
    realSizeRect.setLeft( ( m_cropper.left() - leftDelta ) * xScale );
    realSizeRect.setTop ( ( m_cropper.top()  - topDelta  ) * yScale );
    // Size
    realSizeRect.setWidth(  m_cropper.width()  * xScale );
    realSizeRect.setHeight( m_cropper.height() * yScale );

    m_pixmap = m_pixmap.copy( realSizeRect );

    update();
}
