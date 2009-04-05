/*  filename: Trans.h    2009/03/03  */
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

#ifndef __Trans_h__
#define __Trans_h__

#include "defs.h"
#include "DataAccess.h"
#include <ace/SOCK_Stream.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/INET_Addr.h>

class Transfer : public QThread
{
        Q_OBJECT

signals:
        void sigTransError(Transfer*);
        void sigTransOK(Transfer*);
public:
        Transfer(const quint64 siz, const quint16 port, const QString& address = "", QObject *parent = 0)
                : m_hostAddress(address)
                , m_hostPort(port)
                , totalBytes(siz)
                , bytesTransfered(0)
                , QThread(parent)
        {
                connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
        }
        virtual ~Transfer()
        {
        }

public:
        quint64 BytesTransfered() const { return bytesTransfered; }
        quint64 TotalBytes() const { return totalBytes; }
protected:
        QString m_hostAddress;
        quint16 m_hostPort;
        quint64 totalBytes;
        quint64 bytesTransfered;
};



#endif // __Trans_h__
