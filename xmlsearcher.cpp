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
#include "xmlsearcher.h"

#include <QtCore>
#include <QXmlStreamReader>
#include <QtXml>

//-------------------------------------------------------------
XmlSearcher::XmlSearcher(QObject *parent) : QObject(parent)
{

}
//-------------------------------------------------------------
void XmlSearcher::startSearch(QString fileName, QStringList who, QStringList tree, bool useOrMode, bool enCustomBuffer)
{
    QTime time;
    time.start();
    stopAll = false;
    emit searchProgress(0);
    qint64 rez = startSearchF(fileName, who, tree, useOrMode, enCustomBuffer);

    QString f;
    QString s = prettyTimeValue(f, time.elapsed());

    emit onSearchDone(s + " (" + f + ")", rez);

    if(stopAll)
        emit showMess(tr("Зупинено<br>%1").arg(fileName));

}
//-------------------------------------------------------------
void XmlSearcher::stopAllNow()
{
    stopAll = true;
}
//-------------------------------------------------------------
qint64 XmlSearcher::startSearchF(QString fileName, QStringList who, QStringList tree, bool useOrMode, bool enCustomBuffer)
{
    qint64 fileSize = 0;
    qint64 sizeDone = 0;

    bool searchModeAnd = !useOrMode;


    if(tree.isEmpty()){
        emit showMess(tr("XML схема документу не задана(<br>Перейдіть на сторінку налаштувань"));
        return -1;
    }

    QFile file(fileName);
    if(true){
        QFileInfo fi(fileName);
        fileSize = fi.size();
    }

//    QVariantMap map;
//    for(int i = 0, iMax = tree.size(), t = 0; i < iMax; i++){
//        QString s = tree.at(i);
//        int t2 = s.split("\t").size();
//        s.simplified().trimmed();
//    }

    QHash<QString,QString> hTname2searchStr;
    for(int i = 0, iMax = who.size(); i < iMax; i++){
        QStringList ll = who.at(i).split("\t", QString::SkipEmptyParts);
        hTname2searchStr.insert(ll.first().simplified().trimmed(), ll.last());
    }

    QString firstToken = tree.first().split("\t", QString::SkipEmptyParts).last();
 /*
RECORD
    NAME
    SHORT_NAME
    EDRPOU
    ADDRESS
    BOSS
    KVED
    STAN
    FOUNDERS
        FOUNDER
*/

//    QHash<QString,bool> hToken2tblVisible;
    QStringList lHeader;
    if(true){
        //create header
        for(int i = 0, iMax = tree.size(), t = 0; i < iMax; i++){
            QString s = tree.at(i);
            int t2 = s.split("\t").size();
            int t0 = t;
            t = t2;
            if(t2 > t0){
                //remove previous item
                if(!lHeader.isEmpty()){
//                    hToken2tblVisible.insert(lHeader.last(), false);
                    lHeader.removeLast();
                }
            }else{
                if(t2 < t0)
                    t = t2;
            }
            s = s.split("\t", QString::SkipEmptyParts).last();
            lHeader.append(s);
//            hToken2tblVisible.insert(s, true);
        }
        emit setHeader(lHeader);
    }
    if(lHeader.isEmpty()){
        emit showMess(tr("XML схема документу не задана(<br>Перейдіть на сторінку налаштувань"));
        return -1;
    }

    int lHeaderSize = lHeader.size();

    qint64 rowInTable = 0;
    qint64 maxRowInTable = 100000;



    if(file.open(QFile::ReadOnly)){

//        if(true){
//            QSaveFile sFile("onepart.xml");
//            if(sFile.open(QSaveFile::WriteOnly)){

//                sFile.write(file.readLine(5555));
//                sFile.write(file.readLine(5555));

//                sFile.commit();
//                qDebug() << "sFle" << sFile.fileName() << sFile.errorString();
//            }
//            return 0;
//        }

        QBuffer buffer;
        if(enCustomBuffer){
            buffer.open(QBuffer::WriteOnly);
            fileSize = buffer.write(file.readAll());
            buffer.close();
        }

        QXmlStreamReader xml(&file);
        if(enCustomBuffer){
            buffer.open(QBuffer::ReadOnly);
            xml.setDevice(&buffer);
        }

        bool checkCalibrationTime = true;
        QTime time;
        time.start();
        int counterMax = 33333;

        qDebug() << "repair " <<  xml.hasError() << xml.lineNumber() << xml.documentEncoding() << xml.documentVersion() << xml.device()->bytesAvailable();

        QVariantList lrows;

        int counter = 0;
        qint64 lastBytesAv = xml.device()->bytesAvailable();
        while(!xml.atEnd() && !xml.hasError()){

            if(stopAll)
                break;
            counter++;
            QXmlStreamReader::TokenType token = xml.readNext();

            if(xml.name() == firstToken && token == QXmlStreamReader::StartElement){

                QHash<QString,QString> hTokenName2colTxt;


                QString rootTxt;
                QString xmlName = xml.name().toString();
                //                    qDebug() << "xml "  << xmlName << fileSize << sizeDone << lastBytesAv << ", xml.devAv=" << xml.device()->bytesAvailable();

                addNextItm2hash(xml, hTokenName2colTxt, rootTxt, xmlName, firstToken);

                QStringList lrow;
                int rezFound = 0, rezNfound = 0;
                for(int i = 0; i < lHeaderSize; i++){
                    lrow.append( hTokenName2colTxt.value(lHeader.at(i)));

                    if(hTname2searchStr.contains(lHeader.at(i))){
                        if(lrow.last().contains(hTname2searchStr.value(lHeader.at(i)), Qt::CaseInsensitive))
                            rezFound++;
                        else
                            rezNfound++;
                    }
                }
                if(hTname2searchStr.contains(firstToken)){
                    if(rootTxt.contains(hTname2searchStr.value(firstToken), Qt::CaseInsensitive))
                        rezFound++;
                    else
                        rezNfound++;
                }

                if((searchModeAnd && (rezFound > 0 && rezNfound < 1)) || (!searchModeAnd && rezFound > 0)){
//                    emit addRow2table(lrow);
                    lrows.append(lrow);

                    rowInTable++;
                    if(rowInTable > maxRowInTable){
                        emit showMess(tr("Обмеження по рядкам<br>Макс. к-ть рядків: %1<br>К-ть рядків: %2<br>Перериваю пошук").arg(maxRowInTable).arg(rowInTable));
                        if(!lrows.isEmpty())
                            emit addRows2table(lrows, lrows.size(), lHeaderSize);
                        return rowInTable;
                    }
                }


                if(counter > counterMax){
                    checkCalibrationTime = false;
                    counter = 0;
                    qint64 bytes = xml.device()->bytesAvailable();
                    if(lastBytesAv != bytes){
                        sizeDone += (lastBytesAv - bytes);
                        lastBytesAv = bytes;
                        emit searchProgress( (sizeDone * 100 ) / fileSize );

                        if(!lrows.isEmpty())
                            emit addRows2table(lrows, lrows.size(), lrow.size());
                        lrows.clear();
//                        qDebug() << "xml counter " << sizeDone << fileSize << bytes;

                    }
                }else{
                    if(checkCalibrationTime && time.elapsed() > 9000){
                        checkCalibrationTime = false;
                        counterMax = counter;
                    }
                }




            }else{
                if(token == QXmlStreamReader::EndElement){
                    qDebug() << "xml endElement " << xml.name() << counter;
                    emit searchProgress( 100 );
                    if(!lrows.isEmpty())
                        emit addRows2table(lrows, lrows.size(), lHeaderSize);
                    return rowInTable;
                }
            }



        }

        if(!lrows.isEmpty())
            emit addRows2table(lrows, lrows.size(), lHeaderSize);

        if(xml.hasError()){

            qDebug() << "xml err " << xml.errorString() << xml.tokenString() << xml.text() << xml.atEnd() << sizeDone << fileSize << xml.device()->bytesAvailable();

            emit showMess(tr("Помилка у файлі %1<br>%2<br>Прочитано/Всього байт: %3/%4").arg(file.fileName()).arg(xml.errorString()).arg(sizeDone).arg(fileSize));
            return rowInTable;

            //            xml.readNext();

        }else{
            if(stopAll){
                qint64 bytes = xml.device()->bytesAvailable();
                sizeDone += (lastBytesAv - bytes);
                emit searchProgress( (sizeDone * 100 ) / fileSize );
            }else
                emit searchProgress( 100 );
        }
        file.close();


    }else{
        emit showMess(tr("Can't open file(<br>Error: %1").arg(file.errorString()));
    }
    return rowInTable;
}
//-------------------------------------------------------------
void XmlSearcher::addItm2hash(QXmlStreamReader &xml, QHash<QString, QString> &hTokenName2col, QString &rootTxt, const QString &firstToken)
{
    QXmlStreamReader::TokenType token = xml.readNext();
    qDebug() << "addItm2hash " << xml.name() << (int)token;
    QString lastName = xml.name().toString();
    if(lastName == firstToken)
        return;

    if(token == QXmlStreamReader::StartElement){
        token = xml.readNext();

        qDebug() << "addItm2hash " << xml.name() << (int)token << xml.text().toString() << xml.text();

        if(token == QXmlStreamReader::Characters){//text
            rootTxt.append(xml.text().toString());
            if(hTokenName2col.contains(lastName))
                hTokenName2col.insert(lastName, hTokenName2col.value(lastName) + "\n" + xml.text().toString());
            else
                hTokenName2col.insert(lastName, xml.text().toString());
        }
    }else{
        if(token == QXmlStreamReader::EndElement && lastName == firstToken)
            return;

    }
    //read next
    addItm2hash(xml, hTokenName2col, rootTxt, firstToken);


//    QXmlStreamReader::TokenType token2 = xml.readNext();
//    if (token2 == QXmlStreamReader::StartDocument)
//        continue;

//    if (token2 == QXmlStreamReader::StartElement){


//        if (xml.name() == "etaps")
//            continue;

//        if (xml.name() == "etap")
//            XMLConf.append(parseEtap(xml));
    //    }
}
//-------------------------------------------------------------
bool XmlSearcher::addNextItm2hash(QXmlStreamReader &xml, QHash<QString, QString> &hTokenName2col, QString &rootTxt, const QString &lastName, const QString &firstToken)
{
    while(!xml.atEnd() ){
        QXmlStreamReader::TokenType token = xml.readNext();
        QString xmlName = xml.name().toString();

        if(xml.hasError()){
//            qDebug() << "hasErr " << xml.name() << token << xml.errorString() << lastName << rootTxt;
//            qDebug() << "byte " << xml.device()->readLine(200);
//            qDebug() << "founders=" << hTokenName2col.value("NAME") <<  hTokenName2col.value(lastName).split("\n").size();
            return true;
        }

        if(token == QXmlStreamReader::Characters){//text
//            qDebug() << "xmlTxt " << lastName << xml.text().toString().left(50);

            rootTxt.append(xml.text().toString() + "\n");
            if(hTokenName2col.contains(lastName)){
                hTokenName2col.insert(lastName, hTokenName2col.value(lastName) + "\n" + xml.text().toString());
//                qDebug() << "founders=" << hTokenName2col.value("NAME") <<  hTokenName2col.value(lastName).split("\n").size();
            }else
                hTokenName2col.insert(lastName, xml.text().toString());
            return false;
        }

//        qDebug() << "xmlName " << xmlName << (int)token;

        if(token == QXmlStreamReader::StartElement){
            if(addNextItm2hash(xml, hTokenName2col, rootTxt, xmlName, firstToken))
                return true;
        }else{
            if(token == QXmlStreamReader::EndElement && xmlName == firstToken)
                return true;
        }
    }
    return true;
}
//-------------------------------------------------------------
QString XmlSearcher::prettyTimeValue(QString &f, qint64 mynulo)
{
    f = "hh:mm:ss";
    QString s = "";
    if(mynulo >= 86400000){ //1 day
        f.prepend("d ");
        s = QString::number(mynulo/86400000) + " ";
        mynulo %= 86400000;
    }

    s.append(QString::number(mynulo/3600000).rightJustified(2,'0') + ":");//hh
    if(mynulo >= 3600000)
        mynulo %= 3600000;


    s.append(QString::number(mynulo/60000).rightJustified(2,'0') + ":");//mm
    if(mynulo >= 60000)
        mynulo %= 60000;

    s.append(QString::number(mynulo/1000).rightJustified(2,'0'));//ss
    return s;
}
//-------------------------------------------------------------
