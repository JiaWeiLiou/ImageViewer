#include "imageviewer.h"

ImageViewer::ImageViewer(QWidget *parent)
	: QWidget(parent)
{
	initial();

	/* set widget*/
	setWindowIcon(QIcon(":ImageViewer/Resources/icon.ico"));			// set program's icon
	setMinimumSize(300, 300);											// set widget's minimum size
	setCursor(Qt::CrossCursor);											// set cursor to cross type
	setMouseTracking(true);												// tracking mouse location
	setAcceptDrops(true);												// set widget can be drop
	//showMaximized();													// set widget to show max
}

void ImageViewer::initial()
{
	/* set maximum scale (maximum scale is 1/5 screen width) */
	maxScale = QApplication::desktop()->screenGeometry().width() / 5;	// set maximum scale

	/* set window's title*/
	if (!fileName.isEmpty()) {
		int pos1 = fileName.lastIndexOf('/');
		filePath = fileName.left(pos1 + 1);							//檔案路徑
		fileName =fileName.right(fileName.size() - pos1 - 1);		//檔案名稱
		fileName = QString(tr(" - ")) + fileName;
	}
	QString title = QString(tr("Image Viewer")) + fileName;
	setWindowTitle(title);

	/* calculate scale to show image */
	float scaleW = (float)winW / (float)(imgW - 1);
	float scaleH = (float)winH / (float)(imgH - 1);
	// If the scale is to zoom in, keep scale to 1:1.
	if (scaleW >= 1 && scaleH >= 1) {
		scale = 1;
		minScale = 1;
	// If the scale is to zoom out and have been zoom in, adjudge below.
	} else {
		minScale = scaleW < scaleH ? scaleW : scaleH;
		scale = minScale;
	}

	/* clear imagePoints */
	imagePoints.clear();
}

void ImageViewer::dragEnterEvent(QDragEnterEvent *event)
{
	if (event->mimeData()->hasFormat("text/uri-list")) {
		event->acceptProposedAction();
	}
}

void ImageViewer::dropEvent(QDropEvent *event)
{
	QList<QUrl> urls = event->mimeData()->urls();
	if (urls.isEmpty()) {
		return;
	}

	fileName = urls.first().toLocalFile();
	if (fileName.isEmpty()) {
		return;
	}

	img.load(fileName);		// load image
	initial();				// reset parameter
	update();
}

void ImageViewer::resizeEvent(QResizeEvent *event)
{
	/* calculate scale to show image*/
	float scaleW = (float)winW / (float)(imgW - 1);
	float scaleH = (float)winH / (float)(imgH - 1);
	// If the scale is to zoom in, keep scale to 1:1.
	if (scaleW >= 1 && scaleH >= 1) {
		scale = 1;
		minScale = 1;
	// If the scale is to zoom out and have been zoom in, adjudge below.
	} else {
		// If scale is lower than minimum scale, set scale = minScale.
		// else keep the scale.
		minScale = scaleW < scaleH ? scaleW : scaleH;
		scale = scale < minScale ? minScale : scale;
	}
	update();
}

void ImageViewer::wheelEvent(QWheelEvent *event)
{
	QPointF pixelPos = QPointF(event->posF());			// pixel coordinate
	QPointF imagePos = (pixelPos - newDelta) / scale;	// image coordinate

	// zoom in or ot
	if (event->angleDelta().y() > 0) {
		scale = scale * 1.2 > maxScale ? maxScale : scale * 1.2;
	} else {
		scale = scale / 1.2 < minScale ? minScale : scale / 1.2;
	}
	newDelta = pixelPos - scale * imagePos;
	oldDelta = newDelta;

	update();
}

void ImageViewer::mousePressEvent(QMouseEvent *event)
{
	// drag image
	if (event->buttons() == Qt::LeftButton) {
		pos1 = QPointF(event->pos());
		// set point
	} else if (event->buttons() == Qt::RightButton) {
		// change to image's pixel coordinate
		QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;
		// limit the point in the image
		if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
			// record file.
			imagePoints.push_back(imagePos);
		}
		update();
	}
}

