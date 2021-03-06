/****************************************************************************
**
**   copyright       : (C) 2003-2010 by Pascal Brachet                     
**   http://www.xm1math.net/texmaker/                                      
**
** Parts of this file come from Texworks (GPL) Copyright (C) 2007-2010  Jonathan Kew

** Parts of this file come from the documentation of Qt. It was originally
** published as part of Qt Quarterly.
** Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
**
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file.  Please review the following information
** to ensure GNU General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.  In addition, as a special
** exception, Nokia gives you certain additional rights. These rights
** are described in the Nokia Qt GPL Exception version 1.3, included in
** the file GPL_EXCEPTION.txt in this package.
**
** Qt for Windows(R) Licensees
** As a special exception, Nokia, as the sole copyright holder for Qt
** Designer, grants users of the Qt/Eclipse Integration plug-in the
** right for the Qt/Eclipse Integration to link to functionality
** provided by Qt Designer and its related libraries.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
****************************************************************************/
 
#include "pdfviewer.h"

#include <QtGui>
#include <QAction>
#include <QFontMetrics>
#include <QPainterPath>
#include <QDebug>

#define SYNCTEX_GZ_EXT ".synctex.gz"
#define SYNCTEX_EXT ".synctex"


PdfViewer::PdfViewer( const QString fileName, const QString externalCommand, QWidget* parent, Qt::WFlags flags)
    : QMainWindow( parent, flags )
{
setWindowTitle("Texmaker : pdf preview");
#ifdef Q_WS_MACX
setWindowIcon(QIcon(":/images/logo128.png"));
#else
setWindowIcon(QIcon(":/images/appicon.png"));
#endif
pdf_file=fileName;
viewpdf_command=externalCommand;
lastFile=fileName;
lastPage=1;
fileLoaded=false;
currentPage=1;
currentScale=1;
scanner=NULL;

pagesWidget = new QDockWidget(this);
pagesWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
pagesWidget->setFeatures(QDockWidget::DockWidgetClosable);
pagesWidget->setWindowTitle(tr("Pages"));
addDockWidget(Qt::LeftDockWidgetArea,pagesWidget);

listpagesWidget=new QListWidget(pagesWidget);
pagesWidget->setWidget(listpagesWidget);

scrollArea=new PdfScrollArea(this);
scrollArea->setWidgetResizable(true);
scrollArea->setAlignment(Qt::AlignCenter);
connect(scrollArea, SIGNAL(pageup()), this, SLOT(wheelpageUp()));
connect(scrollArea, SIGNAL(pagedown()), this, SLOT(wheelpageDown()));


pdfWidget = new PdfDocumentWidget();
scrollArea->setWidget(pdfWidget);
setCentralWidget(scrollArea);
connect(pdfWidget, SIGNAL(syncpage(int, const QPointF&)), this, SLOT(jumpToEditor(int, const QPointF&)));

QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
fileMenu->addAction(tr("Exit"), this, SLOT(close()));

QToolBar *toolBar = addToolBar("Pdf controls");

upAct = new QAction(QIcon(":/images/up.png"), tr("Previous"), this);
connect(upAct, SIGNAL(triggered()), this, SLOT(pageUp()));
toolBar->addAction(upAct);

downAct = new QAction(QIcon(":/images/down.png"), tr("Next"), this);
connect(downAct, SIGNAL(triggered()), this, SLOT(pageDown()));
toolBar->addAction(downAct);

connect(listpagesWidget, SIGNAL(itemClicked ( QListWidgetItem*)), this, SLOT(slotItemClicked(QListWidgetItem*)));
connect(pdfWidget, SIGNAL(pageChanged(int)),this, SLOT(updateCurrentPage(int)));

toolBar->addSeparator();

fitWithAct = new QAction(QIcon(":/images/zoom-fit-width.png"), tr("Fit Width"), this);
connect(fitWithAct, SIGNAL(triggered()), this, SLOT(fitWidth()));
toolBar->addAction(fitWithAct);

fitPageAct = new QAction(QIcon(":/images/zoom-fit-best.png"), tr("Fit Page"), this);
connect(fitPageAct, SIGNAL(triggered()), this, SLOT(fitPage()));
toolBar->addAction(fitPageAct);

zoominAct = new QAction(QIcon(":/images/zoom-in.png"), tr("Zoom In"), this);
connect(zoominAct, SIGNAL(triggered()), this, SLOT(zoomIn()));
toolBar->addAction(zoominAct);

zoomoutAct = new QAction(QIcon(":/images/zoom-out.png"), tr("Zoom Out"), this);
connect(zoomoutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));
toolBar->addAction(zoomoutAct);

