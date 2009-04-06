/*  filename: TransProtocolData.cpp    2009/03/03  */
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

#include "TransProtocolData.h"
#include "UserItem.h"

void
ProtocolDataTransfer::run()
{
        switch (m_SocketMode)
        {
        case Slave:
                if (accept() == -1)
                {
                        return;
                }
                break;
        case Active:
                if (connectTo() == -1)
                {
                        return;
                }
                break;
        default:
                return;
        }

        switch (m_Protocol)
        {
        case DoSendFile:
                sendFile();
                break;
        case DoRecvFile:
                recvFile();
                break;
        case DoSendDir:
                sendDir();
                break;
        case DoRecvDir:
                recvDir();
                break;
        case DoSendList:
                sendList();
                break;
        case DoRecvList:
                recvList();
                break;
        }
}

int
ProtocolDataTransfer::accept()
{
        Address raddr;
        if (acceptor_.open(Address(m_hostPort)) == -1)
        {
                ACE_ERROR ((LM_ERROR,
                        ACE_TEXT ("(%P|%t) %p\n"),
                        ACE_TEXT ("Socket open failed")));
                emit sigError(tr("Socket open failed"));
                return -1;
        }
        if (acceptor_.accept(sock_, &raddr) == -1)
        {
                ACE_ERROR ((LM_ERROR,
                        ACE_TEXT ("(%P|%t) %p\n"),
                        ACE_TEXT ("Socket accept failed")));
                sock_.close();
                emit sigError(tr("Socket accept failed"));
                return -1;
        }
        return 0;
}

int
ProtocolDataTransfer::connectTo()
{
        if (connector_.connect(sock_,
                Address(m_hostPort, m_hostAddress.toAscii().data())) == -1)
        {
                ACE_ERROR ((LM_ERROR,
                        ACE_TEXT ("(%P|%t) %p\n"),
                        ACE_TEXT ("Socket connection failed")));
                emit sigError(tr("Socket connection failed"));
                return -1;
        }
        return 0;
}

void
ProtocolDataTransfer::sendFile()
{
        QString const& fName = m_fileName;
        QFile file(fName);
        if (!file.open(QIODevice::ReadOnly))
        {
                sock_.close();
                m_CtrlID = DoError;
                emit sigError(tr("Unable to read the file %1: %2")
                        .arg(fName).arg(file.errorString()));
                return;
        }

        char buf[PayloadSize];
        totalBytes = file.size();

        emit sigStart();

        while (bytesTransfered < totalBytes
                && m_CtrlID == DoStart)
        {
                int len = file.read(buf, PayloadSize);
                bytesTransfered += sock_.send_n(buf, len);
        }

        file.close();
        sock_.close();
        acceptor_.close();

        if (bytesTransfered == totalBytes)
        {
                emit sigComplete();
        }
        else if (m_CtrlID == DoCancel)
        {
                emit sigError(tr("Canceled"));
        }
        else
        {
                emit sigError(tr("Error"));
        }
}

void
ProtocolDataTransfer::recvFile()
{
        QString const& fName = m_fileName;
        QFile file(fName);
        if (!file.open(QIODevice::WriteOnly))
        {
                sock_.close();
                m_CtrlID = DoError;
                emit sigError(tr("Unable to read the file %1: %2")
                        .arg(fName).arg(file.errorString()));
                return;
        }

        char buf[PayloadSize];

        emit sigStart();

        while (bytesTransfered < totalBytes
                && m_CtrlID == DoStart)
        {
                quint64 left = totalBytes - bytesTransfered;
                int len = sock_.recv_n(buf, (left>PayloadSize)?PayloadSize:left);
                bytesTransfered += len;
                file.write(buf, len);
        }
        file.close();
        sock_.close();
        acceptor_.close();

        if (bytesTransfered == totalBytes)
        {
                m_CtrlID = DoFinish;
                emit sigComplete();
        }
        else if (m_CtrlID == DoCancel)
        {
                file.remove();
                emit sigError(tr("Canceled"));
        }
        else
        {
                file.remove();
                emit sigError(tr("Error"));
        }
}

