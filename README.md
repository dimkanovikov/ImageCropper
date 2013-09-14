Image Cropper
=============

Qt Widget for cropping images.

Using:
-------------

Use ImageCropper is easy. First you need to create instance and configure it:

```cpp
// Create instance
imageCropper = new ImageCropper(this);

// If you need to fix proportions of cropping rect
// ... set proportions if needed
imageCropper->setProportion(QSize(4,3));
// ... and fix it
imageCropper->setProportionFixed(true);

// Set image for cropping
imageCropper->setImage(QPixmap(":/img.jpg"));

// Set background color under cropped image pane
imageCropper->setBackgroundColor(Qt::lightGray);

// Set border color of cropping rect helper
imageCropper->setCroppingRectBorderColor(Qt::magenta);
```

And when you need crop image, just do it:

```cpp
QPixmap croppedImage = imageCropper->cropImage();
```

License
-------------

GNU LGPL v3, http://www.gnu.org/copyleft/lesser.html