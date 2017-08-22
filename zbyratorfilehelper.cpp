/****************************************************************************
**
**   Copyright © 2017 Bohdan Zikranets
**   bohdanzikranets@gmail.com
**   https://github.com/HelloZB/xml-search-uk-ufop
**
**  This file is part of xml-search.
**
**  xml-search is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  xml-search is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with xml-search.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
#include "zbyratorfilehelper.h"
#include "QFileDialog"
#include <QSaveFile>
#include <QtPrintSupport>
#include <QTextDocument>
#include <QTextTable>
#include <QTextCursor>


ZbyratorFileHelper::ZbyratorFileHelper(QObject *parent) : QObject(parent)
{

}

//--------------------------------------------------------------

bool ZbyratorFileHelper::save2csvFile(QString &mess, const QString &fileName, QStandardItemModel *model, MySortFilterProxyModel *proxyModel, QList<int> hiddenCols, QList<int> rowsList, QStringList headerList, QStringList tableShapka)
{
    mess.clear();

    if(!proxyModel || !model){
        mess = tr("Abstract model not valid(");
        return false;
    }

    if(headerList.isEmpty()){
        mess = tr("Header list is empty(");
        return false;
    }


    QSaveFile file(fileName);
    if(file.open(QFile::WriteOnly|QFile::Unbuffered)){
        QTextStream stream(&file);
#ifndef SIMPL_EXPORT_2_FILE
        QByteArray tCodec = SettLoader::loadSett(SETT_OPTIONS_TEXTCODEC).toByteArray();
        if(!tCodec.isEmpty())
            stream.setCodec(tCodec.data());
        else
            stream.setCodec("UTF-8");
#endif
        stream << "Text codec:;" << stream.codec()->name() << ";\n";
        if(!tableShapka.isEmpty())
            stream << tableShapka.join("\n").replace(QString(": "), QString(":;")) << "\n";

        stream << QString("#;");
        for(int j = 0, jMax = headerList.size(); j < jMax; j++)
            stream << headerList.at(j) << ";";

        stream << ";\n";

        int valCounter = 0;
        for(int i = 0, iMax = rowsList.size(), colMax = model->columnCount(); i < iMax; i++){
            int row = proxyModel->mapToSource( proxyModel->index(rowsList.at(i) ,0) ).row();

            stream << QString("%1;").arg(i + 1);

            for(int col = 0; col < colMax ; col++){
                if(hiddenCols.contains(col))
                    continue;

                if(model->item(row, col))
                    stream << model->item(row, col)->text().replace(";", ":") << ";";
                else
                    stream <<  ";";

            }
            stream << "\n";
            valCounter++;
        }
        if(file.commit()){
            mess = tr("Counter: %1<br>File:%2<br>").arg(valCounter).arg(fileName);
            return true;
        }

        mess = tr("Can't save file: %1<br>Error: %2").arg(fileName).arg(file.errorString());


    }else{
        mess = tr("Can't open file: %1<br>Error: %2").arg(fileName).arg(file.errorString());
    }
    return false;
}

//--------------------------------------------------------------

bool ZbyratorFileHelper::save2pdfFile(QString &mess, const QString &fileName, QStandardItemModel *model, MySortFilterProxyModel *proxyModel, QList<int> hiddenCols, QList<int> rowsList, QStringList headerList, QStringList tableShapka)//QTableView *tv, const bool &onlySelected)
{
    mess.clear();

    if(!proxyModel || !model){
        mess = tr("Abstract model not valid(");
        return false;
    }

    if(headerList.isEmpty()){
        mess = tr("Header list is empty(");
        return false;
    }


    QString html = "<!DOCTYPE html> <html> <head> <style>" //h3.hidden { visibility: hidden }
                   " table { width:100%; }table, th, td { border: 1px solid black; border-collapse: collapse; }"
            "th, td { padding: 1px; text-align: left; } table#t01 tr:nth-child(even) { background-color: #eee; }"
            "table#t01 tr:nth-child(odd) { background-color:#fff; }table#t01 th	{ background-color: gray; color: black;}"
            "</style>  </head>   <body><div>";

    if(!tableShapka.isEmpty())
        html.append(tableShapka.join("<br>"));



    html.append("</div><br> ");//<table id=\"t01\"><tr>
    html.append("<table style=\" border-width:0px;border-style:solid;border-color:#000000;margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;\" "
                "align=\"center\" width=\"100%\" cellspacing=\"0\" cellpadding=\"0\"> "
"<tr  style=\"background-color: #aaa;\">"
   );


    html.append(tr("<th>#</th>"));

    int colMax = model->columnCount();
    for(int j = 0, jMax = headerList.size(); j < jMax; j++)
         html.append(QString("<th>%1</th>").arg(headerList.at(j)));

    headerList.clear();

    html.append("</tr>");

    int valCounter = 0;
    for(int i = 0, iMax = rowsList.size(); i < iMax; i++){

        int row = proxyModel->mapToSource( proxyModel->index( rowsList.at(i) ,0) ).row();
        html.append(QString("<tr style=\"background-color: #%1;\">").arg( (i % 2 == 0) ? "fff"  : "eee") ); //html.append("<tr>");
        html.append( QString("<td>%1</td>").arg(i + 1));

        for(int col = 0; col < colMax ; col++){
            if(hiddenCols.contains(col))
                continue;
            html.append(QString("<td>%1</td>").arg( model->item(row, col) ? model->item(row, col)->text().toHtmlEscaped() : ""));
        }
        html.append("</tr>");
        valCounter++;
    }
     html.append(QString("</table></body></html>"));
     mess = tr("Value counter: %1<br>File:%2<br>").arg(valCounter).arg(fileName);

     QTextDocument document;
     document.setHtml(html);

     QPrinter printer(QPrinter::PrinterResolution);
     printer.setOutputFormat(QPrinter::PdfFormat);

#ifdef SIMPL_EXPORT_2_FILE
     printer.setOrientation(QPrinter::Landscape);
     printer.setPaperSize(QPrinter::A4);
#else
     int paper = SettLoader::loadSett(SETT_OPTIONS_PDFPAPER).toInt();
     if(paper < 1)
         paper = QPrinter::A4;
     printer.setPaperSize(static_cast<QPrinter::PageSize>(paper));//QPrinter::A4
     printer.setOrientation(SettLoader::loadSett(SETT_OPTIONS_PDFL).toBool() ? QPrinter::Landscape : QPrinter::Portrait);
#endif


     printer.setOutputFileName(fileName);
     printer.setPageMargins(QMarginsF(1, 1, 1, 1));

     document.print(&printer);

     return true;
}

//--------------------------------------------------------------

bool ZbyratorFileHelper::save2odfFile(QString &mess, const QString &fileName, QStandardItemModel *model, MySortFilterProxyModel *proxyModel, QList<int> hiddenCols, QList<int> rowsList, QStringList headerList, QStringList tableShapka)//QTableView *tv, const bool &onlySelected)
{
    mess.clear();

    if(!proxyModel || !model){
        mess = tr("Abstract model not valid(");
        return false;
    }

    if(headerList.isEmpty()){
        mess = tr("Header list is empty(");
        return false;
    }

    QTextDocument document;

    QTextCursor cursor(&document);

    if(!tableShapka.isEmpty())
        cursor.insertText(tableShapka.join("\n"));


    QTextTableFormat tableFormat;
    tableFormat.setCellPadding(5);
    tableFormat.setBackground(QBrush(Qt::white));
    tableFormat.setHeaderRowCount(1);
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));


    headerList.prepend("#");
    cursor.insertTable(2, headerList.size(), tableFormat);
    for(int jj = 0, jjMax = headerList.size(); jj < jjMax; jj++){
        cursor.insertText(headerList.at(jj));
        cursor.movePosition(QTextCursor::NextCell);
    }

    QTextTable *table = cursor.currentTable();

    int valCounter = 0;
    for(int i = 0, iMax = rowsList.size(), colMax = model->columnCount(); i < iMax; i++){
        int row = proxyModel->mapToSource( proxyModel->index(rowsList.at(i) ,0) ).row();
        table->appendRows(1);

        cursor.insertText( QString("%1").arg(i + 1));

        for(int col = 0; col < colMax ; col++){
            if(hiddenCols.contains(col))
                continue;
            cursor.movePosition(QTextCursor::NextCell);
            cursor.insertText(QString("%1").arg(model->item(row, col) ? model->item(row, col)->text() : ""));
        }
        cursor.movePosition(QTextCursor::NextCell);
        valCounter++;
    }
    table->removeRows(valCounter + 1, 1);

    mess = tr("Meter counter: %1<br>File:%2<br>").arg(valCounter).arg(fileName);

    QTextDocumentWriter odfWriter(fileName);
    odfWriter.setFormat("odf");
    odfWriter.setCodec(QTextCodec::codecForName("UTF-8"));
//        odfWriter.setCodec("UTF-8");
    return odfWriter.write(&document);
}

//--------------------------------------------------------------

bool ZbyratorFileHelper::save2htmlFile(QString &mess, const QString &fileName, QStandardItemModel *model, MySortFilterProxyModel *proxyModel, QList<int> hiddenCols, QList<int> rowsList, QStringList headerList, QStringList tableShapka)//QTableView *tv, const bool &onlySelected)
{
    mess.clear();

    if(!proxyModel || !model){
        mess = tr("Abstract model not valid(");
        return false;
    }

    QStringList headerListKey;

    if(headerList.isEmpty()){
        mess = tr("Header list is empty(");
        return false;
    }


    QSaveFile file(fileName);
    if(file.open(QFile::WriteOnly|QFile::Unbuffered)){
        QTextStream stream(&file);
#ifndef SIMPL_EXPORT_2_FILE
        QByteArray tCodec = SettLoader::loadSett(SETT_OPTIONS_TEXTCODEC).toByteArray();
        if(!tCodec.isEmpty())
            stream.setCodec(tCodec.data());
        else
            stream.setCodec("UTF-8");
#else
        stream.setCodec("UTF-8");
#endif
        QString html = "<!DOCTYPE html> <html> <head> <style>h3.hidden { visibility: hidden }"
                       " table { width:100%; }table, th, td { border: 1px solid black; border-collapse: collapse; }"
                       "th, td { padding: 5px; text-align: left; } table#t01 tr:nth-child(even) { background-color: #fff; }"
                       "table#t01 tr:nth-child(odd) { background-color:#eee; }table#t01 th	{ background-color: gray; color: black;}"
                       "</style>  </head>   <body><div>";

        stream << html;

        if(!tableShapka.isEmpty()){
            stream << tableShapka.join("<br>") ;
            headerListKey = modelHeaderListKey(model, hiddenCols);
        }

         stream << "</div><br> <table id=\"t01\"><tr>";
        stream << tr("<th>#</th>");
        for(int j = 0, jMax = headerList.size(); j < jMax; j++)
            stream << QString("<th>%1</th>").arg(headerList.at(j));


        stream << "</tr>";

        int valCounter = 0;
        for(int i = 0, iMax = rowsList.size(), colMax = model->columnCount(); i < iMax; i++){
            int row = proxyModel->mapToSource( proxyModel->index(rowsList.at(i) ,0) ).row();
            stream << "<tr>";

            stream << QString("<td>%1</td>").arg(i + 1);

            for(int col = 0; col < colMax ; col++){
                if(hiddenCols.contains(col))
                    continue;
                stream << QString("<td>%1</td>").arg(model->item(row, col) ? model->item(row, col)->text().toHtmlEscaped() : "");
            }
            stream << "</tr>";
            valCounter++;
        }
        if(tableShapka.isEmpty() && !headerListKey.isEmpty())
            stream << QString("</table> </body></html>");
        else
            stream << QString("</table> <h3 class=\"hidden\">%1</h3></body></html>")
                      .arg(QString(headerListKey.join(";")).toHtmlEscaped());


            if(file.commit()){
                mess = tr("Meter counter: %1<br>File:%2<br>").arg(valCounter).arg(fileName);
                return true;
            }
            mess = tr("Can't save file: %1<br>Error: %2").arg(fileName).arg(file.errorString());



    }else{
        mess = tr("Can't open file: %1<br>Error: %2").arg(fileName).arg(file.errorString());
    }

    return false;
}

//--------------------------------------------------------------

QStringList ZbyratorFileHelper::modelHeaderListKey(QStandardItemModel *model, const QList<int> &hiddenCols)
{
    QStringList header;
    if(model){
        for(int i = 0, iMax = model->columnCount(); i < iMax; i++){
            if(hiddenCols.contains(i))
                continue;
            header.append( model->horizontalHeaderItem(i) ? model->horizontalHeaderItem(i)->data(Qt::UserRole + 2).toString() : tr("col_%1").arg(i) );
        }
    }
    return header;
}

//--------------------------------------------------------------
