#ifndef IMAGECROPPER_H
#define IMAGECROPPER_H

#include "imagecropper_p.h"
#include "imagecropper_e.h"

#include <QtGui/QWidget>

class ImageCropper : public QWidget
{
    Q_OBJECT

public:
    ImageCropper(QWidget *parent = 0);
    ~ImageCropper();

public slots:
	// Установить изображение для обрезки
	void setImage(const QPixmap& _image);
	// Установить цвет фона виджета обрезки
	void setBackgroundColor(const QColor& _backgroundColor);
	// Установить цвет рамки области обрезки
	void setCroppingRectBorderColor(const QColor& _borderColor);
	// Установить пропорции области выделения
	void setProportion(const QSizeF& _proportion );
	// Использовать фиксированные пропорции области виделения
	void setIsProportionFixed ( bool );
	// Обрезать изображение
	QPixmap cropImage ();

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent  (QMouseEvent *ev);
    void mouseMoveEvent   (QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *ev);

private:
    // Местоположение курсора мыши над виджетом
    inline bool isPointNearSide ( int sideCoordinate,
                                  int pointCoordinate
                                  ) const;
	CursorPosition cursorPosition ( const QRectF widgetSize,
									const QPointF mousePosition
                                    );
	QCursor cursorIcon ( const QRectF widgetSize,
						 const QPointF mousePosition
                         );

    // Получить размер виджета после его изменения мышью
    // ======
    // mouseDelta - разница между начальным положением перемещения мыши и текущим
    // proportions - если нужно сохранить пропорции сторон виджета,
    // то обязательно необходимо передать в метод их отношение, в
    // противном случае, их не нужно передавать
    // ------
    // Контракты:
    // 1. Метод должен вызываться, только при зажатой кнопке мыши
    //    (т.е. при перемещении или изменении размера виджета)
	QRectF calculateGeometry(const QRectF sourceGeometry,
                            const CursorPosition cursorPosition,
							const QPointF mouseDelta,
                            const QSizeF proportions = QSizeF()
                            );
    // Получить размер виджета после его изменения мышью
    // Метод изменяет виджет не сохраняя начальных пропорций сторон
    // ------
    // Контракты:
    // 1. Метод должен вызываться, только при зажатой кнопке мыши
    //    (т.е. при перемещении или изменении размера виджета)
	QRectF calculateGeometryWithCustomProportions(const QRectF sourceGeometry,
                                                 const CursorPosition cursorPosition,
												 const QPointF mouseDelta
                                                 );
    // Получить размер виджета после его изменения мышью
    // Метод изменяет виджет сохраняя начальные пропорции сторон
    // ------
    // Контракты:
    // 1. Метод должен вызываться, только при зажатой кнопке мыши
    //    (т.е. при перемещении или изменении размера виджета)
	QRectF calculateGeometryWithFixedProportions(const QRectF sourceGeometry,
                                                 const CursorPosition cursorPosition,
												 const QPointF mouseDelta,
                                                 const QSizeF proportions
                                                 );

private:
	// Private data implementation
	ImageCropperPrivate* p_impl;
};

#endif // IMAGECROPPER_H
