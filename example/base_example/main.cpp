#include <qapplication.h>

#include <easy_translate.hpp>

#include "mainwidget.h"
#include "language.h"

#define APP_LANGUAGES_FILENAME "./language/language.json"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    easytr::setLanguages(APP_LANGUAGES_FILENAME);
    setLanguage(currentLanguage());

    MainWidget w;
    w.show();

    int res = a.exec();

    easytr::updateTranslationsFiles();

    return res;
}
