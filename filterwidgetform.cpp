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
#include "filterwidgetform.h"
#include "ui_filterwidgetform.h"
#include <QShortcut>
#include <QKeySequence>
#include <QListWidgetItem>

#include "settloader.h"

FilterWidgetForm::FilterWidgetForm( QList<int> listFilters, QList<int> listDefFilters, QStringList listLabels, QString objName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterWidgetForm)
{
    ui->setupUi(this);

    connect(ui->pushButton_2, SIGNAL(clicked(bool)), this, SLOT(deleteLater()) );

    QShortcut *shortCut = new QShortcut(QKeySequence::Cancel, this);

    connect(shortCut, SIGNAL(activated()), this, SLOT(deleteLater()) );

    this->listDefFilters = listDefFilters;
    this->objName = objName;

//    listOldFilter = SettLoader::intList2strList(listFilters);
    for(int i = 0, iiMax = listLabels.size(); i < iiMax; i++){
        QListWidgetItem *item = new QListWidgetItem( listLabels.at(i)) ;
        item->setCheckState( listFilters.contains(i) ? Qt::Checked : Qt::Unchecked );
        ui->lWdgt->addItem(item);
    }

}
//---------------------------------------------------------------------------------------
FilterWidgetForm::~FilterWidgetForm()
{
    delete ui;
}

//---------------------------------------------------------------------------------------
void FilterWidgetForm::on_pushButton_clicked()
{
    QStringList listFilter;
    for(int i = 0, iMax = ui->lWdgt->count(); i < iMax; i++){
        if(ui->lWdgt->item(i)->checkState() == Qt::Checked)
            listFilter.append(QString::number(i));
    }

    if(listFilter.isEmpty())
        listFilter = SettLoader::intList2strList(listDefFilters);
//    if(listFilter != listOldFilter)
    emit setNewFilterSett(objName, listFilter);
    deleteLater();
}
//---------------------------------------------------------------------------------------
