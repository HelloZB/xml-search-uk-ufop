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
#include "widget.h"
#include "ui_widget.h"

#include <QtCore>
#include <QFileDialog>
#include <QSettings>
#include <QDesktopWidget>
#include <QStandardPaths>
#include <QMessageBox>
#include <QMenu>
#include <QCompleter>

#include "xmlsearcher.h"
#include "fltrrezwdgt.h"

//-------------------------------------------------------------------
Widget::Widget(const QFont &font4log, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    ui->pteXmlSchema->setFont(font4log);
    ui->pteXmlSchemaFop->setFont(font4log);

    QTimer::singleShot(11, this, SLOT(createObjects()) );


}
//-------------------------------------------------------------------
Widget::~Widget()
{
    delete ui;
}
//-------------------------------------------------------------------
void Widget::onSearchFinished()
{
    //    ui->pushButton->setEnabled(true);
}

void Widget::showMess(QString mess)
{
    QMessageBox::information(this, tr("Ой йой!"), mess);

}

void Widget::addWdgt2stackWdgt(QWidget *w, QString wdgtTitle)
{
    ui->tabWidget->addTab(w, wdgtTitle);
    ui->tabWidget->setCurrentWidget(w);
}
//-------------------------------------------------------------------
void Widget::saveLastVisibleColSett(bool isUr, QVariantMap map)
{
    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);
    settings.beginGroup("visible");
    settings.setValue( isUr ? "ur" : "fop", map );
    settings.endGroup();
}
//-------------------------------------------------------------------
void Widget::createObjects()
{
//    QFont f = ui->pteFltrUr->font();

    tvHelper = new ResizeTableHelper(this);
    tvHelper->parentWidget = ui->tabWidget;

    connect(tvHelper, SIGNAL(addWdgt2stackWdgt(QWidget*,QString)), this, SLOT(addWdgt2stackWdgt(QWidget*,QString)) );


    loadMainWindwSettings();
    reloadSettings();

    ui->rbUr->setChecked(true);
    ui->swUrOrFop->setCurrentIndex(0);

    ui->rbUr_2->setChecked(true);
    ui->swUrOrFop_2->setCurrentIndex(0);

    connect(ui->leFltrUr, SIGNAL(returnPressed()), ui->pbAddFltr, SLOT(animateClick()) );
    connect(ui->leFltrFop, SIGNAL(returnPressed()), ui->pbAddFltrFop, SLOT(animateClick()) );

    QStringList l = getLastComplList();

    addCompl2le(ui->leFltrFop, l);
    addCompl2le(ui->leFltrUr, l);

}
//-------------------------------------------------------------------
void Widget::loadMainWindwSettings()
{
    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);
    settings.beginGroup("main");
    QRect rect = settings.value("rect").toRect();
    if(!rect.isNull()){
        qint32 desktopW = qApp->desktop()->width();
        qint32 desktopH = qApp->desktop()->height();

        if(rect.x()< 10)
            rect.setX(10);
        if(rect.y()<30)
            rect.setY(31);
        if(rect.x()>(desktopW)  || rect.y()>(desktopH)) {
            int lastW = rect.width();
            int lastH = rect.height();
            rect.setX(10);
            rect.setY(31);
            rect.setWidth(lastW);
            rect.setHeight(lastH);
        }
        setGeometry(rect);
    }
    settings.endGroup();

    settings.beginGroup("file-system");
    QString fileName = settings.value("lastFileName").toString();
    QString dirPath = settings.value("lastDirPath").toString();
    settings.endGroup();



    if(!dirPath.isEmpty() && !fileName.isEmpty()){
        QFileInfo fi(dirPath + "/" + fileName);
        if(fi.exists())
            ui->leFileName->setText(fi.absoluteFilePath());
    }
}
//-------------------------------------------------------------------
void Widget::reloadSettings()
{
    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);
    qDebug() << "settings " << settings.fileName();

    settings.beginGroup("options");

    QString treeUr = settings.value("pteXmlSchema", defUrTree()).toString();
    QString treeFop = settings.value("pteXmlSchemaFop", defFopTree()).toString();

    settings.endGroup();

    ui->pteXmlSchema->setPlainText(treeUr);
    ui->pteXmlSchemaFop->setPlainText(treeFop);


    updateCbxItms(ui->cbxFltrUr, treeUr);
    updateCbxItms(ui->cbxFltrFop, treeFop);

}
//-------------------------------------------------------------------
void Widget::saveSettings()
{

    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);
       settings.beginGroup("options");
    settings.setValue("pteXmlSchema", pte2strList(ui->pteXmlSchema).join("\n"));
    settings.setValue("pteXmlSchemaFop", pte2strList(ui->pteXmlSchemaFop).join("\n"));

