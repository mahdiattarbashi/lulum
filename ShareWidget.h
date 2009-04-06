/*  filename: ShareWidget.h    2009/3/27  */
/*************************************************************************
    LuLu Messenger: A LAN Instant Messenger For Chatting and File Exchanging.
    Copyright (C) 2008,2009  Wu Weisheng <wwssir@gmail.com>

    This file is part of LuLu Messenger.

    LuLu Messenger is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LuLu Messenger is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*************************************************************************/
/*************************************************************************
    LuLu 信使: 局域网络即时对话与文件交换通讯工具。
    著作权所有 (C) 2008,2009  武维生 <wwssir@gmail.com>

    此文件是 LuLu 信使源文件的一部分。

    LuLu 信使为自由软件；您可依据自由软件基金会所发表的GNU通用公共授权条款，
    对本程序再次发布和/或修改；无论您依据的是本授权的第三版，或（您可选的）
    任一日后发行的版本。

    LuLu 信使是基于使用目的而加以发布，然而不负任何担保责任；亦无对适售性或
    特定目的适用性所为的默示性担保。详情请参照GNU通用公共授权。

    您应已收到附随于本程序的GNU通用公共授权的副本；如果没有，请参照
    <http://www.gnu.org/licenses/>.
*************************************************************************/

// 说明：    
// 修改：    

#ifndef __ShareWidget_h__
#define __ShareWidget_h__

#include "utils.h"
#include "DataAccess.h"

#define COL_NAME        0
#define COL_SIZE        1
#define COL_TYPE        2
#define COL_SPEED       3
#define COL_PERCENT     4
#define COL_STATE       5

#define COL_LOCAL       3
#define COL_TARGET      6

class FilesView : public QTreeWidget
{
        Q_OBJECT
signals:
        void sigTips(const QString&);

public:
        enum eType
        {
                ViewShared,
                ViewDownload,
                ViewUpload
        };
        FilesView(StringMapType* filesMap)
                : m_dataCollection(filesMap)
                , m_isSelfData(m_dataCollection?false:true)
        {
                if (!m_dataCollection)
                {
                        m_dataCollection = new StringMapType;
                }
                TreeItemType* headerItem = new TreeItemType;
                headerItem->setText(COL_NAME, tr("Name"));
                headerItem->setText(COL_SIZE, tr("Size"));
                headerItem->setText(COL_TYPE, tr("Type"));
                headerItem->setText(COL_LOCAL, tr("Local"));
                this->setHeaderItem(headerItem);
                this->setColumnWidth(COL_NAME, 240);   // File Name
                this->setColumnWidth(COL_SIZE, 80);    // Size
                this->setColumnWidth(COL_TYPE, 40);    // Type
                this->setColumnWidth(COL_LOCAL, 200);    // LOCAL

                this->setIndentation(0);

                UpdateItems();
                this->setSortingEnabled(true);
        }

        FilesView(int typ = ViewShared)
                : m_isSelfData(true)
        {
                m_dataCollection = new StringMapType;

                TreeItemType* headerItem = new TreeItemType;
                headerItem->setText(COL_NAME, tr("Name"));
                headerItem->setText(COL_SIZE, tr("Size"));
                headerItem->setText(COL_TYPE, tr("Type"));
                this->setHeaderItem(headerItem);
                this->setColumnWidth(COL_NAME, 160);   // File Name
                this->setColumnWidth(COL_SIZE, 120);    // Size
                this->setColumnWidth(COL_TYPE, 40);    // Type
                if (typ == ViewDownload
                        || typ == ViewUpload)
                {
                        connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(slotItemDBClicked(QTreeWidgetItem*, int)));

                        headerItem->setText(COL_PERCENT, tr("Percent"));
                        headerItem->setText(COL_SPEED, tr("Speed"));
                        headerItem->setText(COL_STATE, tr("State"));

                        this->setColumnWidth(COL_PERCENT, 50);    // Percent
	                this->setColumnWidth(COL_SPEED, 70);    // Speed
                        this->setColumnWidth(COL_STATE, 90);   // State
                }
                if (typ == ViewUpload)
                {
                        headerItem->setText(COL_TARGET, tr("Remote Peer"));
                        this->setColumnWidth(COL_TARGET, 120);   // Remote Peer
                }

                this->setIndentation(0);

                UpdateItems();
                this->setSortingEnabled(true);
        }

        virtual ~FilesView()
        {
                if (m_isSelfData)
                {
                        delete m_dataCollection;
                }
        }

public slots:
        void UpdateItems()
        {
                this->clear();
                QStringList const& vList = m_dataCollection->values();
                for (int i = 0; i < vList.size(); i++)
                {
                        AddItem(vList[i]);
                }
        }
        void slotItemDBClicked(QTreeWidgetItem* item, int /*column*/)
        {
                if (this->indexOfTopLevelItem(item) != -1
                        && item->data(COL_NAME, TransStateRole).toString() == "FINISH")
                {
                        QString name = item->data(COL_NAME, PathNameRole).toString();
                        QDesktopServices::openUrl(QUrl::fromLocalFile(name));
                }
        }

