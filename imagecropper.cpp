#include "imagecropper.h"

#include <QPainter>
#include <QRect>
#include <QMouseEvent>

namespace {
	const QSize MINIMUM_SIZE = QSize(300, 300);
}

ImageCropper::ImageCropper(QWidget* parent) :
	QWidget(parent),
	p_impl(new ImageCropperPrivate)
{
	setMinimumSize(MINIMUM_SIZE);
	setMouseTracking(true);
}

ImageCropper::~ImageCropper()
{

}

void ImageCropper::setImage(const QPixmap& _image)
{
	p_impl->imageForCropping = _image;
    update();
}

void ImageCropper::setBackgroundColor(const QColor& _backgroundColor)
{
	p_impl->backgroundColor = _backgroundColor;
	update();
}

void ImageCropper::setCroppingRectBorderColor(const QColor& _borderColor)
{
	p_impl->croppingRectBorderColor = _borderColor;
	update();
}

void ImageCropper::setProportion(const QSizeF& _proportion)
{
	// Пропорции хранятся в коэффициентах приращения сторон
	// в интервале значений от нуля до единицы (0;1].
	// Таким образом, при изменении размера области выделения,
	// размеры сторон изменяются на размер зависящий от
	// коэффициентов приращения.

	// Сохраним пропорциональную зависимость области выделения в коэффициентах приращения сторон
	if (p_impl->proportion != _proportion) {
		// ... расчитаем коэффициенты
		float heightDelta = 1;
		float widthDelta = 1;
		if (_proportion.width() > _proportion.height()) {
			widthDelta = (float)_proportion.width() / _proportion.height();
        } else {
			heightDelta = (float)_proportion.height() / _proportion.width();
        }
		// ... сохраним коэффициенты
		p_impl->proportion.setHeight(heightDelta);
		p_impl->proportion.setWidth(widthDelta);
    }

	// Обновим пропорции области выделения
	if ( p_impl->isProportionFixed ) {
		float croppintRectSideRelation =
				(float)p_impl->croppingRect.width() / p_impl->croppingRect.height();
		float proportionSideRelation =
				(float)p_impl->proportion.width() / p_impl->proportion.height();
		// Если область выделения не соответствует необходимым пропорциям обновим её
		if (croppintRectSideRelation != proportionSideRelation) {
			float minSide = p_impl->croppingRect.width() < p_impl->croppingRect.height() ? p_impl->croppingRect.width() : p_impl->croppingRect.height();

			p_impl->croppingRect.setWidth( minSide * p_impl->proportion.width() );
			p_impl->croppingRect.setHeight( minSide * p_impl->proportion.height() );
            update();
        }
    }

}

void ImageCropper::setIsProportionFixed(bool isFixed)
{
	if ( p_impl->isProportionFixed != isFixed ) {
		p_impl->isProportionFixed = isFixed;
		setProportion(p_impl->proportion);
	}
}

QPixmap ImageCropper::cropImage()
{
	/*
	  1 Увеличить/уменьшить область обрезки в соответствии с реальной картинкой
	  1.1. определить расстояние от края (лево, верх)
	  1.2. определить пропорцию области обрезки по отношению к исходному изображению
	  1.3. вырезать область из исходного изображения
	  обновить оригинальную картинку
	  обновить виджет
	  */
	QPixmap scaledPixmap = p_impl->imageForCropping.scaled(this->size(),
									 Qt::KeepAspectRatio,
									 Qt::FastTransformation);
	int leftDelta = 0,
		topDelta = 0;

	if ( this->size().height() == scaledPixmap.height() )
		leftDelta = ( this->width() - scaledPixmap.width() ) / 2;
	else
		topDelta = ( this->height() - scaledPixmap.height() ) / 2;

	float xScale = (float)p_impl->imageForCropping.width()  / scaledPixmap.width(),
		  yScale = (float)p_impl->imageForCropping.height() / scaledPixmap.height();

	QRectF realSizeRect( QPointF(p_impl->croppingRect.left() - leftDelta, p_impl->croppingRect.top() - topDelta),
						p_impl->croppingRect.size() );

	// Top left
	realSizeRect.setLeft( ( p_impl->croppingRect.left() - leftDelta ) * xScale );
	realSizeRect.setTop ( ( p_impl->croppingRect.top()  - topDelta  ) * yScale );
	// Size
	realSizeRect.setWidth(  p_impl->croppingRect.width()  * xScale );
	realSizeRect.setHeight( p_impl->croppingRect.height() * yScale );

//	p_impl->imageForCropping =

//    update();

	return p_impl->imageForCropping.copy( realSizeRect.toRect() );
}


