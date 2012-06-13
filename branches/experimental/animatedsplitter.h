#ifndef ANIMATEDSPLITTER_H
#define ANIMATEDSPLITTER_H

#include <QSplitter>

class AnimatedSplitter : public QSplitter
{
	Q_OBJECT

	Q_PROPERTY(int activeWidgetWidth READ activeWidgetWidth WRITE setActiveWidgetWidth)
public:
	explicit AnimatedSplitter(QWidget *parent = 0);

	void setActiveWidgetWidth(int width);
	int activeWidgetWidth() const;

	void animateActiveWidgetWidth(int width, int time=200);

	void setActiveIndex(int index);
	void setActiveWidget(QWidget *w);
	int activeIndex() const;

signals:
	void animationFinished();
public slots:

private:
	int mActiveIndex;

};


class AutoCollapsingFrame: public QFrame{
	Q_OBJECT
public:
	enum CollapseState {Expanded, Collapsed, Expanding, Collapsing};

	explicit AutoCollapsingFrame(QWidget *parent=0) :
		QFrame(parent), expandedWidth(100), clState(Expanded), mCollapsingEnabled(false), mAnimatedMotion(false)
	{
		containingSplitter = qobject_cast<AnimatedSplitter *>(parent);
		if (containingSplitter)
			connect(containingSplitter, SIGNAL(animationFinished()), this, SLOT(sizingFinished()));
	}
	bool animatedMotion() const {return mAnimatedMotion;}
	bool collapsingEnabled() const {return mCollapsingEnabled;}
	bool collapsed() const {return clState == Collapsed;}
	void setCollapsed(bool collapse) {
		if (!mCollapsingEnabled) return;
		switch (clState) {
		case Expanded:
			if (collapse) {
				expandedWidth = width();
				triggerResize(Collapsing);
			}
			break;
		case Expanding:
			if (collapse) {
				// TODO: stop the animation and start collapsing
				qDebug() << "wah";
			}
			break;
		case Collapsed:
			if (!collapse) {
				triggerResize(Expanding);
			}
			break;
		case Collapsing:
			if (!collapse) {
				// TODO: stop the animation and start expanding
				qDebug() << "wahah";
			}
			break;
		}
	}
signals:
	void collapseStateChanged(CollapseState clState);
public slots:
	void setAnimatedMotion(bool enabled) { mAnimatedMotion = enabled; }
	void setCollapsingEnabled(bool enabled) {if (!enabled) setCollapsed(false); mCollapsingEnabled = enabled;}
protected:
	virtual int collapsedWidth() {qDebug() << 30; return 30;}
	virtual void enterEvent(QEvent *event) {
		QFrame::enterEvent(event);
		setCollapsed(false);
	}
	virtual void leaveEvent(QEvent *event) {
		QFrame::leaveEvent(event);
		setCollapsed(true);
	}
	virtual void resizeEvent(QResizeEvent *event) {
		QFrame::resizeEvent(event);
		if (clState == Collapsed && width() > collapsedWidth()) {
			clState = Expanded;
			qDebug() << "resize-event : state" << clState;
		}
	}
private slots:
	void sizingFinished() { clState = (clState==Expanding) ? Expanded : Collapsed; emit collapseStateChanged(clState);}
private:
	void triggerResize(CollapseState state) {
		if (!containingSplitter) return;
		containingSplitter->setActiveWidget(this);

		if (!mAnimatedMotion) {
			if (state == Collapsing) {
				emit collapseStateChanged(state);
				state = Collapsed;
			}
			if (state == Expanding) {
				state = Expanded;
				emit collapseStateChanged(state);
			}
		}

		qDebug() << "resize-trigg : state" << state << ", exWidth, width:"<< expandedWidth << width();

		switch (state) {
		case Expanded:
			containingSplitter->setActiveWidgetWidth(expandedWidth);
			clState = state;
			emit collapseStateChanged(state);
			break;
		case Collapsed:
			containingSplitter->setActiveWidgetWidth(collapsedWidth());
			clState = state;
			emit collapseStateChanged(state);
			break;
		case Expanding:
			clState = state;
			emit collapseStateChanged(state);
			containingSplitter->animateActiveWidgetWidth(expandedWidth);
			break;
		case Collapsing:
			clState = state;
			emit collapseStateChanged(state);
			containingSplitter->animateActiveWidgetWidth(collapsedWidth());
			break;
		}
	}

	AnimatedSplitter *containingSplitter;
	int expandedWidth;
	CollapseState clState;
	bool mCollapsingEnabled;
	bool mAnimatedMotion;
};

// TODO: Maybe integrate in AutoCollapsingFrame or add basic aspects of the toolbar functionality
/**
 * A panel providing a convenience toggleViewAction
**/
class AutoCollapsingPanel: public AutoCollapsingFrame{
	Q_OBJECT
public:
	explicit AutoCollapsingPanel(QWidget *parent): AutoCollapsingFrame(parent){
		mToggleViewAction = new QAction(this);
		mToggleViewAction->setCheckable(true);
		mToggleViewAction->setChecked(true);
		connect(mToggleViewAction, SIGNAL(toggled(bool)), this, SLOT(viewToggled(bool)));
	}

	QAction *toggleViewAction() const {return mToggleViewAction;}

protected:
	void setVisible(bool visible) {
		AutoCollapsingFrame::setVisible(visible);
		mToggleViewAction->setChecked(visible);
	}

private slots:
	void viewToggled(bool visible) { setVisible(visible); }

private:
	QAction *mToggleViewAction;
};


#endif // ANIMATEDSPLITTER_H
