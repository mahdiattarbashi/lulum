/*  filename: FormTalk.h    2008/12/26  */
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

#ifndef __FormTalk_h__
#define __FormTalk_h__

#include "defs.h"
#include "DataAccess.h"

class EmotionWidget;
class ProtocolDataWidget;
class UserSelectView;
class SharedWatchWidget;

class FormTalk : public QFrame
{
	Q_OBJECT
signals:
        void sigMyClose();
public:
	FormTalk(DataAccess& dacess, UserItem* i);
	~FormTalk(void);

public slots:
        void slotMyClose();
        void slotSend();
        void slotShowLog(bool);
        void slotShowUserInfo(const QString& ip);
        void slotTextFont();
        void slotTextColor();
        void slotSendEmotion();
        void slotSendPicture();
        void slotSendPicture(const QString& pathname);
        void slotCaptureScreen();
        void slotSendFile();
        void slotSendDir();
        void slotCopySend(bool);
        void slotViewShare();
        void onRecv(Message_ptr m);
        void slotRecvPictureReady();

        void slotEmotionViewItemDClicked(const QString&);

        void slotSetView(bool type = 0);
        void slotSetViewFullScreen(bool);

        void slotBrowaserAnchorClicked(const QUrl& link);
        void addTipsToTextBrowser(const QString& tips);

protected:
        virtual void mousePressEvent(QMouseEvent *);
        virtual void closeEvent ( QCloseEvent* event);
        void createActions();
        void setupMainWidget();
        void setupMainToolBar();
        void setupEditToolBar();
        void setupFileTransWidget();

private:
        void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

	QTabWidget* tabWidget;
        QWidget* logWidget;
        QTextBrowser* logBrowser;
	QTextBrowser* textBrowser;
	QTextEdit* textEdit;
        QSplitter* leftVSplitter;
        QSplitter* mainHSplitter;
        QVBoxLayout* mainVLayout;
        QVBoxLayout* leftLayout;
        QVBoxLayout* rightLayout;
        QWidget* leftWidget;
        QWidget* rightWidget;
        QLabel* userLabel;
        QToolBar* editToolBar;
        QColor textEditColor;
        ProtocolDataWidget* fileTransWidget;
        EmotionWidget* emotionView;
        UserSelectView* userSelectWidget;
        SharedWatchWidget* filesSharedWidget;

        QAction* actShowLog;
        QAction* actTextColor;
        QAction* actTextFont;
        QAction* actSendEmotion;
        QAction* actSendPicture;
        QAction* actScreenShot;
        QAction* actSendFile;
        QAction* actSendDir;

        QAction* actSetViewLayout;
        QAction* actShowFullScreen;
        QAction* actSendMsg;
        QAction* actClose;

        QAction* actCopySend;
        QAction* actViewShare;

        UserItem* m_userItem;
        DataAccess& m_DataAccess;

        QTimer m_TransInfoTimer;

        int m_viewType;
        QSize histSize;
        QQueue<IMProto::Message_ptr> m_imagesToSend;
};

#endif // __FormTalk_h__