//    settings.setValue("", ui->pt);
    settings.endGroup();

}
//-------------------------------------------------------------------
void Widget::saveMainSettings()
{
    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);
    settings.beginGroup("main");
    settings.setValue("rect", geometry());
    settings.endGroup();
}
//-------------------------------------------------------------------


//void Widget::on_toolButton_clicked()
//{
//    QString f = QFileDialog::getOpenFileName(this);
//    if(!f.isEmpty())
//        ui->lineEdit->setText(f);


//}

//void Widget::on_pushButton_clicked()
//{
//    ui->pushButton->setEnabled(false);
//    ui->plainTextEdit->clear();
//    emit startSearch(ui->lineEdit->text(), ui->lineEdit_2->text().simplified().trimmed());


//}
//----------------------------------------------------------------------
void Widget::on_pbStartSearch_clicked()
{
    QStringList l = pte2strList((ui->rbUr->isChecked()) ? ui->pteFltrUr : ui->pteFltrFop);
    QString mess;
    QFileInfo fi(ui->leFileName->text());
    if(!fi.exists())
        mess.append(tr("Нема такого файлу: %1<br>").arg(fi.fileName()));
    if(l.isEmpty())
        mess.append(tr("Відсутній фільтр для пошуку"));

    if(!mess.isEmpty()){
        showMess(mess);
        return;
    }

    QStringList ll;
    for(int i = 1, iMax = ui->tabWidget->count(); i < iMax; i++)
        ll.append(ui->tabWidget->tabText(i));

    QString tabText = fi.fileName();
    for(int i = 2; i < 1000; i++){
        if(!ll.contains(tabText))
            break;
        tabText = QString("%1 (%2)").arg(fi.fileName()).arg(i);
    }

    FltrRezWdgt *w = new FltrRezWdgt(tvHelper, l, ui->leFileName->text(), pte2strList( ui->rbUr->isChecked() ? ui->pteXmlSchema : ui->pteXmlSchemaFop), ui->rbOrMode->isChecked(), ui->cbEnCustomBuffer->isChecked(), getLastVisibleCol(ui->rbUr->isChecked()), ui->tabWidget);

    connect(w, SIGNAL(showMess(QString)), this, SLOT(showMess(QString)) );
    connect(w, SIGNAL(saveLastVisibleColSett(bool,QVariantMap)), this, SLOT(saveLastVisibleColSett(bool,QVariantMap)) );
    ui->tabWidget->addTab(w, tabText);
    w->setObjectName(QString::number(QDateTime::currentMSecsSinceEpoch()));
    ui->tabWidget->setCurrentWidget(w);


    QStringList complList;
    if(saveComplList(ui->rbUr->isChecked() ? ui->leFltrUr->text().simplified().trimmed() : ui->leFltrFop->text().simplified().trimmed(), complList)){
        addCompl2le(ui->leFltrFop, complList);
        addCompl2le(ui->leFltrUr, complList);
    }
    saveMainSettings();
}
//----------------------------------------------------------------------
QString Widget::defUrTree() const
{
    return QString("RECORD\n"
                   "\tNAME\n"
                   "\tSHORT_NAME\n"
                   "\tEDRPOU\n"
                   "\tADDRESS\n"
                   "\tBOSS\n"
                   "\tKVED\n"
                   "\tSTAN\n"
                   "\tFOUNDERS\n"
                       "\t\tFOUNDER");
}
//----------------------------------------------------------------------
QString Widget::defFopTree() const
{
    return QString("RECORD\n"
                   "\tFIO\n"
                   "\tADDRESS\n"
                   "\tKVED\n"
                   "\tSTAN");
}
//----------------------------------------------------------------------
QString Widget::lastPath2dir() const
{
    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);
    settings.beginGroup("file-system");
    QString dirPath = settings.value("lastDirPath").toString();
    settings.endGroup();

    if(dirPath.isEmpty())
        dirPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    return dirPath;
}
//----------------------------------------------------------------------
QVariantMap Widget::getLastVisibleCol(const bool &i4urik)
{
    QVariantMap map;
    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);
    settings.beginGroup("visible");
    map = settings.value(i4urik ? "ur" : "fop").toMap();
    settings.endGroup();
    map.insert("isUrikCol\t", i4urik);
    return map;
}
//----------------------------------------------------------------------
QStringList Widget::pte2strList(QPlainTextEdit *pte)
{
    qDebug() << "pte=" << pte->toPlainText();
    QStringList l = pte->toPlainText().replace("\r\n", "\n").replace("\r", "\n").split("\n", QString::SkipEmptyParts);
    return l;
}

