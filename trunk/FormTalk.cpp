/*  filename: FormTalk.cpp    2008/12/26  */
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

#include "FormTalk.h"
#include "FormMain.h"
#include "TransProtocolData.h"
#include "ShareWidget.h"
#include "ScreenShot.h"
#include "UserItem.h"
#include "UserSelectView.h"
#include "IconView.h"
#include "zlib.h"

//////////////////////////////////////////////////////////////////////////
FormTalk::FormTalk(DataAccess& dacess, UserItem* i)
: m_userItem(i)
, m_DataAccess(dacess)
, m_viewType(0)
, filesSharedWidget(0)
{
        setFrameStyle(Sunken | StyledPanel);
        editToolBar = new QToolBar(tr("Text Editor"));
        leftVSplitter = new QSplitter(Qt::Vertical);
        mainVLayout = new QVBoxLayout(this);
        mainVLayout->setMargin(3);
        mainHSplitter = new QSplitter(Qt::Horizontal);
        leftLayout = new QVBoxLayout;
        rightWidget = new QWidget;
        rightLayout = new QVBoxLayout;
        rightLayout->addWidget(rightWidget);
//         mainVLayout->setMargin(2);
        leftWidget = new QWidget;
        rightWidget = new QWidget;
        leftLayout = new QVBoxLayout(leftWidget);
        rightLayout = new QVBoxLayout(rightWidget);
        leftLayout->setMargin(0);
        rightLayout->setMargin(0);

        mainHSplitter->addWidget(leftWidget);
        mainHSplitter->addWidget(rightWidget);
        mainVLayout->addWidget(mainHSplitter);

        createActions();
        setupMainToolBar();
        setupEditToolBar();
        setupMainWidget();
        setupFileTransWidget();

        QList<int> siz;
        siz << 0 << 200 << 80;
        leftVSplitter->setSizes(siz);

        resize(600, 450);
        textEdit->setFocus();
        textEdit->insertHtml("<b></b>");
        QTextCharFormat fmt;
        fmt.setFontPointSize(12.0);
        mergeFormatOnWordOrSelection(fmt);

        connect(this, SIGNAL(sigMyClose()), this, SLOT(slotMyClose()));
}

FormTalk::~FormTalk(void)
{
        if (filesSharedWidget)
        {
	        delete filesSharedWidget;
        }
}

