#ifndef TXSTABWIDGET_H
#define TXSTABWIDGET_H

#include "mostQtHeaders.h"

class LatexEditorView;

class TxsTabWidget : public QTabWidget
{
Q_OBJECT
public:
	explicit TxsTabWidget(QWidget *parent = 0);
	void moveTab(int from,int to);

	QList<LatexEditorView *> editors() const;
	LatexEditorView* currentEditorView() const;

	void setCurrentEditor(LatexEditorView *edView);

signals:
	void tabMoved(int from,int to);
	void tabBarContextMenuRequested(QPoint point);

public slots:
	void gotoNextDocument();
	void gotoPrevDocument();
};

#endif // TXSTABWIDGET_H
