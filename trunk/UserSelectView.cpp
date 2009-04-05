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