void
FormTalk::createActions()
{
        actShowLog = new QAction(QIcon(DataAccess::SkinPath() + "/log.png"), tr("Chat &Log"), this);
        actShowLog->setCheckable(true);
        actShowLog->setIconText(tr("Chat Log"));
        actShowLog->setShortcut(tr("Ctrl+L"));
        actShowLog->setToolTip(tr("Show/Hide Chat Log"));
        connect(actShowLog, SIGNAL(triggered(bool)), SLOT(slotShowLog(bool)));

        // edit tool bar
        actTextFont = new QAction(QIcon(DataAccess::SkinPath() + "/font.png"), tr("Fon&t"), this);
        actTextFont->setIconText(tr("Font"));
        actTextFont->setShortcut(tr("Ctrl+T"));
        actTextFont->setToolTip(tr("Setup Text Font"));
        connect(actTextFont, SIGNAL(triggered()), SLOT(slotTextFont()));

        actTextColor = new QAction(QIcon(DataAccess::SkinPath() + "/text-color.png"), tr("&Color"), this);
        actTextColor->setIconText(tr("Color"));
        actTextColor->setShortcut(tr("Ctrl+Alt+C"));
        actTextColor->setToolTip(tr("Setup text color"));
        connect(actTextColor, SIGNAL(triggered()), SLOT(slotTextColor()));

        actSendEmotion = new QAction(QIcon(DataAccess::SkinPath() + "/emotion.png"), tr("Send &Emotion"), this);
        actSendEmotion->setIconText(tr("Send Emotion"));
        actSendEmotion->setShortcut(tr("Ctrl+E"));
        actSendEmotion->setToolTip(tr("Send a emotion to the other"));
        connect(actSendEmotion, SIGNAL(triggered()), this, SLOT(slotSendEmotion()));

        actSendPicture = new QAction(QIcon(DataAccess::SkinPath() + "/send-picture.png"), tr("Send P&icture"), this);
        actSendPicture->setIconText(tr("Send Picture"));
        actSendPicture->setShortcut(tr("Ctrl+P"));
        actSendPicture->setToolTip(tr("Send a picture to the other"));
        connect(actSendPicture, SIGNAL(triggered()), this, SLOT(slotSendPicture()));

        actScreenShot = new QAction(QIcon(DataAccess::SkinPath() + "/capture.png"), tr("Capture Screen"), this);
        actScreenShot->setIconText(tr("Capture Screen"));
        actScreenShot->setToolTip(tr("Capture screen and insert to edit box"));
        connect(actScreenShot, SIGNAL(triggered()), this, SLOT(slotCaptureScreen()));

        actSendFile = new QAction(QIcon(DataAccess::SkinPath() + "/send-files.png"), tr("Send &File"), this);
        actSendFile->setIconText(tr("Send File"));
        actSendFile->setShortcut(tr("Ctrl+F"));
        actSendFile->setToolTip(tr("Send some files to the other"));
        connect(actSendFile, SIGNAL(triggered()), SLOT(slotSendFile()));

        actSendDir = new QAction(QIcon(DataAccess::SkinPath() + "/send-folder.png"), tr("Send &Directory"), this);
        actSendDir->setIconText(tr("Send Directory"));
        actSendDir->setShortcut(tr("Ctrl+D"));
        actSendDir->setToolTip(tr("Send a directory to the other"));
        connect(actSendDir, SIGNAL(triggered()), SLOT(slotSendDir()));

        // layout
        actSetViewLayout = new QAction(QIcon(DataAccess::SkinPath() + "/layout.png"), tr("Layout"), this);
        actSetViewLayout->setShortcut(tr("Ctrl+Alt+X"));
        connect(actSetViewLayout, SIGNAL(triggered(bool)), this, SLOT(slotSetView(bool)));

        actShowFullScreen = new QAction(QIcon(DataAccess::SkinPath() + "/view-fullscreen.png"), tr("Full Screen"), this);
        actShowFullScreen->setShortcut(tr("Ctrl+Alt+Z"));
        actShowFullScreen->setCheckable(true);
        actShowFullScreen->setToolTip(tr("Set/Cancel full screen mode"));
        connect(actShowFullScreen, SIGNAL(triggered(bool)), SLOT(slotSetViewFullScreen(bool)));

        actSendMsg = new QAction(tr("&Send"), this);
        connect(actSendMsg, SIGNAL(triggered()), SLOT(slotSend()));
        actClose = new QAction(tr("&Close"), this);
        connect(actClose, SIGNAL(triggered()), SLOT(close()));

        QString fName = DataAccess::SkinPath() + "/copy-send-" + m_DataAccess.Language() + ".png";
        if (!QFile::exists(fName))
        {
        	fName = DataAccess::SkinPath() + "/copy-send.png";
        }
        actCopySend = new QAction(QIcon(fName), tr("Copy Send"), this);
        actCopySend->setCheckable(true);
        actCopySend->setToolTip(tr("Add members you want to copy send"));
        connect(actCopySend, SIGNAL(triggered(bool)), SLOT(slotCopySend(bool)));

        actViewShare = new QAction(QIcon(DataAccess::SkinPath() + "/share.png"), tr("View Shared Items"), this);
        connect(actViewShare, SIGNAL(triggered(bool)), SLOT(slotViewShare()));
}

void
FormTalk::slotSetView(bool t)
{
        if (t)
        {
                m_viewType = 2;
        }
        int h = this->height();
//         int w = this->width();
        QList<int> siz = leftVSplitter->sizes();
        QList<int> sizh = mainHSplitter->sizes();
        if (siz.size() < 3)
        {
                siz << h*3/4 << h/4;
                leftVSplitter->setSizes(siz);
                //if (sizh.size() == 2)
                //{
                //        sizh.clear();
                //        if (m_viewType % 3 == 0)
                //        {
                //                sizh << w/2 << w/2;
                //                mainHSplitter->setSizes(siz);
                //        }
                //        else if (m_viewType % 3 == 1)
                //        {
                //                sizh << w*3/4 << w/4;
                //                mainHSplitter->setSizes(siz);
                //        } 
                //        else
                //        {
                //                sizh << w << 0;
                //                splitLeft->insertWidget(0, fileTransWidget);
                //                mainHSplitter->setSizes(siz);
                //        }
                //        m_viewType++;
                //}
                return;
        }
        siz.clear();
        if (m_viewType % 3 == 0)
        {
                siz << h/5+20 << h*3/5-20 << h/5;
                leftVSplitter->setSizes(siz);
        }
        else if (m_viewType % 3 == 1)
        {
                siz << h << 80 << 30;
                leftVSplitter->setSizes(siz);
        } 
        else
        {
                siz << 0 << h*3/4 << h/4;
                leftVSplitter->setSizes(siz);
        }
        leftVSplitter->setCollapsible(1, false);
        leftVSplitter->setCollapsible(2, false);
        mainHSplitter->setCollapsible(0, false);
        m_viewType++;
}

void
FormTalk::slotSetViewFullScreen(bool fs)
{

        fs?this->showFullScreen():this->showNormal();
}

void
FormTalk::setupMainToolBar()
{

}

