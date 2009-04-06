/*  filename: ScreenShot.h    2009/4/6 10:35  */
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

#ifndef __ScreenShot_h__
#define __ScreenShot_h__

#include "defs.h"
#include "DataAccess.h"

class ScreenShot : public QLabel
{
        Q_OBJECT
signals:
        void sigCaptured(const QString& pathname);
public:
        ScreenShot(QWidget* parent = 0)
                : QLabel(parent)
        {
                originalPixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
                this->setPixmap(originalPixmap);
                this->showFullScreen();
        }
public:
protected:
        virtual void mousePressEvent(QMouseEvent* event)
        {
                m_topLeft = event->globalPos();
        }

        virtual void mouseReleaseEvent(QMouseEvent* event)
        {
                if (m_topLeft != m_bottomRight)
                {
                        QPixmap c = originalPixmap.copy(m_topLeft.x(), m_topLeft.y(),
                                m_bottomRight.x()-m_topLeft.x(),
                                m_bottomRight.y()-m_topLeft.y());
                        QDir dir;
                        if (!dir.exists(DataAccess::ConfigPath() +"/CapImages"))
                        {
                                dir.mkdir(DataAccess::ConfigPath() +"/CapImages");
                        }
                        QString const& pathName = DataAccess::ConfigPath() +"/CapImages"
                                + "/ScreenShot-" + QDateTime::currentDateTime().toString(Qt::ISODate).replace(":", "-") + ".jpg";
                        c.save(pathName, "jpeg");

                        emit sigCaptured(pathName);
                        this->close();
                }
        }
        virtual void mouseMoveEvent(QMouseEvent* event)
        {
                m_bottomRight = event->globalPos();
                update();
        }
        virtual void mouseDoubleClickEvent(QMouseEvent* event)
        {
                this->close();
        }
        virtual void keyPressEvent(QKeyEvent* event)
        {
                if (event->key() == 32
                        || event->key() == 27)
                {
                        this->close();
                }
        }

        virtual void paintEvent(QPaintEvent *event)
        {
                QLabel::paintEvent(event);
                QPainter painter(this);
                painter.setPen(Qt::DashDotLine);
                QRect rect(m_topLeft, m_bottomRight);
                painter.drawRect(rect);
        }
private:
        QPoint m_topLeft;
        QPoint m_bottomRight;
        QPixmap originalPixmap;
};

#endif // __ScreenShot_h__
