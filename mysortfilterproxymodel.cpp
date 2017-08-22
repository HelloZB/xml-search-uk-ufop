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
#include "mysortfilterproxymodel.h"
#include <QDebug>

MySortFilterProxyModel::MySortFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
    filterMode.append(0);
    colMax = filterMode.size();
    sortMode = 0;//RegExp
    dateColumn = -1;

//    connect(this, SIGNAL(columnsAboutToBeInserted(QModelIndex,int,int))
}
//========================================================================================================================================

void MySortFilterProxyModel::setFilterMinimumDate(const QDate &date)
{
    minDate = date;
    invalidateFilter();
}

//========================================================================================================================================

void MySortFilterProxyModel::setFilterMaximumDate(const QDate &date)
{
    maxDate = date;
    invalidateFilter();
}

//========================================================================================================================================

bool MySortFilterProxyModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{

    bool retVal = false;
    for(int col = 0; col < colMax && !retVal; col++){
        QModelIndex index = sourceModel()->index(sourceRow, filterMode.at(col), sourceParent);
        retVal = retVal || sourceModel()->data(index).toString().contains(filterRegExp());
        if(retVal && hashSpecFilter.contains(col))
            retVal = (sourceModel()->data(index).toString() == hashSpecFilter.value(col));
    }

//     qDebug() << "filterAcceptsRow " << colMax << sourceModel()->columnCount();


    if(retVal && !hashSpecFilter.isEmpty()){

        for(int col = colMax , cMax = sourceModel()->columnCount(); col < cMax && retVal; col++){
            if(retVal && hashSpecFilter.contains(col)){
                QModelIndex index = sourceModel()->index(sourceRow, col, sourceParent);

                retVal = (sourceModel()->data(index).toString() == hashSpecFilter.value(col));
                if(!retVal)
                    break;

            }
        }
    }
    return retVal;
//    QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
//    QModelIndex index1 = sourceModel()->index(sourceRow, 1, sourceParent);
//    QModelIndex index2 = sourceModel()->index(sourceRow, 1, sourceParent);

//     QDate invalid = QDate::fromString( sourceModel()->data(index2).toString().left(10), "yyyy.MM.dd");

//      qDebug() << "filterAcceptsRow " << invalid <<  sourceModel()->data(index2).toString();

//    return (sourceModel()->data(index0).toString().contains(filterRegExp())
//            || sourceModel()->data(index1).toString().contains(filterRegExp()))
//           && dateInRange(invalid)
//            );
}

//========================================================================================================================================

bool MySortFilterProxyModel::lessThan(const QModelIndex &left,
                                      const QModelIndex &right) const
{
    if(dateColumn >= 0 && left.column() == dateColumn && right.column() == dateColumn && left.data(SortDateTimeRole).toDateTime().isValid() && right.data(SortDateTimeRole).toDateTime().isValid()){
        return (left.data(SortDateTimeRole).toDateTime() < right.data(SortDateTimeRole).toDateTime());
    }

    QString leftString = sourceModel()->data(left).toString();
    QString rightString = sourceModel()->data(right).toString();

    QDateTime invalid = QDateTime::fromString( leftString, "yyyy-MM-dd hh:mm");
//    qDebug() << "lessThan date is " << invalid;
    if (invalid.isValid()){ //leftData.type() == QVariant::DateTime) {
//        qDebug() << "date is valid " << left << right;
        return (QDateTime::fromString( leftString, "yyyy-MM-dd hh:mm") < QDateTime::fromString( rightString, "yyyy-MM-dd hh:mm"));
    } else {

        bool okL, okR;
        qreal l = leftString.toDouble(&okL);
        qreal r = rightString.toDouble(&okR);
        if(okL && okR)
            return (l < r);


        return (QString::localeAwareCompare(leftString, rightString) < 0);
    }
}
//========================================================================================================================================

bool MySortFilterProxyModel::dateInRange(const QDate &date) const
{
    return (!minDate.isValid() || date >= minDate) && (!maxDate.isValid() || date <= maxDate);
}

//========================================================================================================================================
void MySortFilterProxyModel::setNewFileterStr(QString str)
{
    lastStr = str;

    if(sortMode == 1){
        if(!str.isEmpty())
            str = "^(" + str + ")$";
        QRegExp regExp(str, Qt::CaseInsensitive, QRegExp::RegExp);
        this->setFilterRegExp(regExp);
//        this->setFilterFixedString(str);
    }else{
        QRegExp regExp(str, Qt::CaseInsensitive, QRegExp::RegExp);
        this->setFilterRegExp(regExp);
    }

}
//========================================================================================================================================
void MySortFilterProxyModel::setSortingMode(int mode)
{
    sortMode = mode;
//    if(!lastStr.isEmpty() )
    setNewFileterStr(lastStr);
}
//========================================================================================================================================
void MySortFilterProxyModel::setSpecFilter(int col, QString str)
{
    if(str.isEmpty() && hashSpecFilter.contains(col))
        hashSpecFilter.remove(col);
    else
        hashSpecFilter.insert(col, str);
    setNewFileterStr(lastStr);
}
//---------------------------------------------------------
void MySortFilterProxyModel::resetSorting()
{
    sort(-1, Qt::AscendingOrder);
}
//========================================================================================================================================

void MySortFilterProxyModel::setFilterMode(QList<int> filterMode)
{
    this->filterMode = filterMode;
    colMax = filterMode.size();
}
//========================================================================================================================================
void MySortFilterProxyModel::setDateColumn(const int &col)
{
    dateColumn = col;
}
//------------------------------------------------------------------
void MySortFilterProxyModel::enableModelChangedSignal()
{
    disconnect(this, SIGNAL(columnsInserted(QModelIndex,int,int))              , this, SIGNAL(onModelChanged()) );
    disconnect(this, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)) , this, SIGNAL(onModelChanged()) );
    disconnect(this, SIGNAL(columnsRemoved(QModelIndex,int,int))               , this, SIGNAL(onModelChanged()) );
    disconnect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)) , this, SIGNAL(onModelChanged()) );
    disconnect(this, SIGNAL(rowsInserted(QModelIndex,int,int))                 , this, SIGNAL(onModelChanged()) );
    disconnect(this, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int))    , this, SIGNAL(onModelChanged()) );
    disconnect(this, SIGNAL(rowsRemoved(QModelIndex,int,int))                  , this, SIGNAL(onModelChanged()) );

    connect(this, SIGNAL(columnsInserted(QModelIndex,int,int))              , this, SIGNAL(onModelChanged()) );
    connect(this, SIGNAL(columnsMoved(QModelIndex,int,int,QModelIndex,int)) , this, SIGNAL(onModelChanged()) );
    connect(this, SIGNAL(columnsRemoved(QModelIndex,int,int))               , this, SIGNAL(onModelChanged()) );
    connect(this, SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)) , this, SIGNAL(onModelChanged()) );
    connect(this, SIGNAL(rowsInserted(QModelIndex,int,int))                 , this, SIGNAL(onModelChanged()) );
    connect(this, SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int))    , this, SIGNAL(onModelChanged()) );
    connect(this, SIGNAL(rowsRemoved(QModelIndex,int,int))                  , this, SIGNAL(onModelChanged()) );

}
//========================================================================================================================================
