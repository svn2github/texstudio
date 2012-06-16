#ifndef TITLEDPANEL_H
#define TITLEDPANEL_H

#include "mostQtHeaders.h"

class TitledPanelPage;
Q_DECLARE_METATYPE(TitledPanelPage *)

class TitledPanelPage : public QObject
{
	Q_OBJECT
	friend class TitledPanel;
public:
	TitledPanelPage(QWidget* widget, const QString& id, const QString& title, const QIcon &icon=QIcon());
	~TitledPanelPage();

	void addToolbarAction(QAction *act);

	inline QString id() const;
	bool visible() const;

	static TitledPanelPage *fromId(const QString &id);
signals:
	void titleChanged(QString);
	void iconChanged(QIcon);
public slots:
	void setTitle(const QString &title);
	void setIcon(const QIcon &icon);
private:
	static QHash<QString, TitledPanelPage *> allPages;

	QString m_id;
	QString m_title;
	QIcon m_icon;
	QWidget *m_widget;

	QAction *m_visibleAction;
	QAction *m_selectAction;
	QList<QAction *> *m_toolbarActions;
};


class TitledPanel : public QFrame
{
	Q_OBJECT
public:
	explicit TitledPanel(QWidget *parent = 0);

	void appendPage(TitledPanelPage *page, bool guiUpdate=true);
	void removePage(TitledPanelPage *page, bool guiUpdate=true);
	int pageCount() const;

	void setHiddenWidgets(const QStringList& hidden);
	QStringList hiddenWidgets() const;

	TitledPanelPage* currentPage() const;
	QString currentPageId() const;
	void setCurrentPage(const QString &id);
	QAction *toggleViewAction() const;

signals:
	void widgetContextMenuRequested(QWidget* widget, const QPoint& globalPosition);
	void pageChanged(const QString &id);

public slots:
	virtual void setVisible(bool visible);
	void updateTopbar();

private slots:
	void viewToggled(bool visible) { setVisible(visible); }

	void onPageTitleChange();
	void onPageIconChange();

	void setActivePageFromAction();
	void setActivePageFromComboBox(int index);
	void togglePageVisibleFromAction(bool on);
	//void updateToolbarForResizing(CollapseState clState);
	void customContextMenuRequested(const QPoint& localPosition);



private:
	QAction *mToggleViewAction;
	QAction *closeAction;

	QStringList hiddenWidgetsIds;
	QActionGroup *pageSelectActions;    // only for visible widgets
	QList<QWidget*> widgets;

	// visual elements
	QVBoxLayout *vLayout; // containing title bar and contents
	QToolBar *topbar, *collapsedTopbar;
	QStackedWidget* stack;

	QList<TitledPanelPage *> pages;
};

#endif // TITLEDPANEL_H