void ImageCropper::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent( event );

	/*if ( d_ptr->m_pixmap.size() != this->size() )*/ {
		QPixmap scaledPixmap = p_impl->imageForCropping.scaled(this->size(),
                                         Qt::KeepAspectRatio,
										 Qt::FastTransformation);
		QPixmap shownPixmap(this->size());
        QPainter p(&shownPixmap);
		p.fillRect( this->rect(), p_impl->backgroundColor );
        if ( this->size().height() == scaledPixmap.height() ) {
            p.drawPixmap( ( this->width() - scaledPixmap.width() ) / 2, 0, scaledPixmap );
        } else {
            p.drawPixmap( 0, ( this->height() - scaledPixmap.height() ) / 2, scaledPixmap );
        }
        p.end();

		QPainter widgetPainter(this);
        widgetPainter.drawPixmap(0,0,shownPixmap);
        widgetPainter.end();

    }


	/*if ( d_ptr->m_isNeedPaintCropGrid )*/ {
        QPainter painter(this);

        // затемненный фон
        QPainterPath p;
		p.addRect(p_impl->croppingRect);
        p.addRect(this->rect());
		painter.setBrush(QBrush(QColor(0,0,0,120)));
        painter.setPen(Qt::transparent);
        painter.drawPath(p);

        ////
        // Рамка и контрольные точки
		painter.setPen(p_impl->croppingRectBorderColor);
        // Рамка
        painter.setBrush(QBrush(Qt::transparent));
		painter.drawRect(p_impl->croppingRect);
        // Контрольные точки
		painter.setBrush(QBrush(p_impl->croppingRectBorderColor));
        // Вспомогательные параметры
        // X coordinates
		int leftXCoord   = p_impl->croppingRect.left() - 2;
		int centerXCoord = p_impl->croppingRect.center().x() - 3;
		int rightXCoord  = p_impl->croppingRect.right() - 2;
        // Y coordinates
		int topYCoord    = p_impl->croppingRect.top() - 2;
		int middleYCoord = p_impl->croppingRect.center().y() - 3;
		int bottomYCoord = p_impl->croppingRect.bottom() - 2;

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
		QPen dashPen(p_impl->croppingRectBorderColor);
        dashPen.setStyle(Qt::DashLine);
        painter.setPen(dashPen);
        // вертикальная
		painter.drawLine( QPoint(p_impl->croppingRect.center().x(), p_impl->croppingRect.top()),
						  QPoint(p_impl->croppingRect.center().x(), p_impl->croppingRect.bottom()) );
        // горизонтальная
		painter.drawLine( QPoint(p_impl->croppingRect.left(), p_impl->croppingRect.center().y()),
						  QPoint(p_impl->croppingRect.right(), p_impl->croppingRect.center().y()) );

        painter.end();
    }
}


void ImageCropper::mousePressEvent(QMouseEvent *ev)
{

    if (ev->type() == QEvent::MouseButtonDblClick ) {
        cropImage();
    } else {
		p_impl->isMousePressed = true;
		p_impl->startMousePos  = ev->pos();
		p_impl->lastStaticCroppingRect = p_impl->croppingRect;
    }

	this->setCursor( cursorIcon(p_impl->croppingRect, ev->pos()) );
}

