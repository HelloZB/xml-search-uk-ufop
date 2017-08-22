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
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication a(argc, argv);

    int fontSize = qApp->font().pointSize();

    int fontId = QFontDatabase::addApplicationFont(":/font/Noto/NotoSans-Regular.ttf");
    if(fontId >= 0 && !QFontDatabase::applicationFontFamilies(fontId).isEmpty()){
        QFont font(QFontDatabase::applicationFontFamilies(fontId).first());
        font.setStyleStrategy(QFont::PreferAntialias);
        font.setPointSize(fontSize);
        qApp->setFont(font);
        QApplication::setFont(font);
    }


    QFont font4log = qApp->font();

    fontId = QFontDatabase::addApplicationFont(":/font/UbuntuMono-R.ttf");
    if(fontId >= 0 && !QFontDatabase::applicationFontFamilies(fontId).isEmpty()){
        QFont font(QFontDatabase::applicationFontFamilies(fontId).first());
        font.setStyleStrategy(QFont::PreferAntialias);
        font.setPointSize(fontSize);
        font4log = font;

    }

    Widget w(font4log);
    w.show();

    return a.exec();
}