public:
        TreeItemType* AddItem(const QString &finfoVal)
        {
                QStringList const& finfoVals = ParseFileInfoString(finfoVal);
                QString const& finfoKey = m_dataCollection->key(finfoVal);
                QString const& siz = GetFormatedFileSize(finfoVals[1].toULongLong());
                TreeItemType* treeItem = new TreeItemType;
                if (!treeItem)
                {
                        return 0;
                }
                if (finfoVals[0] == "DIR")
                {
                        treeItem->setIcon(COL_NAME, QIcon(DataAccess::SkinPath() + "/folder.png"));
                }
                else
                {
                        treeItem->setIcon(COL_NAME, QIcon(DataAccess::SkinPath() + "/file.png"));
                }
                treeItem->setText(COL_NAME, finfoKey);
                treeItem->setText(COL_SIZE, (finfoVals[0] == "DIR")?"":siz);
                treeItem->setText(COL_TYPE, (finfoVals[0] == "DIR")?tr("Dir"):tr("File"));
                if (!m_isSelfData)
                {
	                treeItem->setText(COL_LOCAL, finfoVals[2]);
                        treeItem->setToolTip(COL_LOCAL, finfoVals[2]);
                }
                treeItem->setData(COL_NAME, FileKeyRole, finfoKey);
                this->addTopLevelItem(treeItem);
                m_itemCollection.insert(finfoKey, treeItem);

                return treeItem;
        }

        QString insertSharedFiles(const QString &finfoVal)
        {
                QStringList const& finfoVals = ParseFileInfoString(finfoVal);
                QString const& pathname = finfoVals[2];

                QFileInfo finfo(pathname);
                bool alreadyExist;
                QString finfoKey = DataAccess::genFileKey(*m_dataCollection, pathname, alreadyExist);
                if (!alreadyExist)
                {
                        m_dataCollection->insert(finfoKey, finfoVal);
                        AddItem(finfoVal);
                }
                else
                {
                        QMessageBox::warning(this, tr("Warning"), tr("The item already exists: %1->%2.").arg(finfoKey).arg(pathname));
                }
                return finfoKey;
        }

        QString insertRemoteItem(const QString &finfoVal)
        {
                QStringList const& finfoVals = ParseFileInfoString(finfoVal);
                QString const& kName = finfoVals[2];

                QFileInfo finfo(kName);
                QString finfoKey = finfoVals[2];
                if (!m_dataCollection->contains(kName))
                {
                        m_dataCollection->insert(kName, finfoVal);
                        AddItem(finfoVal);
                }
                else
                {
                        QMessageBox::warning(this, tr("Warning"), tr("The item already exists: %1->%2.").arg(kName).arg(kName));
                }
                return kName;
        }

        void removeSharedFilesByValue(const QString &finfoVal)
        {
                QString finfoKey = m_dataCollection->key(finfoVal, "");
                if (finfoKey != "")
                {
                        removeSharedFilesByKey(finfoKey);
                }
        }

        void removeSharedFilesByKey(const QString &finfoKey)
        {
                TreeItemType* treeItem = this->takeTopLevelItem(this->indexOfTopLevelItem(m_itemCollection.value(finfoKey, 0)));
                if (treeItem)
                {
                        delete treeItem;
                }
                m_dataCollection->remove(finfoKey);
                m_itemCollection.remove(finfoKey);
        }

        StringMapType* DataCollection() const { return m_dataCollection; }
        TreeItemMapType* ItemCollection() { return &m_itemCollection; }

private:
        QMenu* menuContext;
        StringMapType* m_dataCollection;
        TreeItemMapType m_itemCollection;
        bool m_isSelfData;
};

class SharingWidget : public QFrame
{
        Q_OBJECT
public:
        SharingWidget(DataAccess& daccess);

public slots:
        void slotAddFile();
        void slotAddDir();
        void slotRemoveItem();
        void slotUploadCancel();

public:
        FilesView* SharingView() { return sharingView; }
        FilesView* UploadingView() { return uploadingView; }

protected:
        void CreateActions();
        QWidget* CreateSharingTab();
        QWidget* CreateUploadingTab();

private:
        virtual void closeEvent( QCloseEvent* event)
        {
                this->hide();
                event->ignore();
        }
        QAction* actAddFile;
        QAction* actAddDir;
        QAction* actRemoveItem;
        QAction* actClose;
        QAction* actUploadCancel;

        QTabWidget* tabWidget;
        FilesView* sharingView;
        FilesView* uploadingView;
        DataAccess& m_DataAccess;
};

class SharedWatchWidget : public QFrame
{
        Q_OBJECT

public:
        SharedWatchWidget(DataAccess& daccess, UserItem* uItem);
public:
        FilesView* SharedView() { return sharedView; }
        FilesView* DownloadView() { return downloadView; }
        QStatusBar* StatusBar() const { return statusBar; }

public slots:
        void slotDownload();
        void slotDownloadCancel();
        void slotUpdateData();

protected:
        virtual void closeEvent( QCloseEvent* event)
        {
                this->hide();
                event->ignore();
        }

        void CreateActions();
        QWidget* CreateSharedTab();
        QWidget* CreateDownloadTab();

private:
        QAction* actDownload;
        QAction* actUpdateData;
        QAction* actDownloadCancel;
        QAction* actClose;
        QStatusBar* statusBar;

        QTabWidget* tabWidget;
        FilesView* sharedView;
        FilesView* downloadView;

        UserItem* m_uItem;
        DataAccess& m_DataAccess;
};
#endif // __ShareWidget_h__