void ImageViewer::mouseMoveEvent(QMouseEvent *event)
{
	// drag image
	if (event->buttons() == Qt::LeftButton) {
		pos2 = QPointF(event->pos());
		/* new displacement add last displacement */
		newDelta = pos2 - pos1 + oldDelta;
		update();
		// set point
	} else if (event->buttons() == Qt::RightButton) {
		// change to image's pixel coordinate
		QPointF imagePos = (QPointF(event->pos()) - newDelta) / scale;
		// limit the point in the image
		if (imagePos.x() >= 0 && imagePos.x() <= (imgW - 1) && imagePos.y() >= 0 && imagePos.y() <= (imgH - 1)) {
			// record file.
			imagePoints[imagePoints.size() - 1] = imagePos;
		}
		update();
	}
}

void ImageViewer::mouseReleaseEvent(QMouseEvent *event)
{
	// record the distance of drag image
	oldDelta = newDelta;
	if (imagePoints.size() == 4) {
		imagePoints.push_back(imagePoints[0]);
	}
	update();
}

void ImageViewer::keyPressEvent(QKeyEvent *event)
{
	// press keyboard Enter to output points file
	if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return && imagePoints.size() > 4) {
		QFile file(filePath + "points.txt");
		if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
			QTextStream out(&file);
			out << dec << fixed;
			for (int i = 0; i < 4; ++i) {
				out << imagePoints[i].x() << "\t" << imagePoints[i].y() << endl;
			}
			file.close();
		}
	// press keyboard Esc to give up setting point
	} else if (event->key() == Qt::Key_Escape) {
		imagePoints.clear();	// clear points
		update();
	}
}

void ImageViewer::paintEvent(QPaintEvent *event)
{
	/* modify xDelta */
	// If image's horizontal size to show is longer than winW, set it to center.
	if ((imgW - 1) * scale < winW) {
		newDelta.rx() = winW / 2 - scale * (imgW - 1) / 2;
	// If image's horizontal delta is less than winW, set it to edge.
	} else if (newDelta.x() > 0) {
		newDelta.rx() = 0;
	// If image's horizontal delta is bigger than 0, set it to edge.
	} else if ((imgW - 1) * scale + newDelta.x() < winW) {
		newDelta.rx() = winW - (imgW - 1) * scale;
	}

	/* modify yDelta */
	// If image's vertical size to show is longer than winW, set it to center.
	if ((imgH - 1) * scale < winH) {
		newDelta.ry() = winH / 2 - scale * (imgH - 1) / 2;
	// If image's vertical delta is less than winW, set it to edge.
	} else if (newDelta.ry() > 0) {
		newDelta.ry() = 0;
	// If image's vertical delta is bigger than 0, set it to edge.
	} else if ((imgH - 1) * scale + newDelta.y() < winH) {
		newDelta.ry() = winH - (imgH - 1) * scale;
	}

	QPainter painter(this);

	/* draw image */
	QRectF rect(newDelta.x(), newDelta.y(), (imgW - 1) * scale, (imgH - 1) * scale);	// draw range
	painter.drawImage(rect, img);	// draw image

	/* draw points and lines */
	if (imagePoints.size() > 0) {
		QPointF np, bp;
		for (int i = 0; i < imagePoints.size(); ++i) {
			QPointF np = imagePoints[i] * scale + newDelta;
			if (i == 0) {
				/* draw point at first*/
				painter.setPen(QPen(Qt::red, 5));
				painter.drawPoint(np);
			} else {
				/* draw line to lower level*/
				painter.setPen(QPen(Qt::green, 3));
				painter.drawLine(np, bp);
				/* draw point to upper level */
				painter.setPen(QPen(Qt::red, 5));
				painter.drawPoint(np);
				painter.drawPoint(bp);
			}
			bp = np;
		}
	}
}