void
ProtocolDataTransfer::sendDir()
{
        emit sigListStart();
        totalBytes = 0;
        QStringList fileList = DataAccess::GetFileList(m_dirName, &totalBytes);

        if (m_item)
        {
	        emit sigListReady(fileList);
	        m_lock.lock();
	        m_waitCondition.wait(&m_lock);
	        m_lock.unlock();
        }

        if (m_CtrlID != DoStart)
        {
                return;
        }
        QFileInfo finfo(m_dirName);
        QString const& headkey = fileList[0];
        QStringList headkeys = ParseFileInfoString(headkey);
        if (headkeys.size() < 3)
        {
                m_CtrlID = DoError;
                sock_.close();
                acceptor_.close();
                emit sigError(tr("Error occurred while parsing file information: %1.").arg(headkey));
                return;
        }

        headkeys[2] = finfo.fileName();
        fileList[0] = headkeys.join("$");

        QString const& fstr = fileList.join("/*/");
        std::string bArray = fstr.toUtf8().data();

        emit sigStart();

        // list
        int tmp = 0;
        int siz = bArray.length() + 1;
        tmp = sock_.send_n(&siz, sizeof(siz));
        if (tmp != sizeof(siz))
        {
                sock_.close();
                emit sigError(tr("Error occurred while sending file list size."));
                return;
        }

        tmp = sock_.send_n(bArray.c_str(), siz);
        if (tmp != siz)
        {
                sock_.close();
                emit sigError(tr("Error occurred while sending file list."));
                return;
        }

        fileList.removeAt(0);
        // files
        char buf[PayloadSize];
        while (m_CtrlID == DoStart)
        {
                if (fileList.isEmpty())
                {
                        m_CtrlID = DoError;
                        sock_.close();
                        emit sigError(tr("File list became empty while sending."));
                        return;
                }
                QString const& key = fileList.takeAt(0);
                QStringList keys = ParseFileInfoString(key);
                if (keys.size() < 3)
                {
                        m_CtrlID = DoError;
                        sock_.close();
                        acceptor_.close();
                        emit sigError(tr("Error occurred while parsing file information: %1.").arg(key));
                        return;
                }

                if (keys[0] == "DIR")
                {
                        if (updateDirFileItemInfo)
                        {
	                        emit sigDirFileTransOK(0, keys[2]);
                        }
                        continue;
                }
                else if (keys[0] == "FILE")
                {
                        quint64 bytesSentOfFile = 0;
                        quint64 fsize = keys[1].toULongLong();
                        QString const& fname = m_dirName + keys[2];
                        QFile file(fname);
                        if (!file.open(QIODevice::ReadOnly))
                        {
                                m_CtrlID = DoError;
                                sock_.close();
                                emit sigDirFileTransError(tr("Unable to read the file %1: %2")
                                        .arg(fname).arg(file.errorString()), keys[2]);
                                return;
                        }
                        if (updateDirFileItemInfo)
                        {
	                        emit sigDirFileTransStart(keys[2]);
                        }
                        m_currentFileName = keys[2];
                        m_currentFilePos = 0;

                        tmp = file.read(buf, PayloadSize);
                        tmp = sock_.send(buf, tmp);
                        bytesSentOfFile += tmp;

                        while (bytesSentOfFile < fsize
                                && m_CtrlID == DoStart)
                        {
                                tmp = file.read(buf, PayloadSize);
                                tmp = sock_.send(buf, tmp);
                                bytesSentOfFile += tmp;
                                m_currentFilePos = bytesSentOfFile;
                        }
                        file.close();
                        m_currentFilePos = bytesSentOfFile;
                        bytesTransfered += bytesSentOfFile;
                        if (bytesSentOfFile < fsize 
                                && m_CtrlID == DoCancel)
                        {
                                goto callend;
                        }
                        if (updateDirFileItemInfo)
                        {
	                        emit sigDirFileTransOK(fsize, keys[2]);
                        }

                        if (bytesTransfered == totalBytes)
                        {
                                m_CtrlID = DoFinish;
                                goto callend;
                        }
                }
        }
callend:
        sock_.close();
        acceptor_.close();
        if (m_CtrlID == DoFinish)
        {
                emit sigComplete();
        }
        else if (m_CtrlID == DoCancel)
        {
                emit sigError(tr("Canceled"));
        }
        else
        {
                emit sigError(tr("Error"));
        }
}

