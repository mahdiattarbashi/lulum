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
    LuLu ��ʹ: �������缴ʱ�Ի����ļ�����ͨѶ���ߡ�
    ����Ȩ���� (C) 2008,2009  ��ά�� <wwssir@gmail.com>

    ���ļ��� LuLu ��ʹԴ�ļ���һ���֡�

    LuLu ��ʹΪ������������������������������������GNUͨ�ù�����Ȩ���
    �Ա������ٴη�����/���޸ģ����������ݵ��Ǳ���Ȩ�ĵ����棬������ѡ�ģ�
    ��һ�պ��еİ汾��

    LuLu ��ʹ�ǻ���ʹ��Ŀ�Ķ����Է�����Ȼ�������κε������Σ����޶������Ի�
    �ض�Ŀ����������Ϊ��Ĭʾ�Ե��������������GNUͨ�ù�����Ȩ��

    ��Ӧ���յ������ڱ������GNUͨ�ù�����Ȩ�ĸ��������û�У������
    <http://www.gnu.org/licenses/>.
*************************************************************************/

// ˵����    
// �޸ģ�    

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
