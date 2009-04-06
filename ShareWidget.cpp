/*  filename: ShareWidget.cpp    2009/3/27  */
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

#include "ShareWidget.h"
#include "UserItem.h"
#include "TransProtocolData.h"

SharingWidget::SharingWidget(DataAccess& daccess)
: m_DataAccess(daccess)
{
        CreateActions();
        tabWidget = new QTabWidget;
        tabWidget->addTab(CreateSharingTab(), tr("My Sharing"));
        tabWidget->addTab(CreateUploadingTab(), tr("Uploading"));
        QVBoxLayout* v = new QVBoxLayout(this);
        v->setMargin(0);
        v->addWidget(tabWidget);

        this->resize(640, 400);
        this->setWindowIcon(QIcon(DataAccess::LogosPath() + "/tray.png"));
        this->setWindowTitle(tr("Config Sharing Files and Directories"));
}

void
SharingWidget::CreateActions()
{
        actClose = new QAction(QIcon(DataAccess::SkinPath() + "/error.png"), tr("Close"), this);;
        connect(actClose, SIGNAL(triggered()), this, SLOT(close()));

        actAddFile = new QAction(QIcon(DataAccess::SkinPath() + "/add-files.png"), tr("Add File"), this);
        connect(actAddFile, SIGNAL(triggered()), this, SLOT(slotAddFile()));

        actAddDir = new QAction(QIcon(DataAccess::SkinPath() + "/add-folder.png"), tr("Add Directory"), this);
        connect(actAddDir, SIGNAL(triggered()), this, SLOT(slotAddDir()));

        actRemoveItem = new QAction(QIcon(DataAccess::SkinPath() + "/delete.png"), tr("Remove Selected"), this);
        connect(actRemoveItem, SIGNAL(triggered()), this, SLOT(slotRemoveItem()));

        actUploadCancel = new QAction(QIcon(DataAccess::SkinPath() + "/cancel.png"), tr("Cancel Selected"), this);
        connect(actUploadCancel, SIGNAL(triggered()), this, SLOT(slotUploadCancel()));
}

QWidget*
SharingWidget::CreateSharingTab()
{
        QHBoxLayout* hbutBox = new QHBoxLayout;
        hbutBox->setAlignment(Qt::AlignLeft);
        QToolButton* tBut;
        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actRemoveItem);
        hbutBox->addWidget(tBut);

        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actAddFile);
        hbutBox->addWidget(tBut);

        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actAddDir);
        hbutBox->addWidget(tBut);

        hbutBox->addStretch();
        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actClose);
        hbutBox->addWidget(tBut);

        sharingView = new FilesView(&m_DataAccess.sharingFilesMap);
        QWidget* w = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(w);
        v->setMargin(0);
        v->addWidget(sharingView);
        v->addLayout(hbutBox);
        return w;
}
QWidget*
SharingWidget::CreateUploadingTab()
{
        QHBoxLayout* hbutBox = new QHBoxLayout;
        hbutBox->setAlignment(Qt::AlignRight);
        QToolButton* tBut;
        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actUploadCancel);
        hbutBox->addWidget(tBut);

        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actClose);
        hbutBox->addWidget(tBut);

        uploadingView = new FilesView(FilesView::ViewUpload);
        QWidget* w = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(w);
        v->setMargin(0);
        v->addWidget(uploadingView);
        v->addLayout(hbutBox);
        return w;
}

void
SharingWidget::slotAddFile()
{
        QStringList const& sList = QFileDialog::getOpenFileNames(this, tr("Select Files"), m_DataAccess.OpenDir());
        if (sList.isEmpty())
        {
                return;
        }
        m_DataAccess.updateOpenDir(QFileInfo(sList[0]).path());
        for (int i = 0; i < sList.size(); i++)
        {
                QString const& siz = QString::number(QFileInfo(sList[i]).size());
                sharingView->insertSharedFiles(QString("FILE$") + siz + "$" + sList[i]);
        }
}

