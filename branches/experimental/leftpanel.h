#ifndef LEFTPANEL_H
#define LEFTPANEL_H

#include "mostQtHeaders.h"
#include "animatedsplitter.h"

class LeftPanel : public AutoCollapsingPanel
{
	Q_OBJECT
public:
	explicit LeftPanel(QWidget *parent = 0);
	
	void addSeparator();
	void addWidget(QWidget* widget, const QString& id, const QString& text, const QString& iconName);
	void setWidgetText(const QString& id, const QString& text);
	void setWidgetText(QWidget* widget, const QString& text);
	void setWidgetIcon(const QString& id, const QString& icon);
	void setWidgetIcon(QWidget* widget, const QString& icon);
	int widgetCount() const;
	void setHiddenWidgets(const QString& hidden);
	QString hiddenWidgets() const;
	QWidget* widget(int i) const;
	QWidget* widget(const QString& id) const;
	void setCurrentWidget(QWidget* widget);
	QWidget* currentWidget() const;
	bool isNewLayoutStyleEnabled() const;
signals:
	void widgetContextMenuRequested(QWidget* widget, const QPoint& globalPosition);
public slots:
	void showWidgets(bool newLayoutStyle);
private slots:
	void showPageFromComboBox(int index);
	void showPageFromAction();
	void showPage(const QString &id);
	void currentWidgetChanged(int i);
	void toggleWidgetVisibleFromAction(bool on);
	void updateToolbarForResizing(CollapseState clState);
	void customContextMenuRequested(const QPoint& localPosition);
protected:
	virtual int collapsedWidth() {return 23;}
private:
	void showWidget(const QString& id);
	void hideWidget(const QString& id);
	//void addWidgetOld(QWidget* widget, const QString& id, const QString& text, const QString& iconName, const bool visible);
//	void addWidgetNew(QWidget* widget, const QString& id, const QString& text, const QString& iconName, const bool visible);
	QString widgetId(QWidget* widget) const;

	QStringList hiddenWidgetsIds;
	QList<QWidget*> widgets;
	bool newStyle;

	QVBoxLayout *vLayout; // containing title bar and contents
	QToolBar *topbar, *collapsedTopbar;
	QWidget *contentsArea;

	//old layout
	QToolBox *toolbox;

	//new layout
	QStackedWidget* stack;
	QToolBar *sidebar;
	QComboBox *cbTopbarSelector;
};


#endif // LEFTPANEL_H
