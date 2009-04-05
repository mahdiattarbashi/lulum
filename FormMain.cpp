/*  filename: FormMain.cpp    2008/12/26  */
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

#include "FormMain.h"
#include "FormTalk.h"
#include "UsersMainView.h"
#include "ConfigStyleWidget.h"
#include "ConfigNetworkWidget.h"
#include "Server.h"
#include "UserItem.h"
#include "TransProtocolData.h"
#include "ShareWidget.h"
#include "IconView.h"

FormMain::FormMain(DataAccess& daccess)
: m_DataAccess(daccess)
, trayIcon(0)
, m_msgWaitTimerStart(false)
, sharedMemory("IPMSN")
{
        COMPUTER_OS = GetComputerOsVer();
        PROGRAM_NAME = tr("LuLu Messenger");

        if (!sharedMemory.create(64))
        {
                QMessageBox::information(0, PROGRAM_NAME, tr("Only one Instance can be running at the same time."));
                ACE_OS::exit(0);
                return;
        }

        m_DataAccess.addFaces();

        DataAccess::DefaultGroupName(tr("My Friends"));

        ConfigStyleWidget cs(m_DataAccess);

        if (!initSocket())
        {
                QMessageBox::critical(this,
                        tr("Error"),
                        tr("Failed to initialize the network interface."),
                        QMessageBox::Close);
                close();
                return;
        }

        loginWidget = createLoginWidget();
        if (m_DataAccess.AutoLogin())
        {
                setCentralWidget(loginWidget);
                autoLoginCheckBox->setChecked(true);
                loginBut->click();
        }
        else
        {
                setCentralWidget(loginWidget);
        }
        this->setWindowIcon(QIcon(DataAccess::LogosPath() + "/tray.png"));
        this->setWindowTitle(PROGRAM_NAME);
}

FormMain::~FormMain(void)
{
        m_DataAccess.sendLogoutMsg();
        if (trayIcon)
        {
                delete trayIcon;
        }
}

bool FormMain::initSocket()
{
        RecvThread* rthread= new RecvThread(m_DataAccess, this);
        rthread->start();
        return true;
}

void
FormMain::slotLogin()
{
        m_DataAccess.currentState = 1;

        m_DataAccess.userItemCollection.local->IconName(iconFaceBut->text());
        m_DataAccess.userItemCollection.local->LoginName(nameComboBox->currentText());
        m_DataAccess.userItemCollection.local->Passwd(passwdLineEdit->text());

        if (m_DataAccess.userItemCollection.local->LoginName() == "")
        {
                QMessageBox::information(this, tr("Tool Tip"),
                        tr("Please input your name first."));
                return;
        }

        this->resize(280, 480);
        setCentralWidget(createMainWidget());

        m_DataAccess.updateUserNode(m_DataAccess.userItemCollection.local->IconName(),
                m_DataAccess.userItemCollection.local->LoginName(),
                m_DataAccess.userItemCollection.local->Passwd());

        test();
        test();
        timerTimeout();
        connect(&m_msgWaitTimer, SIGNAL(timeout()), SLOT(msgWaitTimerTimeout()));
        m_DataAccess.addEmotions();
}

QWidget*
FormMain::createLoginWidget()
{
        QWidget* w = new QFrame();
        QLabel* logo = new QLabel(PROGRAM_NAME);
        qsrand(QTime::currentTime().second());
        int i = 0;
        while (QFile::exists(QString(DataAccess::LogosPath() + "/logo") + (i + '0') + ".png"))
        {
                i++;
        }
        QString logoname;
        if (i == 1)
        {
                logoname = QString(DataAccess::LogosPath() + "/logo0.png");
        }
        else if (i != 0)
        {
                logoname = QString(DataAccess::LogosPath() + "/logo") + (qrand()%i+'0') + ".png";
        }
        logo->setPixmap(QPixmap(logoname));

        createIconList();
        nameCheckBox = new QCheckBox(tr("Use host name as user name"));
        connect(nameCheckBox, SIGNAL(stateChanged(int)), SLOT(nameCheckBoxStateChanged(int)));

        autoLoginCheckBox = new QCheckBox(tr("Auto login at next time"));
        connect(autoLoginCheckBox, SIGNAL(stateChanged(int)), SLOT(autoLoginCheckBoxStateChanged(int)));
        autoLoginCheckBox->setChecked(m_DataAccess.AutoLogin());

        nameComboBox = new QComboBox;
        nameComboBox->setEditable(true);

        passwdLineEdit = new QLineEdit;
        passwdLineEdit->setEchoMode(QLineEdit::Password);

        loginBut = new QPushButton(tr("&Login"));
        connect(loginBut, SIGNAL(clicked()), SLOT(slotLogin()));
        QPushButton* pbutE = new QPushButton(tr("&Quit"));
        connect(pbutE, SIGNAL(clicked()), qApp, SLOT(quit()));

        QVBoxLayout* v = new QVBoxLayout(w);
        QFormLayout* flayout = new QFormLayout;
		flayout->setAlignment(Qt::AlignVCenter);
        v->addWidget(logo);
        flayout->addRow(tr("Select Face:"), iconFaceBut);
        flayout->addRow(tr("Host Name:"), nameCheckBox);
        flayout->addRow(tr("User Name:"), nameComboBox);
        flayout->addRow(tr("Password :"), passwdLineEdit);
        flayout->addRow(pbutE, loginBut);
        flayout->addRow("", autoLoginCheckBox);
        v->addLayout(flayout);

        nameComboBox->addItems(m_DataAccess.nameList);
        passwdLineEdit->setText(m_DataAccess.userItemCollection.local->Passwd());

        w->setWindowTitle(PROGRAM_NAME + tr("Login"));

        return w;
}

