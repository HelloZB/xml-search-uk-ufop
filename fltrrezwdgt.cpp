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
#include "fltrrezwdgt.h"
#include "ui_fltrrezwdgt.h"
#include <QTimer>
#include <QtCore>
#include <QMenu>
#include <QAction>
#include <QStandardPaths>
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QClipboard>

#include "xmlsearcher.h"
#include "zbyratorfilehelper.h"

#define FILE_TYPE_PDF   1
#define FILE_TYPE_HTML  2
#define FILE_TYPE_CSV   3
#define FILE_TYPE_ODF   4

//----------------------------------------------------------------------
FltrRezWdgt::FltrRezWdgt(ResizeTableHelper *tvHelper, const QStringList &lFltr, const QString &fileName, const QStringList &lTree, const bool &useOrMode, const bool &enCustomBuffer, const QVariantMap &map4visibleCol, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FltrRezWdgt)
{
    ui->setupUi(this);
    this->tvHelper = tvHelper;
    this->lFltr = lFltr;
    this->fileName = fileName;
    this->lTree = lTree;
    this->enCustomBuffer = enCustomBuffer;
    this->useOrMode = useOrMode;
    this->map4visibleCol = map4visibleCol;
    this->isUrik = this->map4visibleCol.take("isUrikCol\t").toBool();


    QTimer::singleShot(11, this, SLOT(createPage()) );

}

//----------------------------------------------------------------------

FltrRezWdgt::~FltrRezWdgt()
{
    delete ui;
}
//----------------------------------------------------------------------
QVariant FltrRezWdgt::add2actSaveFileData(const QString &fileName, const bool &onlySelected)
{
    QVariantHash h;
    h.insert("f", fileName);
    h.insert("o", onlySelected);
    return QVariant(h);
}
//----------------------------------------------------------------------
QString FltrRezWdgt::actData2fileName(const QVariant &actData)
{
    return actData.toHash().value("f").toString();
}
//----------------------------------------------------------------------
bool FltrRezWdgt::actData2onlySelected(const QVariant &actData)
{
    return actData.toHash().value("o").toBool();
}
//----------------------------------------------------------------------
QString FltrRezWdgt::loadLastPath2dirSett()
{
    QFileInfo fi(fileName);
    return fi.absolutePath();
}
//----------------------------------------------------------------------
QString FltrRezWdgt::getSaveFileName(QString path2dir, QString fileName, int fileType)
{
    if(path2dir.isEmpty())
        path2dir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation ).first();

    QString filters, extStr;
    switch(fileType){
    case FILE_TYPE_PDF: filters = tr("PDF files (*.pdf)"); extStr = ".pdf"; break;
    case FILE_TYPE_HTML: filters = tr("HTML files (*.html)"); extStr = ".html"; break;
    case FILE_TYPE_CSV: filters = tr("CSV files (*.csv)"); extStr = ".csv"; break;
    case FILE_TYPE_ODF: filters = tr("OpenDocument Format files (*.odf)"); extStr = ".odf"; break;
    }
    if(!filters.isEmpty())
        filters.append(";;");
    filters.append(tr("All files (*)"));

    if(!extStr.isEmpty() && !fileName.isEmpty() && fileName.right(extStr.length()).toLower() != extStr)
        fileName.append(extStr);

    fileName = QFileDialog::getSaveFileName(this, "", path2dir + "/" + fileName, filters);
    if(!fileName.isEmpty()){

        if(!extStr.isEmpty() && fileName.right(extStr.length()).toLower() != extStr)
            fileName.append(extStr);

    }
    return fileName;
}
//----------------------------------------------------------------------
QList<int> FltrRezWdgt::getHiddenCols(QTableView *tv, const int &colCount)
{
    QList<int> li;
    if(tv){
        for(int i = 0; i < colCount; i++){
            if(tv->isColumnHidden(i))
                li.append(i);
        }
    }
    return li;
}
//----------------------------------------------------------------------
QStringList FltrRezWdgt::modelHeaderList(QStandardItemModel *model, const QList<int> &hiddenCols)
{
    QStringList header;
    if(model){
        for(int i = 0, iMax = model->columnCount(); i < iMax; i++){
            if(hiddenCols.contains(i))
                continue;
            header.append( model->horizontalHeaderItem(i) ? model->horizontalHeaderItem(i)->text() : tr("Colulmn %1").arg(i) );
        }
    }
    return header;
}
//----------------------------------------------------------------------
QList<int> FltrRezWdgt::getRowsListTo(const int &maxRow)
{
    QList<int> li;
    for(int i = 0; i < maxRow; i++)
        li.append(i);
    return li;
}
//----------------------------------------------------------------------
QList<int> FltrRezWdgt::getRowsListSelected(QItemSelectionModel *selModel)
{
    QList<int> li;
    const QModelIndexList ml = selModel->selectedRows();
    for(int i = 0, iMax = ml.size(); i < iMax; i++){
        if(ml.at(i).isValid())
            li.append(ml.at(i).row());
    }
    return li;
}

