#ifndef TABLEMANIPULATION_H
#define TABLEMANIPULATION_H

#include "qdocument.h"

class LatexTables{
public:
    static void addRow(QDocumentCursor &c,const int numberOfColumns );
    static void addColumn(QDocument *doc,const int lineNumber,const int afterColumn,QStringList *cutBuffer=0);
    static void removeColumn(QDocument *doc,const int lineNumber,const int column,QStringList *cutBuffer=0);
    static void removeRow(QDocument *doc,const int afterLine );
    static int findNextToken(QDocumentCursor &cur,QStringList tokens,bool keepAnchor=false,bool backwards=false);
    static int getColumn(QDocumentCursor &cur);
    static int getNumberOfColumns(QDocumentCursor &cur);

    static QStringList tabularNames;
    static QStringList tabularNamesWithOneOption;
};
#endif // TABLEMANIPULATION_H