QWidget*
FormMain::createMainWidget()
{
        QWidget* cw = new QWidget(this);
        createActions();
        createContextMenus();
        createMainToolBar();
        createTrayIcon();
        textEdit = new QTextEdit;
        userView = new UsersMainView(m_DataAccess);

        QHBoxLayout* h = new QHBoxLayout(cw);

        h->setMargin(2);
        h->addWidget(userView);

        logDock = new QDockWidget(tr("Log"));
        logDock->setAllowedAreas(Qt::RightDockWidgetArea);
        logDock->setWidget(textEdit);
        this->addDockWidget(Qt::RightDockWidgetArea, logDock);

        setWindowTitle(PROGRAM_NAME);
        return cw;
}

void FormMain::test()
{
        logDock->toggleViewAction ()->trigger();
}

void
FormMain::recvMessage()
{
         while (!g_msgQueueBuf.isEmpty())
         {
                 onRecv(g_msgQueueBuf.dequeue());
         }
}

void
FormMain::onRecv(Message_ptr m)
{
        Address from;
        if (From const* f = static_cast<From const*>(m->find(From::id)))
        {
                from = f->address();
        }
        QString const& ip = QString::fromUtf8(from.get_host_addr());

        QString timeStr = "(" + QDateTime::currentDateTime ().toString(Qt::ISODate).replace('T', ' ') + "):";

        if (SignIn const* si = static_cast<SignIn const*>(m->find(SignIn::id)))
        {
                UserItem* uitem = 0;
                if (!static_cast<Ack const*>(m->find(Ack::id)))
                {
                        std::string mac = m_DataAccess.userItemCollection.local->MacAddress().toUtf8().data();
                        std::string host = m_DataAccess.userItemCollection.local->HostName().toUtf8().data();
                        std::string iname = m_DataAccess.userItemCollection.local->IconName().toUtf8().data();
                        std::string name = m_DataAccess.userItemCollection.local->LoginName().toUtf8().data();
                        std::string pwd = m_DataAccess.userItemCollection.local->Passwd().toUtf8().data();
                        Message_ptr m(new Message);
                        m_DataAccess.CreateSignInMsg(m);
                        m->add(Profile_ptr(new Ack(static_cast<SN const*>(m->find(SN::id))->num())));
                        m->add(Profile_ptr (new To(Address(from))));
                        DataAccess::sendMsg(m);
                }

                QString const& ip = QString::fromUtf8(from.get_host_addr());
                QString const& mac = QString::fromAscii(si->property_data(IMProto::MacAddress));
                QString const& host = QString::fromUtf8(si->property_data(IMProto::HostName));
                QString const& iconName = QString::fromUtf8(si->property_data(IMProto::IconName));
                QString const& name = QString::fromUtf8(si->property_data(IMProto::SignName));
                QString const& pwd = QString::fromUtf8(si->property_data(IMProto::Password));
                QString const& osname = QString::fromUtf8(si->property_data(IMProto::OsName));
                QString const& progVer = QString::fromUtf8(si->property_data(IMProto::ProgramVer));
                QString const& protoVer = QString::fromUtf8(si->property_data(IMProto::ProtocolVer));

                if (m_DataAccess.userItemCollection.Collection()->contains(ip))
                {
                        uitem = m_DataAccess.userItemCollection.Collection()->value(ip);
                }
                else
                {
                        if (ip == m_DataAccess.userItemCollection.local->IpAddress())
                        {
                                uitem = new UserItem;
                                uitem->GroupName(tr("MySelf"));
                        }
                        else
                        {
                                uitem = new UserItem(mac);
                                uitem->GroupName(m_DataAccess.DefaultGroupName());
                        }
                        m_DataAccess.userItemCollection.Collection()->insert(ip, uitem);
                }

                if (protoVer < QString::fromUtf8(IMProto::Version))
                {
                        DataAccess::sendTextMsg(tr("The other's protocol version %1 is higher than yours %2."
                                "Please ask for a higher version program from the other.")
                                .arg(IMProto::Version)
                                .arg(protoVer), ip);
                }
                if (progVer < PROGRAM_VER)
                {
                        DataAccess::sendTextMsg(tr("The other's software version %1 is higher than yours %2."
                                "Please ask for a higher version from the other.")
                                .arg(PROGRAM_VER)
                                .arg(progVer), ip);
                }

                uitem->IpAddress(ip);
                uitem->HostName(host);
                uitem->IconName(iconName);
                uitem->LoginName(name);
                uitem->Passwd(pwd);
                uitem->OsName(osname);
                uitem->ProtocolName(PROGRAM_NAME);
                uitem->ProtocolVersion(protoVer);
                uitem->State(UserItem::S_Online);

                if (uitem)
                {
                        userView->UpdateUserTreeItem(uitem);
                        textEdit->append(timeStr + ip + " Login.");
                }
        }
        else if (SignOut const* so = static_cast<SignOut const*>(m->find(SignOut::id)))
        {
                UserItem* uItem = m_DataAccess.userItemCollection.Collection()->value(QString::fromAscii(from.get_host_addr()));
                if (!uItem)
                {
                        return;
                }
                uItem->State(UserItem::S_Offline);
                FormTalk* ft = m_DataAccess.talkDlgMap.value(ip, 0);
                if (ft && ft->isVisible())
                {
                        ft->onRecv(m);
                }

                userView->RemoveUser(uItem);
                textEdit->append(timeStr + ip + " Logout.");
        }
        if (m->find(IMProto::TextNormal)
                || m->find(IMProto::TextZip))
        {
                FormTalk* ft = m_DataAccess.talkDlgMap.value(ip, 0);
                if (ft && ft->isVisible())
                {
                        ft->onRecv(m);
                        return;
                }

                if (m_msgWaitQueue.isEmpty())
                {
                        m_msgWaitIcon = QIcon(DataAccess::SkinPath() + "/chat-one.png");
                }
                m_msgWaitQueue.enqueue(m);
                if (!m_msgWaitTimerStart)
                {
                        m_msgWaitTimer.start(500);
                }
        }
        if (PPictureRecv const* ppr = static_cast<PPictureRecv const*>(m->find(IMProto::PictureRecv)))
        {
                UserItem* uItem = m_DataAccess.userItemCollection.Collection()->value(QString::fromAscii(from.get_host_addr()), 0);
                if (!uItem)
                {
                        return;
                }
                QString const& kName = QString::fromUtf8(ppr->property_data(IMProto::FileKey));
                u16 port = QString::fromUtf8(ppr->property_data(IMProto::ListenPort)).toUShort();

                QString const& fName = uItem->DirMap()->value(kName);
                ProtocolDataTransfer* pdthr = 0;
                pdthr = new ProtocolDataTransfer(0, fName, 0,
                        port, uItem->IpAddress(),
                        ProtocolDataTransfer::DoSendFile);
                pdthr->start();
                return;
        }
        else if (m->find(IMProto::SharedList))
        {
                QStringList keys;
                for (QHash<QString, QString>::iterator i = m_DataAccess.sharingFilesMap.begin(); i != m_DataAccess.sharingFilesMap.end(); i++)
                {
                        QStringList const& finfoVals = ParseFileInfoString(i.value());
                        QString const& ikey = finfoVals[0] + "$" + finfoVals[1] + "$" + i.key();
                        keys << ikey;
                }

                Message_ptr nm(new Message);
                PSharedList* psl = new PSharedList(IMProto::SharedListAck);
                psl->add(IMProto::TextContent, keys.join("/*/").toUtf8().data());
                nm->add(Profile_ptr (psl));
                nm->add(Profile_ptr(new To(from)));
                DataAccess::sendMsg(nm);
        }
        else if (PSharedDownload const* psd = static_cast<PSharedDownload const*>(m->find(IMProto::SharedDownload)))
        {
                u16 port = QString::fromAscii(psd->property_data(IMProto::ListenPort)).toUShort();
                QString const& sn = QString::fromAscii(psd->property_data(IMProto::EventSn));
                QString const& kName = QString::fromUtf8(psd->property_data(IMProto::FileKey));

                QString const& finfoVal = m_DataAccess.sharingFilesMap.value(kName);
                m_DataAccess.sharingFilesWidget->UploadingView()->DataCollection()->insert(kName, finfoVal);

                QString const& gsn = ip + ":" + sn;
                QTreeWidgetItem* dItem = m_DataAccess.sharingFilesWidget->UploadingView()->AddItem(finfoVal);
                m_DataAccess.sharingFilesWidget->UploadingView()->ItemCollection()->insert(gsn, dItem);
                if (!dItem)
                {
                        return;
                }

                QStringList const& keys = ParseFileInfoString(finfoVal);
                QString const& pfName = keys[2];
                if (pfName.isEmpty())
                {
                        //QMessageBox::critical(this, tr("Error"), tr("Unable to find in DirMap key: %1.").arg(keys[2]));
                        return;
                }
                QFileInfo finfo(pfName);

                ProtocolDataTransfer* pdthr = 0;
                if (finfo.isFile())
                {
                        pdthr = new ProtocolDataTransfer(dItem, pfName, 0,
                                port, ip,
                                ProtocolDataTransfer::DoSendFile);
                }
                else  if (finfo.isDir())
                {
                        pdthr = new ProtocolDataTransfer(dItem, pfName,
                                port, ip,
                                ProtocolDataTransfer::DoSendDir);
                }
                if (!pdthr)
                {
                        return;
                }
                dItem->setText(COL_TARGET, ip);
                UserItem* uItem = m_DataAccess.userItemCollection.Collection()->value(QString::fromAscii(from.get_host_addr()), 0);
                if (uItem)
                {
                        dItem->setText(COL_TARGET, uItem->LoginName() + ":" + ip);
                }
                dItem->setData(COL_NAME, UserItemRole, ip);
                dItem->setData(COL_NAME, TransThreadRole, (quint64)pdthr);
                dItem->setData(COL_NAME, TransStateRole, "START");
                dItem->setData(COL_NAME, TransItemSNRole, gsn);
                dItem->setData(COL_NAME, FileKeyRole, kName);
                dItem->setData(COL_NAME, PathNameRole, pfName);
                connect(&m_DataAccess.m_secTimer, SIGNAL(timeout()), pdthr, SLOT(slotUpdate()));
                pdthr->start();

        }
        else if (PSharedDownloadCancel const* psdc = static_cast<PSharedDownloadCancel const*>(m->find(IMProto::SharedDownloadCancel)))
        {
                QString const& sn = QString::fromAscii(psdc->property_data(IMProto::EventSn));
                QString const& gsn = ip + ":" + sn;

                QTreeWidgetItem* dItem = m_DataAccess.sharingFilesWidget->UploadingView()->ItemCollection()->value(gsn, 0);
                if (!dItem)
                {
                        return;
                }
                if (dItem->data(COL_NAME, TransStateRole).toString() == "START")
                {
                        ProtocolDataTransfer* t = (ProtocolDataTransfer*)dItem->data(COL_NAME, TransThreadRole).toULongLong();
                        disconnect(&m_DataAccess.m_secTimer, SIGNAL(timeout()), t, SLOT(slotUpdate()));
                        t->slotCancel();
                }

                if (!delItemWhenFinished)
                {
                        dItem->setData(COL_NAME, TransStateRole, "CANCEL");
                        dItem->setIcon(COL_REJECT, QIcon());
                        dItem->setText(COL_REJECT, "");
                        dItem->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/warning.png"));
                        dItem->setText(COL_STATE, tr("Canceled"));
                }
                else
                {
                        m_DataAccess.sharingFilesWidget->UploadingView()->takeTopLevelItem(m_DataAccess.sharingFilesWidget->UploadingView()->indexOfTopLevelItem(dItem));
                }
        }
        else if (m->find(IMProto::SharedListAck)
                || m->find(IMProto::SharedUploadCancel))
        {
                FormTalk* ft = m_DataAccess.talkDlgMap.value(ip, 0);
                if (ft)
                {
                        ft->onRecv(m);
                        return;
                }
        }
        else if (m->find(IMProto::FileSend)
                || m->find(IMProto::DirFileRecv)
                || m->find(IMProto::DirSend)
                || m->find(IMProto::PictureSend)
                )
        {
                FormTalk* ft = m_DataAccess.talkDlgMap.value(ip, 0);
                if (ft)
                {
                        if (ft->isVisible())
                        {
                                ft->onRecv(m);
                        }
                        return;
                }

                if (m_msgWaitQueue.isEmpty())
                {
                        m_msgWaitIcon = QIcon(DataAccess::SkinPath() + "/chat-recv.png");
                }
                m_msgWaitQueue.enqueue(m);
                if (!m_msgWaitTimerStart)
                {
                        m_msgWaitTimer.start(500);
                }
        }
        else if (m->find(IMProto::DirFileSendCancel)
                || m->find(IMProto::DirFileRecvCancel)
                )
        {
                FormTalk* ft = m_DataAccess.talkDlgMap.value(ip, 0);
                if (ft)
                {
                        ft->onRecv(m);
                }
        }
}

