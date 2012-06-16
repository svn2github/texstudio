/*
	This file contains the dockwidgets at the left/bottom side of txs
*/
#ifndef TOOLWIDGETS_H
#define TOOLWIDGETS_H

#include "mostQtHeaders.h"

#include "titledpanel.h"
#include "logeditor.h"
#include "latexlog.h"
#include "searchresultmodel.h"
#include "qdocumentsearch.h"

#include <QAbstractTextDocumentLayout>

class PreviewWidget : public QScrollArea
{
	Q_OBJECT
public:
	explicit PreviewWidget(QWidget * parent = 0);

public slots:	
	void previewLatex(const QPixmap& previewImage);
	void fitImage();
	void scaleImage(double factor);
	void zoomOut();
	void zoomIn();
	void resetZoom();
	void contextMenu(QPoint point);
	void centerImage();

protected:
	void wheelEvent(QWheelEvent *event);

private:
	QLabel *preViewer;
	double pvscaleFactor;
	bool mCenter;
};

class OutputViewWidget: public TitledPanel {
	Q_OBJECT
public:
	explicit OutputViewWidget(QWidget * parent = 0);

	LatexLogModel* getLogModel();
	void loadLogFile(const QString &logname, const QString & compiledFileName);
	bool logPresent();
	bool isPreviewPanelVisible();
	void setMessage(const QString &message); //set the message text (don't change page and no auto-show)
	void setSearchExpression(QString exp,bool isCase,bool isWord,bool isRegExp);
	int getNextSearchResultColumn(QString text,int col);
	bool childHasFocus();

	virtual void changeEvent(QEvent *event);
public slots:
	void copy();
	void resetMessages(bool noTabChange=false); //remove all messages and jumps to the message page (stays hidden if not visible)
	void resetMessagesAndLog(bool noTabChange=false);
	void resetLog(bool noTabChange=false);
	void selectLogEntry(int logEntryNumber, bool makeVisible=true);
	void showLogOrErrorList(bool noTabChange=false); //this will show the log unless the error list is open
	void showErrorListOrLog(); //this will show the error list unless log is open
	void showPreview();
	void showSearchResults();
	void gotoLogEntry(int logEntryNumber);
	void setTabbedLogView(bool tabbed);
	void previewLatex(const QPixmap& pixmap);
	void addSearch(QList<QDocumentLineHandle *> search, QDocument* doc);
	void clearSearch();
	void insertMessageLine(const QString &message); //inserts the message text (don't change page and no auto-show)
signals:
	void locationActivated(int line, QString fileName); //0-based line, absolute file name
	void logEntryActivated(int logEntryNumber);
	//void tabChanged(int tab);
	void jumpToSearch(QDocument* doc,int lineNumber);
private:
	PreviewWidget *previewWidget;
	QTableView *OutputErrorTable, *SplitViewOutputErrorTable;
	QTreeView *OutputSearchTree;
	LogEditor *OutputMessages,*SplitViewOutputLog;
	//Latex errors
	LatexLogModel * logModel; 
	SearchResultModel *searchResultModel;
	bool logpresent, tabbedLogView;
	
	void retranslateUi();
private slots:
	void clickedOnLogModelIndex(const QModelIndex& index);
	void clickedSearchResult(const QModelIndex& index);
	void gotoLogLine(int logLine);

	void copyMessage();
	void copyAllMessages();
	void copyAllMessagesWithLineNumbers();
	void showMessageInLog();
};

class SearchTreeDelegate: public QItemDelegate {
    Q_OBJECT
public:
        SearchTreeDelegate(QObject * parent = 0 );
protected:
        void paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
        //void drawDisplay( QPainter* painter, const QStyleOptionViewItem& option, const QRect& rect, const QString& text ) const;
        QSize sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const;
};

#endif 
