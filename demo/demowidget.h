#ifndef DEMOWIDGET_H
#define DEMOWIDGET_H

#include <QWidget>

class ImageCropper;
class QLabel;

class DemoWidget : public QWidget
{
	Q_OBJECT
public:
	explicit DemoWidget(QWidget* _parent = 0);
	
signals:
	
private slots:
	void crop();
	
private:
	ImageCropper* m_imageCropper;
	QLabel* m_croppedImage;
};

#endif // DEMOWIDGET_H
