/*  filename: UserItem.h    2008/12/26  */
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

#ifndef __UserItem_h__
#define __UserItem_h__

#include "defs.h"

class UserItem : public QObject
{
	Q_OBJECT

public:
        UserItem(void);
        UserItem(const QString& mac);

	~UserItem(void);
public:
        enum UserState
        {
                S_Online = 1,
                S_Offline,
                S_Busy,
                S_Hide,
                S_Glad,
                S_Depressed
        };
public:
        QString MacAddress() const { return m_macAddress; }
        void MacAddress(QString val) { m_macAddress = val; }
        QString IpAddress() const { return m_ipAddress; }
        void IpAddress(QString val) { m_ipAddress = val; m_addr  = ACE_INET_Addr(Server_Port, m_ipAddress.toAscii().data());}
        QString HostName() const { return m_hostName; }
        void HostName(QString val) { m_hostName = val; }
        QString IconName() const { return m_iconName; }
        void IconName(QString val) { m_iconName = val; }
        QString LoginName() const { return m_loginName; }
        void LoginName(QString val) { m_loginName = val; }
        QString SelfDesc() const { return m_selfDesc; }
        void SelfDesc(QString val) { m_selfDesc = val; }
        QString ComputerID() const { return m_computerID; }
        void ComputerID(QString val) { m_computerID = val; }
        int State() const { return m_state; }
        void State(int val) { m_state = val; }
        QString GroupName() const { return m_groupName; }
        void GroupName(QString val) { m_groupName = val; }
        QString Passwd() const { return m_password; }
        void Passwd(QString val) { m_password = val; }

        QString OsName() const { return m_osName; }
        void OsName(QString val) { m_osName = val; }
        QString ProtocolName() const { return m_protocolName; }
        void ProtocolName(QString val) { m_protocolName = val; }
        QString ProtocolVersion() const { return m_protocolVersion; }
        void ProtocolVersion(QString val) { m_protocolVersion = val; }

        ACE_INET_Addr Addr() const { return m_addr; }
        void Addr(ACE_INET_Addr val) { m_addr = val; m_ipAddress = QString::fromAscii(val.get_host_addr()); }

        QHash<QString, QString>* SendingFilesCollection() { return &m_sendingFilesCollection; }
        QString insertSendingFile(const QString &dir)
        {
                QFileInfo finfo(dir);
                QString key = m_sendingFilesCollection.key(dir, "");
                if (key == "")
                {
                        m_sendingFilesCollection.insert(finfo.fileName(), dir);
                        key = finfo.fileName();
                }
                return key;
        }

        void removeSendingFileByValue(const QString &dir)
        {
                QFileInfo finfo(dir);
                QString key = m_sendingFilesCollection.key(dir, "");
                if (key != "")
                {
                        m_sendingFilesCollection.remove(key);
                }
        }

        void removeSendingByKey(const QString &key)
        {
                m_sendingFilesCollection.remove(key);
        }

private:
        QString m_macAddress;
        QString m_ipAddress;
        QString m_hostName;
        QString m_iconName;
        QString m_loginName;
        QString m_computerID;
        QString m_groupName;
        QString m_password;
        QString m_selfDesc;
        int m_state;
        QString m_osName;
        QString m_protocolName;
        QString m_protocolVersion;

        ACE_INET_Addr m_addr;
        QHash<QString, QString> m_sendingFilesCollection;
};

#endif // __UserItem_h__