void
ProtocolDataTransfer::recvDir()
{
        char buf[PayloadSize];
        int tmp = 0;
        int siz = 0;

		emit sigListStart();

        // list
        tmp = sock_.recv_n(&siz, sizeof(int));
        if (tmp != sizeof(int))
        {
                sock_.close();
                emit sigError(tr("Error occurred while receiving file list size."));
                return;
        }
        char* listBuf = new char[siz];
        tmp = sock_.recv_n(listBuf, siz);
        if (tmp != siz)
        {
                sock_.close();
                delete listBuf;
                emit sigError(tr("Error occurred while receiving file list."));
                return;
        }
        QString const& strBuf = QString::fromUtf8(listBuf, siz);
        QStringList fileList = strBuf.split("/*/");
        if (m_item)
        {
	        emit sigListReady(fileList);
	        m_lock.lock();
	        m_waitCondition.wait(&m_lock);
	        m_lock.unlock();
        }

        if (m_CtrlID != DoStart)
        {
                delete listBuf;
                return;
        }

		emit sigStart();

        QString const& headkey = fileList.takeAt(0);
        QStringList const& headkeys = ParseFileInfoString(headkey);
        totalBytes = headkeys[1].toULongLong();

        // files
        while (m_CtrlID == DoStart)
        {
                if (fileList.isEmpty())
                {
                        m_CtrlID = DoError;
                        sock_.close();
                        delete listBuf;
                        emit sigError(tr("File list became empty while receiving."));
                        return;
                }
                QString const& key = fileList.takeAt(0);
                QStringList keys = ParseFileInfoString(key);
                if (keys.size() < 3)
                {
                        m_CtrlID = DoError;
                        sock_.close();
                        acceptor_.close();
                        delete listBuf;
                        emit sigError(tr("Error occurred while parsing file information: %1.").arg(key));
                        return;
                }

                QString saveName = m_dirName + keys[2];
                if (keys[0] == "DIR")
                {
                        QDir dir;
                        dir.mkpath(saveName);
                        if (updateDirFileItemInfo)
                        {
	                        emit sigDirFileTransOK(0, keys[2]);
                        }
                }
                else if (keys[0] == "FILE")
                {
                        quint64 bytesRecvedOfFile = 0;
                        quint64 fsize = keys[1].toULongLong();
                        QFile file(saveName);
                        if (!file.open(QIODevice::WriteOnly))
                        {
                                m_CtrlID = DoError;
                                sock_.close();
                                acceptor_.close();
                                emit sigDirFileTransError(tr("Unable to save the file %1: %2")
                                        .arg(saveName).arg(file.errorString()), keys[2]);
                                delete listBuf;
                                return;
                        }

                        if (updateDirFileItemInfo)
                        {
	                        emit sigDirFileTransStart(keys[2]);
                        }
                        m_currentFileName = keys[2];
                        m_currentFilePos = 0;
                        tmp = sock_.recv_n(buf, (fsize)>PayloadSize?PayloadSize:fsize);
                        file.write(buf, tmp);
                        bytesRecvedOfFile += tmp;

                        while (bytesRecvedOfFile < fsize
                                && m_CtrlID == DoStart)
                        {
                                siz = (fsize-bytesRecvedOfFile)>PayloadSize?PayloadSize:(fsize-bytesRecvedOfFile);
                                tmp = sock_.recv_n(buf, siz);
                                file.write(buf, tmp);
                                bytesRecvedOfFile += tmp;
                                m_currentFilePos = bytesRecvedOfFile;
                        }
                        file.close();
                        m_currentFilePos = bytesRecvedOfFile;
                        bytesTransfered += bytesRecvedOfFile;
                        if (bytesRecvedOfFile < fsize 
                                && m_CtrlID == DoCancel)
                        {
                                file.remove();
                                goto callend;
                        }
                        if (updateDirFileItemInfo)
                        {
	                        emit sigDirFileTransOK(fsize, keys[2]);
                        }

                        if (bytesTransfered == totalBytes)
                        {
                                m_CtrlID = DoFinish;
                                goto callend;
                        }
                }
        }
callend:
        sock_.close();
        acceptor_.close();
        if (m_CtrlID == DoFinish)
        {
                emit sigComplete();
        }
        else if (m_CtrlID == DoCancel)
        {
                emit sigError(tr("Canceled"));
        }
        else
        {
                emit sigError(tr("Error"));
        }
        delete listBuf;
}

void
ProtocolDataTransfer::sendList()
{
        emit sigStart();

        totalBytes = 0;
        QStringList fileList = DataAccess::GetFileList(m_dirName, &totalBytes);

        QFileInfo finfo(m_dirName);
        QString const& headkey = fileList[0];
        QStringList headkeys = ParseFileInfoString(headkey);
        headkeys[2] = finfo.fileName();
        fileList[0] = headkeys.join("$");

        QString const& fstr = fileList.join("/*/");
        QByteArray const& bArray = fstr.toUtf8();

        int tmp = 0;
        int siz = bArray.size() + 1;
        tmp = sock_.send_n(&siz, sizeof(siz));
        if (tmp != sizeof(siz))
        {
                sock_.close();
                emit sigError(tr("Error occurred while sending file list size."));
                return;
        }

        tmp = sock_.send_n(bArray.data(), siz);
        if (tmp != siz)
        {
                sock_.close();
                emit sigError(tr("Error occurred while sending file list."));

                return;
        }
        sock_.close();

        emit sigError(tr("List OK."));
}

