#include "latexcompleter.h"
#include "qdocumentline.h"
#include "qformatfactory.h"
#include <QApplication>
#include <QItemDelegate>
#include <QKeyEvent>
#include <QPainter>
#include <QStyleOptionViewItem>

#include <QMessageBox>
CompletionWord::CompletionWord(const QString &newWord){
    cursorPos=-1;
    anchorPos=-1;
    QString visibleWord;
    visibleWord.reserve(newWord.length());
    word.reserve(newWord.length());
    bool escape=false;
    bool inDescription=false;
    int formatStart=0;
    for (int i=0;i<newWord.length();i++)
        if (!escape) {
            if (newWord.at(i)==QChar('%')) escape=true;
            else {
                visibleWord+=newWord.at(i);
                if (!inDescription) word.append(newWord.at(i));
            }
        } else {
            escape=false;
            switch (newWord.at(i).toAscii()) {
                case '%': word+='%'; visibleWord+='%'; break;
                case '|': anchorPos=cursorPos; cursorPos=visibleWord.length(); break;
                case '<': 
                    inDescription=true; 
                    formatStart=visibleWord.length(); 
                    break;
                case '>': {
                    inDescription=false;  
                    descriptiveParts.append(QPair<int, int>(formatStart, visibleWord.length()-formatStart));
                    break;
                }
                default:;
            }
        }
    if (anchorPos==-1) anchorPos=cursorPos;
    shownWord=visibleWord;
    lword=word.toLower();
}



//------------------------------Default Input Binding--------------------------------
class CompleterInputBinding: public QEditor::InputBinding{
public:
    CompleterInputBinding():active(0){}
	virtual QString id() const{return "TexMakerX::CompleteInputBinding";}
    virtual QString name() const{return "TexMakerX::CompleteInputBinding";}

    virtual bool mousePressEvent(QMouseEvent* mouse, QEditor *editor){
        resetBinding();
        return false;
    }

    QString getCurWord(){
        //QMessageBox::information(0,curLine.text().mid(curStart,editor->cursor().columnNumber()-curStart),"",0);
        return curLine.text().mid(curStart,editor->cursor().columnNumber()-curStart);
    }

    bool insertCompletedWord(){
        if (completer->list->isVisible() && maxWritten>curStart && completer->list->currentIndex().isValid()) {
            QDocumentCursor cursor=editor->cursor();
            cursor.beginEditBlock();
            QVariant v=completer->list->model()->data(completer->list->currentIndex(),Qt::DisplayRole);
            if (!v.isValid() || !v.canConvert<CompletionWord>()) return false;
            CompletionWord cw= v.value<CompletionWord>();
            QString full=cw.shownWord;
            //int alreadyWrittenLen=editor->cursor().columnNumber()-curStart;            
            //remove current text for correct case
            for (int i=maxWritten-cursor.columnNumber();i>0;i--) cursor.deleteChar(); 
            for (int i=cursor.columnNumber()-curStart;i>0;i--) cursor.deletePreviousChar();
          //  cursor.setColumnNumber(curStart);
            cursor.insertText(full);
            cursor.endEditBlock();
            
            if (QDocument::formatFactory()) 
                for (int i=0;i<cw.descriptiveParts.size();i++) 
                    curLine.addOverlay(QFormatRange(cw.descriptiveParts[i].first+curStart,cw.descriptiveParts[i].second,QDocument::formatFactory()->id("temporaryCodeCompletion")));
            
            //place cursor/add \end
            QDocumentCursor selector=editor->cursor();
            int selectFrom=-1;
            int selectTo=-1;
            int deltaLine=0;
            if (full.startsWith("\\begin")) {
                //int curColumnNumber=cursor.columnNumber();
                QString indent=curLine.indentation();
                int p=full.indexOf("{");
                QString content="content...";
                if (editor->flag(QEditor::AutoIndent)){
                    cursor.insertText( "\n"+indent+"\t"+content+"\n"+indent+"\\end"+full.mid(p,full.indexOf("}")-p+1));
                    indent+="\t";
                } else
                    cursor.insertText( "\n"+indent+content+"\n"+indent+"\\end"+full.mid(p,full.indexOf("}")-p+1));
                if (QDocument::formatFactory()) 
                    for (int i=0;i<cw.descriptiveParts.size();i++) 
                        curLine.next().addOverlay(QFormatRange(indent.size(),content.size(),QDocument::formatFactory()->id("temporaryCodeCompletion")));
                

                if (cw.cursorPos==-1) {
                    deltaLine=1;
                    selectFrom=indent.length();
                    selectTo=indent.length()+content.size();
                } else {
                    selectFrom=cw.anchorPos+curStart;
                    selectTo=cw.cursorPos+curStart;
                }
            } else if (cw.cursorPos>-1) {
                    selectFrom=cw.anchorPos+curStart;
                    selectTo=cw.cursorPos+curStart;
            } else editor->setCursor(cursor); //place after insertion
            if (selectFrom!=-1){
                if (deltaLine>0) selector.movePosition(deltaLine,QDocumentCursor::Down,QDocumentCursor::MoveAnchor);
                selector.setColumnNumber(selectFrom);
                if (selectTo>selectFrom) selector.movePosition(selectTo-selectFrom,QDocumentCursor::Right,QDocumentCursor::KeepAnchor);
                else if (selectTo<selectFrom) selector.movePosition(selectFrom-selectTo,QDocumentCursor::Left,QDocumentCursor::KeepAnchor);
                editor->setCursor(selector);
            }
            return true;
        }
        return false;
    }

