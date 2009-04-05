/*  filename: UsersMainView.h    2009/02/06  */
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

#ifndef __UsersMainView_h__
#define __UsersMainView_h__

#include "DataAccess.h"
#include "UsersView.h"

typedef QStandardItemModel ItemModel;
typedef QTreeWidgetItem Item;

class FormTalk;

class UsersMainView : public UsersView
{
        Q_OBJECT

public:
        UsersMainView(DataAccess& daccess, QWidget* parent = 0);

public:
        void MoveSelectedToGroup(const QString& groupName);

protected:

        virtual void mouseReleaseEvent ( QMouseEvent * event )
        {
                switch (event->button())
                {
                case Qt::RightButton:
                        break;
                default:
                        ;
                }
                UserViewType::mouseReleaseEvent(event);
        }

        void createActions();
        void createMenus();

protected slots:
        void slotTalk();
        void slotSendFile();
        void slotSendDir();
        void slotItemClicked(QTreeWidgetItem* item, int column);
        void slotItemDBClicked(QTreeWidgetItem* item, int column);

        void slotCreateGroup();
        void slotRenameGroup();
        void slotRemoveGroup();
        void slotMoveToGroup(QAction* act);
        void slotOpenNetMeeting();

protected:
        virtual void contextMenuEvent(QContextMenuEvent* event)
        {
                selItem = itemAt(event->pos());
                if (selItem != 0)
                {
                        QStringList const& groupList = m_group_itemMap.keys();
	                if (this->indexOfTopLevelItem(selItem) != -1)
	                {
                                menuGroupToGroup->clear();
                                for (int i = 0; i < groupList.size(); i++)
                                {
                                        QAction* act = new QAction(groupList[i], this);
                                        menuGroupToGroup->addAction(act);
                                }
                                menuGroup->popup(event->globalPos());
	                }
                        else
                        {
                                menuItemToGroup->clear();
                                for (int i = 0; i < groupList.size(); i++)
                                {
                                        QAction* act = new QAction(groupList[i], this);
                                        menuItemToGroup->addAction(act);
                                }
                                menuUser->popup(event->globalPos());
                        }
                }
                else
                {
                        menuContext->popup(event->globalPos());
                }
        }

private:
        QAction* actTalk;
        QAction* actSendFile;
        QAction* actSendDir;
        QAction* actCreateGroup;
        QAction* actRenameGroup;
        QAction* actRemoveGroup;
        QAction* actRoomTalk;
        Item* selItem;

        QMenu* menuContext;
        QMenu* menuUser;
        QMenu* menuGroup;
        QMenu* menuItemToGroup;
        QMenu* menuGroupToGroup;

        QActionGroup* actGroupMoveTo;
        DataAccess& m_DataAccess;
};

#endif // __UsersMainView_h__