void
FormMain::closeEvent( QCloseEvent* event)
{
        if (trayIcon && trayIcon->isVisible())
        {
                hide();
                event->ignore();
        }
        else
        {
                qApp->quit();
        }
}

void
FormMain::createActions()
{
        actLangCN = new QAction(tr("Chinese"), this);
        actLangCN->setCheckable(true);
        actLangEN = new QAction(tr("English"), this);
        actLangEN->setCheckable(true);

        actMinimize = new QAction(tr("Mi&nimize"), this);
        connect(actMinimize, SIGNAL(triggered()), this, SLOT(hide()));

        actMaximize = new QAction(tr("Ma&ximize"), this);
        connect(actMaximize, SIGNAL(triggered()), this, SLOT(showMaximized()));

        actRestore = new QAction(tr("&Restore"), this);
        connect(actRestore, SIGNAL(triggered()), this, SLOT(showNormal()));

        actQuit = new QAction(QIcon(DataAccess::SkinPath() + "/exit.png"), tr("&Quit"), this);
        actQuit->setIconText(tr("Quit"));
        actQuit->setShortcut(tr("Ctrl+Q"));
        actQuit->setToolTip(tr("Quit the program"));
        connect(actQuit, SIGNAL(triggered()), qApp, SLOT(quit()));

        actShowLog = new QAction(QIcon(DataAccess::SkinPath() + "/log.png"), tr("&History Log"), this);
        actShowLog->setIconText(tr("History Log"));
        actShowLog->setShortcut(tr("Ctrl+H"));
        actShowLog->setToolTip(tr("Show or hide log window"));
        connect(actShowLog, SIGNAL(triggered()), this, SLOT(test()));

        actAbout = new QAction(QIcon(DataAccess::SkinPath() + "/help.png"), tr("&About"), this);
        actAbout->setToolTip(tr("Show the program information"));
        connect(actAbout, SIGNAL(triggered()), this, SLOT(slotAbout()));

        actINetMailToUserGroup = new QAction(tr("Send &Mail to User Group"), this);
        actINetForum = new QAction(tr("Online &Forum"), this);
        actINetCheckUpdate = new QAction(tr("Check &Update"), this);
        actINetCommitIssues = new QAction(tr("Commit &Issues"), this);
        actINetJoinUserGroups = new QAction(tr("Join User &Groups"), this);


        actShowLoginWin = new QAction(QIcon(DataAccess::SkinPath() + "/options.png"), tr("Show Login &Window"), this);
        actShowLoginWin->setIconText(tr("Show Login Window"));
        actShowLoginWin->setShortcut(tr("Ctrl+W"));
        actShowLoginWin->setToolTip(tr("Show Login Window"));
        connect(actShowLoginWin, SIGNAL(triggered()), this, SLOT(slotShowLoginWindow()));

        actReLogin = new QAction(QIcon(DataAccess::SkinPath() + "/go.png"), tr("&Login"), this);
        actReLogin->setIconText(tr("Refresh Login"));
        actReLogin->setShortcut(tr("Ctrl+L"));
        actReLogin->setToolTip(tr("Login to network"));
        connect(actReLogin, SIGNAL(triggered()), this, SLOT(timerTimeout()));

        actConfigStyle = new QAction(QIcon(DataAccess::SkinPath() + "/options.png"), tr("&Style Configure"), this);
        actConfigStyle->setShortcut(tr("Ctrl+S"));
        actConfigStyle->setToolTip(tr("Show style config dialog"));
        connect(actConfigStyle, SIGNAL(triggered()), this, SLOT(slotConfigStyle()));

        actConfigNetwork = new QAction(QIcon(DataAccess::SkinPath() + "/option-network.png"), tr("&Network Configure"), this);
        actConfigNetwork->setShortcut(tr("Ctrl+N"));
        actConfigNetwork->setToolTip(tr("Show network config dialog"));
        connect(actConfigNetwork, SIGNAL(triggered()), this, SLOT(slotConfigNetwork()));

        actSendToOne = new QAction(QIcon(DataAccess::SkinPath() + "/chat-one.png"), tr("Send To &One User"), this);
        actSendToOne->setShortcut(tr("Ctrl+O"));
        actSendToOne->setToolTip(tr("Send message to an appointed user"));
        connect(actSendToOne, SIGNAL(triggered()), this, SLOT(slotSendToOneUser()));

        actSendToAll = new QAction(QIcon(DataAccess::SkinPath() + "/chat-multi.png"), tr("Send To &All Users"), this);
        actSendToAll->setShortcut(tr("Ctrl+A"));
        actSendToAll->setToolTip(tr("Send message to all users"));
        connect(actSendToAll, SIGNAL(triggered()), this, SLOT(slotSendToAllUsers()));

        actSetupShare = new QAction(QIcon(DataAccess::SkinPath() + "/share.png"), tr("Config Sharing"), this);
        actSetupShare->setShortcut(tr("Ctrl+E"));
        actSetupShare->setToolTip(tr("Config Sharing Files And Directories"));
        connect(actSetupShare, SIGNAL(triggered()), &m_DataAccess, SLOT(slotSharingConfig()));
}

