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

#ifndef LATEXEDITOR_H
#define LATEXEDITOR_H

#include <QWidget>
#include <QString>
#include <QTextEdit>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QCompleter>

#include "latexhighlighter.h"

//class QCompleter;
//class ParenMatcher;
//class QTextBlock;



class TextMarkPosition{
public:
    int block;
    int index;
    TextMarkPosition():block(-1){}
    TextMarkPosition(const QTextCursor & c){
        block=c.blockNumber();
        index=c.position()-c.block().position();
    }
    TextMarkPosition(int blockNr, int lineXNr){
        block=blockNr;
        index=lineXNr;
    }
};

class LatexEditor : public QTextEdit  {
   Q_OBJECT
public:

QVector<QString> *UserKeyReplace, *UserKeyReplaceAfterWord, *UserKeyReplaceBeforeWord;

LatexEditor(QWidget *parent,QFont & efont, QColor colMath, QColor colCommand, QColor colKeyword);
~LatexEditor();
//static void clearMarkerFormat(const QTextBlock &block, int markerId);
void gotoLine( int line );
bool search( const QString &expr, bool cs, bool wo, bool forward, bool startAtCursor );
void replace( const QString &r);
void commentSelection();
void uncommentSelection();
void indentSelection();
void unindentSelection();
void changeFont(QFont & new_font);
QString getEncoding();
void setEncoding(QString enc);
int getCursorPosition(int parag, int index);
void setCursorPosition(int para, int index);
void removeOptAlt();
int numoflines();
int linefromblock(const QTextBlock& p);
QVector<TextMarkPosition> UserBookmark;
void selectword(int line, int col, QString word);
LatexHighlighter *highlighter;
void setCompleter(QCompleter *completer);
QCompleter *completer() const;
void jumpToMarkPosition(const TextMarkPosition& position);
void jumpChangePositionForward();
void jumpChangePositionBackward();
private:
QString encoding;
QString textUnderCursor() const;
QCompleter *c;
QList<TextMarkPosition> lastChangePositions;
int curChangePosition,lastCursorChangeTop,lastCursorTop,lastBlockLength,lastLineCount;
//ParenMatcher *matcher;
private slots:
void checkSpellingWord();
void checkSpellingDocument();
void insertCompletion(const QString &completion);
void textChanged();
void cursorPositionChanged();
protected:
void paintEvent(QPaintEvent *event);
void contextMenuEvent(QContextMenuEvent *e);
void keyPressEvent ( QKeyEvent * e );
void focusInEvent(QFocusEvent *e);
signals:
void spellme();
void updateLineNumbers();
void linesChanged();
};

#endif