//----------------------------------------------------------------------

void FltrRezWdgt::createPage()
{

    ui->widget->hide();

    model = new QStandardItemModel(this);
    proxy_model = new MySortFilterProxyModel(this);


    QThread *t = new QThread;
    XmlSearcher *xml = new XmlSearcher;

    xml->moveToThread(t);

    connect(xml, SIGNAL(destroyed(QObject*)), t, SLOT(quit()) );
    connect(t, SIGNAL(finished()), t, SLOT(deleteLater()) );
    connect(xml, SIGNAL(onSearchDone(QString,qint64)), xml, SLOT(deleteLater()) );

    connect(this, SIGNAL(destroyed(QObject*)), xml, SLOT(stopAllNow()), Qt::DirectConnection);
    connect(ui->pushButton_4, SIGNAL(clicked(bool)), xml, SLOT(stopAllNow()), Qt::DirectConnection );

    connect(this, SIGNAL(startSearch(QString,QStringList,QStringList,bool,bool)), xml, SLOT(startSearch(QString,QStringList,QStringList,bool,bool)) );

    connect(xml, SIGNAL(showMess(QString)), this, SIGNAL(showMess(QString)) );
    connect(xml, SIGNAL(add2pte(QString)), this, SLOT(appendPte(QString)) );
    connect(xml, SIGNAL(setHeader(QStringList)), this, SLOT(setHeader(QStringList)) );
//    connect(xml, SIGNAL(addRow2table(QStringList)), this, SLOT(addRow2table(QStringList)) );
    connect(xml, SIGNAL(addRows2table(QVariantList,int,int)), this, SLOT(addRows2table(QVariantList,int,int)) );

//    connect(xml, SIGNAL(onSearchDone()), this, SLOT(onSearchFinished()) );
    connect(xml, SIGNAL(searchProgress(int)), ui->progressBar, SLOT(setValue(int)) );


    connect(xml, SIGNAL(onSearchDone(QString,qint64)), this, SLOT(onSearchDone(QString,qint64)) );
    t->start();

    emit startSearch(fileName, lFltr, lTree, useOrMode, enCustomBuffer);


    ui->label->setText(tr("Виконується пошук... %1").arg(QString(lFltr.join("\n")).remove(";").replace("\n", "; ").replace("\t", " - ")));



    proxy_model->setDynamicSortFilter(true);
    proxy_model->setSourceModel(model);
    connect(ui->leFilter, SIGNAL(textChanged(QString)), proxy_model, SLOT(setNewFileterStr(QString)) );

    ui->tableView->setModel(proxy_model);
    ui->tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->tbHide, SIGNAL(clicked(bool)), ui->widget, SLOT(hide()) );
    connect(ui->pbClose, SIGNAL(clicked(bool)), this, SLOT(deleteLater()) );



}

//----------------------------------------------------------------------

