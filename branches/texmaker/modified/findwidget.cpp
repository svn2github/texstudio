/***************************************************************************
 *   copyright       : (C) 2003-2007 by Pascal Brachet                     *
 *   http://www.xm1math.net/texmaker/                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "findwidget.h"
#include <QKeyEvent>

FindWidget::FindWidget(QWidget* parent)
    : QWidget( parent)
{
ui.setupUi(this);
connect(ui.findButton, SIGNAL( clicked() ), this, SLOT( doFind() ) );
connect(ui.findBackwardButton, SIGNAL( clicked() ), this, SLOT( doFindInvert() ) );
connect(ui.closeButton, SIGNAL( clicked() ), this, SLOT( doHide() ) );
ui.findButton->setShortcut(Qt::Key_Return);
ui.findBackwardButton->setShortcut(Qt::Key_Return | Qt::SHIFT);
ui.closeButton->setShortcut(Qt::Key_Escape);
 }

FindWidget::~FindWidget()
{
}
/*
void FindWidget::keyPressEvent ( QKeyEvent * event ){
    if (event->key()==Qt::Key_Shift) shiftPressed=true;
    QWidget::keyPressEvent(event);
}
void FindWidget::keyReleaseEvent ( QKeyEvent * event ){
    if (event->key()==Qt::Key_Shift) shiftPressed=false;
    QWidget::keyReleaseEvent(event);
}
*/

void FindWidget::find(bool invert)
{
if ( !editor ) return;
if ( !editor->search( ui.comboFind->currentText(), ui.checkCase->isChecked(),	ui.checkWords->isChecked(), !invert, !ui.checkBegin->isChecked() ) )
   {
   ui.checkBegin->setChecked( TRUE );
   }
else ui.checkBegin->setChecked( FALSE );
editor->viewport()->repaint();
}

void FindWidget::doFind()
{
    find(false);
}
void FindWidget::doFindInvert()
{
    find(true);

}
void FindWidget::doHide()
{
hide();
if ( editor )
	{
	editor->viewport()->repaint();
	editor->setFocus();
	}
}

void FindWidget::SetEditor(LatexEditor *ed)
{
editor=ed;
}