//----------------------------------------------------------------------

void Widget::saveLastPath2dir(const QString &lastFileName)
{
    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);

    QFileInfo fi(lastFileName);
    settings.beginGroup("file-system");
    QStringList listHistory = settings.value("lastFiles").toStringList();
    if(!listHistory.isEmpty()){
        listHistory.removeAll(lastFileName);
        listHistory = listHistory.mid(0, 10);
    }
    listHistory.prepend(lastFileName);
    settings.setValue("lastFiles", listHistory);
    settings.setValue("lastFileName", fi.fileName());
    settings.setValue("lastDirPath", fi.dir().absolutePath());
    settings.endGroup();
}
//----------------------------------------------------------------------
QStringList Widget::getLastOpenedFileHistory()
{
    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);

    settings.beginGroup("file-system");
    QStringList listHistory = settings.value("lastFiles").toStringList();

    settings.endGroup();

    return listHistory;
}
//----------------------------------------------------------------------
QStringList Widget::getLastComplList()
{
    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);

    settings.beginGroup("completer");
    QStringList listHistory = settings.value("compl").toStringList();
    settings.endGroup();

    if(listHistory.isEmpty())
        listHistory = QString("зареєстровано;припинено;в стані припинення;ОСББ;ТОВ;ПАТ;ВАТ;ЗАТ;"
                              "ТОВ НВФ;ПП;ДП;СТД;МСТ;ТЗОВ;МПП;СФГ;ТОВ ВФ;ФГ;ТДВ;МКП;КП;СТОВ").split(";");

    return listHistory;
}
//----------------------------------------------------------------------
bool Widget::saveComplList(const QString &phraze, QStringList &complList)
{
    complList = getLastComplList();
    if(complList.contains(phraze))
        return false;
    complList = complList.mid(0, 1000);
    complList.prepend(phraze);


    QSettings settings("vilnyj-poshuk", QSettings::IniFormat);

    settings.beginGroup("completer");
    settings.setValue("compl", complList);
    settings.endGroup();

    return true;
}

//----------------------------------------------------------------------

void Widget::updateCbxItms(QComboBox *cbx, const QString &tree)
{
    QString txt = cbx->currentText();
    int indx = cbx->currentIndex();

    cbx->clear();

    QStringList l = tree.split("\n", QString::SkipEmptyParts);

    int txtIndx = -1;
    for(int i = 0, iMax = l.size(); i < iMax; i++){
        QString s = l.at(i).simplified().trimmed();
        cbx->addItem(s);
        if(txtIndx < 0 && s == txt)
            txtIndx = i ;
    }

    if(txtIndx >= 0)
        cbx->setCurrentIndex(txtIndx);
    else if(indx >= 0 && indx < cbx->count())
        cbx->setCurrentIndex(indx);

    if(cbx->currentIndex() < 0)
        cbx->setCurrentIndex(0);

}
//----------------------------------------------------------------------
void Widget::addLine2pte(QPlainTextEdit *pte, QLineEdit *le, const QString &cbxCurrTxt)
{
    QStringList l = pte2strList(pte);
    bool changed = false;
    int maxLineWidth = 0;
    for(int i = 0, iMax = l.size(); i < iMax; i++){
        QStringList ll = l.at(i).split("\t", QString::SkipEmptyParts);

        QString s = ll.first().simplified().trimmed();
        maxLineWidth = qMax(s.length(), maxLineWidth);

        if(s == cbxCurrTxt){
            l.removeAt(i);
            i--;
            iMax--;
            changed = true;
        }
    }

    if(cbxCurrTxt.length() > maxLineWidth){
        maxLineWidth = cbxCurrTxt.length();
        for(int i = 0, iMax = l.size(); i < iMax; i++){
            QStringList ll = l.at(i).split("\t", QString::SkipEmptyParts);

            QString s = ll.first().trimmed();
            s = s.leftJustified(maxLineWidth, ' ');
            ll.replace(0, s);
            l.replace(i, ll.join("\t"));
        }
        changed = true;
    }
    if(changed)
        pte->setPlainText(l.join("\n"));

    if(le->text().simplified().trimmed().isEmpty())
        return;

    QString pair = cbxCurrTxt.leftJustified(maxLineWidth, ' ') + "\t" + le->text().simplified().trimmed();
    pte->appendPlainText(pair);


}
//----------------------------------------------------------------------
void Widget::delLineFromPte(QPlainTextEdit *pte, const QString &cbxCurrTxt)
{
    QStringList l = pte2strList(pte);
    bool changed = false;
    int maxLineWidth = 0;
    for(int i = 0, iMax = l.size(); i < iMax; i++){
        QStringList ll = l.at(i).split("\t", QString::SkipEmptyParts);

        QString s = ll.first().trimmed();
        maxLineWidth = qMax(s.length(), maxLineWidth);
        if(s == cbxCurrTxt){
            l.removeAt(i);
            i--;
            iMax--;
            changed = true;
        }
    }

    if(changed){
        for(int i = 0, iMax = l.size(); i < iMax; i++){
            QStringList ll = l.at(i).split("\t", QString::SkipEmptyParts);

            QString s = ll.first().trimmed();
            s = s.leftJustified(maxLineWidth, ' ');
            ll.replace(0, s);
            l.replace(i, ll.join("\t"));
        }
        pte->setPlainText(l.join("\n"));
    }
}
//----------------------------------------------------------------------
void Widget::addCompl2le(QLineEdit *le, const QStringList &l)
{
    QCompleter *c = new QCompleter(l, le);
    c->setCaseSensitivity(Qt::CaseInsensitive);
    le->setCompleter(c);
}
//----------------------------------------------------------------------