void
FormMain::slotShowLoginWindow()
{
        loginWidget = createLoginWidget();
        loginWidget->show();
}

void
FormMain::slotAbout()
{
        AboutDialog ad;
        ad.exec();
}

void
FormMain::createContextMenus()
{
        menuLanguage = new QMenu(tr("Language"));
        menuLanguage->addAction(actLangCN);
        menuLanguage->addAction(actLangEN);
        connect(menuLanguage, SIGNAL(triggered(QAction*)), this, SLOT(slotMenuLanguageTriggered(QAction*)));

        QMenu* fileMenu = new QMenu(tr("&File"));
        fileMenu->addAction(actReLogin);
        fileMenu->addAction(actShowLoginWin);
        fileMenu->addAction(actConfigNetwork);
        fileMenu->addAction(actConfigStyle);
        fileMenu->addAction(actSetupShare);
        fileMenu->addSeparator();
        fileMenu->addAction(actSendToOne);
        fileMenu->addAction(actSendToAll);
        fileMenu->addSeparator();
        fileMenu->addMenu(menuLanguage);
        fileMenu->addAction(actQuit);
        QMenu* helpMenu = new QMenu(tr("&Help"));
        helpMenu->addAction(actShowLog);
        helpMenu->addSeparator();
        helpMenu->addAction(actINetCheckUpdate);
        helpMenu->addAction(actINetForum);
        helpMenu->addAction(actINetMailToUserGroup);
        helpMenu->addAction(actINetJoinUserGroups);
        helpMenu->addAction(actINetCommitIssues);
        helpMenu->addSeparator();
        helpMenu->addAction(actAbout);
        connect(helpMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotMenuLanguageTriggered(QAction*)));


        QMenuBar* menu = new QMenuBar(this);
        menu->addMenu(fileMenu);
        menu->addMenu(helpMenu);
        setMenuBar(menu);

        addAction(actSendToOne);
        addAction(actSendToAll);
        addAction(actReLogin);
        addAction(actQuit);
        setContextMenuPolicy(Qt::ActionsContextMenu);
}

