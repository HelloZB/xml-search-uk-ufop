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
#include "settloader.h"

#include <QStandardPaths>
#include <QtCore>

//------------------------------------------------------------------------------------------------------------------------------------------------
SettLoader::SettLoader(QObject *parent) : QObject(parent)
{

}

//------------------------------------------------------------------------------------------------------------------------------------------------
QStringList SettLoader::intList2strList(const QList<int> &l)
{
    QStringList sl;
    for(int i = 0, iMax = l.size(); i < iMax; i++)
        sl.append(QString::number(l.at(i)));
    return sl;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
QList<int> SettLoader::strList2intList(const QStringList &l)
{
    QList<int> li;
    for(int i = 0, iMax = l.size(); i < iMax; i++){
        bool ok;
        li.append(l.at(i).toInt(&ok));
        if(!ok)
            li.removeLast();
    }
    return li;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
QStringList SettLoader::getFilterListStr(const int &startIndx, const int &count)
{
    QStringList l;
    for(int i = startIndx, iMax = startIndx + count; i < iMax; i++)
        l.append(QString::number(i));
    return l;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
QList<int> SettLoader::getFilterList(const int &startIndx, const int &count)
{
    QList<int> l;
    for(int i = startIndx, iMax = startIndx + count; i < iMax; i++)
        l.append(i);
    return l;
}
//------------------------------------------------------------------------------------------------------------------------------------------------