void
SharingWidget::slotAddDir()
{
        QString const& dName = QFileDialog::getExistingDirectory(this, tr("Select Directory"), m_DataAccess.OpenDir());
        if (dName.isEmpty())
        {
                return;
        }
        QDir dir(dName);
        if (dir.isRoot())
        {
                QMessageBox::information(this, tr("Tips"), tr("You can't share a root directory."));
                return;
        }
        m_DataAccess.updateOpenDir(QFileInfo(dName).path());

        QString const& siz = QString::number(QFileInfo(dName).size());
        sharingView->insertSharedFiles(QString("DIR$") + siz + "$" + dName);
}

void
SharingWidget::slotRemoveItem()
{
        QTreeWidgetItem* treeItem = sharingView->currentItem();
        if (treeItem)
        {
                if (sharingView->indexOfTopLevelItem(treeItem) == -1)
                {
                        QMessageBox::information(this, tr("Tips"), tr("You can't remove sub item."));
                        return;
                }

                QString const& finfoKey = treeItem->data(COL_NAME, FileKeyRole).toString();
                sharingView->removeSharedFilesByKey(finfoKey);
        }
}

void
SharingWidget::slotUploadCancel()
{
        QTreeWidgetItem* dItem = uploadingView->currentItem();
        if (!dItem)
        {
                return;
        }
        if (uploadingView->indexOfTopLevelItem(dItem) == -1)
        {
                QMessageBox::information(this, tr("Tips"), tr("You can't cancel sub item."));
                return;
        }

        if (dItem->data(COL_NAME, TransStateRole).toString() == "START")
        {
                ProtocolDataTransfer* pdthr = (ProtocolDataTransfer*)(dItem->data(COL_NAME, TransStateRole).toULongLong());
                if (pdthr)
                {

                }
                ProtocolDataTransfer* t = (ProtocolDataTransfer*)dItem->data(COL_NAME, TransThreadRole).toULongLong();
                disconnect(&m_DataAccess.m_secTimer, SIGNAL(timeout()), t, SLOT(slotUpdate()));
                t->slotCancel();

                UserItem* uItem = m_DataAccess.userItemCollection.Collection()->value(dItem->data(COL_NAME, UserItemRole).toString(), 0);
                if (uItem)
                {
                }
                QString const& gsn = dItem->data(COL_NAME, TransItemSNRole).toString();
                QStringList const& snl = gsn.split(":");
                QString const& ip = snl[0];
                QString const& sn = snl[1];
                Message_ptr m(new Message);
                PSharedDownloadCancel* psuc = new PSharedDownloadCancel(IMProto::SharedUploadCancel);
                psuc->add(IMProto::EventSn, sn.toAscii().data());
                m->add(Profile_ptr (psuc));
                m->add(Profile_ptr (new To(Address((u16)Server_Port, ip.toAscii().data()))));
                DataAccess::sendMsg(m);
                if (!delItemWhenFinished)
                {
                        dItem->setData(COL_NAME, TransStateRole, "CANCEL");
                        dItem->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/warning.png"));
                        dItem->setText(COL_STATE, tr("Canceled"));
                }
                else
                {
                        uploadingView->takeTopLevelItem(uploadingView->indexOfTopLevelItem(dItem));
                }
        }
}

//////////////////////////////////////////////////////////////////////////
SharedWatchWidget::SharedWatchWidget(DataAccess& daccess, UserItem* uItem)
: m_DataAccess(daccess)
, m_uItem(uItem)
{
        CreateActions();
        tabWidget = new QTabWidget;
        tabWidget->addTab(CreateSharedTab(), tr("His Sharing"));
        tabWidget->addTab(CreateDownloadTab(), tr("Downloading"));
        QVBoxLayout* v = new QVBoxLayout(this);
        v->setMargin(0);
        v->addWidget(tabWidget);
        statusBar = new QStatusBar;
        v->addWidget(statusBar);
        this->resize(600, 400);
        this->setWindowIcon(QIcon(m_uItem->IconName()));
        this->setWindowTitle(tr("%1's Sharing Files and Directories").arg(m_uItem->LoginName()));
}