void
FormMain::createMainToolBar()
{
        mainToolBar = new QToolBar("Main");
        mainToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        mainToolBar->addAction(actQuit);
        mainToolBar->addAction(actReLogin);
        mainToolBar->addAction(actSetupShare);
        mainToolBar->addAction(actShowLog);
        this->addToolBar(mainToolBar);
}

void
FormMain::createTrayIcon()
{
        trayIconMenu = new QMenu(this);
        trayIconMenu->addAction(actMinimize);
        trayIconMenu->addAction(actMaximize);
        trayIconMenu->addAction(actRestore);
        trayIconMenu->addSeparator();
        trayIconMenu->addAction(actQuit);

        trayIcon = new QSystemTrayIcon(this);
        trayIcon->setToolTip(PROGRAM_NAME +
                ":" + m_DataAccess.userItemCollection.local->LoginName() +
                ":" + m_DataAccess.userItemCollection.local->IpAddress());
        trayIcon->setContextMenu(trayIconMenu);
        trayIcon->setIcon(QIcon(DataAccess::LogosPath() + "/tray.png"));
        trayIcon->show();

        connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
}

void
FormMain::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
        switch (reason)
        {
        case QSystemTrayIcon::Trigger:
                while (!m_msgWaitQueue.isEmpty())
                {
                        Message_ptr m = m_msgWaitQueue.dequeue();
                        if (From const* f = static_cast<From const*>(m->find(From::id)))
                        {
                                Address from = f->address();
                                QString const& ip = QString::fromUtf8(from.get_host_addr());
                                m_DataAccess.getFormTalk(ip)->onRecv(m);
                        }
                }
                break;
        case QSystemTrayIcon::DoubleClick:
                this->show();
                this->setFocus();
                break;
        case QSystemTrayIcon::MiddleClick:
                break;
        default:
                ;
        }
}

