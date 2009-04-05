/*  filename: UsersMainView.cpp    2009/02/06  */
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

#include "UserItem.h"
#include "FormTalk.h"
#include "UserSelectView.h"
#include "UsersMainView.h"

UsersMainView::UsersMainView(DataAccess& daccess, QWidget* parent)
: UsersView(daccess.userItemCollection.Collection(), parent)
, m_DataAccess(daccess)
{
        this->setStyleSheet("QTreeView::item {border: 0px solid #d9d9d9;}"
                "QTreeView::item:hover {border: 1px solid #d9d9d9;}");
        createActions();
        createMenus();

        this->setIndentation(15);
        this->setMinimumWidth(200);
        this->setIconSize(QSize(40, 40));
        this->setEditTriggers(QAbstractItemView::NoEditTriggers);
        connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(slotItemClicked(QTreeWidgetItem*, int)));
        connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(slotItemDBClicked(QTreeWidgetItem*, int)));

        AddGroup(DataAccess::DefaultGroupName());
        for (QHash<QString, UserItem*>::iterator it = m_DataAccess.userItemCollection.Collection()->begin();
                it != m_DataAccess.userItemCollection.Collection()->end(); it++)
        {
                QString const& gName = it.value()->GroupName();
                Item* gItem = m_group_itemMap.value(gName, 0);
                if (!gItem)
                {
                        AddGroup(gName);
                }
        }
}

void
UsersMainView::createActions()
{
        actTalk = new QAction(QIcon(DataAccess::SkinPath() + "/chat-one.png"), tr("&Talk"), this);
        actTalk->setToolTip(tr("Open the talk dialog"));
        connect(actTalk, SIGNAL(triggered()), this, SLOT(slotTalk()));

        actSendFile = new QAction(QIcon(DataAccess::SkinPath() + "/send-files.png"), tr("Send &Files"), this);
        actSendFile->setToolTip(tr("Send some files to the other"));
        connect(actSendFile, SIGNAL(triggered()), this, SLOT(slotSendFile()));

        actSendDir = new QAction(QIcon(DataAccess::SkinPath() + "/send-folder.png"), tr("Send &Directory"), this);
        actSendDir->setToolTip(tr("Send a directory to the other"));
        connect(actSendDir, SIGNAL(triggered()), this, SLOT(slotSendDir()));

        actCreateGroup = new QAction(QIcon(DataAccess::SkinPath() + "/add-group.png"), tr("&Create Group"), this);
        connect(actCreateGroup, SIGNAL(triggered()), this, SLOT(slotCreateGroup()));
        actRenameGroup = new QAction(QIcon(DataAccess::SkinPath() + "/edit.png"), tr("Re&name Group"), this);
        connect(actRenameGroup, SIGNAL(triggered()), this, SLOT(slotRenameGroup()));
        actRemoveGroup = new QAction(QIcon(DataAccess::SkinPath() + "/delete.png"), tr("&Remove Group"), this);
        connect(actRemoveGroup, SIGNAL(triggered()), this, SLOT(slotRemoveGroup()));

        actRoomTalk = new QAction(QIcon(DataAccess::SkinPath() + "/chat-multi.png"), tr("Open A Meeting Room Of This Group"), this);
        connect(actRoomTalk, SIGNAL(triggered()), this, SLOT(slotOpenNetMeeting()));
}

void
UsersMainView::createMenus()
{
        menuItemToGroup = new QMenu(tr("Move &Member To Group"));
        menuGroupToGroup = new QMenu(tr("Move &All Members To Group"));
        menuGroup = new QMenu(tr("Group"));
        menuGroup->addAction(actRoomTalk);
        menuGroup->addSeparator();
        menuGroup->addMenu(menuGroupToGroup);
        menuGroup->addAction(actExpandAllGroup);
        menuGroup->addAction(actCreateGroup);
        menuGroup->addAction(actRenameGroup);
        menuGroup->addAction(actRemoveGroup);
        menuGroup->addSeparator();
        menuGroup->addAction(actRefresh);

        menuUser = new QMenu(tr("User"));
        menuUser->addAction(actTalk);
        menuUser->addAction(actSendFile);
        menuUser->addAction(actSendDir);
        menuUser->addSeparator();
        menuUser->addAction(actExpandAllGroup);
        menuUser->addAction(actCreateGroup);
        menuUser->addMenu(menuItemToGroup);
        menuUser->addSeparator();
        menuUser->addAction(actRefresh);

        menuContext = new QMenu(tr("&Context"));
        menuContext->addAction(actCreateGroup);
        menuContext->addAction(actExpandAllGroup);
        menuContext->addSeparator();
        menuContext->addAction(actRefresh);
        connect(menuItemToGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotMoveToGroup(QAction*)));
        connect(menuGroupToGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotMoveToGroup(QAction*)));
        setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
UsersMainView::slotCreateGroup()
{
        int i = 1;
        QString gName(tr("New Group"));
        while (m_group_itemMap.value(gName, 0))
        {
                gName = tr("New Group") + " " + QString::number(i);
                i++;
        }
        AddGroup(gName);
}