void ImageCropper::mouseMoveEvent(QMouseEvent *ev)
{

	QPointF mouse_pos = ev->posF(); // relative to widget

	if ( !p_impl->isMousePressed ) {
		p_impl->cursorPosition = cursorPosition(p_impl->croppingRect, mouse_pos);
		this->setCursor( cursorIcon(p_impl->croppingRect, mouse_pos) );
    }
	else if ( p_impl->cursorPosition != CursorPositionUndefined )
    {
		QPointF mouseDelta;
		mouseDelta.setX( mouse_pos.x() - p_impl->startMousePos.x() );
		mouseDelta.setY( mouse_pos.y() - p_impl->startMousePos.y() );

		if (p_impl->cursorPosition != CursorPositionMiddle) {
			QRectF newGeometry;
			if ( p_impl->isProportionFixed )
				newGeometry = calculateGeometry(p_impl->lastStaticCroppingRect,
												p_impl->cursorPosition,
                                                mouseDelta,
												p_impl->proportion);
            else
				newGeometry = calculateGeometry(p_impl->lastStaticCroppingRect,
												p_impl->cursorPosition,
                                                mouseDelta);
			p_impl->croppingRect = newGeometry ;
        } else {
			p_impl->croppingRect.moveTo( p_impl->lastStaticCroppingRect.topLeft() + mouseDelta );
        }

        update();
    }
}

