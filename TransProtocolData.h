/*  filename: TransProtocolData.h    2009/03/03  */
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

#ifndef __TransProtocolData_h__
#define __TransProtocolData_h__

#include "Trans.h"
#include "utils.h"
#include "DataAccess.h"

#define COL_NAME        0
#define COL_SIZE        1
#define COL_TYPE        2
#define COL_SPEED       3
#define COL_PERCENT     4
#define COL_REJECT      5
#define COL_ACCEPT      6
#define COL_STATE       7

class ProtocolDataTransfer : public Transfer
{
        Q_OBJECT

signals:
        void sigError(const QString&);
        void sigTips(const QString&);
		void sigStart();
		void sigListStart();
        void sigComplete();

        void sigListReady(const QStringList &);
        void sigDirFileTransStart(const QString& file);
        void sigDirFileTransOK(const quint64 siz, const QString& file);
        void sigDirFileTransError(const QString& err, const QString& file);

public:
        ProtocolDataTransfer(QTreeWidgetItem* item,
                const QString &fileName, const quint64 siz,
                const quint16 port, const QString &address = "",
                int pid = ProtocolDataTransfer::DoRecvFile, QObject *parent = 0)
                : Transfer(siz, port, address, parent)
                , m_item(item)
                , m_Protocol(pid)
                , m_fileName(fileName)
                , m_CtrlID(ProtocolDataTransfer::DoStart)
                , m_histBytes(0)
                , m_currentFileName("")
                , m_currentFilePos(0)
                , m_histFilePos(0)
        {
                m_SocketMode = (pid == DoRecvDir || pid == DoRecvFile || pid == DoRecvList)?Slave:Active;
                if (item)
                {
                        QTreeWidget* tw = m_item->treeWidget();
                        connect(this, SIGNAL(sigTips(const QString&)), tw, SIGNAL(sigTips(const QString&)));

                        connect(this, SIGNAL(sigError(const QString&)), this, SLOT(slotError(const QString&)));
                        connect(this, SIGNAL(sigStart()), this, SLOT(slotStart()));
                        connect(this, SIGNAL(sigComplete()), this, SLOT(slotComplete()));
                }
        }
        ProtocolDataTransfer(QTreeWidgetItem* item,
                const QString &dir,
                const quint16 port, const QString &address = "",
                int pid = ProtocolDataTransfer::DoRecvDir, QObject *parent = 0)
                : Transfer(0, port, address, parent)
                , m_item(item)
                , m_Protocol(pid)
                , m_dirName(dir)
                , m_CtrlID(ProtocolDataTransfer::DoStart)
                , m_histBytes(0)
                , m_currentFileName("")
                , m_currentFilePos(0)
                , m_histFilePos(0)
        {
                m_SocketMode = (pid == DoRecvDir || pid == DoRecvFile || pid == DoRecvList)?Slave:Active;

                if (item)
                {
                        QTreeWidget* tw = m_item->treeWidget();
                        connect(this, SIGNAL(sigTips(const QString&)), tw, SIGNAL(sigTips(const QString&)));

                        connect(this, SIGNAL(sigError(const QString&)), this, SLOT(slotError(const QString&)));
						connect(this, SIGNAL(sigStart()), this, SLOT(slotStart()));
						connect(this, SIGNAL(sigListStart()), this, SLOT(slotListStart()));
                        connect(this, SIGNAL(sigComplete()), this, SLOT(slotComplete()));
	
                        connect(this, SIGNAL(sigListReady(const QStringList &)), this, SLOT(slotAddList(const QStringList &)));
                        connect(this, SIGNAL(sigDirFileTransStart(const QString&)),
	                            this, SLOT(slotDirFileTransStart(const QString&)));
                        connect(this, SIGNAL(sigDirFileTransOK(const quint64, const QString&)),
	                            this, SLOT(slotDirFileTransOK(const quint64, const QString&)));
                        connect(this, SIGNAL(sigDirFileTransError(const QString&, const QString&)),
	                            this, SLOT(slotDirFileTransError(const QString&, const QString&)));
                }
        }
        virtual ~ProtocolDataTransfer()
        {
        }

        QString FileName() const { return m_fileName; }
        QString DirName() const { return m_dirName; }
        int CtrlID() const { return m_CtrlID; }
        int Protocol() const { return m_Protocol; }

public:
        enum enumCtrl
        {
                DoInit,
                DoStart,
                DoCancel,
                DoFinish,
                DoError
        };

