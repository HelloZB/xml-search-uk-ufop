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
#ifndef ZBYRATORFILEHELPER_H
#define ZBYRATORFILEHELPER_H

#include <QObject>
#include <QStandardItemModel>
#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>
#include "mysortfilterproxymodel.h"

class ZbyratorFileHelper : public QObject
{
    Q_OBJECT
public:
    explicit ZbyratorFileHelper(QObject *parent = 0);

    static bool save2csvFile(QString &mess, const QString &fileName, QStandardItemModel *model, MySortFilterProxyModel *proxyModel, QList<int> hiddenCols, QList<int> rowsList, QStringList headerList, QStringList tableShapka);// QTableView *tv, const bool &onlySelected);

    static bool save2pdfFile(QString &mess, const QString &fileName, QStandardItemModel *model, MySortFilterProxyModel *proxyModel, QList<int> hiddenCols, QList<int> rowsList, QStringList headerList, QStringList tableShapka);//QTableView *tv, const bool &onlySelected);

    static bool save2odfFile(QString &mess, const QString &fileName, QStandardItemModel *model, MySortFilterProxyModel *proxyModel, QList<int> hiddenCols, QList<int> rowsList, QStringList headerList, QStringList tableShapka);//QTableView *tv, const bool &onlySelected);

    static bool save2htmlFile(QString &mess, const QString &fileName, QStandardItemModel *model, MySortFilterProxyModel *proxyModel, QList<int> hiddenCols, QList<int> rowsList, QStringList headerList, QStringList tableShapka);//QTableView *tv, const bool &onlySelected);

    static QStringList modelHeaderListKey(QStandardItemModel *model, const QList<int> &hiddenCols);


signals:
    void showMess(QString mess);



//    QWidget *parentWidget;

};

#endif // ZBYRATORFILEHELPER_H
