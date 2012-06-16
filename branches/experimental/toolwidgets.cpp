#include "toolwidgets.h"
#include "math.h"
#include "smallUsefulFunctions.h"
#include "qdocument.h"

void adjustScrollBar(QScrollBar *scrollBar, double factor)
{
	scrollBar->setValue(int(factor * scrollBar->value()
							+ ((factor - 1) * scrollBar->pageStep()/2)));
}


PreviewWidget::PreviewWidget(QWidget * parent): QScrollArea(parent){
	setBackgroundRole(QPalette::Base);

	mCenter=false;

	preViewer = new QLabel(this);
	preViewer->setBackgroundRole(QPalette::Base);
	preViewer->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	preViewer->setScaledContents(true);
	preViewer->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(preViewer,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenu(QPoint)));
	connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(contextMenu(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	setWidget(preViewer);
}

void PreviewWidget::scaleImage(double factor)
{
	REQUIRE(preViewer->pixmap());
	pvscaleFactor *= factor;
	preViewer->resize(pvscaleFactor * preViewer->pixmap()->size());

	adjustScrollBar(horizontalScrollBar(), factor);
	adjustScrollBar(verticalScrollBar(), factor);

}

void PreviewWidget::previewLatex(const QPixmap& previewImage){
	preViewer->setPixmap(previewImage);
	preViewer->adjustSize();
	pvscaleFactor=1.0;
}

void PreviewWidget::fitImage(){
	REQUIRE(preViewer->pixmap());
	// needs to be improved
	QSize m_size=size()-QSize(2,2);
	QSize m_labelSize=preViewer->size();
	qreal ratio=1.0*m_labelSize.height()/m_labelSize.width();
	qreal ratioPreviewer=1.0*m_size.height()/m_size.width();
	int h,w;
	if(ratioPreviewer>ratio){
		h=qRound(ratio*m_size.width());
		w=m_size.width();
		pvscaleFactor=1.0*w/preViewer->pixmap()->size().width();
	} else {
		h=m_size.height();
		w=qRound(m_size.height()/ratio);
		pvscaleFactor=1.0*h/preViewer->pixmap()->size().height();;
	}
	preViewer->resize(w,h);
	//setWidgetResizable(true);
}
void PreviewWidget::centerImage(){
	mCenter=!mCenter;
	if(mCenter) setAlignment(Qt::AlignCenter);
	else setAlignment(Qt::AlignLeft|Qt::AlignTop);
	scaleImage(1.0);
}

void PreviewWidget::zoomOut(){
	scaleImage(1/1.4);
}

void PreviewWidget::zoomIn(){
	scaleImage(1.4);
}

void PreviewWidget::resetZoom(){
	pvscaleFactor=1.0;
	scaleImage(1.0);
}

void PreviewWidget::wheelEvent(QWheelEvent *event){
	if (!preViewer->pixmap()) return;
	if(event->modifiers()==Qt::ControlModifier){
		float numDegrees = event->delta() / 8.0f;
		float numSteps = numDegrees / 15.0f;
		scaleImage(pow(1.4,numSteps));
		event->accept();
	} else QScrollArea::wheelEvent(event);
}

void PreviewWidget::contextMenu(QPoint point) {
	if (!preViewer->pixmap()) return;
	QMenu menu;
	menu.addAction(tr("zoom in "),this, SLOT(zoomIn()));
	menu.addAction(tr("zoom out"),this, SLOT(zoomOut()));
	menu.addAction(tr("reset zoom"),this, SLOT(resetZoom()));
	menu.addAction(tr("fit"),this, SLOT(fitImage()));
	if(mCenter) menu.addAction(tr("left-align image"),this, SLOT(centerImage()));
	else menu.addAction(tr("center image"),this, SLOT(centerImage()));
	QWidget* menuParent = qobject_cast<QWidget*>(sender());
	Q_ASSERT(menuParent);
	if (!menuParent) menuParent = preViewer;
	menu.exec(menuParent->mapToGlobal(point));
}




const int LAYOUT_PAGE_MESSAGES=0;
const int LAYOUT_PAGE_LOG=1;
const int LAYOUT_PAGE_ERRORS=2;
const int LAYOUT_PAGE_PREVIEW=3;
const int LAYOUT_PAGE_SEARCH=4;
	
OutputViewWidget::OutputViewWidget(QWidget * parent): TitledPanel(parent), logModel(0), logpresent(false), tabbedLogView(false){

	logModel = new LatexLogModel(this);//needs loaded line marks
	searchResultModel = new SearchResultModel(this);

	SplitViewOutputErrorTable= new QTableView(this);
	OutputErrorTable= new QTableView(this); // second table view for tab log view

	// Search Results tree
	SearchTreeDelegate *searchDelegate=new SearchTreeDelegate(this);
	OutputSearchTree= new QTreeView(this);
	OutputSearchTree->setUniformRowHeights(true);
	OutputSearchTree->setModel(searchResultModel);
	OutputSearchTree->setItemDelegate(searchDelegate);
	connect(OutputSearchTree,SIGNAL(clicked(QModelIndex)),this,SLOT(clickedSearchResult(QModelIndex)));

	QFontMetrics fm(QApplication::font());
	for (int i=0;i<2;i++){ //setup tables
		QTableView* table=(i==0)?SplitViewOutputErrorTable:OutputErrorTable;
		table->setModel(logModel);

		table->setSelectionBehavior(QAbstractItemView::SelectRows);
		table->setSelectionMode(QAbstractItemView::SingleSelection);
		table->setColumnWidth(0,fm.width("> "));
		table->setColumnWidth(1,20*fm.width("w"));
		table->setColumnWidth(2,fm.width("WarningW"));
		table->setColumnWidth(3,fm.width("Line WWWWW"));
		table->setColumnWidth(4,20*fm.width("w"));
		connect(table, SIGNAL(clicked(const QModelIndex &)), this, SLOT(clickedOnLogModelIndex(const QModelIndex &)));
	
		table->horizontalHeader()->setStretchLastSection(true);
		table->setMinimumHeight(5*(fm.lineSpacing()+4));

        QAction * act = new QAction(tr("&Copy"),table);
		connect(act, SIGNAL(triggered()), SLOT(copyMessage()));
		table->addAction(act);
        act = new QAction(tr("&Copy all"),table);
		connect(act, SIGNAL(triggered()), SLOT(copyAllMessages()));
		table->addAction(act);
        act = new QAction(tr("&Copy all with line numbers"),table);
		connect(act, SIGNAL(triggered()), SLOT(copyAllMessagesWithLineNumbers()));
		table->addAction(act);
        act = new QAction(tr("&Show in log"),table);
		connect(act, SIGNAL(triggered()), SLOT(showMessageInLog()));;
		table->addAction(act);

		table->setContextMenuPolicy(Qt::ActionsContextMenu);
	}

	OutputMessages = new LogEditor(this);
	OutputMessages->setFocusPolicy(Qt::ClickFocus);
	OutputMessages->setMinimumHeight(3*(fm.lineSpacing()+4));
	OutputMessages->setReadOnly(true);
	connect(OutputMessages, SIGNAL(clickOnLogLine(int)),this,SLOT(gotoLogLine(int)));

	SplitViewOutputLog = new LogEditor(this);
	SplitViewOutputLog->setFocusPolicy(Qt::ClickFocus);
	SplitViewOutputLog->setMinimumHeight(3*(fm.lineSpacing()+4));
	SplitViewOutputLog->setReadOnly(true);
	connect(SplitViewOutputLog, SIGNAL(clickOnLogLine(int)),this,SLOT(gotoLogLine(int)));


	QVBoxLayout* OutputVLayout= new QVBoxLayout(); //contains the widgets for the normal mode (OutputTable + OutputLogTextEdit)
	OutputVLayout->setSpacing(0);
	OutputVLayout->setMargin(0);

	// add widget to log view
	appendPage(new TitledPanelPage(OutputMessages, "messages", tr("Messages")), false);

	OutputVLayout->addWidget(SplitViewOutputErrorTable);
	OutputVLayout->addWidget(SplitViewOutputLog);
	QWidget* tempWidget=new QWidget (this);
	tempWidget->setLayout(OutputVLayout);

	TitledPanelPage *page = new TitledPanelPage(tempWidget, "log", tr("Log File"));
	QAction *act = new QAction(QIcon(":images/test/Transition.png"), "Test action (no functionality)", page);
	page->addToolbarAction(act);


	appendPage(page, false);
	appendPage(new TitledPanelPage(OutputErrorTable, "errors", tr("Errors")), false);



	// previewer
	previewWidget = new PreviewWidget(this);
	//previewWidget->hide();
	//TODO why crashes?
	appendPage(new TitledPanelPage(previewWidget, "preview", tr("Preview")), false);

	// global search results
	appendPage(new TitledPanelPage(OutputSearchTree, "search", tr("Search")));


	// order for tabbar
	/*
	logViewerTabBar=new QTabBar(this);
	logViewerTabBar->addTab("m");
	logViewerTabBar->addTab("l");
	logViewerTabBar->addTab("e");
	logViewerTabBar->addTab("p");
	logViewerTabBar->addTab("s");
	retranslateUi();
	logViewerTabBar->hide(); //internal default is non tabbed mode


	connect(logViewerTabBar, SIGNAL(currentChanged(int)),
			OutputStackWidget, SLOT(setCurrentIndex(int)));

	connect(logViewerTabBar, SIGNAL(currentChanged(int)),
	        this, SIGNAL(tabChanged(int)));
	*/
}
void OutputViewWidget::setTabbedLogView(bool tabbed){
	tabbedLogView=tabbed;
	if (tabbed) {
// TODO
//		this->setTitleBarWidget(logViewerTabBar);
		SplitViewOutputErrorTable->hide();
	} else {
// TODO
//		this->setTitleBarWidget(0);
		SplitViewOutputErrorTable->show();
	}
}
void OutputViewWidget::previewLatex(const QPixmap& pixmap){
	previewWidget->previewLatex(pixmap);
	//showPreview();	
}

void OutputViewWidget::clickedSearchResult(const QModelIndex& index){
	QDocument* doc = searchResultModel->getDocument(index);
	if (!doc) return;
	emit jumpToSearch(doc,searchResultModel->getLineNumber(index));
}

LatexLogModel* OutputViewWidget::getLogModel(){
	return logModel;
}

//copied and modified from qbytearray.cpp
//should be an optimization for qtextstream, but doesn't really improve anything
QByteArray simplifyLineConserving(const QByteArray& ba)
{
	if (ba.size() == 0)
		return QByteArray();
	QByteArray result(ba.size(), Qt::Uninitialized);
	const char *from = ba.constData();
	const char *fromend = from + ba.size();
	int outc=0;
	char *to = result.data();
	for (;;) {
		while (from!=fromend && isspace(uchar(*from)))
			from++;
		while (from!=fromend && !isspace(uchar(*from)))
			to[outc++] = *from++;
		if (from!=fromend) {
			if (uchar(*from) == '\n' || uchar(*from) == '\r') to[outc++] = '\n';
			else to[outc++] = ' ';
		} else
			break;
	}
	if (outc > 0 && to[outc-1] == ' ')
		outc--;
	result.resize(outc);
	return result;
}

void OutputViewWidget::loadLogFile(const QString &logname, const QString & compiledFileName){
	SplitViewOutputLog->clear();
	QFile f(logname);
	if (f.open(QIODevice::ReadOnly)) {

		if (f.size() > 2*1024*1024 && 
		    !txsConfirmWarning(tr("The logfile is very large (> %1 MB) are you sure you want to load it?").arg(f.size() / 1024 / 1024))) 
			return;
		
		//QByteArray fullLog = simplifyLineConserving(f.readAll());
		// TODO: if we want to habe simplification here it has to be smarter.
		// The above version trims whitespace, which leads to undesired effects due to the 80 char
		// line width of the log. "line\n 1"  would become "\line\n1" and, when rejoining lines for error/warning detection "line1".
		// Do we need this or can we just leave the output as it is?
		QByteArray fullLog = f.readAll();
		f.close();
		
		int sure;
		QTextCodec * codec = guessEncodingBasic(fullLog, &sure);
        if (!sure || !codec) codec = QTextCodec::codecForLocale();
		
		SplitViewOutputLog->setPlainText(codec->toUnicode(fullLog));
		
		logModel->parseLogDocument(SplitViewOutputLog->document(), compiledFileName);

		logpresent=true;		
		//update table size
		SplitViewOutputErrorTable->resizeColumnsToContents();
		SplitViewOutputErrorTable->resizeRowsToContents();
		OutputErrorTable->resizeColumnsToContents();
		OutputErrorTable->resizeRowsToContents();
		
		selectLogEntry(0,false);
	}
}
bool OutputViewWidget::logPresent(){
	return logpresent;
}
bool OutputViewWidget::isPreviewPanelVisible(){
	if (!isVisible()) return false;
	return currentPageId() == "preview";
}
void OutputViewWidget::setMessage(const QString &message){
	setCurrentPage("message");
	OutputMessages->setText(message);
}
void OutputViewWidget::insertMessageLine(const QString &message){
	OutputMessages->insertLine(message);
}
void OutputViewWidget::copy(){
	if (currentPageId() == "errors")
		copyMessage();
	else
		SplitViewOutputLog->copy();
}

void OutputViewWidget::resetMessages(bool noTabChange){
	OutputMessages->clear();
	if(!noTabChange) setCurrentPage("messages");
}
void OutputViewWidget::resetMessagesAndLog(bool noTabChange){
	resetMessages(noTabChange);
	resetLog(noTabChange);
}
void OutputViewWidget::resetLog(bool /*noTabChange*/){
	logpresent=false;
}
void OutputViewWidget::selectLogEntry(int logEntryNumber, bool makeVisible){
	if (logEntryNumber<0 || logEntryNumber>=logModel->count()) return;
	if (makeVisible) showErrorListOrLog();
	SplitViewOutputErrorTable->scrollTo(logModel->index(logEntryNumber,1),QAbstractItemView::PositionAtCenter);
	SplitViewOutputErrorTable->selectRow(logEntryNumber);
	OutputErrorTable->scrollTo(logModel->index(logEntryNumber,1),QAbstractItemView::PositionAtCenter);
	OutputErrorTable->selectRow(logEntryNumber);
	SplitViewOutputLog->setCursorPosition(logModel->at(logEntryNumber).logline, 0);
}
void OutputViewWidget::showLogOrErrorList(bool noTabChange){
	if (!isVisible()) show();
	QString id = currentPageId();
	if (id != "log" && id != "errors" && !noTabChange)
		setCurrentPage("log");
}
void OutputViewWidget::showErrorListOrLog(){
	if (!isVisible()) show();
	if (currentPageId() != "log")
		setCurrentPage("errors");
}

void OutputViewWidget::showPreview(){
	if (!isVisible()) show();
	setCurrentPage("preview");
}

void OutputViewWidget::showSearchResults(){
	if (!isVisible()) show();
	setCurrentPage("search");
}
void OutputViewWidget::gotoLogEntry(int logEntryNumber) {
	if (logEntryNumber<0 || logEntryNumber>=logModel->count()) return;
	//select entry in table view/log
	//OutputTable->scrollTo(logModel->index(logEntryNumber,1),QAbstractItemView::PositionAtCenter);
	//OutputLogTextEdit->setCursorPosition(logModel->at(logEntryNumber).logline, 0);
	selectLogEntry(logEntryNumber);
	//notify editor	
	emit logEntryActivated(logEntryNumber);
}

void OutputViewWidget::retranslateUi(){
	/* TODO obsolete
	logViewerTabBar->setTabText(0,tr("Messages"));
	logViewerTabBar->setTabText(1,tr("Log File"));
	logViewerTabBar->setTabText(2,tr("Errors"));
	logViewerTabBar->setTabText(3,tr("Preview"));
	logViewerTabBar->setTabText(4,tr("Search Results"));
	*/
}

void OutputViewWidget::clickedOnLogModelIndex(const QModelIndex& index){
	gotoLogEntry(index.row());
}
void OutputViewWidget::gotoLogLine(int logLine){
	gotoLogEntry(logModel->logLineNumberToLogEntryNumber(logLine));
}

void OutputViewWidget::copyMessage(){
	QModelIndex curMessage = tabbedLogView ? OutputErrorTable->currentIndex() : SplitViewOutputErrorTable->currentIndex();
	if (!curMessage.isValid()) return;
	curMessage = logModel->index(curMessage.row(), 3);
	REQUIRE(QApplication::clipboard());
	QApplication::clipboard()->setText(logModel->data(curMessage, Qt::DisplayRole).toString());
}
void OutputViewWidget::copyAllMessages(){
	QStringList result;
	for (int i=0;i<logModel->count();i++)
		result << logModel->data(logModel->index(i, 3), Qt::DisplayRole).toString();
	REQUIRE(QApplication::clipboard());
	QApplication::clipboard()->setText(result.join("\n"));
}
void OutputViewWidget::copyAllMessagesWithLineNumbers(){
	QStringList result;
	for (int i=0;i<logModel->count();i++)
		result << logModel->data(logModel->index(i, 2), Qt::DisplayRole).toString() +": "+logModel->data(logModel->index(i, 3), Qt::DisplayRole).toString();
	REQUIRE(QApplication::clipboard());
	QApplication::clipboard()->setText(result.join("\n"));
}

void OutputViewWidget::showMessageInLog(){
	setCurrentPage("log");
	QModelIndex curMessage = tabbedLogView ? OutputErrorTable->currentIndex() : SplitViewOutputErrorTable->currentIndex();
	if (!curMessage.isValid()) return;
	gotoLogEntry(curMessage.row());
}


void OutputViewWidget::addSearch(QList<QDocumentLineHandle *> lines, QDocument* doc){
	SearchInfo search;
	search.doc = doc;
	search.lines = lines;
	searchResultModel->addSearch(search);
}
void OutputViewWidget::clearSearch(){
	searchResultModel->clear();
}
void OutputViewWidget::setSearchExpression(QString exp,bool isCase,bool isWord,bool isRegExp){
        searchResultModel->setSearchExpression(exp,isCase,isWord,isRegExp);
}
int OutputViewWidget::getNextSearchResultColumn(QString text,int col){
        return searchResultModel->getNextSearchResultColumn(text,col);
}
bool OutputViewWidget::childHasFocus(){
	return SplitViewOutputLog->hasFocus() || (tabbedLogView?OutputErrorTable->hasFocus():SplitViewOutputErrorTable->hasFocus());
}

void OutputViewWidget::changeEvent(QEvent *event){
	switch (event->type()) {
	case QEvent::LanguageChange:
		retranslateUi();
		break;
	default:
		break;
	}	
}

//====================================================================
// CustomDelegate for search results
//====================================================================
SearchTreeDelegate::SearchTreeDelegate(QObject *parent):QItemDelegate(parent)
{
    ;
}

void SearchTreeDelegate::paint( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
{
    QPalette::ColorGroup    cg  = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;

    /*if( cg == QPalette::Normal && !(option.state & QStyle::State_Active) )
        cg = QPalette::Inactive;*/

    if( option.state & QStyle::State_Selected )
    {
        painter->fillRect( option.rect, option.palette.brush(cg, QPalette::Highlight) );
        painter->setPen( option.palette.color(cg, QPalette::HighlightedText) );
    }
    else
    {
        painter->setPen( option.palette.color(cg, QPalette::Text) );
    }

    if( index.data().toString().isEmpty() )
        return;
    painter->save();
    QString text=index.data().toString();
    QRect r=option.rect;
    QStringList textList=text.split("|");
    for(int i=0;i<textList.size();i++){
        QString temp=textList.at(i);
        int w=option.fontMetrics.width(temp);
        if(i%2) {
            painter->fillRect( QRect(r.left(),r.top(),w,r.height()), QBrush(Qt::yellow) );
        }
        painter->drawText(r,Qt::AlignLeft || Qt::AlignTop || Qt::TextSingleLine, temp);
        r.setLeft(r.left()+w+1);
    }
    painter->restore();
}

QSize SearchTreeDelegate::sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
 {
       QFontMetrics fontMetrics = option.fontMetrics;
       QRect rect = fontMetrics.boundingRect(index.data().toString());
       return QSize(rect.width(), rect.height());
}