QWidget*
SharedWatchWidget::CreateSharedTab()
{
        QHBoxLayout* hbutBox = new QHBoxLayout;
        hbutBox->setAlignment(Qt::AlignLeft);
        QToolButton* tBut;
        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actUpdateData);
        hbutBox->addWidget(tBut);

        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actDownload);
        hbutBox->addWidget(tBut);

        hbutBox->addStretch();

        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actClose);
        hbutBox->addWidget(tBut);

        sharedView = new FilesView(FilesView::ViewShared);
        QWidget* w = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(w);
        v->setMargin(0);
        v->addWidget(sharedView);
        v->addLayout(hbutBox);
        return w;
}
QWidget*
SharedWatchWidget::CreateDownloadTab()
{
        QHBoxLayout* hbutBox = new QHBoxLayout;
        hbutBox->setAlignment(Qt::AlignRight);
        QToolButton* tBut;
        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actDownloadCancel);
        hbutBox->addWidget(tBut);

        tBut = new QToolButton;
        tBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        tBut->setDefaultAction(actClose);
        hbutBox->addWidget(tBut);

        downloadView = new FilesView(FilesView::ViewDownload);
        QWidget* w = new QWidget;
        QVBoxLayout* v = new QVBoxLayout(w);
        v->setMargin(0);
        v->addWidget(downloadView);
        v->addLayout(hbutBox);
        return w;
}
void
SharedWatchWidget::CreateActions()
{
        actClose = new QAction(QIcon(DataAccess::SkinPath() + "/error.png"), tr("Close"), this);;
        connect(actClose, SIGNAL(triggered()), this, SLOT(close()));

        actDownload = new QAction(QIcon(DataAccess::SkinPath() + "/save-as.png"), tr("Download Selected"), this);
        connect(actDownload, SIGNAL(triggered()), this, SLOT(slotDownload()));

        actUpdateData = new QAction(QIcon(DataAccess::SkinPath() + "/refresh.png"), tr("Update List"), this);
        connect(actUpdateData, SIGNAL(triggered()), this, SLOT(slotUpdateData()));

        actDownloadCancel = new QAction(QIcon(DataAccess::SkinPath() + "/cancel.png"), tr("Cancel Selected"), this);
        connect(actDownloadCancel, SIGNAL(triggered()), this, SLOT(slotDownloadCancel()));
}

