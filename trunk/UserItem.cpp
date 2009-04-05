/*  filename: UserItem.cpp    2008/12/26  */
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

#include "UserItem.h"
#include "utils.h"
#include <ace/OS_NS_netdb.h>
#include "DataAccess.h"

UserItem::UserItem(void)
{
        m_macAddress = GetMacAddrString();
        ACE_INET_Addr addr;
        m_hostName = addr.get_host_name();
        hostent* host = ACE_OS::getipnodebyname(addr.get_host_name(), AF_INET);

        char* a = *host->h_addr_list;
        uint p1 = (unsigned char)a[0];
        uint p2 = (unsigned char)a[1];
        uint p3 = (unsigned char)a[2];
        uint p4 = (unsigned char)a[3];
        m_ipAddress = QString("%1.%2.%3.%4")
                .arg(QString::number(p1))
                .arg(QString::number(p2))
                .arg(QString::number(p3))
                .arg(QString::number(p4));

        m_addr  = ACE_INET_Addr(Server_Port, m_ipAddress.toAscii().data());
}

UserItem::UserItem(const QString& mac)
{
        m_macAddress = mac;
}

UserItem::~UserItem(void)
{
}