void ImageCropper::mouseReleaseEvent(QMouseEvent *ev)
{
	p_impl->isMousePressed = false;

	this->setCursor( cursorIcon(p_impl->croppingRect, ev->pos()) );
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
CursorPosition ImageCropper::cursorPosition ( const QRectF cropRect,
											  const QPointF mousePosition
                                              )
{
	CursorPosition cursorPosition = CursorPositionUndefined;

    if ( cropRect.contains( mousePosition ) ) {


        // Двухстороннее направление
        if ( isPointNearSide( cropRect.top(), mousePosition.y() ) &&
             isPointNearSide( cropRect.left(), mousePosition.x() ) )
			cursorPosition = CursorPositionTopLeft;
        else if ( isPointNearSide( cropRect.bottom(), mousePosition.y() ) &&
                  isPointNearSide( cropRect.left(), mousePosition.x() ) )
			cursorPosition = CursorPositionBottomLeft;
        else if ( isPointNearSide( cropRect.top(), mousePosition.y() )  &&
                  isPointNearSide( cropRect.right(), mousePosition.x() ) )
			cursorPosition = CursorPositionTopRight;
        else if ( isPointNearSide( cropRect.bottom(), mousePosition.y() ) &&
                  isPointNearSide( cropRect.right(), mousePosition.x() ) )
			cursorPosition = CursorPositionBottomRight;
        // Одностороннее направление
		else if ( isPointNearSide( cropRect.left(), mousePosition.x() ) )   cursorPosition = CursorPositionLeft;
		else if ( isPointNearSide( cropRect.right(), mousePosition.x() ) )  cursorPosition = CursorPositionRight;
		else if ( isPointNearSide( cropRect.top(), mousePosition.y() ) )    cursorPosition = CursorPositionTop;
		else if ( isPointNearSide( cropRect.bottom(), mousePosition.y() ) ) cursorPosition = CursorPositionBottom;
        // Без направления
		else cursorPosition = CursorPositionMiddle;
    }

    return cursorPosition;
}

// Получить иконку курсора соответствующую местоположению мыши
QCursor ImageCropper::cursorIcon( const QRectF widgetSize,
								  const QPointF mousePosition )
{

    QCursor cursorIcon;

    switch ( cursorPosition( widgetSize, mousePosition ) )
    {
	case CursorPositionTopRight:
	case CursorPositionBottomLeft:
        cursorIcon = QCursor( Qt::SizeBDiagCursor );
        break;
	case CursorPositionTopLeft:
	case CursorPositionBottomRight:
        cursorIcon = QCursor( Qt::SizeFDiagCursor );
        break;
	case CursorPositionTop:
	case CursorPositionBottom:
        cursorIcon = QCursor( Qt::SizeVerCursor );
        break;
	case CursorPositionLeft:
	case CursorPositionRight:
        cursorIcon = QCursor( Qt::SizeHorCursor );
        break;
	case CursorPositionMiddle:
		if ( p_impl->isMousePressed ) cursorIcon = QCursor( Qt::ClosedHandCursor );
        else                    cursorIcon = QCursor( Qt::OpenHandCursor );
        break;
	case CursorPositionUndefined:
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
QRectF ImageCropper::calculateGeometry(const QRectF sourceGeometry,
                                 const CursorPosition cursorPosition,
								 const QPointF mouseDelta,
                                 const QSizeF proportions
                                 )
{
	QRectF resultGeometry;

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
QRectF ImageCropper::calculateGeometryWithCustomProportions(const QRectF sourceGeometry,
                                                      const CursorPosition cursorPosition,
													  const QPointF mouseDelta
                                                      )
{
	QRectF resultGeometry = sourceGeometry;

    switch ( cursorPosition )
    {
	case CursorPositionTopLeft:
        resultGeometry.setLeft( sourceGeometry.left() + mouseDelta.x() );
        resultGeometry.setTop ( sourceGeometry.top()  + mouseDelta.y() );
        break;
	case CursorPositionTopRight:
        resultGeometry.setTop  ( sourceGeometry.top()   + mouseDelta.y() );
        resultGeometry.setRight( sourceGeometry.right() + mouseDelta.x() );
        break;
	case CursorPositionBottomLeft:
        resultGeometry.setBottom( sourceGeometry.bottom() + mouseDelta.y() );
        resultGeometry.setLeft  ( sourceGeometry.left()   + mouseDelta.x() );
        break;
	case CursorPositionBottomRight:
        resultGeometry.setBottom( sourceGeometry.bottom() + mouseDelta.y() );
        resultGeometry.setRight ( sourceGeometry.right()  + mouseDelta.x() );
        break;
	case CursorPositionTop:
        resultGeometry.setTop( sourceGeometry.top() + mouseDelta.y() );
        break;
	case CursorPositionBottom:
        resultGeometry.setBottom( sourceGeometry.bottom() + mouseDelta.y() );
        break;
	case CursorPositionLeft:
        resultGeometry.setLeft( sourceGeometry.left() + mouseDelta.x() );
        break;
	case CursorPositionRight:
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
QRectF ImageCropper::calculateGeometryWithFixedProportions(const QRectF sourceGeometry,
                                                     const CursorPosition cursorPosition,
													 const QPointF mouseDelta,
                                                     const QSizeF proportions
                                                 )
{
	int delta = abs(mouseDelta.x()) >= abs(mouseDelta.y()) ? mouseDelta.x() : mouseDelta.y();

	QRectF resultGeometry = sourceGeometry;


    switch ( cursorPosition )
    {
	case CursorPositionTopLeft:
	case CursorPositionLeft:
        resultGeometry.setTop  ( sourceGeometry.top()   + proportions.height() * delta );
        resultGeometry.setLeft ( sourceGeometry.left()  + proportions.width()  * delta );
        break;
	case CursorPositionTopRight:
	case CursorPositionRight:
	case CursorPositionTop:
        if (abs(mouseDelta.x()) >= abs(mouseDelta.y())) {
            resultGeometry.setTop  ( sourceGeometry.top()   + proportions.height() * delta * -1 );
            resultGeometry.setRight( sourceGeometry.right() + proportions.width()  * delta );
        } else {
            resultGeometry.setTop  ( sourceGeometry.top()   + proportions.height() * delta );
            resultGeometry.setRight( sourceGeometry.right() + proportions.width()  * delta * -1 );
        }
        break;
	case CursorPositionBottomLeft:
        if (abs(mouseDelta.x()) <= abs(mouseDelta.y())) {
            resultGeometry.setBottom( sourceGeometry.bottom() + proportions.height() * delta );
            resultGeometry.setLeft  ( sourceGeometry.left()   + proportions.width()  * delta * -1);
        } else {
            resultGeometry.setBottom( sourceGeometry.bottom() + proportions.height() * delta * -1 );
            resultGeometry.setLeft  ( sourceGeometry.left()   + proportions.width()  * delta );
        }
        break;
	case CursorPositionBottomRight:
	case CursorPositionBottom:
        resultGeometry.setBottom( sourceGeometry.bottom() + proportions.height() * delta );
        resultGeometry.setRight ( sourceGeometry.right()  + proportions.width()  * delta );
        break;

    default:
        break;
    }

    return resultGeometry;
}