void
ProtocolDataTransfer::recvList()
{
        emit sigStart();

        int tmp = 0;
        int siz = 0;
        tmp = sock_.recv_n(&siz, sizeof(int));
        if (tmp != sizeof(int))
        {
                sock_.close();
                emit sigError(tr("Error occurred while receiving file list size."));
                return;
        }

        char* buf = new char[siz];
        tmp = sock_.recv_n(buf, siz);
        if (tmp != siz)
        {
                sock_.close();
                emit sigError(tr("Error occurred while receiving file list."));
                return;
        }
        sock_.close();

        QByteArray bArray(buf, siz);

        QStringList fileList = QString(bArray).split("/*/");

        delete buf;
        emit sigError(tr("List OK."));
}

//////////////////////////////////////////////////////////////////////////
ProtocolDataWidget::ProtocolDataWidget(DataAccess& dataAccess, UserItem* uitem, QWidget* parent)
: QTreeWidget(parent)
, m_userItem(uitem)
, m_DataAccess(dataAccess)
{
        createActions();
        createMenus();
        this->setAlternatingRowColors(true);
        QTreeWidgetItem* headerItem = new QTreeWidgetItem;
        headerItem->setText(COL_NAME, tr("Name"));
        headerItem->setText(COL_SIZE, tr("Size"));
        headerItem->setText(COL_TYPE, tr("Type"));
        headerItem->setText(COL_PERCENT, tr("Percent"));
        headerItem->setText(COL_SPEED, tr("Speed"));
        headerItem->setText(COL_STATE, tr("State"));
        this->setHeaderItem(headerItem);
        this->setEditTriggers(QAbstractItemView::NoEditTriggers);
        this->setColumnWidth(COL_NAME, 180);    // File Name
        this->setColumnWidth(COL_SIZE, 110);    // Size
        this->setColumnWidth(COL_TYPE, 40);     // Sign
        this->setColumnWidth(COL_PERCENT, 50);  // Percent
        this->setColumnWidth(COL_SPEED, 70);    // Speed
        this->setColumnWidth(COL_STATE, 60);    // State

        setWindowTitle(tr("ProtocolData Trans Watcher"));
}

void
ProtocolDataWidget::createActions()
{
        actCancelAllItem = new QAction(QIcon(DataAccess::SkinPath() + "/cancel.png"), tr("Cancel &All Item"), this);
        connect(actCancelAllItem, SIGNAL(triggered()), this, SLOT(slotCancelAllItem()));

        actRemoveFinishedItem = new QAction(QIcon(DataAccess::SkinPath() + "/error.png"), tr("Remove &Finished Item"), this);
        connect(actRemoveFinishedItem, SIGNAL(triggered()), this, SLOT(slotRemoveFinishedItem()));

        actRemoveCanceledItem = new QAction(QIcon(DataAccess::SkinPath() + "/cancel.png"), tr("Remove &Canceled Item"), this);
        connect(actRemoveCanceledItem, SIGNAL(triggered()), this, SLOT(slotRemoveCanceledItem()));

        actRemoveErrorItem = new QAction(QIcon(DataAccess::SkinPath() + "/error.png"), tr("Remove &Error Item"), this);
        connect(actRemoveErrorItem, SIGNAL(triggered()), this, SLOT(slotRemoveErrorItem()));
}

