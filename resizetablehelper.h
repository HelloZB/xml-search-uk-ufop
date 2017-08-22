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
#ifndef RESIZETABLEHELPER_H
#define RESIZETABLEHELPER_H

#include <QObject>
#include <QTableView>
#include <QtCore>

class ResizeTableHelper : public QObject
{
    Q_OBJECT
public:
    explicit ResizeTableHelper(QObject *parent = 0);
    QWidget *parentWidget;

    static QStringList intList2strList(const QList<int> &l  );

signals:
    void startTmrResizeTv();

    void killOldWdgt();

    void addWdgt2stackWdgt(QWidget *w, QString wdgtTitle);

public slots:
    void addTable2resizeLater(QTableView *tv);

    void addTable2resizeLaterHeight(QTableView *tv);


    void showFilterWidget(QList<int> listFilters, QList<int> listDefFilters, QStringList listLabels, QObject *obj);


private slots:
    void onTmrResizeLater();


private:
    QList<QTableView*> listTv;
    QList<QTableView*> listHeightTv;


};

#endif // RESIZETABLEHELPER_H
