#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QtWidgets/QWidget>
#include <QDesktopWidget>
#include <QApplication>
#include <QIcon>
#include <QCursor>
#include <QImage>
#include <QResizeEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPointF>
#include <QVector>
#include <QFile>
#include <QTextStream>

#define imgW (img.size().width())	// image width
#define imgH (img.size().height())	// image height
#define winW (width())				// window width
#define winH (height())				// window height

class ImageViewer : public QWidget
{
	Q_OBJECT

public:
	ImageViewer(QWidget *parent = Q_NULLPTR);

protected:
	void resizeEvent(QResizeEvent *event);		// window resize
	void wheelEvent(QWheelEvent *event);		// wheel zoom in and out
	void mousePressEvent(QMouseEvent *event);	// mouse press		(overload from QWidget)
	void mouseMoveEvent(QMouseEvent *event);	// mouse move		(overload from QWidget)
	void mouseReleaseEvent(QMouseEvent *event);	// mouse release	(overload from QWidget)
	void keyPressEvent(QKeyEvent *event);		// keyboard press	(overload from QWidget)
	void paintEvent(QPaintEvent *event);		// drawing the result

private:
	QImage img;				// store image
	float maxScale = 0.0f;	// maximum scale
	float minScale = 0.0f;	// minimun scale
	float scale = 0.0f;		// scale to draw
	QPointF newDelta;		// new displacement
	QPointF oldDelta;		// old displacement
	QPointF pos1;			// mouse press position 1
	QPointF pos2;			// mouse press position 2
	QVector<QPointF> imagePoints;	// record image points' pixel
};

#endif IMAGEVIEWER_H
