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
#include "resizetablehelper.h"
#include <QHeaderView>
#include "fltrrezwdgt.h"
#include "filterwidgetform.h"

ResizeTableHelper::ResizeTableHelper(QObject *parent) : QObject(parent)
{
    QTimer *tmr = new QTimer(this);
    tmr->setSingleShot(true);
    tmr->setInterval(55);
    connect(this, SIGNAL(startTmrResizeTv()), tmr, SLOT(start()) );
    connect(tmr, SIGNAL(timeout()), this, SLOT(onTmrResizeLater()) );

}
//----------------------------------------------------------------------
QStringList ResizeTableHelper::intList2strList(const QList<int> &l)
{
    QStringList sl;
    for(int i = 0, iMax = l.size(); i < iMax; i++)
        sl.append(QString::number(l.at(i)));
    return sl;
}
//----------------------------------------------------------------------
void ResizeTableHelper::addTable2resizeLater(QTableView *tv)
{

    emit startTmrResizeTv();

    if(tv && !listTv.contains(tv)){

        if(tv->horizontalHeader()->sectionResizeMode(0) != QHeaderView::ResizeToContents)
            tv->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

        listTv.append(tv);
    }

}
//----------------------------------------------------------------------
void ResizeTableHelper::addTable2resizeLaterHeight(QTableView *tv)
{
    emit startTmrResizeTv();

    if(tv && !listHeightTv.contains(tv)){
        if(tv->verticalHeader()->sectionResizeMode(0) != QHeaderView::ResizeToContents)
            tv->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

        listHeightTv.append(tv);
    }
}
//----------------------------------------------------------------------
void ResizeTableHelper::showFilterWidget(QList<int> listFilters, QList<int> listDefFilters, QStringList listLabels, QObject *obj)
{
    emit killOldWdgt();
    if(obj){
        FltrRezWdgt *ww = qobject_cast<FltrRezWdgt *>(obj);
        if(ww){

            FilterWidgetForm  *w = new FilterWidgetForm(listFilters, listDefFilters, listLabels, ww->objectName(), parentWidget);

            connect(w, SIGNAL(setNewFilterSett(QString,QStringList)), ww, SLOT(setNewShowColumnSett(QString,QStringList)) );
            connect(this, SIGNAL(killOldWdgt()), w, SLOT(deleteLater()) );

            emit addWdgt2stackWdgt(w, tr("Показувати колонки"));

        }
    }
}
//----------------------------------------------------------------------
void ResizeTableHelper::onTmrResizeLater()
{
    QList<QTableView*> listTv = this->listTv;
    this->listTv.clear();

    for(int i = 0, imax = listTv.size(); i < imax; i++){
        QTableView *tv = listTv.at(i);
        try{
            if(tv){
                tv->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

            }
        }catch(...){
            continue;
        }
    }


    listTv = listHeightTv;
    this->listHeightTv.clear();

    for(int i = 0, imax = listTv.size(); i < imax; i++){
        QTableView *tv = listTv.at(i);
        try{
            if(tv){
                tv->verticalHeader()->setSectionResizeMode(QHeaderView::Interactive);

            }
        }catch(...){
            continue;
        }
    }

}
//----------------------------------------------------------------------