void
UsersMainView::slotRenameGroup()
{
        bool ok;
        QString const& oldName = selItem->data(0, GroupNameRole).toString();
        QString text = QInputDialog::getText(this, tr("Rename Group"),
                tr("Group Name:"), QLineEdit::Normal,
                oldName, &ok);
        if (ok && !text.isEmpty())
        {
                m_group_itemMap.remove(oldName);
                m_group_itemMap.insert(text, selItem);
                selItem->setData(0, GroupNameRole, text);
                selItem->setText(0, selItem->data(0, GroupNameRole).toString() + " (" + QString::number(selItem->childCount()) + ")");
                int cnt = selItem->childCount();
                for (int i = 0; i < cnt; i++)
                {
                        QString const& key = selItem->child(i)->data(0, UserItemRole).toString();
                        UserItem* uitem = UsersView::GetUserItem(key);
                        uitem->GroupName(text);
                        m_DataAccess.addRemoteUser(uitem);
                }
        }
}

void
UsersMainView::slotRemoveGroup()
{
        QString const& groupName = selItem->data(0, GroupNameRole).toString();
        if (groupName == DataAccess::DefaultGroupName())
        {
                QMessageBox::warning(this, tr("Warning"), tr("You can't delete default group."));
                return;
        }
        RemoveGroup(groupName);
}

void
UsersMainView::slotMoveToGroup(QAction* act)
{
        if (this->indexOfTopLevelItem(selItem) != -1)
        {
                QString const& groupName = act->text();
                Item* groupItem = m_group_itemMap.value(groupName, 0);
                if (!groupItem)
                {
                        QMessageBox::information(this, tr("Tips"), tr("The group: %1 does not exist.").arg(groupName));
                        return;
                }
                groupItem->setExpanded(true);
                QList<QTreeWidgetItem*> const& children = selItem->takeChildren();
                groupItem->addChildren(children);
                groupItem->setText(0, groupItem->data(0, GroupNameRole).toString() + " (" + QString::number(groupItem->childCount()) + ")");
                selItem->setText(0, selItem->data(0, GroupNameRole).toString() + " (" + QString::number(selItem->childCount()) + ")");

                for (int i = 0; i < children.size(); i++)
                {
                        QString const& key = children[i]->data(0, UserItemRole).toString();
                        UserItem* uitem = UsersView::GetUserItem(key);
                        uitem->GroupName(groupName);
                }
        }
        else
        {
                MoveSelectedToGroup(act->text());
        }
}

void
UsersMainView::MoveSelectedToGroup(const QString& groupName)
{
        Item* groupItem = m_group_itemMap.value(groupName, 0);
        QList<QTreeWidgetItem*> selItems = this->selectedItems();
        for (int i = 0; i < selItems.size(); i++)
        {
                if (this->indexOfTopLevelItem(selItems[i]) != -1)
                {
                        continue;
                }
                QString const& key = selItems[i]->data(0, UserItemRole).toString();
                UserItem* uitem = UsersView::GetUserItem(key);
                if (uitem->GroupName() == groupName)
                {
                        continue;
                }
                Item* pItem = selItems[i]->parent();
                int idx = pItem->indexOfChild(selItems[i]);
                groupItem->addChild(pItem->takeChild(idx));
                groupItem->setText(0, groupItem->data(0, GroupNameRole).toString() + " (" + QString::number(groupItem->childCount()) + ")");
                pItem->setText(0, pItem->data(0, GroupNameRole).toString() + " (" + QString::number(pItem->childCount()) + ")");
                uitem->GroupName(groupName);
        }
}

void
UsersMainView::slotTalk()
{
        if (selItem)
        {
                emit itemDoubleClicked(selItem, 0);
        }
}

void
UsersMainView::slotSendFile()
{
        QString const& key = selItem->data(0, UserItemRole).toString();
        FormTalk* ft = m_DataAccess.getFormTalk(key);
        ft->slotSendFile();
}

void
UsersMainView::slotSendDir()
{
        QString const& key = selItem->data(0, UserItemRole).toString();
        FormTalk* ft = m_DataAccess.getFormTalk(key);
        ft->slotSendDir();
}

void
UsersMainView::slotItemDBClicked(QTreeWidgetItem* item, int /*column*/)
{
        if (this->indexOfTopLevelItem(item) != -1)
        {
                return;
        }
        QString const& key = item->data(0, UserItemRole).toString();
        FormTalk*ft = m_DataAccess.getFormTalk(key);
        ft->slotSetView(true);
}

void
UsersMainView::slotItemClicked(QTreeWidgetItem* item, int /*column*/)
{
        if (this->indexOfTopLevelItem(item) != -1)
        {
                item->setExpanded(!item->isExpanded());
                return;
        }
}


void
UsersMainView::slotOpenNetMeeting()
{
        UserSelectView* usv = new UserSelectView(m_DataAccess.userItemCollection.Collection());
        usv->show();
}