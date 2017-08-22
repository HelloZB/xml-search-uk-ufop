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
#ifndef MYSORTFILTERPROXYMODEL_H
#define MYSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QDate>
#include <QtCore>
#include <QTime>

#define SortDateTimeRole    Qt::UserRole + 50


class MySortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit MySortFilterProxyModel(QObject *parent = 0);

    QDate filterMinimumDate() const { return minDate; }
    void setFilterMinimumDate(const QDate &date);

    QDate filterMaximumDate() const { return maxDate; }
    void setFilterMaximumDate(const QDate &date);

    void setFilterMode(QList<int> filterMode);

    void setDateColumn(const int &col);

    QList<int> getFilterMode() const { return filterMode; }

    void enableModelChangedSignal();

signals:
    void onModelChanged();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public slots:
    void setNewFileterStr(QString str);
    void setSortingMode(int mode);

    void setSpecFilter(int col, QString str);

    void resetSorting();

private:
    bool dateInRange(const QDate &date) const;
    QList<int> filterMode;
    int colMax;
    QDate minDate;
    QDate maxDate;

    int sortMode;
    int dateColumn;
    QString lastStr;

    QHash<int, QString> hashSpecFilter;
};

#endif // MYSORTFILTERPROXYMODEL_H
