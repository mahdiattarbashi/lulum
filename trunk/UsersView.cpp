/*  filename: UsersView.cpp    2009/03/20  */
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
#include "UsersView.h"

UsersView::UsersView(UserItemMapType* uItemMap, QWidget* parent)
: UserViewType(parent)
, m_collection(uItemMap)
, m_isSelfCollection(!uItemMap)
{
        if (!uItemMap)
        {
                m_collection = new UserItemMapType;
        }
        TreeItemType* headerItem = new TreeItemType;
        headerItem->setText(0, tr("Friends List"));
        this->setHeaderItem(headerItem);
        CreateActions();
}

UsersView::UsersView(QList<UserItem*> uItems, QWidget* parent)
: UserViewType(parent)
, m_isSelfCollection(true)
{
        m_collection = new UserItemMapType;

        TreeItemType* headerItem = new TreeItemType;
        headerItem->setText(0, tr("Friends List"));
        this->setHeaderItem(headerItem);

        for (int i = 0; i < uItems.size(); i++)
        {
                m_collection->insert(uItems[i]->IpAddress(), uItems[i]);
        }
        CreateActions();
}

UsersView::~UsersView(void)
{
        if (m_isSelfCollection)
        {
                delete m_collection;
        }
}

void
UsersView::CreateActions()
{
        actExpandAllGroup = new QAction(QIcon(DataAccess::SkinPath() + "/expand0.png"), tr("Expand/Fold Groups"), this);
        connect(actExpandAllGroup, SIGNAL(triggered()), this, SLOT(slotExpandAllGroup()));

        actRefresh = new QAction(QIcon(DataAccess::SkinPath() + "/refresh.png"), tr("Re&fresh"), this);
        connect(actRefresh, SIGNAL(triggered()), this, SLOT(slotUpdateItems()));
}

UserItem*
UsersView::GetUserItem(const QString& key)
{
        return m_collection->value(key, 0);
}

TreeItemType*
UsersView::AddGroup(const QString& groupName)
{
        TreeItemType* groupItem = new TreeItemType;
        groupItem->setToolTip(0, tr("Click to fold or unfold the group."));
        groupItem->setSizeHint(0, QSize(0, 20));
        groupItem->setText(0, groupName);
        groupItem->setText(0, groupName + " (" + QString::number(groupItem->childCount()) + ")");

        groupItem->setData(0, GroupNameRole, groupName);
        m_group_itemMap.insert(groupName, groupItem);
        this->addTopLevelItem(groupItem);
        return groupItem;
}

void
UsersView::RemoveGroup(const QString& groupName)
{
        TreeItemType* groupItem = m_group_itemMap.value(groupName, 0);
        if (!groupItem)
        {
                QMessageBox::information(this, tr("Tips"), tr("The group: %1 does not exist.").arg(groupName));
                return;
        }
        TreeItemType* destGItem = m_group_itemMap.value(DataAccess::DefaultGroupName(), 0);
        QList<TreeItemType*> const& selItems = groupItem->takeChildren();
        destGItem->addChildren(selItems);
        destGItem->setText(0, destGItem->data(0, GroupNameRole).toString() + " (" + QString::number(destGItem->childCount()) + ")");
        for (int i = 0; i < selItems.size(); i++)
        {
                QString const& key = selItems[i]->data(0, UserItemRole).toString();
                UserItem* uitem = m_collection->value(key);
                uitem->GroupName(DataAccess::DefaultGroupName());
        }

        m_group_itemMap.remove(groupName);
        delete this->takeTopLevelItem(indexOfTopLevelItem(groupItem));
}