    virtual bool keyPressEvent(QKeyEvent *event, QEditor *editor)
    {
        if (event->key()==Qt::Key_Shift || event->key()==Qt::Key_Alt || event->key()==Qt::Key_Control) 
            return false;
        if (!active) return false; //we should never have been called
        bool handled=false;
        if (event->key()==Qt::Key_Backspace) {
            maxWritten--;
            editor->cursor().deletePreviousChar();
            if (editor->cursor().columnNumber()<=curStart) {
                resetBinding();
                return true;
            } else if (editor->cursor().columnNumber()+1<=curStart && !showAlways){
                completer->list->hide();
                return true;
            }
            handled=true;
        } else if (event->key()==Qt::Key_Delete) {
            if (editor->cursor().columnNumber()<maxWritten) maxWritten--;
            editor->cursor().deleteChar();
            handled=true;
        } else if (event->key()==Qt::Key_Left) {
            if (event->modifiers()) {
                resetBinding();
                return false;
            }
            editor->setCursorPosition(curLine.lineNumber(),editor->cursor().columnNumber()-1);
            if (editor->cursor().columnNumber()<=curStart) resetBinding();
            handled=true;
        } else if (event->key()==Qt::Key_Right) { 
            if (event->modifiers()) {
                resetBinding();
                return false;
            }
            editor->setCursorPosition(curLine.lineNumber(),editor->cursor().columnNumber()+1);
            if (editor->cursor().columnNumber()>maxWritten) resetBinding();
            handled=true;
        } else if (event->key()==Qt::Key_Up) { 
            if (!completer->list->isVisible()) {
                resetBinding();
                return false;
            }
            QModelIndex ind=completer->list->model()->index(completer->list->currentIndex().row()-1,0,QModelIndex());
            if (ind.isValid()) completer->list->setCurrentIndex(ind);
            return true;
        } else if (event->key()==Qt::Key_Down) { 
            if (!completer->list->isVisible()) {
                resetBinding();
                return false;
            }
            QModelIndex ind=completer->list->model()->index(completer->list->currentIndex().row()+1,0,QModelIndex());
            if (ind.isValid()) completer->list->setCurrentIndex(ind);
            return true;
        } else if (event->key()==Qt::Key_PageUp) { 
            if (!completer->list->isVisible()) {
                resetBinding();
                return false;
            }
            QModelIndex ind=completer->list->model()->index(completer->list->currentIndex().row()-5,0,QModelIndex());
            if (!ind.isValid()) ind=completer->list->model()->index(0,0,QModelIndex());
            if (ind.isValid()) completer->list->setCurrentIndex(ind);
            return true;
        } else if (event->key()==Qt::Key_PageDown) { 
            if (!completer->list->isVisible()) {
                resetBinding();
                return false;
            }
            QModelIndex ind=completer->list->model()->index(completer->list->currentIndex().row()+5,0,QModelIndex());
            if (!ind.isValid()) ind=completer->list->model()->index(completer->list->model()->rowCount()-1,0,QModelIndex());
            if (ind.isValid()) completer->list->setCurrentIndex(ind);
            return true;
        } else if (event->key()==Qt::Key_Home) { 
            if (!completer->list->isVisible()) {
                resetBinding();
                return false;
            }
            QModelIndex ind=completer->list->model()->index(0,0,QModelIndex());
            if (ind.isValid()) completer->list->setCurrentIndex(ind);
            return true;
        } else if (event->key()==Qt::Key_End) { 
            if (!completer->list->isVisible()) {
                resetBinding();
                return false;
            }
            QModelIndex ind=completer->list->model()->index(completer->list->model()->rowCount()-1,0,QModelIndex());
            if (ind.isValid()) completer->list->setCurrentIndex(ind);
            return true;
        } else if (event->key()==Qt::Key_Return || event->key()==Qt::Key_Enter) { 
            if (!insertCompletedWord()) {
                editor->cursor().insertText("\n");
                curLine=editor->document()->line(curLine.lineNumber()+1);
                editor->setCursorPosition(curLine.lineNumber(),curLine.length());
            }
            resetBinding();
            return true;
        } else {
            if (event->text().length()!=1 || event->text()==" ") {
                resetBinding();
                return false;
            } 
        
            QChar written=event->text().at(0);
            if (written=='\\') {
                if (getCurWord()=="\\") {
                    curStart+=2;
                    maxWritten=curStart;
                } else if (getCurWord()=="") {
                    maxWritten=curStart+1;
                } else {
                    insertCompletedWord();
                    curStart=editor->cursor().columnNumber();
                    maxWritten=curStart+1;
                }
                editor->cursor().insertText(written);
                handled=true;
            } else if (completer->acceptChar(written,editor->cursor().columnNumber()-curStart)) { 
                maxWritten++;
                editor->cursor().insertText(written);
                if (editor->cursor().columnNumber()+1>curStart)  
                    completer->list->show();
                handled=true;
            } else {
                resetBinding();
                return false;
            }
        }
        completer->updateList(getCurWord());
        if (!completer->list->currentIndex().isValid()) 
            completer->list->setCurrentIndex(completer->list->model()->index(0,0,QModelIndex()));
        return handled;
    }