        enum enumProtocol
        {
                DoSendFile,
                DoRecvFile,
                DoSendList,
                DoRecvList,
                DoSendDir,
                DoRecvDir
        };

        enum enumSocketMode
        {
                Active = 1,
                Slave
        };
public slots:
        void slotCancel();
        void slotUpdate();
        void slotError(const QString&);
		void slotStart();
		void slotListStart();
        void slotComplete();
        void slotAddList(const QStringList &fileList);
        void slotRemoveItem();

        void slotDirFileTransStart(const QString& file);
        void slotDirFileTransOK(const quint64 siz, const QString& file);
        void slotDirFileTransError(const QString& err, const QString& file);
protected:
        int accept();
        int connectTo();
        void run();
        void sendFile();
        void recvFile();
        void sendDir();
        void recvDir();
        void sendList();
        void recvList();

        QTreeWidgetItem* findItemChild(const QString& file);
protected:
        int m_Protocol;
        int m_SocketMode;
        int m_CtrlID;
        QString m_fileName;
        QString m_dirName;

        QTreeWidgetItem* m_item;
        ACE_SOCK_Acceptor acceptor_;
        ACE_SOCK_Connector connector_;
        ACE_SOCK_Stream sock_;

        QMutex m_lock;
        QWaitCondition m_waitCondition;

        QTime m_totalTime;
        QTime m_histTime;
        quint64 m_histBytes;
        QString m_currentFileName;
        quint64 m_currentFilePos;
        quint64 m_histFilePos;
};

inline void
ProtocolDataTransfer::slotCancel()
{
        disconnect(this, 0, 0, 0);
        m_CtrlID = DoCancel;

        acceptor_.close();
        sock_.close();
        m_lock.lock();
        m_waitCondition.wakeAll();
        m_lock.unlock();
        if (!m_item)
        {
                return;
        }
        int msec = m_totalTime.elapsed();
        quint64 speedBytes = (int)((totalBytes*1000)/(double)msec);
        QString const& speed = GetFormatedTransSpeed(speedBytes);
        m_item->setText(COL_SPEED, speed);
        m_item->setText(COL_SIZE, GetFormatedFileSize(bytesTransfered + m_currentFilePos) + "/" + GetFormatedFileSize(totalBytes));
        m_item->setText(COL_PERCENT, QString::number((bytesTransfered + m_currentFilePos)*100.0/totalBytes, 'f', 2) + "%");
//         slotRemoveItem();
}

inline void
ProtocolDataTransfer::slotError(const QString& state)
{
        disconnect(this, 0, 0, 0);
        if (!m_item)
        {
                return;
        }
        m_item->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/error.png"));
        m_item->setText(COL_STATE, state);

        if (DoRecvFile == m_Protocol
                || DoRecvDir == m_Protocol)
        {
	        QString tips = tr("Error occurred while receiving %1: %2.")
	                .arg(m_item->text(COL_TYPE))
	                .arg(m_item->text(COL_NAME));
	        emit sigTips(tips);
        }
        m_item->setData(COL_NAME, TransStateRole, "ERROR");

//         slotRemoveItem();
}

inline void
ProtocolDataTransfer::slotStart()
{
        if (!m_item)
        {
                return;
        }
        m_histTime.restart();
        m_totalTime.restart();
        if (DoRecvFile == m_Protocol
                || DoRecvDir == m_Protocol)
        {
	        m_item->setText(COL_STATE, tr("Receiving"));
                m_item->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/receiving.png"));
        }
        else
        {
                m_item->setText(COL_STATE, tr("Sending"));
                m_item->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/sending.png"));
        }
        m_item->setData(COL_NAME, TransStateRole, "START");
}

inline void
ProtocolDataTransfer::slotListStart()
{
        if (DoRecvFile == m_Protocol
                || DoRecvDir == m_Protocol)
        {
                m_item->setText(COL_STATE, tr("Listing"));
                m_item->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/receiving.png"));
        }
        else
        {
                m_item->setText(COL_STATE, tr("Listing"));
                m_item->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/sending.png"));
        }
        m_item->setData(COL_NAME, TransStateRole, "START");
}

