#include <qapplication.h>

#include <easy_translate.hpp>

#include "mainwindow.h"

#define APP_LANGUAGES_FILENAME "./language/language.json"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    easytr::setLanguages(APP_LANGUAGES_FILENAME);

    int res = a.exec();

    return res;
}
