/***************************************************************************
** This file is part of SailfishWidgets
**
** Copyright (c) 2014 Dametrious Peyton
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

#include "dir.h"

/*!
   \class Dir
   \since 5.0
   \brief The Dir class

   \inmodule FileManagement

   This class will expose \c QDir functionality to QML.

   An example usage is as follows.
   \qml
   Dir {
        id: fileList
        filter: Dir.AllEntries | Dir.Hidden
        sort: Dir.DirsFirst | Dir.Name

        onPathChanged: refresh()
    }
   \endqml

   It does not support file manipulation (creation, reading, writing, delete). This class may be extended in the future as needed.

   Back to \l {Sailfish Widgets}

 */

/*!
   \enum Dir::DirFilter

   Provides the filters available for file restrictions.

   \value Dirs
   \value Files
   \value          Drives
   \value         NoSymLinks
   \value         AllEntries
   \value        TypeMask
   \value        Readable
   \value       Writable
   \value       Executable
   \value       PermissionMask
   \value       Modified
   \value      Hidden
   \value      System
   \value     AccessMask
   \value      AllDirs
   \value      CaseSensitive
   \value     NoDot
   \value       NoDotDot
   \value NoDotAndDotDot
   \value NoFilter
 */


/*!
   \enum Dir::DirSortFlag

   Provides the filters available for file sorting.

   \value Name
   \value Time
   \value Size
   \value Unsorted
   \value SortByMask
   \value DirsFirst
   \value Reversed
   \value IgnoreCase
   \value DirsLast
   \value LocaleAware
   \value Type
   \value NoSort
 */


const QString Dir::m_configDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
const QString Dir::m_dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
const QString Dir::m_cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
const QString Dir::m_homeDir = QDir::homePath();

/*!
 \fn Dir::Dir(QObject *parent)
 Constructs a new \l {Dir} with the given \c QObject \a parent
 */
Dir::Dir(QObject *parent) :
    QObject(parent),
    QDir(),
    m_fileList(QQmlListProperty<File>(this, 0,
                                      &Dir::dclAppendObject,
                                      &Dir::dclCountObject,
                                      &Dir::dclAtIndex,
                                      &Dir::dclClearObject))
{

}

/*!
 \fn Dir::dirName() const

 Returns the directory name. Unlike \c QDir::dirName() it will convert the root path to \c / .
 */
QString Dir::dirName() const { return isRoot() ? root().path() : QDir::dirName(); }

/*!
 \fn QString Dir::XdgCache() const

 Returns the XDG Cache directory (usu. /home/nemo/.cache/yourApp)
 */
QString Dir::XdgCache() const {
    return Dir::m_cacheDir;
}

/*!
 \fn QString Dir::XdgConfig() const

 Returns the XDG Config directory (usu. /home/nemo/.config/yourApp)
 */
QString Dir::XdgConfig() const {
    return Dir::m_configDir;
}

/*!
 \fn QString Dir::XdgData() const

 Returns the XDG Data directory (usu. /home/nemo/.local/share/yourApp)
 */
QString Dir::XdgData() const {
    return Dir::m_dataDir;
}

/*!
 \fn QString Dir::XdgHome() const

 Returns the XDG Home directory (usu. /home/nemo). Equivalent to \c QDir::homePath()
 */
QString Dir::XdgHome() const {
    return Dir::m_homeDir;
}

/*!
 \fn Dir::entryList()
 Returns the files in this \l {Dir} object's \c Dir::path(). It provides this object's
 \c filter and \c sort properties as arguments to \c QDir::entryList().

 This applies the following \c DirFilter enums (always).

 \list
   \li \c QDir::Readable
   \li \c QDir::NoDot
 \endlist

 */
QStringList Dir::entryList() {
    return QDir::entryList(Filters(m_filter) | QDir::Readable | QDir::NoDot, SortFlags(m_sort));
}

/*!
 \fn Dir::files()
 Returns the files in this directory as \l {File} objects.

 If the internal file list is unpopulated, a call to this method will populate it.

 It will return the same content as \c entryList().

 \sa Dir::entryList()
 */
QQmlListProperty<File> Dir::files() {
    if(getList().isEmpty())
        foreach(QString s, entryList())
            getList().append(new File(absoluteFilePath(s), this));

    return m_fileList;
}

/*!
   \fn int Dir::filter() const

   Returns the internal file list filter.
 */
int Dir::filter() const { return m_filter; }

/*!
 \fn void Dir::refresh()

 It will clear the current list and call files().
 Afterwords it will emit the filesChanged() method.
 */
void Dir::refresh() {
    clearList();
    files();
    emit filesChanged();
}

/*!
 \fn Dir::setFilter(int filter)

 It set the current \a filter.
 Afterwords it will emit the \c filterChanged() method.

 \sa Dir::filterChanged()
 */
void Dir::setFilter(int filter) {
    m_filter = filter;
    emit filterChanged();
}

/*!
 \fn Dir::setPath(const QString &p)

 It will set the current path to \a p after running QDir::absolutePath().

 To workaround an issue with indefinate /.. when navigating code, it will
 force references of /.. to the root directory.

 Afterwords it will emit the pathChanged() method.
 */
void Dir::setPath(const QString &p) {
    QDir nPath(p);
    //Weird workaround for inifinite /../..
    QDir::setPath(nPath.path() == "/.." ? "/" : nPath.absolutePath());
    emit pathChanged();
}

/*!
 \fn Dir::setSort(int sort)

 It will set this object's \a sort method
 Afterwords it will emit the \c sortChanged() method.

 \sa Dir::sortChanged()
 */
void Dir::setSort(int sort) {
    m_sort = sort;
    emit sortChanged();
}

/*!
  \fn Dir::sort() const
  \brief Gets the current sort method.

 */
int Dir::sort() const { return m_sort; }


/*!
 \fn void Dir::filterChanged()
 Emitted when the \c filter is udpated.
 */

/*!
 \fn void Dir::filesChanged()
 Emitted when the internal file list is udpated.
 */

/*!
 \fn void Dir::sortChanged()
 Emitted when the \c sort is udpated.
 */


/*!
 \fn void Dir::pathChanged()
 Emitted when the \c path is updated.
 */
