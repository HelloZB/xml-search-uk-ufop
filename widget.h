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
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtCore>
#include <QFont>
#include <QComboBox>
#include <QPlainTextEdit>
#include <QLineEdit>

#include "resizetablehelper.h"
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(const QFont &font4log, QWidget *parent = 0);
    ~Widget();

    ResizeTableHelper *tvHelper;

signals:
    void startSearch(QString fileName, QString who);


public slots:
    void onSearchFinished();

    void showMess(QString mess);

    void addWdgt2stackWdgt(QWidget *w, QString wdgtTitle);

    void saveLastVisibleColSett(bool isUr, QVariantMap map);

private slots:
    void createObjects();

    void loadMainWindwSettings();

    void reloadSettings();

    void saveSettings();

    void saveMainSettings();

//    void on_toolButton_clicked();

//    void on_pushButton_clicked();



    void on_pbStartSearch_clicked();

    void on_rbUr_clicked();

    void on_rbFop_clicked();

    void on_rbUr_2_clicked();

    void on_rbFop_2_clicked();

    void on_tbSettings_clicked();

    void on_pbGoHome_clicked();

    void on_tbReset_clicked();

    void on_tbPath2file_clicked();

    void on_pbAddFltr_clicked();

    void on_tbRemoveFltrUr_clicked();

    void on_pbAddFltrFop_clicked();

    void on_tbRemoveFltrFop_clicked();

    void on_tbFileHistory_clicked();

    void onActionOpenFile();

private:
    Ui::Widget *ui;


    QString defUrTree() const;
    QString defFopTree() const;

    QString lastPath2dir() const;

    QVariantMap getLastVisibleCol(const bool &i4urik);

    QStringList pte2strList(QPlainTextEdit *pte);

    void saveLastPath2dir(const QString &lastFileName);

    QStringList getLastOpenedFileHistory();

    QStringList getLastComplList();

    bool saveComplList(const QString &phraze, QStringList &complList);


    void updateCbxItms(QComboBox *cbx, const QString &tree);

    void addLine2pte(QPlainTextEdit *pte, QLineEdit *le, const QString &cbxCurrTxt);

    void delLineFromPte(QPlainTextEdit *pte, const QString &cbxCurrTxt);

    void addCompl2le(QLineEdit *le, const QStringList &l);



};

#endif // WIDGET_H
