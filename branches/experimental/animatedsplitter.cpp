#include "animatedsplitter.h"
#include "QPropertyAnimation"

AnimatedSplitter::AnimatedSplitter(QWidget *parent) :
    QSplitter(parent)
{
	mActiveIndex = 0;
}

void AnimatedSplitter::setActiveWidgetWidth(int width)
{
	if (mActiveIndex >=0 && mActiveIndex < count()) {
		qDebug() << "w" << width;
		// We have to explicitly redistribute the sizes, otherwise
		// any additional/missing space is distributed amongst the widgets
		// according to the relative weight of the sizes. see. setSizes() documentation
		QList<int> sz(sizes());
		int delta = width - sz.at(mActiveIndex);  // < 0 for shrinking
		if (mActiveIndex < count()-1 && sz.at(mActiveIndex+1) > delta) {
			// take place from right
			sz.replace(mActiveIndex, width);
			sz.replace(mActiveIndex+1, sz.at(mActiveIndex+1) - delta);
		} else if (mActiveIndex > 0 && sz.at(mActiveIndex-1) > delta) {
			// take place from left
			sz.replace(mActiveIndex, width);
			sz.replace(mActiveIndex-1, sz.at(mActiveIndex-1) - delta);
		} else {
			// fallback:
			// TODO: the widget likely will not have the final width "width", because of space redistribution
			sz.replace(mActiveIndex, width);
			qDebug() << "AnimatedSplitter fallback: no cannot take space from adjacent widgets";
		}
		setSizes(sz);
	}
}

int AnimatedSplitter::activeWidgetWidth() const
{
	if (mActiveIndex >=0 && mActiveIndex < count()) return sizes().at(mActiveIndex);
	return 0;
}

void AnimatedSplitter::animateActiveWidgetWidth(int width, int time)
{
	int min, max;
	getRange(0, &min, &max);
	qDebug() << min << max;
	QPropertyAnimation *animation = new QPropertyAnimation(this, "activeWidgetWidth");
	animation->setDuration(time);
	animation->setStartValue(activeWidgetWidth());
	animation->setEndValue(width);
	connect(animation, SIGNAL(finished()), this, SIGNAL(animationFinished()));
	animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void AnimatedSplitter::setActiveIndex(int index)
{
	mActiveIndex = index;
}

void AnimatedSplitter::setActiveWidget(QWidget *w)
{
	for (int i=0; i<count(); i++) {
		if (widget(i) == w) {
			mActiveIndex = i;
			break;
		}
	}
}

int AnimatedSplitter::activeIndex() const
{
	return mActiveIndex;
}
