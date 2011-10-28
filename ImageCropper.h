#ifndef IMAGECROPPER_H
#define IMAGECROPPER_H

#include <QtGui/QLabel>





enum CursorPosition
{
    None,
    Middle,
    Top,
    Bottom,
    Left,
    Right,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

class ImageCropper : public QWidget
{
    Q_OBJECT

public:
    ImageCropper(QWidget *parent = 0);
    ~ImageCropper();

public slots:
    void setProportion ( QSizeF proportion );
    void cropImage ();
    void setPixmap(const QPixmap &);
    void setIsProportionFixed ( bool );

protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent  (QMouseEvent *ev);
    void mouseMoveEvent   (QMouseEvent *ev);
    virtual void mouseReleaseEvent(QMouseEvent *ev);

private:
    QRect m_cropper,
          m_storedCropper;
    QPixmap m_pixmap;

    ///////////////////////////////////////
    CursorPosition m_cursorPosition;
    bool m_isMousePressed,
         m_isProportionFixed;
    QPoint m_startMousePos;
    // width  - приращение по ширине
    // height - приращение по высоте
    QSizeF m_proportion;
    ////////////////////////////////////////


    // Местоположение курсора мыши над виджетом
    inline bool isPointNearSide ( int sideCoordinate,
                                  int pointCoordinate
                                  ) const;
    CursorPosition cursorPosition ( const QRect widgetSize,
                                    const QPoint mousePosition
                                    );
    QCursor cursorIcon ( const QRect widgetSize,
                         const QPoint mousePosition
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
    QRect calculateGeometry(const QRect sourceGeometry,
                            const CursorPosition cursorPosition,
                            const QPoint mouseDelta,
                            const QSizeF proportions = QSizeF()
                            );
    // Получить размер виджета после его изменения мышью
    // Метод изменяет виджет не сохраняя начальных пропорций сторон
    // ------
    // Контракты:
    // 1. Метод должен вызываться, только при зажатой кнопке мыши
    //    (т.е. при перемещении или изменении размера виджета)
    QRect calculateGeometryWithCustomProportions(const QRect sourceGeometry,
                                                 const CursorPosition cursorPosition,
                                                 const QPoint mouseDelta
                                                 );
    // Получить размер виджета после его изменения мышью
    // Метод изменяет виджет сохраняя начальные пропорции сторон
    // ------
    // Контракты:
    // 1. Метод должен вызываться, только при зажатой кнопке мыши
    //    (т.е. при перемещении или изменении размера виджета)
    QRect calculateGeometryWithFixedProportions (const QRect sourceGeometry,
                                                 const CursorPosition cursorPosition,
                                                 const QPoint mouseDelta,
                                                 const QSizeF proportions
                                                 );
};

#endif // IMAGECROPPER_H