TreeItemType*
UsersView::AddUser(UserItem* uItem)
{
        QString const& gName = uItem->GroupName();
        TreeItemType* gItem = m_group_itemMap.value(gName, 0);
        if (!gItem)
        {
                gItem = AddGroup(gName);
        }
        TreeItemType* uTreeItem = new TreeItemType;
        if (QFile::exists(uItem->IconName()))
        {
                uTreeItem->setIcon(0, QIcon(uItem->IconName()));
        }
        else
        {
                uTreeItem->setIcon(0, QIcon(":/faces/qq/0.png"));
        }
        uTreeItem->setText(0, uItem->LoginName());
        QString tooltip =
                tr("User Name:") + "\t" + uItem->LoginName() + "\n" +
                tr("Host Name:") + "\t"+ uItem->HostName() + "\n" + 
                tr("IP Address:") + "\t" + uItem->IpAddress() + "\n" + 
                tr("Mac Address:") + "\t" + uItem->MacAddress() + "\n" + 
                tr("OS Platform:") + "\t" + uItem->OsName();
        uTreeItem->setData(0, Qt::ToolTipRole, tooltip);
        uTreeItem->setData(0, UserItemRole, uItem->IpAddress());
        m_user_itemMap.insert(uItem->IpAddress(), uTreeItem);
        gItem->addChild(uTreeItem);
        gItem->setText(0, gName + " (" + QString::number(gItem->childCount()) + ")");
        return uTreeItem;
}

TreeItemType*
UsersView::UpdateUserTreeItem(UserItem* uItem)
{
        TreeItemType* uTreeItem = m_user_itemMap.value(uItem->IpAddress(), 0);
        if (!uTreeItem)
        {
                return AddUser(uItem);
        }

        if (QFile::exists(uItem->IconName()))
        {
                uTreeItem->setIcon(0, QIcon(uItem->IconName()));
        }
        else
        {
                uTreeItem->setIcon(0, QIcon(":/faces/qq/0.png"));
        }
        uTreeItem->setText(0, uItem->LoginName());
        QString tooltip =
                tr("User Name:") + "\t" + uItem->LoginName() + "\n" +
                tr("Host Name:") + "\t"+ uItem->HostName() + "\n" + 
                tr("IP Address:") + "\t" + uItem->IpAddress() + "\n" + 
                tr("Mac Address:") + "\t" + uItem->MacAddress() + "\n" + 
                tr("OS Platform:") + "\t" + uItem->OsName();
        uTreeItem->setData(0, Qt::ToolTipRole, tooltip);

        return uTreeItem;
}

TreeItemType*
UsersView::AddNewUser(UserItem* uItem)
{
        m_collection->insert(uItem->IpAddress(), uItem);
        return this->AddUser(uItem);
}

QList<UserItem*>
UsersView::AddNewUsers(const QList<UserItem*>& uItems)
{
        for (int i = 0; i < uItems.size(); i++)
        {
                AddNewUser(uItems[i]);
        }
        return uItems;
}

void
UsersView::RemoveUser(UserItem* uItem)
{
        QString const& gName = uItem->GroupName();
        TreeItemType* uTreeItem = m_user_itemMap.value(uItem->IpAddress(), 0);
        if (!uTreeItem)
        {
                return;
        }
        TreeItemType* gItem = m_group_itemMap.value(gName, 0);
        if (!gItem)
        {
                return;
        }
        m_user_itemMap.remove(uItem->IpAddress());
        gItem->removeChild(uTreeItem);
        gItem->setText(0, gItem->data(0, GroupNameRole).toString() + " (" + QString::number(gItem->childCount()) + ")");
}

void
UsersView::RemoveUser(const QString& key)
{
        UserItem* uItem = m_collection->value(key, 0);
        if (!uItem)
        {
                return;
        }
        this->RemoveUser(uItem);
}

void
UsersView::slotUpdateItems()
{
        setUpdatesEnabled(false);
        this->clear();
        m_user_itemMap.clear();
        m_group_itemMap.clear();
        AddGroup(DataAccess::DefaultGroupName());
        for (UserItemMapType::const_iterator it = m_collection->begin(); it != m_collection->end(); it++)
        {
                this->AddUser(it.value());
        }

        for (TreeItemMapType::const_iterator it = m_group_itemMap.begin(); it != m_group_itemMap.end(); it++)
        {
                TreeItemType* gItem = it.value();
                gItem->setText(0, gItem->data(0, GroupNameRole).toString() + " (" + QString::number(gItem->childCount()) + ")");
                gItem->setExpanded(true);
        }
        setUpdatesEnabled(true);
}

void
UsersView::slotExpandAllGroup()
{
        for (int i = 0; i < this->topLevelItemCount(); i++)
        {
                QTreeWidgetItem* root = this->topLevelItem(i);
                root->setExpanded(!root->isExpanded());
        }
}

