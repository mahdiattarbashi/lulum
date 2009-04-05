/*  filename: Server.cpp    2009/02/09  */
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

#include "Server.h"
#include "FormMain.h"

QQueue<IMProto::Message_ptr> g_msgQueue;
QQueue<IMProto::Message_ptr> g_msgQueueBuf;

QWaitCondition msgQueueNotEmpty;
QMutex mutex;

ACE_HANDLE
Server_Events::get_handle(void) const
{
        return this->mcast_dgram_.get_handle();
}

Server_Events::Server_Events(DataAccess& daccess)
: m_DataAccess(daccess)
{
        // Use ACE_SOCK_Dgram_Mcast factory to subscribe to multicast group.

        if(ACE_OS::hostname(this->hostname_,
                MAXHOSTNAMELEN) == -1)
        {
                ACE_ERROR((LM_ERROR, "%p\n", "hostname"));
        }
        else
        {
                for(int i = 0; i < m_DataAccess.addrList.size(); i++)
                {
                        Address addr = Address(Server_Port, m_DataAccess.addrList[i].toAscii().data());
                        if(this->mcast_dgram_.join(addr) == -1)
                        {
                                ACE_ERROR((LM_ERROR,  "%p: %s\n", ACE_TEXT("subscribe"), m_DataAccess.addrList[i].toAscii().data()));
                        }
                }
        }
}

Server_Events::~Server_Events(void)
{
}

int
Server_Events::handle_input(ACE_HANDLE)
{
        // Receive message from multicast group.

        ssize_t retcode =
                this->mcast_dgram_.recv(buf_,
                PayloadSize,
                this->remote_addr_);
        if(retcode != -1)
        {
                Address from(Server_Port, remote_addr_.get_host_addr());

                u32 len = retcode;
                char* data = new char[len];
                memcpy(data, buf_, len);

                Message_ptr m(new Message());

                m->add(Profile_ptr(new From(from)));

                istreamT is(data, len, 1); // Always little-endian.
                u32 msize;
                is >> msize;
                if(msize != len)
                {
                        return 0;
                }

                while(is.rd_ptr() != is.wr_ptr())
                {
                        u16 id, size;

                        if(!((is >> id) &&(is >> size))) break;

                        //cerr << 6 << "reading profile with id " << id << " "
                        //     << size << " bytes long" << endl;
                        Profile::Header hdr(id, size);

                        switch(id)
                        {
                        case IMProto::SNId:
                                m->add(Profile_ptr(new SN(hdr, is)));
                                continue;
                        case IMProto::AckId:
                                m->add(Profile_ptr(new Ack(hdr, is)));
                                continue;
                        case IMProto::DataId:
                                m->add(Profile_ptr(new Data(hdr, is)));
                                continue;
                        case IMProto::SignUpId:
                                m->add(Profile_ptr(new SignUp(hdr, is)));
                                continue;
                        case IMProto::SignInId:
                                m->add(Profile_ptr(new SignIn(hdr, is)));
                                continue;
                        case IMProto::SignOutId:
                                m->add(Profile_ptr(new SignOut(hdr, is)));
                                continue;

                        case IMProto::TextNormal:
                                m->add(Profile_ptr(new PTextNormal(hdr, is)));
                                continue;
                        case IMProto::TextZip:
                                m->add(Profile_ptr(new PTextZip(hdr, is)));
                                continue;
                        case IMProto::FileSend:
                                m->add(Profile_ptr(new PFileSend(hdr, is)));
                                continue;
                        case IMProto::DirFileRecv:
                                m->add(Profile_ptr(new PDirFileRecv(hdr, is)));
                                continue;
                        case IMProto::DirFileSendCancel:
                                m->add(Profile_ptr(new PDirFileSendCancel(hdr, is)));
                                continue;
                        case IMProto::DirFileRecvCancel:
                                m->add(Profile_ptr(new PDirFileRecvCancel(hdr, is)));
                                continue;
                        case IMProto::DirSend:
                                m->add(Profile_ptr(new PDirSend(hdr, is)));
                                continue;
                        case IMProto::PictureSend:
                                m->add(Profile_ptr(new PPictureSend(hdr, is)));
                                continue;
                        case IMProto::PictureRecv:
                                m->add(Profile_ptr(new PPictureRecv(hdr, is)));
                                continue;
                        case IMProto::SharedList:
                                m->add(Profile_ptr(new PSharedList(hdr, is)));
                                continue;
                        case IMProto::SharedListAck:
                                m->add(Profile_ptr(new PSharedList(hdr, is)));
                                continue;
                        case IMProto::SharedDownload:
                                m->add(Profile_ptr(new PSharedDownload(hdr, is)));
                                continue;
                        case IMProto::SharedDownloadCancel:
                                m->add(Profile_ptr(new PSharedDownloadCancel(hdr, is)));
                                continue;
                        case IMProto::SharedUploadCancel:
                                m->add(Profile_ptr(new PSharedUploadCancel(hdr, is)));
                                continue;
                        default:
                                is.skip_bytes(size);
                                continue;
                        }

//                         if(id == SN::id)
//                         {
//                                 m->add(Profile_ptr(new SN(hdr, is)));
//                         }
//                         else if(id == Ack::id)
//                         {
//                                 m->add(Profile_ptr(new Ack(hdr, is)));
//                         }
//                         else if(id == SignUp::id)
//                         {
//                                 m->add(Profile_ptr(new SignUp(hdr, is)));
//                         }
//                         else if(id == SignIn::id)
//                         {
//                                 m->add(Profile_ptr(new SignIn(hdr, is)));
//                         }
//                         else if(id == SignOut::id)
//                         {
//                                 m->add(Profile_ptr(new SignOut(hdr, is)));
//                         }
//                         else if(id == Data::id)
//                         {
//                                 m->add(Profile_ptr(new Data(hdr, is)));
//                         }
//                         else
//                         {
//                         }
                }

                mutex.lock();
                g_msgQueue.enqueue(m);
                msgQueueNotEmpty.wakeAll();
                mutex.unlock();

                delete data;
                return 0;
        }
        else
                return -1;
}

void Consumer::run()
{
        for(; ; )
        {
                mutex.lock();
                if(g_msgQueue.isEmpty())
                {
                        msgQueueNotEmpty.wait(&mutex);
                }
                g_msgQueueBuf.enqueue(g_msgQueue.dequeue());
                emit sigRecvReady();
                mutex.unlock();
        }
}

void RecvThread::run()
{
        g_msgQueue.clear();
        g_msgQueueBuf.clear();
        // Instantiate a server which will receive messages for DURATION
        // seconds.
        Server_Events* server_events = new Server_Events(m_DataAccess);
        // Instance of the ACE_Reactor.
        ACE_Reactor reactor;

        if(reactor.register_handler(server_events,
                ACE_Event_Handler::READ_MASK) == -1)
        {
                ACE_ERROR((LM_ERROR,
                        "%p\n%a",
                        "register_handler",
                        1));
        }

        ACE_DEBUG((LM_DEBUG,
                "starting up server\n"));

        Consumer cusumer;

        bool ok = connect(&cusumer, SIGNAL(sigRecvReady()), m_formMain, SLOT(recvMessage()));
        if(!ok)
        {
                qApp->quit();
        }
        cusumer.start();
        for(;;)
        {
                reactor.handle_events();
        }
}

