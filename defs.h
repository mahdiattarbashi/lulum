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