    void cursorPositionChanged(QEditor* edit){
        if (edit!=editor) return; //should never happen
        QDocumentCursor c=editor->cursor();
        if (c.line()!=curLine || c.columnNumber()<curStart) resetBinding();
    }

    void resetBinding(){
        if (!active) return;
        editor->setInputBinding(oldBinding);
        editor->setFocus();
        if (completer) {
            completer->list->hide();
            completer->disconnect(editor,SIGNAL(cursorPositionChanged()),completer,SLOT(cursorPositionChanged()));
        }
        active=false;
    }
    
    void bindTo(QEditor * edit, LatexCompleter* caller, bool forced, int start){
        if (active) resetBinding();
        active=true;
        completer=caller;
        editor=edit;
        oldBinding=editor->inputBinding();
        editor->setInputBinding(this);        
        curStart=start>0?start:0;
        maxWritten=editor->cursor().columnNumber();
        if (maxWritten<start) maxWritten=start;
        curLine=editor->cursor().line();
        showAlways=forced;//curWord!="\\";
        if (showAlways) {
            completer->updateList(getCurWord());
            completer->list->show();
            completer->list->setCurrentIndex(completer->list->model()->index(0,0,QModelIndex()));
        }
    }


    bool isActive(){ return active;}
private:
    bool active;
    bool showAlways;
    LatexCompleter *completer;
    QEditor * editor;
    QEditor::InputBinding* oldBinding;
    QString completionWord;
    int curStart,maxWritten;
    QDocumentLine curLine;
};

