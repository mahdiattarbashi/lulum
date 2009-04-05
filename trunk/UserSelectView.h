/*  filename: UserSelectView.h    2009/03/20  */
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

#ifndef __UserSelectView_h__
#define __UserSelectView_h__

#include "UsersView.h"

class UserSelectView : public UsersView
{
        Q_OBJECT

public:
        UserSelectView(UserItemMapType* uItemMap = 0, QWidget* parent = 0);
        UserSelectView(QList<UserItem*> uItems, QWidget* parent = 0);

public:
        virtual TreeItemType* AddGroup(const QString& groupName);
        virtual TreeItemType* AddUser(UserItem* uItem);
        QList<UserItem*> GetSelectedUserItems();
        void SetAllChecked(bool checked);

        void CreateContextMenu();

public slots:
        void slotItemChanged(QTreeWidgetItem* tItem, int column);
protected:
        virtual void contextMenuEvent(QContextMenuEvent* event)
        {
                menuContext->popup(event->globalPos());
        }

private:
        QAction* actTalk;
        QAction* actSendFile;
        QAction* actSendDir;
        QAction* actCreateGroup;
        QAction* actRenameGroup;
        QAction* actRemoveGroup;

        QMenu* menuContext;
        QMenu* menuUser;
        QMenu* menuGroup;
        QMenu* menuItemToGroup;
        QMenu* menuGroupToGroup;

        QTreeWidgetItem* selItem;
};

#endif // __UserSelectView_h__
