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