CompleterInputBinding *completerInputBinding = new CompleterInputBinding();
//------------------------------Item Delegate--------------------------------
class CompletionItemDelegate: public QItemDelegate
{
public:
    CompletionItemDelegate(QObject* parent = 0): QItemDelegate(parent)
    {
    }
 
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QVariant v=index.model()->data(index);
        if (!v.isValid() || !v.canConvert<CompletionWord>()) return;
        CompletionWord cw=v.value<CompletionWord>();
        QFont fNormal=option.font;
        QFont fItalic=option.font;
        fItalic.setItalic(true);
        if ((QStyle::State_HasFocus | QStyle::State_Selected) & option.state) {
            //painter->setBackground(option.palette.brush(QPalette::Highlight));
            painter->fillRect(option.rect,option.palette.brush(QPalette::Highlight));
            painter->setPen(option.palette.color(QPalette::HighlightedText));
        } else {
            //painter->setBackground(option.palette.brush(QPalette::Base));
            painter->fillRect(option.rect,option.palette.brush(QPalette::Base));//doesn't seem to be necessary
            painter->setPen(option.palette.color(QPalette::Text));
        }
        QRect r=option.rect;
        r.setLeft(r.left()+2);
        if (cw.descriptiveParts.empty()) 
            painter->drawText(r,Qt::AlignLeft || Qt::AlignTop || Qt::TextSingleLine, cw.shownWord);
        else {
            QFontMetrics fmn(fNormal);
            QFontMetrics fmi(fItalic);
            int p=0;
            for (int i=0;i<cw.descriptiveParts.size();i++) {
                QString temp=cw.shownWord.mid(p,cw.descriptiveParts[i].first-p);
                painter->drawText(r,Qt::AlignLeft || Qt::AlignTop || Qt::TextSingleLine, temp);
                r.setLeft(r.left()+fmn.width(temp));
                temp=cw.shownWord.mid(cw.descriptiveParts[i].first,cw.descriptiveParts[i].second);
                painter->setFont(fItalic);
                painter->drawText(r,Qt::AlignLeft || Qt::AlignTop || Qt::TextSingleLine, temp);
                r.setLeft(r.left()+fmi.width(temp)+1);
                p=cw.descriptiveParts[i].first+cw.descriptiveParts[i].second;
                painter->setFont(fNormal);
            }
            painter->drawText(r,Qt::AlignLeft || Qt::AlignTop || Qt::TextSingleLine, cw.shownWord.mid(p));
        }
    }
};
 


//----------------------------list model------------------------------------
int CompletionListModel::rowCount(const QModelIndex &parent) const{
    return words.count();
}
QVariant CompletionListModel::data(const QModelIndex &index, int role) const{
 if (!index.isValid())
     return QVariant();

 if (index.row() >= words.size())
     return QVariant();

 if (role == Qt::DisplayRole){
     QVariant temp;
     temp.setValue(words.at(index.row()));
     return temp;
 } else
     return QVariant();
}
QVariant CompletionListModel::headerData(int section, Qt::Orientation orientation,
                     int role) const{
    return QVariant();
}
 void CompletionListModel::setWords(const QList<CompletionWord> &baselist, const QString &word){
     if (word==curWord) return;
     words.clear();
     for (int i=0;i<baselist.count();i++){
        if (baselist[i].word.startsWith(word,Qt::CaseInsensitive)) 
            words.append(baselist[i]);
     }
     if (words.size()>2) //prefer matching case
        if (!words[0].word.startsWith(word,Qt::CaseSensitive) && words[1].word.startsWith(word,Qt::CaseSensitive)) 
            words.swap(0,1);
     curWord=word;
     reset();
 }

//------------------------------completer-----------------------------------
QList <CompletionWord> LatexCompleter::words;
QSet <QChar> LatexCompleter::acceptedChars;
int LatexCompleter::maxWordLen = 0;

LatexCompleter::LatexCompleter(QObject *p)
 : QCodeCompletionEngine(p)
{
 //   addTrigger("\\");
    list=0;
}
LatexCompleter::~LatexCompleter(){
//    delete list;
}
void LatexCompleter::setWords(const QStringList &newwords){
    
    acceptedChars.clear();
    words.clear();
    foreach (QString str, newwords){
        words.append(CompletionWord(str));
        foreach (QChar c, str) acceptedChars.insert(c);
    }
    qSort(words);
    
    if (maxWordLen==0) {
        int newWordMax=0;
        QFont f=QApplication::font();
        f.setItalic(true);
        QFontMetrics fm(f);
        for (int i=0;i<words.size();i++) {
            int temp=fm.width(words[i].shownWord)+words[i].descriptiveParts.size()+10;
            if (temp>newWordMax) newWordMax=temp;
        }
        maxWordLen=newWordMax;
    }
}