void
ProtocolDataWidget::createMenus()
{
        menuContext = new QMenu(tr("&Context"));
        menuContext->addAction(actCancelAllItem);
        menuContext->addAction(actRemoveFinishedItem);
        menuContext->addAction(actRemoveCanceledItem);
        menuContext->addAction(actRemoveErrorItem);
        setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
ProtocolDataWidget::slotRemoveFinishedItem()
{
        QList<QTreeWidgetItem*> itemList;
        for (int i = 0; i < this->topLevelItemCount(); i++)
        {
                QTreeWidgetItem* root = this->topLevelItem(i);
                QString const& tsr = root->data(COL_NAME, TransStateRole).toString();
                if (tsr == "FINISH")
                {
                        itemList << this->topLevelItem(i);
                }
        }

        while (itemList.size() > 0)
        {
                QTreeWidgetItem* item = this->takeTopLevelItem(indexOfTopLevelItem(itemList.takeAt(0)));
                delete item;
        }
}

void
ProtocolDataWidget::slotRemoveCanceledItem()
{
        QList<QTreeWidgetItem*> itemList;
        for (int i = 0; i < this->topLevelItemCount(); i++)
        {
                QTreeWidgetItem* root = this->topLevelItem(i);
                QString const& tsr = root->data(COL_NAME, TransStateRole).toString();
                if (tsr == "CANCEL")
                {
                        itemList << this->topLevelItem(i);
                }
        }

        while (itemList.size() > 0)
        {
                QTreeWidgetItem* item = this->takeTopLevelItem(indexOfTopLevelItem(itemList.takeAt(0)));
                delete item;
        }
}

void
ProtocolDataWidget::slotRemoveErrorItem()
{
        QList<QTreeWidgetItem*> itemList;
        for (int i = 0; i < this->topLevelItemCount(); i++)
        {
                QTreeWidgetItem* root = this->topLevelItem(i);
                QString const& tsr = root->data(COL_NAME, TransStateRole).toString();
                if (tsr == "ERROR")
                {
                        itemList << this->topLevelItem(i);
                }
        }

        while (itemList.size() > 0)
        {
                QTreeWidgetItem* item = this->takeTopLevelItem(indexOfTopLevelItem(itemList.takeAt(0)));
                delete item;
        }
}

void
ProtocolDataWidget::slotCancelAllItem()
{
        for (int i = 0; i < this->topLevelItemCount(); i++)
        {
                QTreeWidgetItem* root = this->topLevelItem(i);
                this->setCurrentItem(root);
                slotCancel();
        }
        slotRemoveFinishedItem();
        slotRemoveCanceledItem();
        slotRemoveErrorItem();
}

void
ProtocolDataWidget::onRecv(Message_ptr m)
{
        Address from;
        if (From const* f = static_cast<From const*>(m->find(From::id)))
        {
                from = f->address();
        }

        if (PFileSend const* pfs = static_cast<PFileSend const*>(m->find(IMProto::FileSend)))
        {
                QString const& sn = QString::fromUtf8(pfs->property_data(IMProto::EventSn));
                QString const& kName = QString::fromUtf8(pfs->property_data(IMProto::FileKey));
                QString const& fName = QString::fromUtf8(pfs->property_data(IMProto::FileName));
                quint64 siz = QString::fromUtf8(pfs->property_data(IMProto::FileSize)).toULongLong();
                //u16 port = QString::fromUtf8(pfs->property_data(IMProto::ListenPort)).toUShort();

                QTreeWidgetItem* root = new QTreeWidgetItem;
                this->addTopLevelItem(root);
                m_RecvItemMap.insert(sn, root);
                QString const& sizstr = GetFormatedFileSize(siz);

                root->setText(COL_NAME, fName);
                root->setText(COL_SIZE, sizstr);
                root->setText(COL_TYPE, tr("File"));
                root->setText(COL_STATE, tr("waiting to receive"));
                root->setText(COL_PERCENT, "0%");
                root->setData(COL_NAME, FileKeyRole, kName);
                root->setData(COL_NAME, FileSizeRole, siz);
                root->setData(COL_NAME, DirFileRole, "FILE");
                root->setData(COL_NAME, SendRecvRole, "R");
                root->setData(COL_NAME, TransStateRole, "WAIT");
                root->setData(COL_NAME, TransItemSNRole, sn);
                root->setIcon(COL_NAME, QIcon(DataAccess::SkinPath() + "/download-file.png"));
                root->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/clock.png"));
                emit sigTips(tr("The other request to send %1:%2. Please switch to file transferring window to process.")
                        .arg(tr("File"))
                        .arg(fName));
        }
        else if (PDirFileRecv const* pfr = static_cast<PDirFileRecv const*>(m->find(IMProto::DirFileRecv)))
        {
                QString const& sn = QString::fromUtf8(pfr->property_data(IMProto::EventSn));
                QString const& kName = QString::fromUtf8(pfr->property_data(IMProto::FileKey));
                u16 port = QString::fromUtf8(pfr->property_data(IMProto::ListenPort)).toUShort();

                QTreeWidgetItem* item = m_SendItemMap.value(sn, 0);
                if (!item)
                {
                        //QMessageBox::critical(this, tr("Error"), tr("Unable to find item in file SendItemMap. key: %1.").arg(keys[2]));
                        return;
                }
                QString const& pfName = m_userItem->SendingFilesCollection()->value(kName);
                if (pfName.isEmpty())
                {
                        //QMessageBox::critical(this, tr("Error"), tr("Unable to find in SendingFilesCollection key: %1.").arg(keys[2]));
                        return;
                }
                QFileInfo finfo(pfName);

                ProtocolDataTransfer* pdthr = 0;
                if (finfo.isFile())
                {
                        pdthr = new ProtocolDataTransfer(item, pfName, 0,
                                port, m_userItem->IpAddress(),
                                ProtocolDataTransfer::DoSendFile);
                        connect(&m_timer, SIGNAL(timeout()), pdthr, SLOT(slotUpdate()));
                }
                else  if (finfo.isDir())
                {
                        pdthr = new ProtocolDataTransfer(item, pfName,
                                port, m_userItem->IpAddress(),
                                ProtocolDataTransfer::DoSendDir);
                        connect(&m_timer, SIGNAL(timeout()), pdthr, SLOT(slotUpdate()));
                }
                if (!pdthr)
                {
                        return;
                }
                item->setData(COL_NAME, TransThreadRole, (quint64)pdthr);
                if (!m_timer.isActive())
                {
                        m_timer.start(1000);
                }
                pdthr->start();
        } 
        else if (ProfileWithProperties const* pds = static_cast<ProfileWithProperties const*>(m->find(IMProto::DirSend)))
        {
                QString const& sn = QString::fromUtf8(pds->property_data(IMProto::EventSn));
                QString const& kName = QString::fromUtf8(pds->property_data(IMProto::FileKey));
                QString const& dName = QString::fromUtf8(pds->property_data(IMProto::FileName));
                quint64 siz = QString::fromUtf8(pds->property_data(IMProto::FileSize)).toULongLong();
                //u16 port = QString::fromUtf8(pfs->property_data(IMProto::ListenPort)).toUShort();

                QTreeWidgetItem* root = new QTreeWidgetItem;
                this->addTopLevelItem(root);
                m_RecvItemMap.insert(sn, root);
                QString const& sizstr = tr("unknown");

                root->setText(COL_NAME, dName);
                root->setText(COL_SIZE, sizstr);
                root->setText(COL_TYPE, tr("Dir"));
                root->setText(COL_STATE, tr("waiting to receive"));
                root->setText(COL_PERCENT, "0%");
                root->setData(COL_NAME, FileKeyRole, kName);
                root->setData(COL_NAME, FileSizeRole, siz);
                root->setData(COL_NAME, DirFileRole, "DIR");
                root->setData(COL_NAME, SendRecvRole, "R");
                root->setData(COL_NAME, TransStateRole, "WAIT");
                root->setData(COL_NAME, TransItemSNRole, sn);
                root->setIcon(COL_NAME, QIcon(DataAccess::SkinPath() + "/download-folder.png"));
                root->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/clock.png"));
                emit sigTips(tr("The other request to send %1:%2. Please switch to file transferring window to process.")
                        .arg(tr("Dir"))
                        .arg(dName));
        } 
        else if (PDirFileSendCancel const* pfsc = static_cast<PDirFileSendCancel const*>(m->find(IMProto::DirFileSendCancel)))
        {
                QString const& sn = QString::fromUtf8(pfsc->property_data(IMProto::EventSn));
                QTreeWidgetItem* item = m_RecvItemMap.value(sn);
                if (!item)
                {
                        //QMessageBox::critical(this, tr("Error"), tr("Unable to find item in file RecvItemMap. key: %1.").arg(keys[2]));
                        return;
                }
                QString tips = tr("The other canceled sending %1: %2.").arg(item->text(COL_TYPE)).arg(item->text(COL_NAME));
                emit sigTips(tips);
                m_RecvItemMap.remove(sn);

                QString const& tsr = item->data(COL_NAME, TransStateRole).toString();
                if (tsr == "START")
                {
                        ProtocolDataTransfer* t = (ProtocolDataTransfer*)item->data(COL_NAME, TransThreadRole).toULongLong();
                        disconnect(&m_timer, SIGNAL(timeout()), t, SLOT(slotUpdate()));
                        t->slotCancel();
                }

                if (!delItemWhenFinished)
                {
                        item->setData(COL_NAME, TransStateRole, "CANCEL");
                        item->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/warning.png"));
                        item->setText(COL_STATE, tr("Canceled"));
                }
                else
                {
                        this->takeTopLevelItem(this->indexOfTopLevelItem(item));
                }
        } 
        else if (ProfileWithProperties const* pfrc = static_cast<ProfileWithProperties const*>(m->find(IMProto::DirFileRecvCancel)))
        {
                QString const& sn = QString::fromUtf8(pfrc->property_data(IMProto::EventSn));
                QTreeWidgetItem* item = m_SendItemMap.value(sn, 0);
                if (!item)
                {
                        //QMessageBox::critical(this, tr("Error"), tr("Unable to find item in file SendItemMap. key: %1.").arg(keys[2]));
                        return;
                }
                QString tips = tr("The other canceled receiving %1: %2.").arg(item->text(COL_TYPE)).arg(item->text(COL_NAME));
                emit sigTips(tips);
                m_SendItemMap.remove(sn);

                QString const& tsr = item->data(COL_NAME, TransStateRole).toString();
                if (tsr == "START")
                {
                        ProtocolDataTransfer* t = (ProtocolDataTransfer*)item->data(COL_NAME, TransThreadRole).toLongLong();
                        disconnect(&m_timer, SIGNAL(timeout()), t, SLOT(slotUpdate()));
                        t->slotCancel();
                }
                if (!delItemWhenFinished)
                {
                        item->setData(COL_NAME, TransStateRole, "CANCEL");
                        item->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/warning.png"));
                        item->setText(COL_STATE, tr("Canceled"));
                }
                else
                {
                        this->takeTopLevelItem(this->indexOfTopLevelItem(item));
                }
        }
}

QTreeWidgetItem*
ProtocolDataWidget::addItem(const QString& fileInfo, const QString& sn, const QString& kName)
{
        QStringList keys = ParseFileInfoString(fileInfo);
        quint64 siz = keys[1].toULongLong();
        QString state = tr("wait sending");
        QString sizstr = GetFormatedFileSize(siz);
        QTreeWidgetItem* root = new QTreeWidgetItem;
        this->addTopLevelItem(root);
        m_SendItemMap.insert(sn, root);

        if (keys[0] == "DIR")
        {
                sizstr = tr("unknown");
        }
        root->setText(COL_NAME, keys[2]);
        root->setText(COL_SIZE, sizstr);
        root->setText(COL_TYPE, (keys[0] == "DIR")?tr("Dir"):tr("File"));
        root->setText(COL_STATE, state);
        root->setText(COL_PERCENT, "0%");
        root->setData(COL_NAME, FileKeyRole, kName);
        root->setData(COL_NAME, FileSizeRole, siz);
        root->setData(COL_NAME, DirFileRole, keys[0]);
        root->setData(COL_NAME, SendRecvRole, "S");
        root->setData(COL_NAME, TransStateRole, "WAIT");
        root->setData(COL_NAME, TransItemSNRole, sn);
        if (keys[0] == "DIR")
        {
                root->setIcon(COL_NAME, QIcon(DataAccess::SkinPath() + "/upload-folder.png"));
        }
        else
        {
                root->setIcon(COL_NAME, QIcon(DataAccess::SkinPath() + "/upload-file.png"));
        }
        root->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/clock.png"));

        return root;
}

void
ProtocolDataWidget::slotSave()
{
        QTreeWidgetItem* item = this->currentItem();
        if (!item)
        {
                QMessageBox::information(this, tr("Tips"), tr("Please choose the item that you want to operate."));
                return;
        }
        QString const& kName = item->data(COL_NAME, FileKeyRole).toString();

        if (m_userItem->SendingFilesCollection()->value(kName, "").isEmpty())
        {
                QMessageBox::critical(this, tr("Error"), tr("Unable to find  key: %1 in ReceivingFilesCollection.").arg(kName));
                return;
        }

        QString const& fType = item->data(COL_NAME, DirFileRole).toString();
        QString const& srr = item->data(COL_NAME, SendRecvRole).toString();
        QString const& state = item->data(COL_NAME, TransStateRole).toString();
        if (srr == "S")
        {
                QMessageBox::information(this, tr("Tips"), tr("You can't save a send item."));
                return;
        }
        if (state == "WAIT")
        {
                quint64 siz = item->data(0, FileSizeRole).toULongLong();
                quint16 port = DataAccess::genTcpPort();
                ProtocolDataTransfer* pdthr = 0;
                if (fType == "FILE")
                {
                        QString const &saveName = QFileDialog::getSaveFileName(this, tr("Choose Save File Name"),
                                m_DataAccess.SaveDir() + "/" + item->text(COL_NAME));
                        if (saveName.isEmpty())
                        {
                                return;
                        }
                        QFileInfo finfo(saveName);
                        m_DataAccess.updateSaveDir(finfo.absolutePath());

                        pdthr = new ProtocolDataTransfer(item, saveName, siz, port);
                        connect(&m_timer, SIGNAL(timeout()), pdthr, SLOT(slotUpdate()));
                }
                else if (fType == "DIR")
                {
                        QString dir = QFileDialog::getExistingDirectory(this,
                                tr("Choose Save Directory Name"), m_DataAccess.SaveDir());
                        if (dir.isEmpty())
                        {
                                return;
                        }
                        m_DataAccess.updateSaveDir(dir);
                        QString saveDir;
                        if (dir.endsWith("/"))
                        {
                                saveDir = dir + item->text(COL_NAME);
                        }
                        else
                        {
                                saveDir = dir + "/" + item->text(COL_NAME);
                        }
                        QDir qDir;
                        qDir.mkpath(saveDir);

                        pdthr = new ProtocolDataTransfer(item, saveDir, port);
                        connect(&m_timer, SIGNAL(timeout()), pdthr, SLOT(slotUpdate()));
                }
                if (!pdthr)
                {
                        return;
                }
                m_timer.start(1000);
                pdthr->start();

                QString const& sn = m_RecvItemMap.key(item);
                Message_ptr m(new Message);
                PDirFileRecv* pfr = new PDirFileRecv(IMProto::DirFileRecv);
                pfr->add(IMProto::ListenPort, QString::number(port).toUtf8().data(), false);
                pfr->add(IMProto::EventSn, sn.toUtf8().data(), false);
                pfr->add(IMProto::FileKey, kName.toUtf8().data(), false);
                pfr->update_size();
                m->add(Profile_ptr (pfr));
                m->add(Profile_ptr(new To(m_userItem->Addr())));
                DataAccess::sendMsg(m);

                item->setData(COL_NAME, TransThreadRole, (quint64)pdthr);
        }
}

void
ProtocolDataWidget::slotCancel()
{
        QTreeWidgetItem* item = this->currentItem();
        if (!item)
        {
                QMessageBox::information(this, tr("Tips"), tr("Please choose the item that you want to operate."));
                return;
        }
        QString const& kName = item->data(COL_NAME, FileKeyRole).toString();

        if (m_userItem->SendingFilesCollection()->value(kName, "").isEmpty())
        {
                QMessageBox::critical(this, tr("Error"), tr("Unable to find  key: %1 in SendingFilesCollection.").arg(kName));
                return;
        }

        QString const& tsr = item->data(COL_NAME, TransStateRole).toString();
        if (tsr == "START" || tsr == "WAIT")
        {
                QString const& srr = item->data(COL_NAME, SendRecvRole).toString();
                QString const& sn = item->data(COL_NAME, TransItemSNRole).toString();
                if (srr == "S")
                {
                        Message_ptr m(new Message);
                        PDirFileSendCancel* pfsc = new PDirFileSendCancel(IMProto::DirFileSendCancel);
                        pfsc->add(IMProto::EventSn, sn.toUtf8().data());
                        m->add(Profile_ptr (pfsc));
                        m->add(Profile_ptr (new To(m_userItem->Addr())));
                        DataAccess::sendMsg(m);

                        QString tips = tr("You canceled sending %1: %2.").arg(item->text(COL_TYPE)).arg(item->text(COL_NAME));
                        emit sigTips(tips);
                        m_SendItemMap.remove(sn);
                } 
                else if (srr == "R")
                {
                        Message_ptr m(new Message);
                        PDirFileRecvCancel* pfrc = new PDirFileRecvCancel(IMProto::DirFileRecvCancel);
                        pfrc->add(IMProto::EventSn, sn.toUtf8().data());
                        m->add(Profile_ptr (pfrc));
                        m->add(Profile_ptr (new To(m_userItem->Addr())));
                        DataAccess::sendMsg(m);

                        QString tips = tr("You canceled receiving %1: %2.").arg(item->text(COL_TYPE)).arg(item->text(COL_NAME));
                        emit sigTips(tips);
                        m_RecvItemMap.remove(sn);
                }
                if (tsr == "START")
                {
                        ProtocolDataTransfer* t = (ProtocolDataTransfer*)item->data(COL_NAME, TransThreadRole).toULongLong();
                        disconnect(&m_timer, SIGNAL(timeout()), t, SLOT(slotUpdate()));
                        t->slotCancel();
                }
                if (!delItemWhenFinished)
                {
                        item->setData(COL_NAME, TransStateRole, "CANCEL");
                        item->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/warning.png"));
                        item->setText(COL_STATE, tr("Canceled"));
                }
                else
                {
                        this->takeTopLevelItem(this->indexOfTopLevelItem(item));
                }
        }
}