void
FormMain::slotIconFaceItemClicked(const QString& file)
{
        iconFaceBut->setIcon(QIcon(file));
        iconFaceBut->setText(file);
        m_DataAccess.updateUserNode(file,
                m_DataAccess.userItemCollection.local->LoginName(),
                m_DataAccess.userItemCollection.local->Passwd());

}

void
FormMain::slotMenuLanguageTriggered(QAction* act)
{
        if (act == actINetCheckUpdate)
        {
                if (QDesktopServices::openUrl(QUrl("http://www.wu266.com")))
                {
	                return;
                }
        }
        else if (act == actINetMailToUserGroup)
        {
                QString lang = m_DataAccess.Language();
                if (QDesktopServices::openUrl(QUrl("mailto:lulum@googlegroups.com?subject=[lulu-messenger]&body=hi:")))
                {
                        return;
                }
        }
        else if (act == actINetForum)
        {
                QString lang = m_DataAccess.Language();
                if (QDesktopServices::openUrl(QUrl("http://www.wu266.com")))
                {
                        return;
                }
        }
        else if (act == actINetJoinUserGroups)
        {
                QString lang = m_DataAccess.Language();
                if (QDesktopServices::openUrl(QUrl("http://www.wu266.com")))
                {
	                return;
                }
        }
        else if (act == actINetCommitIssues)
        {
                if (QDesktopServices::openUrl(QUrl("http://code.google.com/p/lulum/issues/entry")))
                {
	                return;
                }
        }
        else if (act->text() == tr("Chinese"))
        {
                actLangEN->setChecked(false);

                m_DataAccess.Language("zh_CN");
                QMessageBox::information(this, tr("Tips"), tr("You need restart the program to take effect."));
        }
        else if (act->text() == tr("English"))
        {
                actLangCN->setChecked(false);
                m_DataAccess.Language("en_US");
                QMessageBox::information(this, tr("Tips"), tr("You need restart the program to take effect."));
        }
}

