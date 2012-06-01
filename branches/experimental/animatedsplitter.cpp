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
		QList<int> sz(sizes());
		sz.replace(mActiveIndex, width);
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
