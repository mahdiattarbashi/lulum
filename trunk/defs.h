/*  filename: defs.h   2009/01/15      */
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

#ifndef __defs_h__
#define __defs_h__

#include <QtCore>
#include <QApplication>
#include <QtGui>
#include "Protocol.h"
#include "utils.h"

using namespace IPM;
using namespace IMProto;

extern QString PROGRAM_NAME;
extern QString PROGRAM_VER;
extern QString COMPUTER_OS;
extern QString COMPUTER_UserName;
extern QString COMPUTER_UserDomain;

#define Server_Port     22220
#define PayloadSize     65536

#define FileSizeRole    Qt::UserRole+100
#define TransThreadRole Qt::UserRole+101
#define SendRecvRole    Qt::UserRole+102
#define DirFileRole     Qt::UserRole+103
#define TransStateRole  Qt::UserRole+104
#define TransItemSNRole Qt::UserRole+105
#define FileKeyRole     (Qt::UserRole+106)
#define PathNameRole    (Qt::UserRole+107)

#define GroupNameRole   (Qt::UserRole+1)
#define UserItemRole    (Qt::UserRole+2)
#define DataKeyRole     (Qt::UserRole+3)

class UserItem;

typedef QTreeWidget UserViewType;
typedef QTreeWidgetItem TreeItemType;
typedef QHash<QString, QTreeWidgetItem*> TreeItemMapType;
typedef QHash<QString, UserItem*> UserItemMapType;
typedef QHash<QString, QString> StringMapType;

#endif // __defs_h__