void
FormMain::createIconList()
{
        iconFaceBut = new QToolButton;
        iconFaceBut->setIconSize(QSize(160, 40));
        iconFaceBut->setIcon(QIcon(m_DataAccess.userItemCollection.local->IconName()));
        iconFaceBut->setText(m_DataAccess.userItemCollection.local->IconName());
        connect(iconFaceBut, SIGNAL(clicked()), m_DataAccess.faceView, SLOT(show()));
        connect(m_DataAccess.faceView, SIGNAL(iconClicked(const QString&)), this, SLOT(slotIconFaceItemClicked(const QString&)));

        QString const& path = m_DataAccess.FacePath();
        QDir imagesDir(path);
        QStringList nameList = imagesDir.entryList(QStringList("*.png"), QDir::Files, QDir::Name);
        int listSize = nameList.size();
        for (int i = 0; i < listSize; i++)
        {
                QString name = nameList.at(i);
        }
}

void
FormMain::nameCheckBoxStateChanged(int state)
{
        if (state == Qt::Checked)
        {
                int idx = nameComboBox->findText(m_DataAccess.userItemCollection.local->HostName());
                if (idx == -1)
                {
                        nameComboBox->addItem(m_DataAccess.userItemCollection.local->HostName());
                        idx = nameComboBox->findText(m_DataAccess.userItemCollection.local->HostName());
                }
                nameComboBox->setCurrentIndex(idx);
                nameComboBox->setEnabled(false);
        }
        else
        {
                nameComboBox->setEnabled(true);
        }
}