void Widget::on_rbUr_clicked()
{
    ui->swUrOrFop->setCurrentIndex(0);
}
//----------------------------------------------------------------------
void Widget::on_rbFop_clicked()
{
    ui->swUrOrFop->setCurrentIndex(1);
}
//----------------------------------------------------------------------
void Widget::on_rbUr_2_clicked()
{
    ui->swUrOrFop_2->setCurrentIndex(0);
}
//----------------------------------------------------------------------
void Widget::on_rbFop_2_clicked()
{
    ui->swUrOrFop_2->setCurrentIndex(1);

}
//----------------------------------------------------------------------

void Widget::on_tbSettings_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}
//----------------------------------------------------------------------
void Widget::on_pbGoHome_clicked()
{
    saveMainSettings();
    saveSettings();
    reloadSettings();
    ui->stackedWidget->setCurrentIndex(0);
}
//----------------------------------------------------------------------

void Widget::on_tbReset_clicked()
{
//    reloadSettings();
    if(ui->rbUr_2->isChecked())
        ui->pteXmlSchema->setPlainText(defUrTree());
    else
        ui->pteXmlSchemaFop->setPlainText(defFopTree());

//    ui->swUrOrFop_2->setCurrentIndex(0);
}
//----------------------------------------------------------------------
void Widget::on_tbPath2file_clicked()
{

    QString f = QFileDialog::getOpenFileName(this, tr("Відкрити файл"), lastPath2dir(), "XML File (*.xml)");
    if(!f.isEmpty()){
        ui->leFileName->setText(f);
        saveLastPath2dir(f);
    }
}
//----------------------------------------------------------------------

void Widget::on_pbAddFltr_clicked()
{
    addLine2pte(ui->pteFltrUr, ui->leFltrUr, ui->cbxFltrUr->currentText());
}
//----------------------------------------------------------------------

void Widget::on_tbRemoveFltrUr_clicked()
{
    delLineFromPte(ui->pteFltrUr, ui->cbxFltrUr->currentText());
}
//----------------------------------------------------------------------

void Widget::on_pbAddFltrFop_clicked()
{
    addLine2pte(ui->pteFltrFop, ui->leFltrFop, ui->cbxFltrFop->currentText());

}
//----------------------------------------------------------------------
void Widget::on_tbRemoveFltrFop_clicked()
{
    delLineFromPte(ui->pteFltrFop, ui->cbxFltrFop->currentText());

}
//----------------------------------------------------------------------

void Widget::on_tbFileHistory_clicked()
{

    QStringList l = getLastOpenedFileHistory();
    if(l.isEmpty())
        return;

    QMenu *menu = new QMenu(ui->tbFileHistory);

    for(int i = 0, iMax = l.size(); i < iMax; i++){
        QFileInfo fi(l.at(i));
        if(!fi.exists())
            continue;
        QAction *a = new QAction(fi.fileName(), menu);
        a->setData(l.at(i));
        connect(a, SIGNAL(triggered(bool)), this, SLOT(onActionOpenFile()) );
        menu->addAction(a);
    }

    menu->exec(ui->tbFileHistory->mapToGlobal(QPoint(0, ui->tbFileHistory->height())));
    menu->deleteLater();

}
//----------------------------------------------------------------------
void Widget::onActionOpenFile()
{
    QObject *obj = QObject::sender();
    QAction *a = qobject_cast<QAction *>(obj);
    if(a){
        ui->leFileName->setText(a->data().toString());
    }
}
//----------------------------------------------------------------------