void
FormTalk::setupMainWidget()
{
        QLabel* userIcon = new QLabel;
        userIcon->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        userIcon->setPixmap(QIcon(m_userItem->IconName()).pixmap(20, 20));
        userLabel = new QLabel("<a href=\"" + m_userItem->LoginName() + "\">"+ "<FONT color=#0000ff>" + m_userItem->LoginName() + "</FONT></a>");
        userLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        userLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
        connect(userLabel, SIGNAL(linkActivated(const QString&)), SLOT(slotShowUserInfo(const QString&)));

        textBrowser = new QTextBrowser;
        textBrowser->setOpenLinks(false);
        textBrowser->setOpenExternalLinks(false);
        textBrowser->setMinimumSize(420, 50);
        connect(textBrowser, SIGNAL(anchorClicked(const QUrl&)), SLOT(slotBrowaserAnchorClicked(const QUrl&)));

        textEdit = new QTextEdit;
        textEdit->setAcceptRichText(true);
        textEdit->setMinimumSize(420, 30);

        logWidget = new QFrame;
        logBrowser = new QTextBrowser;
        logBrowser->setOpenLinks(false);
        logBrowser->setOpenExternalLinks(false);
        logBrowser->setWindowTitle(tr("Chat Log") + m_userItem->IpAddress());
        connect(logBrowser, SIGNAL(anchorClicked(const QUrl&)), SLOT(slotBrowaserAnchorClicked(const QUrl&)));
        QVBoxLayout* logLayout = new QVBoxLayout(logWidget);
        logLayout->setMargin(0);
        logLayout->addWidget(new QLabel(tr("Chat Log")));
        logLayout->addWidget(logBrowser);

        QPushButton *pbutSend = new QPushButton(tr("&Send"));
        pbutSend->setDefault(true);
        QPushButton *pbutClose = new QPushButton(tr("&Close"));
        connect(pbutSend, SIGNAL(clicked()), SLOT(slotSend()));
        connect(pbutClose, SIGNAL(clicked()), SLOT(close()));

        QHBoxLayout* labelHLayout = new QHBoxLayout;
        labelHLayout->setMargin(0);
        labelHLayout->addWidget(userIcon);
        labelHLayout->addWidget(userLabel);
        labelHLayout->addStretch();
        labelHLayout->setSizeConstraint(QLayout::SetMaximumSize);

        QHBoxLayout *butHLayout = new QHBoxLayout;
        butHLayout->setDirection(QBoxLayout::RightToLeft);
        butHLayout->addWidget(pbutSend);
        butHLayout->addWidget(pbutClose);
        butHLayout->addStretch();

        QToolButton* layBut = new QToolButton;
        layBut->setDefaultAction(actSetViewLayout);
        butHLayout->addWidget(layBut);
        layBut = new QToolButton;
        layBut->setDefaultAction(actShowFullScreen);
        butHLayout->addWidget(layBut);
        QToolButton *pbutHist = new QToolButton;
        pbutHist->setDefaultAction(actShowLog);
        butHLayout->addWidget(pbutHist);

        QWidget* broWidget = new QWidget;
        QVBoxLayout* broLayout = new QVBoxLayout(broWidget);
        broLayout->setMargin(0);
        broLayout->addLayout(labelHLayout);
        broLayout->addWidget(textBrowser);

        QWidget* editWidget = new QWidget;
        QVBoxLayout* editLayout = new QVBoxLayout(editWidget);
        editLayout->setMargin(1);
        editLayout->addWidget(editToolBar);
        editLayout->addWidget(textEdit);

        leftVSplitter->addWidget(broWidget);
        leftVSplitter->addWidget(editWidget);

        leftLayout->addWidget(leftVSplitter);
        leftLayout->addLayout(butHLayout);
}

void
FormTalk::setupEditToolBar()
{
        editToolBar->setIconSize(QSize(20, 20));

        editToolBar->addAction(actTextFont);
        editToolBar->addAction(actTextColor);
        editToolBar->addAction(actSendEmotion);
        editToolBar->addAction(actSendPicture);
        editToolBar->addAction(actScreenShot);
        editToolBar->addAction(actSendFile);
        editToolBar->addAction(actSendDir);
        editToolBar->addAction(actViewShare);
        editToolBar->addAction(actCopySend);
        editToolBar->addAction(actShowLog);

        emotionView = m_DataAccess.emotionView;
}

