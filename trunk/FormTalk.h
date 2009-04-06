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