scaleComboBox = new QComboBox(toolBar);
scaleFactors << 0.25 << 0.5 << 0.75 << 1. << 1.25 << 1.5 << 2. << 3. << 4.;
QStringList scalePercents;
scalePercents << "25%" << "50%" << "75%" << "100%" << "125%" << "150%" << "200%" << "300%" << "400%";
scaleComboBox->addItems(scalePercents);
connect(scaleComboBox, SIGNAL(currentIndexChanged(int)),this, SLOT(scaleDocument(int)));
toolBar->addWidget(scaleComboBox);

toolBar->addSeparator();

searchLineEdit = new QLineEdit(toolBar);
connect(searchLineEdit, SIGNAL(returnPressed()), this, SLOT(searchDocument()));
toolBar->addWidget(searchLineEdit);

findButton=new QPushButton(tr("Find"),toolBar);
connect(findButton, SIGNAL(clicked()), this, SLOT(searchDocument()));
toolBar->addWidget(findButton);

toolBar->addSeparator();
QWidget* spacer = new QWidget();
spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
toolBar->addWidget(spacer);

externButton=new QPushButton(tr("External Viewer"),toolBar);
connect(externButton, SIGNAL(clicked()), this, SLOT(runExternalViewer()));
toolBar->addWidget(externButton);

searchLineEdit->setEnabled(false);
findButton->setEnabled(false);
scaleComboBox->setEnabled(false);
externButton->setEnabled(false);
upAct->setEnabled(false);
downAct->setEnabled(false);
fitWithAct->setEnabled(false);
fitPageAct->setEnabled(false);
zoominAct->setEnabled(false);
zoomoutAct->setEnabled(false);

openFile(pdf_file,viewpdf_command);
}

PdfViewer::~PdfViewer()
{
if (scanner != NULL) synctex_scanner_free(scanner);
if (proc && proc->state()==QProcess::Running) 
	{
	proc->kill(); 
	delete proc ;
	}
}

void PdfViewer::openFile(QString fn,QString ec)
{
pdf_file=fn;
viewpdf_command=ec;
int lpage=lastPage;
if (scanner != NULL) 
  {
  synctex_scanner_free(scanner);
  scanner = NULL;
  }
if (pdfWidget->setDocument(fn)) 
    {
    searchLineEdit->setEnabled(true);
    findButton->setEnabled(true);
    scaleComboBox->setEnabled(true);
    externButton->setEnabled(true);
    upAct->setEnabled(false);
    downAct->setEnabled(true);
    fitWithAct->setEnabled(true);
    fitPageAct->setEnabled(true);
    zoominAct->setEnabled(true);
    zoomoutAct->setEnabled(true);
    listpagesWidget->clear();
    QString syncFile = QFileInfo(fn).canonicalFilePath();
    scanner = synctex_scanner_new_with_output_file(syncFile.toUtf8().data(), NULL, 1);
     for (int i = 1; i <= pdfWidget->document()->numPages(); ++i)
      {
      listpagesWidget->addItem(tr("Page")+" "+QString::number(i)); 
      }
    if ((fn==lastFile) && (lpage <= pdfWidget->document()->numPages()) && (lpage>1))
      {
      currentPage=lpage;
      pdfWidget->setPage(currentPage);
      }
    else 
      {
      currentPage=1;
      lastPage=1;
      }
    fileLoaded=true;
    lastFile=fn;
    updateCurrentPage(currentPage);
    QFontMetrics fm(qApp->font());
    listpagesWidget->setFixedWidth(30+fm.width(tr("Page")+" "+QString::number(pdfWidget->document()->numPages())));
    } 
    else 
      {
      QMessageBox::warning( this,tr("Error"),tr("File not found"));
      fileLoaded=false;
      }
}

