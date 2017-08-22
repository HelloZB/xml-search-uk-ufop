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
#ifndef FILTERWIDGETFORM_H
#define FILTERWIDGETFORM_H

#include <QWidget>

namespace Ui {
class FilterWidgetForm;
}

class FilterWidgetForm : public QWidget
{
    Q_OBJECT

public:
    explicit FilterWidgetForm(QList<int> listFilters, QList<int> listDefFilters, QStringList listLabels, QString objName, QWidget *parent = 0);
    ~FilterWidgetForm();

signals:
    void setNewFilterSett(QString objName, QStringList listIndx);

private slots:

    void on_pushButton_clicked();

public slots:


private:
    Ui::FilterWidgetForm *ui;

    QList<int> listDefFilters;
    QString objName;
//    QStringList listOldFilter;
};

#endif // FILTERWIDGETFORM_H
