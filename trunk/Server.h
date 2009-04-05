/*  filename: Server.h    2009/02/09  */
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

#ifndef __Server_h__
#define __Server_h__

#include "defs.h"
#include <ace/OS.h>
#include <ace/SOCK_Dgram_Mcast.h>
#include <ace/Event_Handler.h>
#include <ace/Reactor.h>
#include "DataAccess.h"

extern QQueue<IMProto::Message_ptr> g_msgQueueBuf;

class Server_Events : public QObject, public ACE_Event_Handler
{
        Q_OBJECT

public:
        Server_Events (DataAccess& daccess);
        ~Server_Events (void);

        virtual int handle_input (ACE_HANDLE fd);
        virtual ACE_HANDLE get_handle (void) const;

private:
        char buf_[PayloadSize];
        char hostname_[MAXHOSTNAMELEN];

        ACE_Reactor *reactor_;
        ACE_SOCK_Dgram_Mcast mcast_dgram_;
        ACE_INET_Addr remote_addr_;
        ACE_Vector<ACE_INET_Addr> mcast_addr_;
        
        DataAccess& m_DataAccess;
};

//////////////////////////////////////////////////////////////////////////
class FormMain;

class RecvThread : public QThread
{
        Q_OBJECT

public:
        RecvThread(DataAccess& daccess, FormMain* fm, QObject* parent = 0)
                : QThread(parent)
                , m_formMain(fm)
                , m_DataAccess(daccess)
        {
        }

protected:
        void run();
private:
        FormMain* m_formMain;
        DataAccess& m_DataAccess;
};

class Consumer : public QThread
{
        Q_OBJECT

signals:
        void sigRecvReady();

public:
        Consumer(QObject* parent = 0)
                : QThread(parent)
        {

        }

protected:
        void run();
private:
};


#endif // __Server_h__