void PdfViewer::jumpToPdfFromSource(QString sourceFile, int source_line)
{
if (!fileLoaded) return;
if (scanner == NULL) return;
const QFileInfo sourceFileInfo(sourceFile);
QDir curDir(QFileInfo(pdf_file).canonicalPath());
synctex_node_t node = synctex_scanner_input(scanner);
QString name;
bool found = false;
while (node != NULL) 
  {
  name = QString::fromUtf8(synctex_scanner_get_name(scanner, synctex_node_tag(node)));
  const QFileInfo fi(curDir, name);
  if (fi == sourceFileInfo) 
    {
    found = true;
    break;
    }
  node = synctex_node_sibling(node);
  }
if (!found) return;
if (synctex_display_query(scanner, name.toUtf8().data(), source_line, 0) > 0) 
  {
  int page = -1;
  QPainterPath path;
  while ((node = synctex_next_result(scanner)) != NULL) 
    {
    if (page == -1) page = synctex_node_page(node);
    if (synctex_node_page(node) != page) continue;
    QRectF nodeRect(synctex_node_box_visible_h(node),
				    synctex_node_box_visible_v(node) - synctex_node_box_visible_height(node),
				    synctex_node_box_visible_width(node),
				    synctex_node_box_visible_height(node) + synctex_node_box_visible_depth(node));
    path.addRect(nodeRect);
    }
  if (page > 0) 
    {
    gotoPage(page);
    QRectF r = path.boundingRect();
    scrollArea->ensureVisible((int)((r.left() + r.right()) / 2 * pdfWidget->physicalDpiX() / 72 * currentScale),
										(int)((r.top() + r.bottom()) / 2 * pdfWidget->physicalDpiY() / 72 * currentScale));
//    path.setFillRule(Qt::WindingFill);
//    pdfWidget->setHighlightPath(path);
//    pdfWidget->update();
    }
  }
}

void PdfViewer::gotoPage(int page)
{
if (!fileLoaded) return;
if ((page <= pdfWidget->document()->numPages()) && (page>=1))
  {
  currentPage=page;
  lastPage=currentPage;
  pdfWidget->setPage(currentPage);
  scrollArea->scrolltoMax();
  }
}

void PdfViewer::updateCurrentPage(int index)
{
if (!fileLoaded) return;
currentPage=index;
lastPage=currentPage;
if (currentPage==1) upAct->setEnabled(false);
else upAct->setEnabled(true);
if (currentPage==pdfWidget->document()->numPages()) downAct->setEnabled(false);
else downAct->setEnabled(true);
QList<QListWidgetItem *> fItems=listpagesWidget->findItems (tr("Page")+" "+QString::number(index),Qt::MatchRecursive);
if ((fItems.size()>0 ) && (fItems.at(0))) listpagesWidget->setCurrentItem(fItems.at(0));
}

void PdfViewer::scaleDocument(int index)
{
if (!fileLoaded) return;
currentScale=scaleFactors[index];
pdfWidget->setScale(currentScale);
}

void PdfViewer::searchDocument()
{
if (!fileLoaded) return;
QRectF location;
if (searchLineEdit->text().isEmpty()) return;
location = pdfWidget->searchForwards(searchLineEdit->text());
//else location = pdfWidget->searchBackwards(searchLineEdit->text());

QPoint target = pdfWidget->matrix().mapRect(location).center().toPoint();
scrollArea->ensureVisible(target.x(), target.y());
}

void PdfViewer::pageUp()
{
if (!fileLoaded) return;
if (currentPage>1)
  {
  currentPage--;
  lastPage=currentPage;
  pdfWidget->setPage(currentPage);
  }
}

