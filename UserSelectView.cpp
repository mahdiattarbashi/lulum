/*  filename: UserSelectView.cpp    2009/03/20  */
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

#include "DataAccess.h"
#include "UserItem.h"
#include "UserSelectView.h"

UserSelectView::UserSelectView(UserItemMapType* uItemMap, QWidget* parent)
: UsersView(uItemMap, parent)
{
        slotUpdateItems();
//         connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(slotItemChanged(QTreeWidgetItem*, int)));
        connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(slotItemChanged(QTreeWidgetItem*, int)));
        CreateContextMenu();
}

UserSelectView::UserSelectView(QList<UserItem*> uItems, QWidget* parent)
: UsersView(uItems, parent)
{
        slotUpdateItems();
//         connect(this, SIGNAL(itemClicked(QTreeWidgetItem*, int)), SLOT(slotItemChanged(QTreeWidgetItem*, int)));
        connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), SLOT(slotItemChanged(QTreeWidgetItem*, int)));
        CreateContextMenu();
}

void
UserSelectView::CreateContextMenu()
{
        this->setIndentation(15);
        QTreeWidgetItem* header = this->headerItem();
        header->setText(0, tr("Copy Send List") + "(" + tr("Select Members") + ")");
        menuContext = new QMenu(tr("User Select View Context Menu"));
        menuContext->addAction(actExpandAllGroup);
        menuContext->addAction(actRefresh);
}


void
UserSelectView::slotItemChanged(QTreeWidgetItem* item, int column)
{
        if (this->indexOfTopLevelItem(item) != -1)
        {
                int cnt = item->childCount();
                if (item->checkState(0) == Qt::Checked)
                {
                        for (int i = 0; i < cnt; i++)
                        {
                                QTreeWidgetItem* chld = item->child(i);
                                chld->setCheckState(0, Qt::Checked);
                        }
                }
                else if (item->checkState(0) == Qt::Unchecked)
                {
                        for (int i = 0; i < cnt; i++)
                        {
                                QTreeWidgetItem* chld = item->child(i);
                                chld->setCheckState(0, Qt::Unchecked);
                        }
                }
        }
        else
        {
                QTreeWidgetItem* p = item->parent();
                p->setCheckState(0, Qt::PartiallyChecked);
        }
}

TreeItemType*
UserSelectView::AddGroup(const QString& groupName)
{
        TreeItemType* i = UsersView::AddGroup(groupName);
        i->setCheckState(0, Qt::Unchecked);
        i->setToolTip(0, "");
        return i;
}

TreeItemType*
UserSelectView::AddUser(UserItem* uItem)
{
        TreeItemType* i = UsersView::AddUser(uItem);
        i->setCheckState(0, Qt::Unchecked);
        i->setToolTip(0, "");
        return i;
}

QList<UserItem*>
UserSelectView::GetSelectedUserItems()
{
        QList<UserItem*> selList;
        int topCount = this->topLevelItemCount();
        for (int i = 0; i < topCount; i++)
        {
                TreeItemType* topi = this->topLevelItem(i);
                int csCount = topi->childCount();
                for (int j = 0; j < csCount; j++)
                {
                        TreeItemType* chld = topi->child(j);
                        if (chld->checkState(0) == Qt::Checked)
                        {
                                QString const& key = chld->data(0, UserItemRole).toString();
                                if (UserItem* u = m_collection->value(key, 0))
                                {
	                                selList << u;
                                }
                        }
                }
        }
        return selList;
}

void
UserSelectView::SetAllChecked(bool checked)
{
        int topCount = this->topLevelItemCount();
        for (int i = 0; i < topCount; i++)
        {
                TreeItemType* topi = this->topLevelItem(i);
                int csCount = topi->childCount();
                for (int j = 0; j < csCount; j++)
                {
                        TreeItemType* chld = topi->child(j);
                        chld->setCheckState(0, checked?(Qt::Checked):(Qt::Unchecked));
                }
        }
}