void
FormMain::autoLoginCheckBoxStateChanged(int state)
{
        if (state == Qt::Checked)
        {
                m_DataAccess.AutoLogin(true);
        }
        else
        {
                m_DataAccess.AutoLogin(false);
        }
}

void
FormMain::timerTimeout()
{
        m_DataAccess.sendLoginMsg();
}

void
FormMain::msgWaitTimerTimeout()
{
        for (int i = 0; i < m_msgWaitQueue.size(); i++)
        {
                Message_ptr m = m_msgWaitQueue.dequeue();
                if (From const* f = static_cast<From const*>(m->find(From::id)))
                {
                        Address from = f->address();
                        QString const& ip = QString::fromUtf8(from.get_host_addr());
                        FormTalk* ft = m_DataAccess.talkDlgMap.value(ip, 0);
                        if (ft && ft->isVisible())
                        {
	                        m_DataAccess.getFormTalk(ip)->onRecv(m);
                        }
                        else
                        {
                                m_msgWaitQueue.enqueue(m);
                        }
                }
        }

        static bool showIcon = true;
        if (showIcon)
        {
	        trayIcon->setIcon(m_msgWaitIcon);
                showIcon = false;
        }
        else
        {
                trayIcon->setIcon(QIcon(DataAccess::LogosPath() + "/tray.png"));
                showIcon = true;
        }

        if (m_msgWaitQueue.isEmpty())
        {
                m_msgWaitTimer.stop();
                trayIcon->setIcon(QIcon(DataAccess::LogosPath() + "/tray.png"));
                trayIcon->show();
        }
}

void
FormMain::slotConfigStyle()
{
        ConfigStyleWidget* csw = new ConfigStyleWidget(m_DataAccess);
        csw->exec();
        delete csw;
}

void
FormMain::slotConfigNetwork()
{
        ConfigNetworkWidget* csw = new ConfigNetworkWidget(m_DataAccess);
        csw->exec();
        delete csw;
}

void
FormMain::slotSendToOneUser()
{
        QDialog dlg;

        dlg.setWindowTitle(tr("Send message to an appointed user"));
        QComboBox* ipEdit = new QComboBox;
        ipEdit->setEditable(true);
        QFormLayout* formLayout = new QFormLayout;
        formLayout->addRow(tr("Send To") + " IP:", ipEdit);

        QTextEdit* tEdit = new QTextEdit;
        tEdit->setFocus();

        QDialogButtonBox* dbutbox = new QDialogButtonBox(QDialogButtonBox::Ok);
        connect(dbutbox, SIGNAL(accepted()), &dlg, SLOT(accept()));

        QVBoxLayout* vlayout = new QVBoxLayout(&dlg);
        vlayout->addLayout(formLayout);
        vlayout->addWidget(tEdit);
        vlayout->addWidget(dbutbox);

        if (dlg.exec())
        {
                const QString& text = tEdit->toPlainText();
                if (text == "")
                {
                        return;
                }
                m_DataAccess.sendTextMsg(text, ipEdit->currentText());
        }
}

void
FormMain::slotSendToAllUsers()
{
        QDialog dlg;

        dlg.setWindowTitle(tr("Send message to All users on your network"));
        QLabel* lbl = new QLabel(tr("Message Contents:"));
        QTextEdit* tEdit = new QTextEdit;
        tEdit->setFocus();

        QDialogButtonBox* dbutbox = new QDialogButtonBox(QDialogButtonBox::Ok);
        connect(dbutbox, SIGNAL(accepted()), &dlg, SLOT(accept()));

        QVBoxLayout* vlayout = new QVBoxLayout(&dlg);
        vlayout->addWidget(lbl);
        vlayout->addWidget(tEdit);
        vlayout->addWidget(dbutbox);

        if (dlg.exec())
        {
                const QString& text = tEdit->toPlainText();
                if (text == "")
                {
                        return;
                }
                else if (text.size() >= (PayloadSize - 1024))
                {
                        QMessageBox::warning(this, tr("Warning"), tr("The message to be sent is too lang."));
                        return;
                }
                Message_ptr m(new Message);
                m_DataAccess.CreateTextMsg(text, m);
                DataAccess::BCastMsg(m);
        }
}
