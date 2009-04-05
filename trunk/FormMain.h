/*  filename: FormMain.h    2008/12/26  */
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

#ifndef __FormMain_h__
#define __FormMain_h__

#include "defs.h"
#include "DataAccess.h"

using namespace IPM;

class AboutDialog : public QDialog
{
        Q_OBJECT

public:
        AboutDialog(QWidget* parent = 0)
                : QDialog(parent)
        {
                QLabel* logo = new QLabel(PROGRAM_NAME);
                QString logoname = QString(DataAccess::LogosPath() + "/logo0.png");
                logo->setPixmap(QPixmap(logoname));

                QLabel* name = new QLabel("<b>" + PROGRAM_NAME + " V" + PROGRAM_VER + "</b>");
                QLabel* disp = new QLabel(tr("The <b>%1</b> program is a tool\r\n"
                        "for LAN communication.\r\n").arg(PROGRAM_NAME));
                disp->setWordWrap(true);
                QLabel* cpright = new QLabel(tr("Copy Right (C) 2009 Wu Weisheng <wwssir@gmail.com>"));

                QDialogButtonBox* dbutbox = new QDialogButtonBox(QDialogButtonBox::Ok);
                dbutbox->setCenterButtons(true);
                connect(dbutbox, SIGNAL(accepted()), this, SLOT(accept()));

                QVBoxLayout* v = new QVBoxLayout(this);
                v->addWidget(logo);
                v->addWidget(name);
                v->addWidget(disp);
                v->addWidget(cpright);
                v->addSpacing(40);
                v->addWidget(dbutbox);

                setWindowTitle(tr("About")+PROGRAM_NAME);
        }
protected:
private:
};

class UsersMainView;
class FormMain;
class UserItem;

class FormMain : public QMainWindow
{
        Q_OBJECT

public slots:
        void test();
        void slotShowLoginWindow();
        void recvMessage();
        void onRecv(Message_ptr m);

public:
        FormMain(DataAccess& daccess);
        ~FormMain(void);

protected:
        virtual void closeEvent ( QCloseEvent* event);
        bool initSocket();

        QWidget* createLoginWidget();
        QWidget* createMainWidget();
        void createIconList();
        void createActions();
        void createTrayIcon();
        void createMainToolBar();
        void createContextMenus();

private slots:
        void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
        void nameCheckBoxStateChanged(int state);
        void autoLoginCheckBoxStateChanged(int state);
        void timerTimeout();
        void msgWaitTimerTimeout();
        void slotLogin();
        void slotAbout();
        void slotConfigStyle();
        void slotConfigNetwork();
        void slotSendToOneUser();
        void slotSendToAllUsers();
        void slotIconFaceItemClicked(const QString&);
        void slotMenuLanguageTriggered(QAction*);
private:

        QWidget* loginWidget;
        QPushButton* loginBut;
        QComboBox* nameComboBox;
        QCheckBox* nameCheckBox;
        QCheckBox* autoLoginCheckBox;
        QLineEdit* passwdLineEdit;
        QToolBar* mainToolBar;
        QTextEdit* textEdit;
        UsersMainView* userView;
        QDockWidget* logDock;
        QToolButton* iconFaceBut;

        QAction *actMinimize;
        QAction *actMaximize;
        QAction *actRestore;
        QAction *actAbout;
        QAction *actQuit;
        QAction *actShowLog;
        QAction *actReLogin;
        QAction *actShowLoginWin;
        QAction *actConfigStyle;
        QAction *actConfigNetwork;
        QAction *actSendToOne;
        QAction *actSendToAll;

        QAction *actLangCN;
        QAction *actLangEN;
        QAction *actSetupShare;
        QAction *actINetForum;
        QAction *actINetMailToUserGroup;
        QAction *actINetCheckUpdate;
        QAction *actINetCommitIssues;
        QAction *actINetJoinUserGroups;
        QMenu* menuLanguage;

        QSystemTrayIcon *trayIcon;
        QMenu *trayIconMenu;

        QQueue<IMProto::Message_ptr> m_msgWaitQueue;
        QIcon m_msgWaitIcon;
        QTimer m_msgWaitTimer;
        bool m_msgWaitTimerStart;

        DataAccess& m_DataAccess;
        QSharedMemory sharedMemory;
};


#endif // __FormMain_h__
