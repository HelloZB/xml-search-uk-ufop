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
#ifndef XMLSEARCHER_H
#define XMLSEARCHER_H

#include <QObject>
#include <QXmlStreamReader>
#include <QtCore>

class XmlSearcher : public QObject
{
    Q_OBJECT
public:
    explicit XmlSearcher(QObject *parent = 0);

signals:
    void searchProgress(int val);

    void onSearchDone(QString timeEl, qint64 rezCount);

    void showMess(QString mess);

    void add2model(QString fio, QString addr, QString kved, QString stan);

    void add2pte(QString line);

    void setHeader(QStringList lHeader);

    void addRow2table(QStringList lrow);
    void addRows2table(QVariantList lrows, int lrowsSize, int strListSize);



public slots:
    void startSearch(QString fileName, QStringList who, QStringList tree, bool useOrMode, bool enCustomBuffer);

    void stopAllNow();

private:
    qint64 startSearchF(QString fileName, QStringList who, QStringList tree, bool useOrMode, bool enCustomBuffer);

    void addItm2hash(QXmlStreamReader &xml, QHash<QString,QString> &hTokenName2col, QString &rootTxt, const QString &firstToken);

    bool addNextItm2hash(QXmlStreamReader &xml, QHash<QString,QString> &hTokenName2col, QString &rootTxt, const QString &lastName, const QString &firstToken);

    QString prettyTimeValue(QString &f, qint64 mynulo);

    bool stopAll;

};

#endif // XMLSEARCHER_H