void FltrRezWdgt::appendPte(QString s)
{
    qDebug() << "s=" << s;
}
//----------------------------------------------------------------------
void FltrRezWdgt::setHeader(QStringList lHeader)
{
    qDebug() << "setHeader " << lHeader;
    model->clear();
    QList<int> li;
    for(int i = 0, iMax = lHeader.size(); i < iMax; i++){
        model->setHorizontalHeaderItem(i, new QStandardItem(lHeader.at(i)));
        if(map4visibleCol.contains(lHeader.at(i)))
            ui->tableView->setColumnHidden(i, !map4visibleCol.value(lHeader.at(i), true).toBool());
        li.append(i);
    }
    proxy_model->setFilterMode(li);

    lastHeader = lHeader;

}
//----------------------------------------------------------------------
void FltrRezWdgt::addRow2table(QStringList lrow)
{
    QList<QStandardItem*> li;
    for(int i = 0, iMax = lrow.size(); i < iMax; i++)
        li.append(new QStandardItem(lrow.at(i)));
    model->appendRow(li);
}
//----------------------------------------------------------------------
void FltrRezWdgt::onSearchDone(QString timeEl, qint64 rezCount)
{
    ui->label->setText(tr("Пошук завершено. Затрачено часу %1. Знайдено співпадінь: %2").arg(timeEl).arg(rezCount));
}
//----------------------------------------------------------------------
void FltrRezWdgt::addRows2table(QVariantList lrows, int lrowsSize, int strListSize)
{
    for(int i = 0; i < lrowsSize; i++){
        QList<QStandardItem*> li;
        QStringList lrow = lrows.at(i).toStringList();
        for(int j = 0; j < strListSize; j++)
            li.append(new QStandardItem(lrow.at(j)));
        model->appendRow(li);
    }
}
//----------------------------------------------------------------------
void FltrRezWdgt::setNewShowColumnSett(QString objName, QStringList listIndx)
{
    if(objName == objectName()){
        QVariantMap map;
        for(int i = 0, iMax = lastHeader.size(); i < iMax; i++){
            QString s = QString::number(i);
            ui->tableView->setColumnHidden(i, !listIndx.contains(s));
            map.insert(lastHeader.at(i), listIndx.contains(s));
        }
        emit saveLastVisibleColSett(isUrik, map);
        map4visibleCol = map;
    }
}
//----------------------------------------------------------------------
QList<QAction *> FltrRezWdgt::exportActions(QMenu *prnt, const bool &onlySelected, const QString &defFileName)
{
    QVariant var = add2actSaveFileData(defFileName, onlySelected);
    QAction *actPdf = new QAction(tr("PDF"), prnt);
    actPdf->setData(var);
    actPdf->setIcon(QIcon(":/katynko/svg/application-pdf.svg"));
    connect(actPdf, SIGNAL(triggered(bool)), this, SLOT(onExport2pdfAct()) );


    QAction *actHTML = new QAction(tr("HTML"), prnt);
    actHTML->setData(var);
    actHTML->setIcon(QIcon(":/katynko/svg/text-html.svg"));
    connect(actHTML, SIGNAL(triggered(bool)), this, SLOT(onExport2htmlAct()) );

    QAction *actCSV = new QAction(tr("CSV"), prnt);
    actCSV->setData(var);
    actCSV->setIcon(QIcon(":/katynko/svg/x-office-spreadsheet.svg"));
    connect(actCSV, SIGNAL(triggered(bool)), this, SLOT(onExport2csvAct()) );

    QAction *actODF = new QAction(tr("OpenDocument Format"), prnt);
    actODF->setData(var);
    actODF->setIcon(QIcon(":/katynko/svg/x-office-document.svg"));
    connect(actODF, SIGNAL(triggered(bool)), this, SLOT(onExport2odfAct()) );

    QList<QAction*> la;
    la.append(actPdf);
    la.append(actHTML);
    la.append(actCSV);
    la.append(actODF);
    return la;
}
//----------------------------------------------------------------------
void FltrRezWdgt::showMessExt(QString mess, QString filename)
{
    if(QMessageBox::question(this, tr("Відкрити файл?"), tr("Файл збережено, відкрити?<br>%1").arg(mess), QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes){
#ifdef Q_OS_WIN
        QDesktopServices::openUrl(QUrl("file:///" + filename));

#else
        QDesktopServices::openUrl(QUrl("file://" + filename));
#endif
    }
}

//----------------------------------------------------------------------


void FltrRezWdgt::on_tableView_clicked(const QModelIndex &index)
{
    int row = proxy_model->mapToSource(index).row();


    QStringList l;
    for(int i = 0, imax = lastHeader.size(); i < imax; i++){
        QStringList list = model->item(row, i)->text().split("\n");
        for(int j = 0, jMax = list.size(), lhLen = lastHeader.at(i).length(); j < jMax; j++)
            l.append( ((j == 0) ? lastHeader.at(i) : QString("_").rightJustified(lhLen,  '_')) + "\t-\t" +  list.at(j));
    }
    ui->pteDetail->setPlainText(l.join("\n"));
    ui->widget->show();
}
//----------------------------------------------------------------------

void FltrRezWdgt::on_pushButton_clicked()
{
    tvHelper->addTable2resizeLater(ui->tableView);
}
//----------------------------------------------------------------------
void FltrRezWdgt::on_pushButton_2_clicked()
{
    tvHelper->addTable2resizeLaterHeight(ui->tableView);

}

//----------------------------------------------------------------------
void FltrRezWdgt::on_tableView_customContextMenuRequested(const QPoint &pos)
{
    QTableView *parnt = ui->tableView;
    QMenu *menu = new QMenu(parnt);

    bool hasItems = (parnt->model()->rowCount() > 0);
    int selRowCount = parnt->selectionModel()->selectedRows().size();
    bool hasSelItems = (selRowCount > 0 );

    if(true){
        QAction *a = new QAction(tr("Reset sorting"), menu);
        a->setIcon(QIcon(":/katynko/svg/view-refresh.svg"));
        connect(a, SIGNAL(triggered(bool)), this, SLOT(onResetSortingAct()) );
        menu->addAction(a);
    }

    if(true){

        menu->addSeparator();

        QFileInfo fi(fileName);
        QString fn = fi.fileName();
         if(fn.right(4).toLower() == ".xml")
             fn.chop(4);
        QString defFileName = "результат пошуку " +  fn;

        QMenu *menuExpAll = new QMenu(tr("Export all to"), menu);
        menuExpAll->setIcon(QIcon(":/katynko/svg/document-save.svg"));
        menuExpAll->addActions(exportActions(menu, false, defFileName));
        menuExpAll->setEnabled(hasItems);
        menu->addMenu(menuExpAll);

        QMenu *expSelected = new QMenu(tr("Export selected to"), menu);
        expSelected->addActions(exportActions(menu, true, defFileName));
        expSelected->setEnabled(hasSelItems);
        menu->addMenu(expSelected);
    }

    if(true){

        if(hasSelItems ){
            menu->addSeparator();
            menu->addAction(actCopySelectedRowAsTxt(menu, true, selRowCount));
        }
    }

    menu->exec(parnt->mapToGlobal(pos));
    menu->deleteLater();
}
//----------------------------------------------------------------------
void FltrRezWdgt::onResetSortingAct()
{
    proxy_model->resetSorting();
}
//----------------------------------------------------------------------
void FltrRezWdgt::onExport2pdfAct()
{
    QObject *obj = QObject::sender();
    QAction *a = qobject_cast<QAction *>(obj);
    QTableView *tv = ui->tableView;
    if(tv && a){

        QString fileName = getSaveFileName(loadLastPath2dirSett() , actData2fileName(a->data()), FILE_TYPE_PDF);
        if(fileName.isEmpty())
            return;

        MySortFilterProxyModel *proxyModel = proxy_model;// = proxyModelFromTv(tv);
        QStandardItemModel *model = this->model;// = modelFromTv(tv);


        QList<int> hiddenCols = getHiddenCols(tv, model->columnCount());
        QList<int> rowsList = actData2onlySelected(a->data()) ? getRowsListSelected(tv->selectionModel()) : getRowsListTo(proxyModel->rowCount()) ;

        QString mess;

        if(ZbyratorFileHelper::save2pdfFile(mess, fileName, model, proxyModel, hiddenCols, rowsList, modelHeaderList(model, hiddenCols), QStringList())){   //, tv, actData2onlySelected(a->data()))){
            showMessExt(tr("%1<br>Open file?").arg(mess), fileName);

        }else{
            emit showMess(mess);
        }
    }

}
//----------------------------------------------------------------------
void FltrRezWdgt::onExport2htmlAct()
{
    QObject *obj = QObject::sender();
    QAction *a = qobject_cast<QAction *>(obj);
    QTableView *tv = ui->tableView;

    if(tv && a){
        QString fileName = getSaveFileName(loadLastPath2dirSett(), actData2fileName(a->data()), FILE_TYPE_HTML);
        if(fileName.isEmpty())
            return;

        MySortFilterProxyModel *proxyModel = proxy_model;// = proxyModelFromTv(tv);
        QStandardItemModel *model = this->model;// = modelFromTv(tv);


        QList<int> hiddenCols = getHiddenCols(tv, model->columnCount());
        QList<int> rowsList = actData2onlySelected(a->data()) ? getRowsListSelected(tv->selectionModel()) : getRowsListTo(proxyModel->rowCount()) ;

        QString mess;

        if(ZbyratorFileHelper::save2htmlFile(mess, fileName, model, proxyModel, hiddenCols, rowsList, modelHeaderList(model, hiddenCols), QStringList())){   //, tv, actData2onlySelected(a->data()))){
            showMessExt(tr("%1<br>Open file?").arg(mess), fileName);
        }else{
            emit showMess(mess);
        }
    }

}
//----------------------------------------------------------------------
void FltrRezWdgt::onExport2csvAct()
{
    QObject *obj = QObject::sender();
    QAction *a = qobject_cast<QAction *>(obj);
    QTableView *tv = ui->tableView;

    if(tv && a){
        QString fileName = getSaveFileName(loadLastPath2dirSett(), actData2fileName(a->data()), FILE_TYPE_CSV);
        if(fileName.isEmpty())
            return;

        MySortFilterProxyModel *proxyModel = proxy_model;// = proxyModelFromTv(tv);
        QStandardItemModel *model = this->model;// = modelFromTv(tv);


        QList<int> hiddenCols = getHiddenCols(tv, model->columnCount());
        QList<int> rowsList = actData2onlySelected(a->data()) ? getRowsListSelected(tv->selectionModel()) : getRowsListTo(proxyModel->rowCount()) ;

        QString mess;

        if(ZbyratorFileHelper::save2csvFile(mess, fileName, model, proxyModel, hiddenCols, rowsList, modelHeaderList(model, hiddenCols), QStringList())){   // tv, actData2onlySelected(a->data()))){
            showMessExt(tr("%1<br>Open file?").arg(mess), fileName);
        }else{
            emit showMess(mess);
        }
    }

}
//----------------------------------------------------------------------
void FltrRezWdgt::onExport2odfAct()
{
    QObject *obj = QObject::sender();
    QAction *a = qobject_cast<QAction *>(obj);
    QTableView *tv = ui->tableView;

    if(tv && a){
        QString fileName = getSaveFileName(loadLastPath2dirSett(), actData2fileName(a->data()), FILE_TYPE_ODF);
        if(fileName.isEmpty())
            return;

        MySortFilterProxyModel *proxyModel = proxy_model;// = proxyModelFromTv(tv);
        QStandardItemModel *model = this->model;// = modelFromTv(tv);

        QList<int> hiddenCols = getHiddenCols(tv, model->columnCount());
        QList<int> rowsList = actData2onlySelected(a->data()) ? getRowsListSelected(tv->selectionModel()) : getRowsListTo(proxyModel->rowCount()) ;

        QString mess;

        if(ZbyratorFileHelper::save2odfFile(mess, fileName, model, proxyModel, hiddenCols, rowsList, modelHeaderList(model, hiddenCols), QStringList())){   //, tv, actData2onlySelected(a->data()))){
            showMessExt(tr("%1<br>Open file?").arg(mess), fileName);
        }else{
            emit showMess(mess);
        }
    }

}
//----------------------------------------------------------------------
void FltrRezWdgt::onCopySelectedRows()
{
    QTableView *tv = ui->tableView;

    QStandardItemModel *model = this->model;
    MySortFilterProxyModel *proxy_model = this->proxy_model;
    QItemSelectionModel *selModel;

    int colCount = 0;
    QList<int> hiddenCols;


    selModel = tv->selectionModel();
    colCount = tv->model()->columnCount();
    hiddenCols = getHiddenCols(tv, colCount);


    if(colCount > 0 && selModel && model && !selModel->selectedRows(0).isEmpty()){
        QStringList list;
        const QModelIndexList mIndxList = selModel->selectedRows(0);

        /* QVariantList l;
            l.append(modelDevices->item(row, 0)->data(Qt::UserRole + 2).toString());
            l.append(modelDevices->item(row, 0)->data(Qt::UserRole + 1).toHash());

            lRecoverLvDevHistory.prepend(QVariant(l));*/

        for(int i = 0, iMax = mIndxList.size(); i < iMax; i++){
            QStringList lRow;
            int row = proxy_model->mapToSource(mIndxList.at(i)).row() ;
            for(int col = 0; col < colCount; col++){
                if(hiddenCols.contains(col))
                    continue;
                lRow.append(model->item(row, col)->text().simplified());
            }
            list.append(lRow.join("\t"));
        }


        QClipboard *clipboard = QApplication::clipboard();


            QString joinStr = "\n";
#ifndef Q_OS_LINUX
            joinStr = "\r\n";
#endif
            clipboard->setText(list.join(joinStr));


    }
}
//----------------------------------------------------------------------
QAction *FltrRezWdgt::actCopySelectedRowAsTxt(QMenu *prnt, const bool &hasProxy, const int &selCount, const int dataType)
{
    QAction *act = new QAction( QString("%1 (%2)").arg(tr("Copy selected as plain text")).arg(selCount), prnt);
    act->setIcon(QIcon(":/katynko/svg/edit-copy.svg"));
    if(dataType < 0)
        act->setData( hasProxy);
    else
        act->setData(dataType);
    connect(act, SIGNAL(triggered(bool)), this, SLOT(onCopySelectedRows()) );
    return act;
}
//----------------------------------------------------------------------

void FltrRezWdgt::on_pushButton_3_clicked()
{
    QList<int> listVisible;
    QList<int> listDef;
    for(int i = 0, imax = lastHeader.size(); i < imax; i++){
        if(map4visibleCol.value(lastHeader.at(i), true).toBool())
            listVisible.append(i);
        listDef.append(i);
    }


    tvHelper->showFilterWidget(listVisible , listDef , lastHeader, this);
}
