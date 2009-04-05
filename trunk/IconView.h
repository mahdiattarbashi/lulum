/*  filename: IconView.h    2009/03/07  */
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

#ifndef __IconView_h__
#define __IconView_h__

#include "defs.h"
#include "DataAccess.h"

#define NameRole        UserItemRole
class IconView : public QListWidget
{
        Q_OBJECT

public:
        IconView(QWidget *parent = 0)
                : QListWidget(parent)
        {
                setViewMode(QListView::IconMode);
        }

        IconView(const QString& path)
        {
                setViewMode(QListView::IconMode);
                setWindowFlags(Qt::Popup);
                QDir dir(path);
                QStringList filters;
                filters << "*.bmp" << "*.png" << "*.jpg" << "*.jpeg" << "*.gif" << "*.svg" << "*.tif" << "*.tiff";
                dir.setNameFilters(filters);
                QFileInfoList fiList = dir.entryInfoList();
                dir.cdUp();
                for (int i = 0; i < fiList.size(); i++)
                {
                        QListWidgetItem* item = new QListWidgetItem(this);
                        item->setIcon(QIcon(fiList.at(i).absoluteFilePath()));
                        item->setData(NameRole, dir.relativeFilePath(fiList.at(i).absoluteFilePath()));
                }
        }
protected:
private:
};

class IconLoaderThread : public QThread
{
public:
        IconLoaderThread(QHash<QString, QIcon>* iconMap, const QString& path)
                : m_iconMap(iconMap)
                , m_path(path)
        {
                connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
        }
        virtual void run()
        {
                QDir edir(m_path);
                QFileInfoList dirList = edir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
                QStringList filters;
                filters << "*.bmp" << "*.png" << "*.jpg" << "*.jpeg" << "*.gif" << "*.svg" << "*.tif" << "*.tiff";
                QFileInfoList dfileList = edir.entryInfoList(filters, QDir::Files);
                for (int i = 0; i < dfileList.size(); i++)
                {
//                         QString const& key  = pdir.relativeFilePath(dfileList[i].absoluteFilePath());
                        QString const& val = dfileList[i].absoluteFilePath();
                        m_iconMap->insert(val, QIcon(val));
                }

                QString rpath = edir.absolutePath();
                QDir pdir(rpath);
                for (int i = 0; i < dirList.size(); i++)
                {
                        QDir subdir(dirList[i].absoluteFilePath());
                        subdir.setNameFilters(filters);
                        QFileInfoList fiList = subdir.entryInfoList();
                        for (int j = 0; j < fiList.size(); j++)
                        {
//                                 QString const& key  = pdir.relativeFilePath(fiList[j].absoluteFilePath());
                                QString const& val = fiList[j].absoluteFilePath();
                                m_iconMap->insert(val, QIcon(val));
                        }
                }
        }
protected:
private:
        QHash<QString, QIcon>* m_iconMap;
        QString m_path;
};

class EmotionWidget : public QFrame
{
        Q_OBJECT

signals:
        void iconClicked(const QString&);
        void tabAddOK();
public:
        EmotionWidget(QHash<QString, QIcon>* imap)
                :m_imap(imap)
        {
                resize(640, 300);
                setWindowFlags(Qt::Popup);
                tabWidget = new QTabWidget(this);

//                 QDir dir(m_DataAccess.EmotionPath());
//                 QFileInfoList fiList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
//                 for (int i = 0; i < fiList.size(); i++)
//                 {
//                         addTab(fiList[i].absoluteFilePath());
//                 }
                tabWidget->resize(640, 300);
        }

        void addTab(const QString& path)
        {
                QFileInfo finfo(path);
                IconView* iview = new IconView(path);
                tabWidget->addTab(iview, finfo.fileName());
                connect(iview, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(hide()));
                connect(iview, SIGNAL(itemClicked(QListWidgetItem*)), this, SIGNAL(itemClicked(QListWidgetItem*)));
        }

public slots:
        void addTabs()
        {
                setUpdatesEnabled(false);
                QDir qdir(DataAccess::ConfigPath());
                for (QHash<QString, QIcon>::iterator i = m_imap->begin(); i != m_imap->end(); i++)
                {
                        QString const& key = i.key();
                        QString rpath;
                        if (key.startsWith(":"))
                        {
                                rpath = key;
                        } 
                        else
                        {
                                rpath = qdir.relativeFilePath(key);
                        }
                        QIcon const& icon = i.value();
                        QStringList const& keys = rpath.split("/");
                        int siz = keys.size();
                        if (siz < 2)
                        {
                                continue;
                        }

                        IconView* iv = tabMap.value(keys[siz-2], 0);
                        if (!iv)
                        {
                                iv = new IconView;
                                tabWidget->addTab(iv, keys[siz-2]);
                                tabMap.insert(keys[siz-2], iv);
                                connect(iv, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(itemClicked(QListWidgetItem*)));
                        }
                        QListWidgetItem* item = new QListWidgetItem(iv);
                        item->setIcon(icon);
                        item->setData(NameRole, rpath);
                }
                setUpdatesEnabled(true);
                emit tabAddOK();
        }

        void itemClicked(QListWidgetItem* item)
        {
                QString const& key = item->data(NameRole).toString();
                if (key.startsWith(":"))
                {
	                emit iconClicked(key);
                } 
                else
                {
                        emit iconClicked(DataAccess::ConfigPath() + "/" + key);
                }
                hide();
                disconnect(this, 0, 0, 0);
        }
protected:
        virtual void focusOutEvent(QFocusEvent *)
        {
                hide();
                disconnect(this, 0, 0, 0);
        }
        virtual void mouseDoubleClickEvent(QMouseEvent *)
        {
                hide();
                disconnect(this, 0, 0, 0);
        }

private:
        QTabWidget* tabWidget;
        QHash<QString, IconView*> tabMap;
        QHash<QString, QIcon>* m_imap;
};
#endif // __IconView_h__
 
