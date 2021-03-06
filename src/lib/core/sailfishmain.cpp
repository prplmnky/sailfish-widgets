/***************************************************************************
** This file is part of SailfishWidgets
**
** Copyright (c) 2015 Dametrious Peyton
**
** $QT_BEGIN_LICENSE:GPLV3$
** SailfishWidgets is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** SailfishWidgets is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with SailfishWidgets.  If not, see <http://www.gnu.org/licenses/>.
** $QT_END_LICENSE$
**
**************************************************************************/

#include "sailfishmain.h"
#include <applicationsettings.h>

#include <QDebug>
#include <QGuiApplication>
#include <QQuickView>
#include <QTranslator>
#include <QQmlEngine>

/*!
   \namespace SailfishMain
   \since 5.2
   \brief The SailfishMain namespace

   \inmodule Core

   This namespace supports the quick construction of SailfishApps.

   Usage of this library requires that you define the following in the RPM spec file's \code >> macros ... << macros \endcode section.
   \code
   %define __requires_exclude ^libapplicationsettings|libcore.*$
   \endcode

   \c {SailfishMain} namespace has the following functions

   Back to \l {Sailfish Widgets}
 */

/*!
 \fn int SailfishMain::main(int argc, char *argv[], const QString& appName, const QString& settingsFile, const QString& localeSetting)

 Constructs a Sailfish application using \a appName and \a settingsFile to load the application settings with a given \a localeSetting property.
 If no locale was discovered, the default translation bundle is loaded using \a appName .qm file.

 Finally, a Sailfish application is constructed using the \a argc and \a argv parameters.
 */
int SailfishMain::main(int argc, char *argv[], const QString& appName, const QString& settingsFile, const QString& localeSetting) {
    if(!appName.isEmpty() && !settingsFile.isEmpty()) {
        ApplicationSettings settings(appName, settingsFile);
        qDebug() << settings.isValid(localeSetting);
        qDebug() << settings.applicationName();
        qDebug() << settings.fileName();
        settings.refresh();
        QGuiApplication* app(SailfishApp::application(argc, argv));

        qDebug() << SailfishApp::pathTo("lib").toString();
        app->addLibraryPath("/usr/share/" + appName + "/lib");
        foreach (const QString &path, app->libraryPaths())
            qDebug() << path;

        //Set the library path to the 'lib' dir of the application
        //Jolla specifies that shared libs go there

        installLanguage(appName, settings.isValid(localeSetting) ? settings.value(localeSetting).toString() : "", app);

        //Start the app
        QQuickView* view(SailfishApp::createView());
        // This is needed to import custom QML Plugins/Modules
        view->engine()->addImportPath("/usr/share/" + appName);
        view->engine()->addImportPath("/usr/share/" + appName + "/lib");
        view->engine()->addPluginPath("/usr/share/" + appName + "/lib");
        foreach (const QString& path, view->engine()->pluginPathList())
            qDebug() << "Plugin path: " << path;
        view->setSource(SailfishApp::pathTo("qml/" + appName + ".qml"));
        view->show();
        return app->exec();
    }

    return SailfishApp::main(argc, argv);
}

/*!
  \fn bool SailfishMain::installLanguage(const QString& appName, const QString& locale, QGuiApplication* app)

 Installs the \a locale for the given \a appName of the application \a app.

 Returns true if successful.
 */
bool SailfishMain::installLanguage(const QString& appName, const QString& locale, QGuiApplication* app) {
    QTranslator* translator(new QTranslator(app));
    //TODO: link to liblanguage for default locale
    QString qm = appName + (locale.isEmpty() || locale == "app" ? ".qm" : ("-" + locale + ".qm"));
    QString path = SailfishApp::pathTo(QString("translations")).toLocalFile();
    qDebug() << "qm: " << qm;
    qDebug() << "path: " << path;
    if(translator->load(qm, path)) {
        bool result = app->installTranslator(translator);
        qDebug() << "app loaded " << qm << result;
        return result;
    }
    qDebug() << "didn't load translator file " << qm;
    qDebug() << "loaded default locale";
    return false;
}
