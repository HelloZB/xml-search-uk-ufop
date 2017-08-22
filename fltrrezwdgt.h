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
#ifndef FLTRREZWDGT_H
#define FLTRREZWDGT_H

#include <QWidget>
#include <QStandardItemModel>
#include "mysortfilterproxymodel.h"
#include "resizetablehelper.h"
#include <QtCore>

namespace Ui {
class FltrRezWdgt;
}

class FltrRezWdgt : public QWidget
{
    Q_OBJECT



public:
    explicit FltrRezWdgt(ResizeTableHelper *tvHelper, const QStringList &lFltr, const QString &fileName, const QStringList &lTree, const bool &useOrMode,
                         const bool &enCustomBuffer, const QVariantMap &map4visibleCol, QWidget *parent = 0);
    ~FltrRezWdgt();
    ResizeTableHelper *tvHelper;
    static QVariant add2actSaveFileData(const QString &fileName, const bool &onlySelected);
    static QString actData2fileName(const QVariant &actData);
    static bool actData2onlySelected(const QVariant &actData);

    QString loadLastPath2dirSett();
    QString getSaveFileName(QString path2dir, QString fileName, int fileType);
    static QList<int> getHiddenCols(QTableView *tv, const int &colCount);
    static QStringList modelHeaderList(QStandardItemModel *model, const QList<int> &hiddenCols);

    static QList<int> getRowsListTo(const int &maxRow);
    static QList<int> getRowsListSelected(QItemSelectionModel *selModel);

signals:
    void showMess(QString mess);

    void startSearch(QString fileName, QStringList who, QStringList tree, bool useOrMode, bool enCustomBuffer);

    void saveLastVisibleColSett(bool isUr, QVariantMap map);

private slots:
    void createPage();

    void on_tableView_clicked(const QModelIndex &index);



    void on_pushButton_clicked();

    void on_pushButton_2_clicked();


    void on_tableView_customContextMenuRequested(const QPoint &pos);


    void onResetSortingAct();


    //onExport2
    void onExport2pdfAct();

    void onExport2htmlAct();

    void onExport2csvAct();

    void onExport2odfAct();


    void onCopySelectedRows();

    void on_pushButton_3_clicked();

public slots:
    void appendPte(QString s);

    void setHeader(QStringList lHeader);


    void addRow2table(QStringList lrow);

    void onSearchDone(QString timeEl, qint64 rezCount);

    void addRows2table(QVariantList lrows, int lrowsSize, int strListSize);

    void setNewShowColumnSett(QString objName, QStringList listIndx);

private:
    Ui::FltrRezWdgt *ui;

    QList<QAction*> exportActions(QMenu *prnt, const bool &onlySelected, const QString &defFileName);

    QAction *actCopySelectedRowAsTxt(QMenu *prnt, const bool &hasProxy, const int &selCount, const int dataType = -1);

    void showMessExt(QString mess, QString filename);

    QStringList lFltr, lTree;
    QString fileName;

    bool enCustomBuffer, useOrMode;

    MySortFilterProxyModel *proxy_model;
    QStandardItemModel *model;

    QStringList lastHeader;
    bool isUrik;
    QVariantMap map4visibleCol;

};

#endif // FLTRREZWDGT_H