void
FormTalk::setupFileTransWidget()
{
        fileTransWidget = new ProtocolDataWidget(m_DataAccess, m_userItem);
        connect(fileTransWidget, SIGNAL(sigTips(const QString&)), SLOT(addTipsToTextBrowser(const QString&)));

        QVBoxLayout* vbutBox = new QVBoxLayout;
        vbutBox->setAlignment(Qt::AlignCenter);
        QToolButton* tBut;
        tBut = new QToolButton;
        tBut->setIcon(QIcon(DataAccess::SkinPath() + "/save-as.png"));
        tBut->setText(tr("Save"));
        tBut->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        connect(tBut, SIGNAL(clicked()), fileTransWidget, SLOT(slotSave()));
        vbutBox->addWidget(tBut);

        tBut = new QToolButton;
        tBut->setIcon(QIcon(DataAccess::SkinPath() + "/cancel.png"));
        tBut->setText(tr("Cancel"));
        tBut->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        connect(tBut, SIGNAL(clicked()), fileTransWidget, SLOT(slotCancel()));
        vbutBox->addWidget(tBut);
        vbutBox->setMargin(0);

        QFrame* w = new QFrame;
        QHBoxLayout* h = new QHBoxLayout(w);
        h->setMargin(0);
        h->addWidget(fileTransWidget);
        h->addLayout(vbutBox);
        leftVSplitter->insertWidget(0, w);
//         rightLayout->addWidget(fileTransWidget);
//         fileTransWidget->setParent(rightWidget);

        userSelectWidget = new UserSelectView(m_DataAccess.userItemCollection.Collection());
}

void
FormTalk::mousePressEvent(QMouseEvent *)
{
        emotionView->hide();
}

void
FormTalk::slotMyClose()
{
}