inline void
ProtocolDataTransfer::slotComplete()
{

        if (!m_item)
        {
                return;
        }
        if (!delItemWhenFinished)
        {
                int msec = m_totalTime.elapsed();
                quint64 speedBytes = (int)((totalBytes*1000)/(double)(msec?msec:1));
                QString const& speed = GetFormatedTransSpeed(speedBytes);
                m_item->setText(COL_SPEED, speed);
                m_item->setText(COL_SIZE, GetFormatedFileSize(bytesTransfered) + "/" + GetFormatedFileSize(totalBytes));
                m_item->setText(COL_PERCENT, "100%");
                m_item->setIcon(COL_REJECT, QIcon());
                m_item->setText(COL_REJECT, "");
                m_item->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/ok.png"));
                m_item->setText(COL_STATE, tr("Finished"));
                m_item->setData(COL_NAME, TransStateRole, "FINISH");
        }

        if (DoRecvFile == m_Protocol
                || DoRecvDir == m_Protocol)
        {
                QString name = (m_fileName == "" ? m_dirName: m_fileName);
                QString nameLabel = "<a href=\"" + name + "\">" + name + "</a>";

                QString tips = tr("Receiving %1: %2 has finished. Saved to %3.")
                        .arg(m_item->text(COL_TYPE))
                        .arg(m_item->text(COL_NAME))
                        .arg(nameLabel);
                emit sigTips(tips);
        }
        else
        {
                QString tips = tr("Sending %1: %2 has finished.")
                        .arg(m_item->text(COL_TYPE))
                        .arg(m_item->text(COL_NAME));
                emit sigTips(tips);
        }
        if (delItemWhenFinished)
        {
                disconnect(this, 0, 0, 0);
                slotRemoveItem();
        }
}

inline void
ProtocolDataTransfer::slotUpdate()
{
        if (!m_item)
        {
                return;
        }
        int msec = m_histTime.elapsed();
        quint64 currBytes = bytesTransfered + m_currentFilePos;
        quint64 bytes = currBytes - m_histBytes;
        quint64 speedBytes = (int)((bytes*1000)/(double)msec);
        QString const& speed = GetFormatedTransSpeed(speedBytes);
        m_item->setText(COL_SPEED, speed);
        m_item->setText(COL_PERCENT, QString::number(currBytes*100.0/totalBytes, 'f', 2) + "%");
        m_item->setText(COL_SIZE, GetFormatedFileSize(currBytes) + "/" + GetFormatedFileSize(totalBytes));
        m_histBytes = currBytes;

        if (!m_currentFileName.isEmpty()
                && updateDirFileItemInfo)
        {
                QTreeWidgetItem* chld = findItemChild(m_currentFileName);
                if (!chld)
                {
                        return;
                }
                quint64 fszi = chld->data(COL_NAME, FileSizeRole).toULongLong();
                chld->setText(COL_SIZE, GetFormatedFileSize(m_currentFilePos) + "/" + GetFormatedFileSize(fszi));
                chld->setText(COL_SPEED, speed);
                chld->setText(COL_PERCENT, QString::number(m_currentFilePos*100.0/fszi, 'f', 2) + "%");
        }
        m_histTime.restart();
}

inline void
ProtocolDataTransfer::slotRemoveItem()
{
        QTreeWidget* tw = m_item->treeWidget();
        QTreeWidgetItem* it = tw->takeTopLevelItem(tw->indexOfTopLevelItem(m_item));
        if (it)
        {
                delete m_item;
                m_item = 0;
        }
}

