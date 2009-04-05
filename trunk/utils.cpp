/*  filename: utils.cpp    2009/03/02  */
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

#include "utils.h"
#include <ace/OS_NS_netdb.h>
#include <ace/INET_Addr.h>

namespace IPM
{
        QString GetComputerUserName()
        {
                return "";
        }

        QString GetComputerUserDomain()
        {
                return "";
        }

        QString GetComputerOsVer()
        {
#ifdef WIN32
                //QString program = "cmd";

                //QProcess *myProcess = new QProcess;
                //myProcess->start("set");
                //if (!myProcess->waitForFinished())
                //{
                //        return 0;
                //}
                //QByteArray result = myProcess->readAll();

                //return QString::fromLocal8Bit(result);
                QSysInfo::WinVersion ver = QSysInfo::windowsVersion();
                if (ver == QSysInfo::WV_2000)
                {
                        return "Windows 2000";
                }
                else if (ver == QSysInfo::WV_2003)
                {
                        return "Windows Server 2003";
                }
                else if (ver == QSysInfo::WV_XP)
                {
                        return "Windows XP";
                }
                else if (ver == QSysInfo::WV_VISTA)
                {
                        return "Windows Vista";
                }
                else 
                {
                        return "Windows Other Version";
                }
#else
                return "Linux";
#endif // WIN32

        }



        QString
                GetMacAddrString()
        {
#ifdef WIN32
                ACE_OS::macaddr_node_t mac;
                int ret = ACE_OS::getmacaddress(&mac);

                if (ret == -1)
                {
                        return "";
                }
                return QString("%1-%2-%3-%4-%5-%6")
                        .arg((uint)mac.node[0], 2, 16, QChar('0'))
                        .arg((uint)mac.node[1], 2, 16, QChar('0'))
                        .arg((uint)mac.node[2], 2, 16, QChar('0'))
                        .arg((uint)mac.node[3], 2, 16, QChar('0'))
                        .arg((uint)mac.node[4], 2, 16, QChar('0'))
                        .arg((uint)mac.node[5], 2, 16, QChar('0'))
                        .toUpper();
#else
                QString program = "ifconfig";
                QStringList arguments;
                arguments << "-a";

                QProcess *myProcess = new QProcess;
                myProcess->start(program, arguments);
                if (!myProcess->waitForFinished())
                {
                        return 0;
                }
                QByteArray result = myProcess->readAll();
                QString s = QString::fromUtf8(result.data());
                QStringList slist = s.split("\n");
                while (!slist.isEmpty())
                {
                        const QString& line = slist.takeAt(0);
                        if (line.startsWith("eth")
                                && line.contains("Link encap:"))
                        {
                                QRegExp rx("\\s([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})");
                                QString str = "Offsets: 12 14 99 231 7";
                                QStringList list;
                                int pos = 0;

                                pos = rx.indexIn(line, pos);
                                QString mac = rx.cap(1).toUpper();
                                if (!mac.isEmpty())
                                {
                                        QStringList const& ml = mac.split(":");
                                        return ml.join("-");
                                }
                        }
                }
                return "FF-FF-FF-FF-FF-FF";
#endif // WIN32
        }

        QString
                GetFormatedFileSize(quint64 bytes)
        {
                // According to the Si standard KB is 1000 bytes, KiB is 1024
                // but on windows sizes are calculated by dividing by 1024 so we do what they do.
                const quint64 kb = 1024;
                const quint64 mb = 1024 * kb;
                const quint64 gb = 1024 * mb;
                const quint64 tb = 1024 * gb;
                if (bytes >= tb)
                        return QLocale().toString(bytes / (double)tb, 'f', 2) + QString::fromLatin1("TB");
                if (bytes >= gb)
                        return QLocale().toString(bytes / (double)gb, 'f', 2) + QString::fromLatin1("GB");
                if (bytes >= mb)
                        return QLocale().toString(bytes / (double)mb, 'f', 2) + QString::fromLatin1("MB");
                if (bytes >= kb)
                        return QLocale().toString(bytes / (double)kb, 'f', 2) + QString::fromLatin1("KB");
                return QLocale().toString(bytes) + QString::fromLatin1("B");
        }

        QString
                GetFormatedTransSpeed(quint64 bytes)
        {
                // According to the Si standard KB is 1000 bytes, KiB is 1024
                // but on windows sizes are calculated by dividing by 1024 so we do what they do.
                const quint64 kb = 1024;
                const quint64 mb = 1024 * kb;
                if (bytes >= mb)
                        return QLocale().toString(bytes / (double)mb, 'f', 2) + "MB/S";
                if (bytes >= kb)
                        return QLocale().toString(bytes / (double)kb, 'f', 2) + "KB/S";
                return QLocale().toString(bytes) + "B/S";
        }

        QStringList
                ParseFileInfoString(const QString &key)
        {
                QStringList keys;
                keys << key.section('$', 0, 0);
                keys << key.section('$', 1, 1);
                keys << key.section('$', 2);
                return keys;
        }

        QString GetVersionString(uint ver)
        {
                ACE_INET_Addr v((ushort)0, ver);
                return QString::fromAscii(v.get_host_addr());
        }
}