void
FormTalk::closeEvent( QCloseEvent* event)
{
        if (fileTransWidget->model()->hasChildren())
        {
                for (int i = 0; i < fileTransWidget->topLevelItemCount(); i++)
                {
                        QTreeWidgetItem* item = fileTransWidget->topLevelItem(i);
                        QString const& state = item->data(COL_NAME, TransStateRole).toString();
                        if (state == "WAIT"
                                || state == "START")
                        {
                                if (QMessageBox::warning(this,
                                        tr("Warning"),
                                        tr("There are file transferring items need to be dealt with. Do you want to cancel them?"),
                                        QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                                {
                                        event->ignore();
                                        return;
                                }
                                else
                                {
                                        fileTransWidget->slotCancelAllItem();
                                        m_viewType = 2;
                                        slotSetView();
                                        break;
                                }
                        }
                }
        }
        if (!textBrowser->document()->isEmpty())
        {
                QFile file(m_DataAccess.LogPath() + "/" + m_userItem->IpAddress() + QString(".html"));
                if (!file.exists())
                {
                        if (!file.open(QFile::WriteOnly | QFile::Text))
                        {
                                QMessageBox::warning(this, PROGRAM_NAME,
                                        tr("Unable to save the file %1: %2")
                                        .arg(file.fileName())
                                        .arg(file.errorString()));
                                return;
                        }
                }
                else
                {
                        if (!file.open(QFile::Append | QFile::Text))
                        {
                                QMessageBox::warning(this, PROGRAM_NAME,
                                        tr("Unable to save the file %1: %2")
                                        .arg(file.fileName())
                                        .arg(file.errorString()));
                                return;
                        }
                }

                file.write(textBrowser->toHtml().toLocal8Bit());
        }

        textBrowser->clear();
        textEdit->clear();
        this->hide();
        event->ignore();
}

void
FormTalk::slotShowLog(bool show)
{
        if (show)
        {
                histSize = this->size();
                logWidget->resize(300, 300);
                rightLayout->addWidget(logWidget);
                QList<int> siz;
                siz << histSize.width() << 300;
                mainHSplitter->setSizes(siz);
	        QFile file(m_DataAccess.LogPath() + "/" + m_userItem->IpAddress() + QString(".html"));
	
	        if (!file.exists())
	        {
	                return;
	        }
	        if (!file.open(QIODevice::ReadOnly))
	        {
	                return;
	        }
	
	        logBrowser->setHtml(QString::fromLocal8Bit(file.readAll().data()));
        } 
        else
        {
                logWidget->resize(0, 0);
                rightLayout->removeWidget(logWidget);
                if (rightLayout->isEmpty())
                {
	                QList<int> siz;
	                siz << histSize.width() << 0;
	                mainHSplitter->setSizes(siz);
                }
        }
}

void
FormTalk::slotShowUserInfo(const QString& ip)
{
        QDialog dlg;

        dlg.setWindowTitle(tr("User Information"));
        QFormLayout* formLayout = new QFormLayout;
        formLayout->addRow(tr("I am"), new QLabel(ip));
        formLayout->addRow(tr("Group:"), new QLabel(m_userItem->GroupName()));
        formLayout->addRow(tr("Host Name:"), new QLabel(m_userItem->HostName()));
        formLayout->addRow(tr("IP Address:"), new QLabel(m_userItem->IpAddress()));
        formLayout->addRow(tr("Mac Address:"), new QLabel(m_userItem->MacAddress()));
        formLayout->addRow(tr("Protocol Name:"), new QLabel(m_userItem->ProtocolName()));
        formLayout->addRow(tr("Protocol Version:"), new QLabel(m_userItem->ProtocolVersion()));
        formLayout->addRow(tr("OS Platform:"), new QLabel(m_userItem->OsName()));

        QDialogButtonBox* dbutbox = new QDialogButtonBox(QDialogButtonBox::Ok);
        connect(dbutbox, SIGNAL(accepted()), &dlg, SLOT(accept()));

        QVBoxLayout* vlayout = new QVBoxLayout(&dlg);
        vlayout->addLayout(formLayout);
        vlayout->addWidget(dbutbox);

        dlg.exec();
}

void
FormTalk::slotBrowaserAnchorClicked(const QUrl& link)
{
        QString name = link.toString();
        QDesktopServices::openUrl(QUrl::fromLocalFile(name));
}

void
FormTalk::onRecv(Message_ptr m)
{
        if (PTextNormal const* ptn = static_cast<PTextNormal const*>(m->find(IMProto::TextNormal)))
        {
                std::string ts;
                if (ptn->property_data(IMProto::TimeStamp))
                {
                        ts = ptn->property_data(IMProto::TimeStamp);
                }
                QString timestamp;
                if (ts.empty())
                {
                        timestamp = QDateTime::currentDateTime ().toString(Qt::ISODate);
                } 
                else
                {
                        timestamp = QString::fromAscii(ts.c_str());
                }

                if (From const* f = static_cast<From const*>(m->find(From::id)))
                {
                        textBrowser->append("<span style=\" font-size:12pt;color:#0000ff;\">"
                                + QString("<b>") + m_userItem->LoginName()
                                + "</b>(" + timestamp + ")" + QString(tr(":"))
                                + "</span>");
                }
                else
                {
                        textBrowser->append("<span style=\" font-size:12pt;color:#008000;\">"
                                + QString("<b>") + m_DataAccess.userItemCollection.local->LoginName()
                                + "</b>(" + timestamp + ")" + QString(tr(":"))
                                + "</span>");
                }

                std::string c = ptn->property_data(IMProto::TextContent);
                if (c.empty())
                {
                        return;
                }
                QString const& content = QString::fromUtf8(c.c_str());
                textBrowser->append(QString("  ") + content);
        }
        else if (PTextZip const* ptz = static_cast<PTextZip const*>(m->find(IMProto::TextZip)))
        {
                std::string ts;
                if (ptz->property_data(IMProto::TimeStamp))
                {
                        ts = ptz->property_data(IMProto::TimeStamp);
                }
                QString timestamp;
                if (ts.empty())
                {
                        timestamp = QDateTime::currentDateTime ().toString(Qt::ISODate);
                } 
                else
                {
                        timestamp = QString::fromAscii(ts.c_str());
                }

                if (From const* f = static_cast<From const*>(m->find(From::id)))
                {
                        textBrowser->append("<span style=\" font-size:12pt;color:#0000ff;\">"
                                + QString("<b>") + m_userItem->LoginName()
                                + "</b>(" + timestamp + ")" + QString(tr(":"))
                                + "</span>");
                }
                else
                {
                        textBrowser->append("<span style=\" font-size:12pt;color:#008000;\">"
                                + QString("<b>") + m_DataAccess.userItemCollection.local->LoginName()
                                + "</b>(" + timestamp + ")" + QString(tr(":"))
                                + "</span>");
                }

                if (Data const* d = static_cast<Data const*>(m->find(Data::id)))
                {
                        uLongf ulen, clen;
                        ulen = QString::fromAscii(ptz->property_data(IMProto::TextLength)).toUInt();
                        clen = QString::fromAscii(ptz->property_data(IMProto::TextZipLen)).toUInt();
                        if (ulen == 0)
                        {
                                ulen = 1024*1024;
                        }

                        if (clen != d->size())
                        {
                                QMessageBox::critical(this, tr("Error"), tr("Size is incorrect while parsing message."));
                                return;
                        }
                        Bytef* ubuf = new Bytef[ulen];
                        if (!ubuf)
                        {
                                QMessageBox::critical(this, tr("Error"), tr("Allocate memory error while parsing message."));
                                return;
                        }
                        if (Z_OK != uncompress(ubuf, &ulen, (const Bytef*)d->buf(), clen))
                        {
                                QMessageBox::critical(this, tr("Error"), tr("Uncompress message error while parsing message."));
                                return;
                        }
                        textBrowser->append(QString("  ") + QString::fromUtf8((const char*)ubuf, ulen));
                }
        }

        if (PPictureSend const* pps = static_cast<PPictureSend const*>(m->find(IMProto::PictureSend)))
        {
                quint64 siz = QString::fromUtf8(pps->property_data(IMProto::FileSize)).toULongLong();
                QString const& kName = QString::fromUtf8(pps->property_data(IMProto::FileKey));
                QString const& fName = QString::fromUtf8(pps->property_data(IMProto::FileName));

                QString const& saveName = m_DataAccess.RecvImgPath() + "/" + fName;
                QFile file(saveName);
                if (file.exists())
                {
                        uint qs = QTime::currentTime().toString(Qt::ISODate).remove(":").toUInt();//
                        qsrand(qs);
                        file.rename(m_DataAccess.RecvImgPath() + "/" +  QString::number(qrand()) + fName);
                }
                // recv image file
                quint16 port = DataAccess::genTcpPort();
                ProtocolDataTransfer* pdthr = 0;
                pdthr = new ProtocolDataTransfer(0, saveName, siz, port);
                connect(pdthr, SIGNAL(sigComplete()), this, SLOT(slotRecvPictureReady()));
                pdthr->start();
                Message_ptr nm(new Message);
                PPictureRecv* ppr = new PPictureRecv(IMProto::PictureRecv);
                ppr->add(IMProto::ListenPort, QString::number(port).toAscii().data(), false);
                ppr->add(IMProto::FileKey, kName.toUtf8().data(), false);
                ppr->update_size();
                nm->add(Profile_ptr (ppr));
                nm->add(Profile_ptr (new To(m_userItem->Addr())));
                DataAccess::sendMsg(nm);

                return;
        }
        else if (PSharedList const* psla = static_cast<PSharedList const*>(m->find(IMProto::SharedListAck)))
        {
                if (!filesSharedWidget)
                {
                        return;
                }
                filesSharedWidget->SharedView()->setSortingEnabled(false);

                filesSharedWidget->SharedView()->clear();
                filesSharedWidget->SharedView()->DataCollection()->clear();

                QString const& bArray = QString::fromUtf8(psla->property_data(IMProto::TextContent));
                if (bArray.isEmpty())
                {
                        filesSharedWidget->StatusBar()->showMessage(tr("List Updated"), 10000);
                        return;
                }
                QStringList const& keys = bArray.split("/*/");
                for (int i = 0; i < keys.size(); i++)
                {
                        filesSharedWidget->SharedView()->insertRemoteItem(keys[i]);
                }
                filesSharedWidget->StatusBar()->showMessage(tr("List Updated"), 10000);
                filesSharedWidget->SharedView()->setSortingEnabled(true);
        }
        else if (PSharedUploadCancel const* psuc = static_cast<PSharedUploadCancel const*>(m->find(IMProto::SharedUploadCancel)))
        {
                QString const& sn = QString::fromAscii(psuc->property_data(IMProto::EventSn));

                QTreeWidgetItem* dItem = filesSharedWidget->DownloadView()->ItemCollection()->value(sn, 0);
                if (!dItem)
                {
                        return;
                }
                if (dItem->data(COL_NAME, TransStateRole).toString() == "START")
                {
                        ProtocolDataTransfer* t = (ProtocolDataTransfer*)dItem->data(COL_NAME, TransThreadRole).toUInt();
                        disconnect(&m_DataAccess.m_secTimer, SIGNAL(timeout()), t, SLOT(slotUpdate()));
                        t->slotCancel();
                }
                if (!delItemWhenFinished)
                {
                        dItem->setData(COL_NAME, TransStateRole, "CANCEL");
                        dItem->setIcon(COL_STATE, QIcon(DataAccess::SkinPath() + "/warning.png"));
                        dItem->setText(COL_STATE, tr("Canceled"));
                }
                else
                {
                        filesSharedWidget->DownloadView()->takeTopLevelItem(filesSharedWidget->DownloadView()->indexOfTopLevelItem(dItem));
                }
        }
        else if (m->find(IMProto::FileSend)
                || m->find(IMProto::DirSend)
                || m->find(IMProto::DirFileRecv)
                || m->find(IMProto::DirFileSendCancel)
                || m->find(IMProto::DirFileRecvCancel)
                )
        {
                fileTransWidget->onRecv(m);
                QList<int> siz = leftVSplitter->sizes();
                if (siz.first() == 0)
                {
                        m_viewType = 0;
                        slotSetView();
                }
        }

        if (SignOut const* so = static_cast<SignOut const*>(m->find(SignOut::id)))
        {
                addTipsToTextBrowser(tr("The other has signed out."));
                if (QMessageBox::warning(this,
                        tr("Warning"),
                        tr("The other has signed out. Do you want to close the talk window?"),
                        QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
                {
                        return;
                }
                else
                {
                        this->close();
                }
        }
}

void FormTalk::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
{
        QTextCursor cursor = textEdit->textCursor();
        if (!cursor.hasSelection())
                cursor.select(QTextCursor::WordUnderCursor);
        cursor.mergeCharFormat(format);
        textEdit->mergeCurrentCharFormat(format);
}

void
FormTalk::slotTextColor()
{
        textEditColor = QColorDialog::getColor(textEditColor);
        if (!textEditColor.isValid())
        {
                return;
        }
        QIcon ic = actTextColor->icon();
        QPixmap pix = ic.pixmap(ic.actualSize(QSize(100, 100)));
        QSize s = pix.size();
        QPainter pter(&pix);

        pter.setPen(textEditColor);
        pter.setBrush(textEditColor);
        pter.drawRect(QRect(0, s.height()-3, s.width()-1, 3));
        actTextColor->setIcon(pix);

        textEdit->insertHtml("<b></b>");
        QTextCharFormat fmt;
        fmt.setForeground(textEditColor);
        mergeFormatOnWordOrSelection(fmt);
}

void
FormTalk::slotTextFont()
{
        bool ok;
        QFont font = QFontDialog::getFont(&ok, textEdit->currentCharFormat().font(), this, tr("Config Text Font"));
        if (ok)
        {
                QTextCharFormat fmt;
                fmt.setFont(font);
                mergeFormatOnWordOrSelection(fmt);
        }
}

void
FormTalk::slotSend()
{
        textEdit->setFocus();
        if (textEdit->toPlainText() == "")
        {
                return;
        }
        QTextCharFormat fmt;
        fmt.setForeground(textEditColor);
        mergeFormatOnWordOrSelection(fmt);

        Message_ptr m(new Message);
        m_DataAccess.CreateTextMsg(textEdit->toHtml(), m);

        if (m->size() >= PayloadSize)
        {
                QMessageBox::warning(this, tr("Warning"), tr("The message to be sent is too lang."));
                return;
        }
        QList<UserItem*> toList;
        if (actCopySend->isChecked())
        {
	        toList = userSelectWidget->GetSelectedUserItems();
        }
        toList.prepend(m_userItem);
        DataAccess::MultiSendMsg(m, toList);
        this->onRecv(m);
        textEdit->clear();
        while (!m_imagesToSend.isEmpty())
        {
	        Message_ptr imgm = m_imagesToSend.dequeue();
                DataAccess::MultiSendMsg(imgm, toList);
        }
}

void
FormTalk::slotEmotionViewItemDClicked(const QString& file)
{
        textEdit->insertHtml(QString("<img src=\"") + file +"\" />");
}

void
FormTalk::slotSendEmotion()
{
        connect(emotionView, SIGNAL(iconClicked(const QString&)), this, SLOT(slotEmotionViewItemDClicked(const QString&)));
        emotionView->show();
}

void
FormTalk::slotSendPicture()
{
        QStringList nameList = QFileDialog::getOpenFileNames(this,
                tr("Choose Image Files"), m_DataAccess.OpenDir(),
                tr("Images (*.png *.bmp *.jpg *.jpeg *.gif *.svg *.tif *.tiff)"));

        if (nameList.isEmpty())
        {
                return;
        }
        QFileInfo finfo(nameList[0]);
        m_DataAccess.updateOpenDir(finfo.absolutePath());

        for (int i = 0; i < nameList.size(); i++)
        {
                QString fName = nameList.at(i);
                slotSendPicture(fName);
        }
}

void
FormTalk::slotSendPicture(const QString& pathname)
{
        QFile file(pathname);
        if (!file.open(QIODevice::ReadOnly))
        {
                QMessageBox::warning(this, tr("Error"),
                        tr("Unable to read the file %1: %2")
                        .arg(pathname).arg(file.errorString()));
                return;
        }
        file.close();
        QFileInfo finfo(pathname);
        quint64 len = finfo.size();
        Message_ptr m(new Message);
        IMProto::PPictureSend* pps = new IMProto::PPictureSend(IMProto::PictureSend);
        QString const& kName = m_userItem->insertSendingFile(pathname);
        pps->add(IMProto::FileKey, kName.toUtf8().data(), false);
        pps->add(IMProto::FileSize, QString::number(len).toUtf8().data(), false);
        pps->add(IMProto::FileName, finfo.fileName().toUtf8().data(), false);
        pps->update_size();
        m->add(Profile_ptr (pps));
        m->add(Profile_ptr (new To(m_userItem->Addr())));
        m_imagesToSend.enqueue(m);
        this->onRecv(m);
        textEdit->insertHtml(QString("<img src=\"") + m_DataAccess.RecvImgPath() + "/" + finfo.fileName().toUtf8().data() +"\" />");
}

void
FormTalk::slotCaptureScreen()
{
        ScreenShot* ss = new ScreenShot;
        connect(ss, SIGNAL(sigCaptured(const QString&)), this, SLOT(slotSendPicture(const QString&)));
}

void
FormTalk::slotSendFile()
{
        QStringList nameList = QFileDialog::getOpenFileNames(this, tr("Choose Files"), m_DataAccess.OpenDir());

        if (nameList.isEmpty())
        {
                return;
        }
        QFileInfo finfo(nameList[0]);
        m_DataAccess.updateOpenDir(finfo.absolutePath());

        for (int i = 0; i < nameList.size(); i++)
        {
                QString fName = nameList.at(i);

                QFile file(fName);
                if (!file.open(QIODevice::ReadOnly))
                {
                        QMessageBox::warning(this, tr("Error"),
                                tr("Unable to read the file %1: %2")
                                .arg(fName).arg(file.errorString()));
                        continue;
                }
                file.close();
                QFileInfo finfo(fName);
                quint64 len = finfo.size();

                QString const& sn = QString::number(DataAccess::genTcpPort());
                Message_ptr m(new Message);
                IMProto::PFileSend* pfs = new IMProto::PFileSend(IMProto::FileSend);
                QString const& kName = m_userItem->insertSendingFile(fName);
                pfs->add(IMProto::EventSn, sn.toUtf8().data(), false);
                pfs->add(IMProto::FileKey, kName.toUtf8().data(), false);
                pfs->add(IMProto::FileSize, QString::number(len).toUtf8().data(), false);
                pfs->add(IMProto::FileName, finfo.fileName().toUtf8().data(), false);
                pfs->update_size();
                m->add(Profile_ptr (pfs));
                m->add(Profile_ptr (new To(m_userItem->Addr())));
                DataAccess::sendMsg(m);
                QString keyself = "FILE$" + QString::number(len) + "$" + fName;
                fileTransWidget->addItem(keyself, sn, kName);

                QList<int> siz = leftVSplitter->sizes();
                if (siz.first() == 0)
                {
                        m_viewType = 0;
                        slotSetView();
                }
        }
}

void
FormTalk::slotSendDir()
{
        QString dName = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), m_DataAccess.OpenDir());

        if (dName.isEmpty())
        {
                return;
        }
        QDir dir(dName);
        if (dir.isRoot())
        {
                QMessageBox::information(this, tr("Tips"), tr("You can't send a root directory."));
                return;
        }
        if (dName.endsWith("/"))
        {
                dName = dName.remove(dName.lastIndexOf("/"), 1);
        }
        QFileInfo finfo(dName);
        m_DataAccess.updateOpenDir(finfo.path());

        quint64 len = finfo.size();
        QString const& sn = QString::number(DataAccess::genTcpPort());
        Message_ptr m(new Message);
        IMProto::PDirSend* pds = new IMProto::PDirSend(IMProto::DirSend);
        QString const& kName = m_userItem->insertSendingFile(dName);
        pds->add(IMProto::EventSn, sn.toUtf8().data(), false);
        pds->add(IMProto::FileKey, kName.toUtf8().data(), false);
        pds->add(IMProto::FileSize, QString::number(len).toUtf8().data(), false);
        pds->add(IMProto::FileName, finfo.fileName().toUtf8().data(), false);
        pds->update_size();
        m->add(Profile_ptr (pds));
        m->add(Profile_ptr (new To(m_userItem->Addr())));
        DataAccess::sendMsg(m);
        QString keyself = "DIR$" + QString::number(len) + "$" + dName;
        fileTransWidget->addItem(keyself, sn, kName);

        QList<int> siz = leftVSplitter->sizes();
        if (siz.first() == 0)
        {
                m_viewType = 0;
                slotSetView();
        }
}

void
FormTalk::slotCopySend(bool copysend)
{
        if (copysend)
        {
                histSize = this->size();
                userSelectWidget->resize(300, 300);
                rightLayout->addWidget(userSelectWidget);
                QList<int> siz;
                siz << histSize.width() << 300;
                mainHSplitter->setSizes(siz);
        } 
        else
        {
                userSelectWidget->resize(0, 0);
                rightLayout->removeWidget(userSelectWidget);
                rightLayout->update();
                if (rightLayout->isEmpty())
                {
	                QList<int> siz;
	                siz << histSize.width() << 0;
	                mainHSplitter->setSizes(siz);
                }
        }
}

void
FormTalk::slotViewShare()
{
        if (!filesSharedWidget)
        {
	        filesSharedWidget = new SharedWatchWidget(m_DataAccess, m_userItem);
        }
        filesSharedWidget->show();
        filesSharedWidget->activateWindow();
        Message_ptr m(new Message);
        PSharedList* psl = new PSharedList(IMProto::SharedList);
        m->add(Profile_ptr (psl));
        m->add(Profile_ptr (new To(m_userItem->Addr())));
        DataAccess::sendMsg(m);
}

void
FormTalk::addTipsToTextBrowser(const QString& tips)
{
        QString timeStr = "(" + QDateTime::currentDateTime ().toString(Qt::ISODate).replace('T', ' ') + ")";

        textBrowser->append("<img src=\":/skin/info.png\"/><span style=\" font-size:12pt; color:#0000ff;\">" + tr("<b>Tips:</b>") + timeStr + "</span>");
        QString tip = QString("<span style=\" font-size:10pt; color:#000000; white-space:normal;\">") + "  " + tips + "</span>";
        textBrowser->append(tip);
        if (fileTransWidget->topLevelItemCount() == 0)
        {
                m_viewType = 2;
                slotSetView();
        }
}

void
FormTalk::slotRecvPictureReady()
{
        int w = size().width();
        int h = size().height();
        this->resize(w+2, h);
        this->resize(w, h);
}