inline void
ProtocolDataTransfer::slotAddList(const QStringList &fList)
{
        if (!m_item)
        {
                return;
        }
        QStringList fileList = fList;
        QString dKey = fileList.takeAt(0);
        QStringList keys = ParseFileInfoString(dKey);
        if (keys.size() < 3)
        {
                m_item->setText(COL_STATE, tr("Parse dir error"));
                return;
        }

        QTreeWidgetItem* root = m_item;
        root->setText(COL_SIZE, GetFormatedFileSize(keys[1].toULongLong()));
        root->setData(COL_NAME, FileSizeRole, keys[1].toULongLong());

        for (int i = 0; i < fileList.size(); i++)
        {
                QString key = fileList.at(i);
                QStringList keys = ParseFileInfoString(key);
                if (keys.size() < 3)
                {
                        continue;
                }

                QTreeWidgetItem* rowitem = new QTreeWidgetItem;
                root->addChild(rowitem);
                rowitem->setText(COL_NAME, keys[2]);
                rowitem->setText(COL_SIZE, GetFormatedFileSize(keys[1].toULongLong()));
                rowitem->setText(COL_TYPE, (keys[0] == "DIR")?tr("Dir"):tr("File"));
                if (updateDirFileItemInfo)
                {
	                rowitem->setText(COL_STATE, tr("Waiting"));
	                rowitem->setText(COL_PERCENT, "0%");
                }
                rowitem->setData(COL_NAME, FileSizeRole, keys[1].toULongLong());
                if (keys[0] == "DIR")
                {
                        rowitem->setIcon(COL_NAME, QIcon(DataAccess::SkinPath() + "/folder.png"));
                }
                else
                {
                        rowitem->setIcon(COL_NAME, QIcon(DataAccess::SkinPath() + "/file.png"));
                }
        }

        m_lock.lock();
        m_waitCondition.wakeAll();
        m_lock.unlock();
}

inline QTreeWidgetItem*
ProtocolDataTransfer::findItemChild(const QString& file)
{
        if (!m_item)
        {
                return 0;
        }
        int cnt = m_item->childCount();
        for (int i = 0; i < cnt; i++)
        {
                QTreeWidgetItem* chld = m_item->child(i);
                if (chld->text(COL_NAME) == file)
                {
                        return chld;
                }
        }
        return 0;
}

inline void
ProtocolDataTransfer::slotDirFileTransStart(const QString& file)
{
        QTreeWidgetItem* chld = findItemChild(file);
        if (!chld)
        {
                return;
        }
        chld->setText(COL_STATE, tr("Transferring"));
}

inline void
ProtocolDataTransfer::slotDirFileTransOK(const quint64 siz, const QString& file)
{
        QTreeWidgetItem* chld = findItemChild(file);
        if (!chld)
        {
                return;
        }
        chld->setText(COL_SIZE, GetFormatedFileSize(siz) + "/" + GetFormatedFileSize(siz));
        chld->setText(COL_PERCENT, "100%");
        chld->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/ok.png"));
        chld->setText(COL_STATE, tr("Finished"));
        chld->setData(COL_NAME, TransStateRole, "FINISH");
        m_item->setText(COL_SIZE, GetFormatedFileSize(bytesTransfered) + "/" + GetFormatedFileSize(totalBytes));
        m_item->setText(COL_PERCENT, QString::number(bytesTransfered*100.0/totalBytes, 'f', 2) + "%");
}

inline void
ProtocolDataTransfer::slotDirFileTransError(const QString& err, const QString& file)
{
        QTreeWidgetItem* chld = findItemChild(file);
        if (!chld)
        {
                return;
        }
        chld->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/error.png"));
        chld->setText(COL_STATE, err);
        m_item->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/error.png"));
        m_item->setText(COL_STATE, err);
}

//////////////////////////////////////////////////////////////////////////
class DirTransfer;
class UserItem;

class ProtocolDataWidget : public QTreeWidget
{
        Q_OBJECT
signals:
        void sigTips(const QString&);

public:
        ProtocolDataWidget(DataAccess& dataAccess, UserItem* uitem, QWidget* parent = 0);

public slots:
        QTreeWidgetItem* addItem(const QString& fileInfo, quint64 sn, const QString& kName);
        void slotAccept();
        void slotReject();
        void slotItemClicked(QTreeWidgetItem* item, int column);
        void onRecv(Message_ptr m);

        void slotRemoveFinishedItem();
        void slotRemoveErrorItem();
        void slotCancelAllItem();

protected:
        void createActions();
        void createMenus();
        virtual void contextMenuEvent(QContextMenuEvent* event)
        {
                menuContext->popup(event->globalPos());
        }

private:
        QMenu* menuContext;
        QAction* actRemoveFinishedItem;
        QAction* actRemoveErrorItem;
        QAction* actCancelAllItem;

        QPushButton* butAccept;
        QPushButton* butReject;
        UserItem* m_userItem;
        QTimer m_timer;

        DataAccess& m_DataAccess;
        QHash<uint, QTreeWidgetItem*> m_SendItemMap;
        QHash<uint, QTreeWidgetItem*> m_RecvItemMap;
};

#endif // __TransProtocolData_h__