void
SharedWatchWidget::slotDownload()
{
        QTreeWidgetItem* treeItem = sharedView->currentItem();
        if (treeItem)
        {
                quint16 port = m_DataAccess.genTcpPort();
                QString const& sn = QString::number(port);
                QString const& finfoKey = treeItem->data(COL_NAME, FileKeyRole).toString();
                QString const& finfoVal = sharedView->DataCollection()->value(finfoKey);

                downloadView->DataCollection()->insert(finfoKey, finfoVal);
                QTreeWidgetItem* dItem = downloadView->AddItem(finfoVal);
                if (!dItem)
                {
                        return;
                }
                downloadView->ItemCollection()->insert(sn, dItem);

                QStringList const keys = ParseFileInfoString(finfoVal);

                quint64 siz = keys[1].toULongLong();

                ProtocolDataTransfer* pdthr = 0;
                QString saveName;
                if (keys[0] == "FILE")
                {
                        saveName = QFileDialog::getSaveFileName(this, tr("Choose Save File Name"),
                                m_DataAccess.SaveDir() + "/" + treeItem->text(COL_NAME));
                        if (saveName.isEmpty())
                        {
                                return;
                        }
                        QFileInfo finfo(saveName);
                        m_DataAccess.updateSaveDir(finfo.absolutePath());

                        pdthr = new ProtocolDataTransfer(dItem, saveName, siz, port);
                }
                else if (keys[0] == "DIR")
                {
                        QString dir = QFileDialog::getExistingDirectory(this,
                                tr("Choose Save Directory Name"), m_DataAccess.SaveDir());
                        if (dir.isEmpty())
                        {
                                return;
                        }
                        m_DataAccess.updateSaveDir(dir);
                        if (dir.endsWith("/"))
                        {
                                saveName = dir + treeItem->text(COL_NAME);
                        }
                        else
                        {
                                saveName = dir + "/" + treeItem->text(COL_NAME);
                        }
                        QDir qDir;
                        qDir.mkpath(saveName);

                        pdthr = new ProtocolDataTransfer(dItem, saveName, port);
                }
                if (!pdthr)
                {
                        return;
                }
                dItem->setData(COL_NAME, TransThreadRole, (quint64)pdthr);
                dItem->setData(COL_NAME, TransStateRole, "START");
                dItem->setData(COL_NAME, TransItemSNRole, sn);
                dItem->setData(COL_NAME, FileKeyRole, finfoKey);
                dItem->setData(COL_NAME, PathNameRole, saveName);
                connect(&m_DataAccess.m_secTimer, SIGNAL(timeout()), pdthr, SLOT(slotUpdate()));
                if (!m_DataAccess.m_secTimer.isActive())
                {
                        m_DataAccess.m_secTimer.start(1000);
                }
                pdthr->start();

                Message_ptr m(new Message);
                PSharedDownload* psd = new PSharedDownload(IMProto::SharedDownload);
                psd->add(IMProto::ListenPort, QString::number(port).toAscii().data(), false);
                psd->add(IMProto::EventSn, sn.toAscii().data(), false);
                psd->add(IMProto::FileKey, finfoKey.toUtf8().data(), false);
                psd->update_size();
                m->add(Profile_ptr (psd));
                m->add(Profile_ptr (new To(m_uItem->Addr())));
                DataAccess::sendMsg(m);

                statusBar->showMessage(tr("Download started. Switch to 'Downloading' page to view state"), 10000);
                tabWidget->setCurrentIndex(1);
        }
}

void
SharedWatchWidget::slotDownloadCancel()
{
        QTreeWidgetItem* dItem = downloadView->currentItem();
        if (!dItem)
        {
                return;
        }
        if (downloadView->indexOfTopLevelItem(dItem) == -1)
        {
                QMessageBox::information(this, tr("Tips"), tr("You can't cancel sub item."));
                return;
        }

        if (dItem->data(COL_NAME, TransStateRole).toString() == "START")
        {
                ProtocolDataTransfer* pdthr = (ProtocolDataTransfer*)(dItem->data(COL_NAME, TransStateRole).toULongLong());
                if (pdthr)
                {

                }
                ProtocolDataTransfer* t = (ProtocolDataTransfer*)dItem->data(COL_NAME, TransThreadRole).toULongLong();
                disconnect(&m_DataAccess.m_secTimer, SIGNAL(timeout()), t, SLOT(slotUpdate()));
                t->slotCancel();

                Message_ptr m(new Message);
                PSharedDownloadCancel* psdc = new PSharedDownloadCancel(IMProto::SharedDownloadCancel);
                psdc->add(IMProto::EventSn, dItem->data(COL_NAME, TransItemSNRole).toString().toAscii().data());
                m->add(Profile_ptr (psdc));
                m->add(Profile_ptr (new To(m_uItem->Addr())));
                DataAccess::sendMsg(m);

                statusBar->showMessage(tr("Download canceled(%1:%2).").arg(dItem->text(COL_TYPE)).arg(dItem->text(COL_NAME)), 10000);
                if (!delItemWhenFinished)
                {
                        dItem->setData(COL_NAME, TransStateRole, "CANCEL");
                        dItem->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/warning.png"));
                        dItem->setText(COL_STATE, tr("Canceled"));
                }
                else
                {
                        downloadView->takeTopLevelItem(downloadView->indexOfTopLevelItem(dItem));
                }
        }
}
void
SharedWatchWidget::slotUpdateData()
{
        Message_ptr m(new Message);
        PSharedList* psl = new PSharedList(IMProto::SharedList);
        m->add(Profile_ptr (psl));
        m->add(Profile_ptr (new To(m_uItem->Addr())));
        DataAccess::sendMsg(m);
        statusBar->showMessage(tr("Updating List"), 10000);
}