void PdfViewer::pageDown()
{
if (!fileLoaded) return;
if (currentPage<pdfWidget->document()->numPages())
  {
  currentPage++;
  lastPage=currentPage;
  pdfWidget->setPage(currentPage);
  }
}

void PdfViewer::wheelpageUp()
{
if (!fileLoaded) return;
if (currentPage>1)
  {
  currentPage--;
  lastPage=currentPage;
  pdfWidget->setPage(currentPage);
  scrollArea->scrolltoMax();
  }
}

void PdfViewer::wheelpageDown()
{
if (!fileLoaded) return;
if (currentPage<pdfWidget->document()->numPages())
  {
  currentPage++;
  lastPage=currentPage;
  pdfWidget->setPage(currentPage);
  scrollArea->scrolltoMin();
  }
}

void PdfViewer::fitWidth()
{
if (!fileLoaded) return;
qreal portWidth = scrollArea->viewport()->width();
QSizeF	pageSize = pdfWidget->document()->page(currentPage-1)->pageSizeF();
if (pageSize.width()!=0) currentScale = portWidth / pageSize.width()*72.0/pdfWidget->physicalDpiX();
else return;
if (currentScale < 0.25) currentScale = 0.25;
else if (currentScale > 4) currentScale = 4;
pdfWidget->setScale(currentScale);
pdfWidget->setPage();
}

void PdfViewer::fitPage()
{
if (!fileLoaded) return;
qreal portWidth = scrollArea->viewport()->width();
qreal portHeight = scrollArea->viewport()->height();
QSizeF	pageSize = pdfWidget->document()->page(currentPage-1)->pageSizeF();
qreal scaleW;
qreal scaleH;
if (pageSize.width()!=0) scaleW = portWidth / pageSize.width()*72.0/pdfWidget->physicalDpiX();
else return;
if (pageSize.height()!=0) scaleH = portHeight / pageSize.height()*72.0/pdfWidget->physicalDpiY();
else return;
if (scaleH < scaleW) currentScale=scaleH;
else currentScale=scaleW ;
if (currentScale < 0.25) currentScale = 0.25;
else if (currentScale > 4) currentScale = 4;
pdfWidget->setScale(currentScale);
pdfWidget->setPage();
}

void PdfViewer::zoomIn()
{
if (!fileLoaded) return;
currentScale+=0.1;
if (currentScale > 4) currentScale = 4;
pdfWidget->setScale(currentScale);
}

void PdfViewer::zoomOut()
{
if (!fileLoaded) return;
currentScale-=0.1;
if (currentScale < 0.25) currentScale = 0.25;
pdfWidget->setScale(currentScale);
}

void PdfViewer::runExternalViewer()
{
if (!fileLoaded) return;
QString command=viewpdf_command;
command.replace("%.pdf","\""+pdf_file+"\"");
QFileInfo fi(pdf_file);
if (fi.exists()) 
	{
	proc = new QProcess( this );
	proc->setWorkingDirectory(fi.absolutePath());
	proc->start(command);
	}
}

void PdfViewer::slotItemClicked(QListWidgetItem* item)
{
QString txt=item->text().section(" ",-1);
currentPage=txt.toInt();
pdfWidget->setPage(currentPage);
}

void PdfViewer::jumpToEditor(int page, const QPointF& pos)
{
if (scanner == NULL) return;
if (synctex_edit_query(scanner, page+1, pos.x(), pos.y()) > 0) 
  {
  synctex_node_t node;
  while ((node = synctex_next_result(scanner)) != NULL) 
    {
    QString filename = QString::fromUtf8(synctex_scanner_get_name(scanner, synctex_node_tag(node)));
    QDir curDir(QFileInfo(pdf_file).canonicalPath());
    emit openDocAtLine(QFileInfo(curDir, filename).canonicalFilePath(),synctex_node_line(node));
    break;
    }
  }
}

void PdfViewer::keyPressEvent ( QKeyEvent * e ) 
{
if ( e->key()==Qt::Key_F1) 
    {
    emit sendFocusToEditor();
    }
else QMainWindow::keyPressEvent(e);
}