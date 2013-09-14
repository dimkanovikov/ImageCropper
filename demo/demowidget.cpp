#include "demowidget.h"
#include <imagecropper.h>

#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>

DemoWidget::DemoWidget(QWidget* _parent) :
	QWidget(_parent)
{
	// Настраиваем виджет обрезки изображений
	m_imageCropper = new ImageCropper(this);
	m_imageCropper->resize( 600, 600 );
	m_imageCropper->setProportion(QSize(4,3));
	m_imageCropper->setImage(QPixmap(":/img.jpg"));
	m_imageCropper->setBackgroundColor( Qt::lightGray );
	m_imageCropper->setCroppingRectBorderColor( Qt::magenta);
	// Использовать фиксированные пропорции
	QCheckBox* fixedProportionsCombo = new QCheckBox("Fix proportion to (4x3)", this);
	connect(fixedProportionsCombo, SIGNAL(toggled(bool)),
			m_imageCropper, SLOT(setProportionFixed(bool)));
	// Кадрировать выделенную область
	QPushButton* cropBtn = new QPushButton("Crop", this);
	connect(cropBtn, SIGNAL(clicked()), this, SLOT(crop()));
	// Область для вставки обрезанного изображения
	m_croppedImage = new QLabel(this);
	// Настраиваем отображение
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(m_imageCropper);
	layout->addWidget(fixedProportionsCombo);
	layout->addWidget(cropBtn);
	layout->addWidget(m_croppedImage);
}

void DemoWidget::crop()
{
	m_croppedImage->setPixmap(m_imageCropper->cropImage());
}