void LatexCompleter::complete(const QDocumentCursor& c, const QString& trigger){
    if (!editor()) return;
    QDocumentLine line=c.line();
    QPoint offset=line.cursorToDocumentOffset(c.columnNumber()-1);
    offset.setY(offset.y()+line.document()->y(line)+line.document()->fontMetrics().lineSpacing());
    offset=editor()->mapFromContents(offset);
    int left;int temp;
    editor()->getPanelMargins(&left,&temp,&temp,&temp);
    offset.setX(offset.x()+left);
    if (!list) {
        list=new QListView(editor());;
        list->resize(200>maxWordLen?200:maxWordLen,100);
        listModel=new CompletionListModel(list);
        list->setModel(listModel);
        list->setFocusPolicy(Qt::NoFocus);
        list->setItemDelegate(new CompletionItemDelegate(list));
    }
    if (offset.y()+list->height()>editor()->height()) {
        offset.setY(offset.y()-line.document()->fontMetrics().lineSpacing()-list->height());
    }
    list->move(offset);
    //list->show();
    
    if (c.getPreviousChar()!='\\') {
        int start=c.columnNumber()-1;
        QString eow="~!@#$%^&*()_+{}|:\"<>?,./;[]-= \n\r`+�";
        QString lineText=c.line().text();
        for (int i=c.columnNumber()-1;i>=0;i--){
            if (lineText.at(i)==QChar('\\')) {
                start=i;
                break;
            } else if (eow.contains(lineText.at(i))) 
                break;
        }
        completerInputBinding->bindTo(editor(),this,true,start);
    } else completerInputBinding->bindTo(editor(),this,false,c.columnNumber()-1);

    //line.document()->cursor(0,0).insertText(QString::number(offset.x())+":"+QString::number(offset.y()));
    connect(editor(),SIGNAL(cursorPositionChanged()),this,SLOT(cursorPositionChanged()));
}

QCodeCompletionEngine* LatexCompleter::clone(){
    LatexCompleter* temp =new LatexCompleter(this);
    temp->words=words;
    return temp;
}
QString LatexCompleter::language() const{
    return "Latex";
}
QStringList LatexCompleter::extensions() const{
    return QStringList() << ".tex";
}

void LatexCompleter::updateList(QString word){
    QString cur=""; //needed to preserve selection
    if (list->isVisible() && list->currentIndex().isValid())
        cur=list->model()->data(list->currentIndex(),Qt::DisplayRole).toString();
    ((CompletionListModel*)(list->model()))->setWords(words,word);
    if (cur!="") {
        int p=((CompletionListModel*)(list->model()))->words.indexOf(cur); 
        if (p>=0) list->setCurrentIndex(list->model()->index(p,0,QModelIndex()));
    }
}
bool LatexCompleter::acceptChar(QChar c,int pos){
    if (((c>=QChar('a')) && (c<=QChar('z'))) ||
        ((c>=QChar('A')) && (c<=QChar('Z'))) ||
        ((c>=QChar('0')) && (c<=QChar('9')))) return true;
    if (pos<=1) return false;
    return acceptedChars.contains(c);
}
CompletionWord LatexCompleter::wordToCompletionWord(const QString &str){
    for (int i=0; i<words.count();i++)
        if (words[i].word==str) 
          return words[i];
    //check again in lower case
    QString lcomp=str.toLower();
    for (int i=0; i<words.count();i++)
        if (words[i].lword==lcomp) 
          return words[i];
   return CompletionWord();
}
void LatexCompleter::cursorPositionChanged(){
    if (!completerInputBinding || !completerInputBinding->isActive()) {
        disconnect(editor(),SIGNAL(cursorPositionChanged()),this,SLOT(cursorPositionChanged()));
        return;
    }
    completerInputBinding->cursorPositionChanged(editor());
}
