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
#ifndef SETTLOADER_H
#define SETTLOADER_H

#include <QSettings>
#include <QObject>



class SettLoader : public QObject
{
    Q_OBJECT
public:
    explicit SettLoader(QObject *parent = 0);



     //helper
     static QStringList intList2strList(const QList<int> &l  );
     static QList<int> strList2intList(const QStringList &l);


     static QStringList getFilterListStr(const int &startIndx, const int &count);

     static QList<int> getFilterList(const int &startIndx, const int &count);





signals:

public slots:

};

#endif // SETTLOADER_H
