/*  filename: DataAccess.cpp    2009/02/09  */
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
#include <ace/SOCK_Dgram.h>
#include "UserItem.h"
#include "IconView.h"
#include "FormTalk.h"
#include "zlib.h"
#include "ShareWidget.h"

namespace IPM
{
        bool delItemWhenFinished = false;
        bool updateDirFileItemInfo = false;
}

quint16 DataAccess::tcpPort = 50001;

#ifdef WIN32
QString DataAccess::m_configPath = ("data");
#else
QString DataAccess::m_configPath = (QDir::homePath() + "/.lulu/");
#endif
QString DataAccess::m_langPath = (DataAccess::ConfigPath() + "/languages");
QString DataAccess::m_logPath = (DataAccess::ConfigPath() + "/logs");
QString DataAccess::m_skinPath = (":/skin");
QString DataAccess::m_facePath = (DataAccess::ConfigPath() + "/faces");
QString DataAccess::m_emotionPath = (DataAccess::ConfigPath() + "/emotions");
QString DataAccess::m_recvImgPath = (DataAccess::ConfigPath() + "/RecvImages");
QString DataAccess::m_logosPath = (":/logos");

QString DataAccess::m_histOpenDir = (".");
QString DataAccess::m_histSaveDir = (".");

QString DataAccess::m_defaultGroupName = "";

QStringList DataAccess::addrList = QStringList();

DataAccess::DataAccess(void)
: domDocument("LLM")
, m_StyleName("Cleanlooks")
, currentState(0)
, sharingFilesWidget(0)
, faceView(0)
, emotionView(0)
{
        m_ConfigFileName = m_configPath + "/" + "config.xml";
        m_SSTFileName = m_configPath + "/" + "default.sst";

        QDir dir;
        if (!dir.exists(m_configPath))  // config
        {
                dir.mkpath(m_configPath);
        }
        if (!dir.exists(m_langPath)) // languages path
        {
                dir.mkpath(m_langPath);
        }
        if (!dir.exists(m_logPath))     // log
        {
                dir.mkpath(m_logPath);
        }
        if (!dir.exists(m_skinPath))    // skin
        {
                dir.mkpath(m_skinPath);
        }
        if (!dir.exists(m_facePath))    // face
        {
                dir.mkpath(m_facePath);
        }
        if (!dir.exists(m_emotionPath)) // emotions
        {
                dir.mkpath(m_emotionPath);
        }
        if (!dir.exists(m_recvImgPath)) // recv image path
        {
                dir.mkpath(m_recvImgPath);
        }
        if (!QFile::exists(m_ConfigFileName))
        {
                init();
                write();
        }
        if (!QFile::exists(m_SSTFileName))
        {
                DataAccess::writeSSTFile(m_SSTFileName);
        }

        read();
        sharingFilesWidget = new SharingWidget(*this);
}

DataAccess::~DataAccess(void)
{
        for (QHash<QString, UserItem*>::iterator i = userItemCollection.Collection()->begin(); i != userItemCollection.Collection()->end(); i++)
        {
                addRemoteUser(i.value());
        }
        write();
        if (sharingFilesWidget)
        {
                delete sharingFilesWidget;
        }
        if (faceView)
        {
	        delete faceView;
        }
        if (emotionView)
        {
	        delete emotionView;
        }
        QList<FormTalk*> ftList = talkDlgMap.values();
        for (int i = 0; i < ftList.size(); i++)
        {
                if (ftList[i])
                {
                        ftList[i]->deleteLater();
                }
        }

        delete translatorQt;
        delete translator;
}

void
DataAccess::addFaces()
{
        faceView = new EmotionWidget(&faceMap);
        IconLoaderThread* iltFace_1 = new IconLoaderThread(&faceMap, m_facePath);
        IconLoaderThread* iltFace = new IconLoaderThread(&faceMap, ":/faces/qq");
        IconLoaderThread* iltFace1 = new IconLoaderThread(&faceMap, ":/faces/qq2");
        connect(iltFace_1, SIGNAL(finished()), iltFace, SLOT(start()));
        connect(iltFace, SIGNAL(finished()), iltFace1, SLOT(start()));
        connect(iltFace1, SIGNAL(finished()), faceView, SLOT(addTabs()));
        iltFace_1->start();
        iltFace->wait();
}

void
DataAccess::addEmotions()
{
        emotionView = new EmotionWidget(&emotionMap);
        connect(emotionView, SIGNAL(tabAddOK()), this, SLOT(slotEmotionsAdded()));
        IconLoaderThread* ilt_1 = new IconLoaderThread(&emotionMap, m_emotionPath);
        IconLoaderThread* ilt = new IconLoaderThread(&emotionMap, ":/ww");
        IconLoaderThread* ilt1 = new IconLoaderThread(&emotionMap, ":/qq");
        IconLoaderThread* ilt2 = new IconLoaderThread(&emotionMap, ":/pidgin");
        IconLoaderThread* ilt3 = new IconLoaderThread(&emotionMap, ":/pp");
        IconLoaderThread* ilt4 = new IconLoaderThread(&emotionMap, ":/smile");
        connect(ilt_1, SIGNAL(finished()), ilt, SLOT(start()));
        connect(ilt, SIGNAL(finished()), ilt1, SLOT(start()));
        connect(ilt1, SIGNAL(finished()), ilt2, SLOT(start()));
        connect(ilt2, SIGNAL(finished()), ilt3, SLOT(start()));
        connect(ilt3, SIGNAL(finished()), ilt4, SLOT(start()));
        connect(ilt4, SIGNAL(finished()), emotionView, SLOT(addTabs()));
        ilt->start();
}

void
DataAccess::sendMsg(Message_ptr m)
{
        ostreamT os (m->size(), 1); // Always little-endian.

        Address addr;
        if (To const* to = static_cast<To const*>(m->find(To::id)))
        {
                addr = to->address();
                m->remove(To::id);
        }
        else
        {
                return;
        }

        os << *m;

        ACE_SOCK_Dgram udp(ACE_Addr::sap_any);
        udp.send(os.buffer(), os.length(), addr);
        udp.close();
}

void
DataAccess::MultiSendMsg(Message_ptr m, QList<UserItem*> const& toList)
{
        ostreamT os (m->size(), 1); // Always little-endian.

        Address addr;
        if (To const* to = static_cast<To const*>(m->find(To::id)))
        {
                addr = to->address();
                m->remove(To::id);
        }

        os << *m;

        ACE_SOCK_Dgram udp(ACE_Addr::sap_any);
        for (int i = 0; i < toList.size(); i++)
        {
	        udp.send(os.buffer(), os.length(), toList[i]->Addr());
        }
        udp.close();
}

void
DataAccess::slotSharingConfig()
{
        sharingFilesWidget->show();
}

void
DataAccess::slotEmotionsAdded()
{
        if (currentState != 0)
        {
                sendLoginMsg();
        }
}

bool
DataAccess::CreateTextMsg(const QString& text, Message_ptr& m)
{
        std::string c = text.toUtf8().data();
        if (c.size() < (PayloadSize - 1024))
        {
                IMProto::PTextNormal* ptn = new IMProto::PTextNormal(IMProto::TextNormal);
                ptn->add(IMProto::TextContent, c.c_str());
                if (!m->add(Profile_ptr(ptn)))
                {
                        return false;
                }
        }
        else
        {
                uLongf slen = c.length()+1;
                uLongf clen = compressBound(slen) + 128;
                Bytef* cbuf = new Bytef[clen];
                if (!cbuf)
                {
                        QMessageBox::critical(0, tr("Error"), tr("Allocate memory error while sending message."));
                        return false;
                }
                if (Z_OK != compress(cbuf, &clen, (const Bytef*)c.c_str(), slen))
                {
                        QMessageBox::critical(0, tr("Error"), tr("Compress message error while sending message."));
                        return false;
                }
                IMProto::PTextZip* ptz = new IMProto::PTextZip(IMProto::TextZip);
                ptz->add(IMProto::TextLength, QString::number(slen).toAscii().data(), false);
                ptz->add(IMProto::TextZipLen, QString::number(clen).toAscii().data(), false);
                ptz->update_size();
                if (!m->add(Profile_ptr(ptz)))
                {
                        return false;
                }
                m->add(Profile_ptr(new Data(cbuf, clen)));
        }
        return true;
}

bool
DataAccess::CreateSignInMsg(Message_ptr& m)
{
        SignIn_ptr si(new SignIn);
        si->add(IMProto::MacAddress, userItemCollection.local->MacAddress().toUtf8().data(), false);
        si->add(IMProto::HostName, userItemCollection.local->HostName().toUtf8().data(), false);
        si->add(IMProto::IconName, userItemCollection.local->IconName().toUtf8().data(), false);
        si->add(IMProto::SignName, userItemCollection.local->LoginName().toUtf8().data(), false);
        si->add(IMProto::Password, userItemCollection.local->Passwd().toUtf8().data(), false);
        si->add(IMProto::ProgramVer, PROGRAM_VER.toAscii().data(), false);
        si->add(IMProto::ProtocolVer, IMProto::Version, false);
        si->add(IMProto::OsName, COMPUTER_OS.toUtf8().data(), false);
        si->update_size();
        m->add(si);
        return true;
}

void
DataAccess::BCastMsg(Message_ptr m)
{
        for (int i = 0; i < addrList.size(); i++)
        {
                m->remove(To::id);
                m->add(Profile_ptr (new To(Address(Server_Port, addrList[i].toUtf8().data()))));
                sendMsg(m);
        }
}

void
DataAccess::sendLoginMsg()
{
        for (int i = 0; i < addrList.size(); i++)
        {
                Message_ptr m(new Message);
                CreateSignInMsg(m);
                m->add(Profile_ptr(new To(Address(Server_Port, addrList[i].toUtf8().data()))));
                sendMsg(m);
        }
}

void
DataAccess::sendLogoutMsg()
{
        for (int i = 0; i < addrList.size(); i++)
        {
                Message_ptr m(new Message);
                SignOut_ptr s(new SignOut);
                s->add(IMProto::MacAddress, userItemCollection.local->MacAddress().toUtf8().data(), false);
                s->add(IMProto::HostName, userItemCollection.local->HostName().toUtf8().data(), false);
                s->update_size();
                m->add(s);
                m->add(Profile_ptr (new To(Address(Server_Port, addrList[i].toUtf8().data()))));
                sendMsg(m);
        }
}

void
DataAccess::sendTextMsg(const QString& text, const QString& toIP)
{
        Message_ptr m(new Message);
        CreateTextMsg(text, m);
        m->add(Profile_ptr (new To(Address(Server_Port, toIP.toUtf8().data()))));

        if (m->size() >= PayloadSize)
        {
                QMessageBox::warning(0, tr("Warning"), tr("The message to be sent is too lang."));
                return;
        }
        sendMsg(m);
}

QString
DataAccess::genFileKey(const QHash<QString, QString>& hash, const QString& pathname, bool& alreadyExist)
{
        alreadyExist = false;
        QStringList const& vals = hash.values();
        for (int i = 0; i < vals.size(); i++)
        {
                if (vals[i].endsWith(pathname))
                {
                        alreadyExist = true;
                        return hash.key(vals[i]);
                }
        }
        QFileInfo finfo(pathname);
        QString const& finfoKey = finfo.fileName();
        if (hash.value(finfoKey, "") == "")
        {
                return finfoKey;
        }
        else
        {
                int i = 0;
                QString tmp;
                tmp = finfoKey + "(" + QString::number(i) + ")";
                while (hash.value(tmp, "") != "")
                {
                        tmp = finfoKey + QString::number(i);
                }
                return tmp;
        }
}

bool
DataAccess::write()
{
        // save config data
        QDomElement root = domDocument.documentElement();

        // shared files
        if (sharingFilesMap.size() > 0)
        {
	        QDomElement sfiles = domDocument.createElement("SharedFiles");
	        root.appendChild(sfiles);
	        QList<QString> shareVals = sharingFilesMap.values();
	        for (int i = 0; i < shareVals.size(); i++)
	        {
	                QDomElement itm = domDocument.createElement("Item");
	                sfiles.appendChild(itm);
                        QStringList const& keys = ParseFileInfoString(shareVals[i]);
	                QDomText sfilesItem = domDocument.createTextNode(keys[2]);
	                itm.appendChild(sfilesItem);
	        }
                root.replaceChild(sfiles, root.firstChildElement("SharedFiles"));
        }

        // write xml file
        QFile file(m_ConfigFileName);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
                QMessageBox::warning(this, PROGRAM_NAME,
                        tr("Unable to save the file %1: %2")
                        .arg(file.fileName())
                        .arg(file.errorString()));
                return false;
        }

        const int IndentSize = 4;

        QTextStream out(&file);
        domDocument.save(out, IndentSize);
        file.close();
        return true;
}

bool
DataAccess::read()
{
        QString errorStr;
        int errorLine;
        int errorColumn;

        QFile file(m_ConfigFileName);
        if (!file.open(QIODevice::ReadOnly))
        {
                return false;
        }

        if (!domDocument.setContent(&file, false, &errorStr, &errorLine,
                                         &errorColumn))
        {
                file.close();
                QMessageBox::information(window(), PROGRAM_NAME,
                        tr("Parse error at line %1, column %2:\n%3")
                        .arg(errorLine)
                        .arg(errorColumn)
                        .arg(errorStr));
                return false;
        }
        file.close();

        // root
        QDomElement root = domDocument.documentElement();
        if (root.tagName() != "LLM")
        {
                QMessageBox::information(window(), PROGRAM_NAME,
                        tr("The file is not an %1 config file.").arg(PROGRAM_NAME));
                return false;
        }
        else if (root.hasAttribute("version")
                && root.attribute("version") != "1.0")
        {
                QMessageBox::information(window(), PROGRAM_NAME,
                        tr("The file is not an %1 version 1.0 config"
                        "file.").arg(PROGRAM_NAME));
                return false;
        }

        // lang
        QDomElement lang = root.firstChildElement("Language");
        m_language = lang.text();
        SetTranslator(m_language);

        // auto login
        QDomElement AutoLogin = root.firstChildElement("AutoLogin");
        if (AutoLogin.text() == "0"
                || AutoLogin.text() == "false")
        {
                m_autoLogin = false;
        }
        else
        {
                m_autoLogin = true;
        }

        // state
        QDomElement st = root.firstChildElement("State");
        defaultState = st.attribute("default").toInt();
        QDomElement itm = st.firstChildElement("Item");
        while(!itm.isNull())
        {
                stateMap.insert(itm.attribute("index").toInt(), itm.text());
                itm = itm.nextSiblingElement("Item");
        }

        // dir
        QDomElement saveDir = root.firstChildElement("SaveDir");
        m_histSaveDir = saveDir.text();

        QDomElement openDir = root.firstChildElement("OpenDir");
        m_histOpenDir = openDir.text();

        // style
        QDomElement Style = root.firstChildElement("Style");
        m_StyleName = Style.attribute("name");
        if (Style.attribute("useStandardPalette") == "true")
        {
	        m_isUseStandardPalette = true;
        } 
        else
        {
                m_isUseStandardPalette = false;
        }

        QDomElement Listen = root.firstChildElement("Listen");
        QDomElement child = Listen.firstChildElement("Address");
        while(!child.isNull())
        {
                QString tname = child.text();
                addrList.push_back(tname);
                child = child.nextSiblingElement("Address");
        }

        QDomElement User = root.firstChildElement("User");
        userItemCollection.local->IconName(User.attribute("icon"));
        userItemCollection.local->Passwd(User.attribute("password"));
        QDomElement Name = User.firstChildElement("Name");
        while(!Name.isNull())
        {
                QString tname = Name.text();
                if (Name.hasAttribute("default")
                        && Name.attribute("default") == "true")
                {
                        userItemCollection.local->LoginName(tname);
                        nameList.prepend(tname);
                }
                else
                {
                        nameList.append(tname);
                }
                Name = Name.nextSiblingElement("Name");
        }

        // remote user info
        QDomElement remoteUser = root.firstChildElement("RemoteUser");
        QDomElement ruchild = remoteUser.firstChildElement("Item");
        while(!ruchild.isNull())
        {
                QDomElement mac = ruchild.firstChildElement("Mac");
                QDomElement ip = ruchild.firstChildElement("IP");
                QDomElement group = ruchild.firstChildElement("Group");
                UserItem* uitem = new UserItem(mac.text());
                uitem->IpAddress(ip.text());
                uitem->GroupName(group.text());
                userItemCollection.Collection()->insert(ip.text(), uitem);

                ruchild = ruchild.nextSiblingElement("Item");
        }

        // shared files
        QDomElement sfiles = root.firstChildElement("SharedFiles");
        QDomElement sfilesItem = sfiles.firstChildElement("Item");
        while(!sfilesItem.isNull())
        {
                QString const& pathName = sfilesItem.text();
                QFileInfo finfo(pathName);
                QString finfoVal;
                if (finfo.isFile())
                {
                        finfoVal = "FILE$" + QString::number(finfo.size()) + "$" + pathName;
                }
                else
                {
                        finfoVal = "DIR$" + QString::number(finfo.size()) + "$" + pathName;
                }
                bool exist;
                QString const& key = genFileKey(sharingFilesMap, pathName, exist);
                sharingFilesMap.insert(key, finfoVal);
                sfilesItem = sfilesItem.nextSiblingElement("Item");
        }

        return true;
}

void
DataAccess::init()
{
        m_language = QLocale::system().name();
        SetTranslator(m_language);

        stateMap.insert(UserItem::S_Offline, tr("offline"));
        stateMap.insert(UserItem::S_Online, tr("online"));
        stateMap.insert(UserItem::S_Hide, tr("hide"));
        stateMap.insert(UserItem::S_Busy, tr("busy"));
        stateMap.insert(UserItem::S_Glad, tr("glad"));
        stateMap.insert(UserItem::S_Depressed, tr("depressed"));

        m_histOpenDir = QDir::currentPath();
        m_histSaveDir = QDir::currentPath();

        QDomElement root = domDocument.createElement("LLM");
        root.setAttribute("version", "1.0");
        domDocument.appendChild(root);

        // state
        QDomElement st = domDocument.createElement("State");
        st.setAttribute("default", 1);
        root.appendChild(st);

        QList<int> stateKeys = stateMap.keys();
        for (int i = 0; i < stateKeys.size(); i++)
        {
                addState(stateKeys[i], stateMap.value(stateKeys[i]));
        }
        stateMap.clear();

        // lang
        QDomElement lang = domDocument.createElement("Language");
        root.appendChild(lang);
        QDomText langtxt = domDocument.createTextNode(m_language);
        lang.appendChild(langtxt);

        // login
        QDomElement autologin = domDocument.createElement("AutoLogin");
        root.appendChild(autologin);
        QDomText altxt = domDocument.createTextNode("0");
        autologin.appendChild(altxt);

        // dir
        QDomElement sdir = domDocument.createElement("SaveDir");
        root.appendChild(sdir);
        QDomText savedirtxt = domDocument.createTextNode(m_histSaveDir);
        sdir.appendChild(savedirtxt);

        QDomElement odir = domDocument.createElement("OpenDir");
        root.appendChild(odir);
        QDomText opendirtxt = domDocument.createTextNode(m_histOpenDir);
        odir.appendChild(opendirtxt);

        // style
        QDomElement Style = domDocument.createElement("Style");
#ifdef WIN32
        Style.setAttribute("name", "WindowsXP");
#else
        Style.setAttribute("name", "Cleanlooks");
#endif
        Style.setAttribute("useStandardPalette", "true");
        root.appendChild(Style);

        // listen
        QDomElement Listen = domDocument.createElement("Listen");
        root.appendChild(Listen);

        QDomElement tag1 = domDocument.createElement("Address");
        Listen.appendChild(tag1);
        QDomText t1 = domDocument.createTextNode("224.5.6.8");
        tag1.appendChild(t1);

//         QDomElement tag2 = domDocument.createElement("Address");
//         Listen.appendChild(tag2);
//         QDomText t2 = domDocument.createTextNode("255.255.255.255");
//         tag2.appendChild(t2);

        // user info
        QDomElement User = domDocument.createElement("User");
        User.setAttribute("icon", ":/faces/qq/0.png");
        User.setAttribute("password", "xxxxx");
        root.appendChild(User);

        QDomElement Name = domDocument.createElement("Name");
        Name.setAttribute("default", "true");
        User.appendChild(Name);
        QDomText name1 = domDocument.createTextNode(userItemCollection.local->HostName());
        Name.appendChild(name1);
}

void
DataAccess::Language(QString val)
{
        m_language = val;
        QDomElement root = domDocument.documentElement();
        QDomElement olang = root.firstChildElement("Language");
        QDomElement nlang = domDocument.createElement("Language");
        QDomText langtxt = domDocument.createTextNode(m_language);
        nlang.appendChild(langtxt);
        root.replaceChild(nlang, olang);

        SetTranslator(m_language);
}

void
DataAccess::SetTranslator(QString val)
{
        if (translator)
        {
	        QApplication::removeTranslator(translator);
                translator = 0;
        }
        if (translatorQt)
        {
	        QApplication::removeTranslator(translatorQt);
                translatorQt = 0;
        }

        if (!val.isEmpty())
        {
                QString fName = ":/config/" + QString("IPM_") + val + ".qm";
                if (!QFile::exists(fName))
                {
                        fName = m_langPath + QString("/IPM_") + val + ".qm";
                }
	        translator = new QTranslator;
	        translator->load(fName);
	        QApplication::installTranslator(translator);

                fName = ":/config/" + QString("qt_") + val + ".qm";
                if (!QFile::exists(fName))
                {
                        fName = m_langPath + QString("/qt_") + val + ".qm";
                }
	        translatorQt = new QTranslator;
	        translatorQt->load(":/config/" + QString("qt_") + val + ".qm");
	        QApplication::installTranslator(translatorQt);
        }
}

void
DataAccess::AutoLogin(bool val)
{
        m_autoLogin = val;
        QDomElement root = domDocument.documentElement();
        QDomElement AutoLogin = root.firstChildElement("AutoLogin");
        QDomElement autologin = domDocument.createElement("AutoLogin");
        QDomText altxt = domDocument.createTextNode(val?"1":"0");
        autologin.appendChild(altxt);
        root.replaceChild(autologin, AutoLogin);
}

void
DataAccess::updateUserNode(const QString& icon, const QString& name, const QString& pwd)
{
        QDomElement root = domDocument.documentElement();
        QDomElement User = root.firstChildElement("User");
        QDomElement newUser = domDocument.createElement("User");
        newUser.setAttribute("icon", icon);
        newUser.setAttribute("password", pwd);

        QDomElement newName = domDocument.createElement("Name");
        newName.setAttribute("default", "true");
        newUser.appendChild(newName);
        QDomText nameText = domDocument.createTextNode(name);
        newName.appendChild(nameText);

        QDomElement Name = User.firstChildElement("Name");
        while(!Name.isNull())
        {
                QString tname = Name.text();
                if (tname == name)
                {
                        Name = Name.nextSiblingElement("Name");
                        continue;
                }
                QDomElement newName = domDocument.createElement("Name");
                newName.setAttribute("default", "false");
                newUser.appendChild(newName);
                QDomText nameText = domDocument.createTextNode(tname);
                newName.appendChild(nameText);

                Name = Name.nextSiblingElement("Name");
        }
        root.replaceChild(newUser, User);
}

void
DataAccess::updateStyle(const QString& name, bool standardPal)
{
        QDomElement root = domDocument.documentElement();
        QDomElement Style = root.firstChildElement("Style");
        Style.setAttribute("name", name);
        Style.setAttribute("useStandardPalette", standardPal?"true":"false");
        m_StyleName = name;
        m_isUseStandardPalette = standardPal;
}

void
DataAccess::addAddress(const QString& addr)
{
        QDomElement root = domDocument.documentElement();
        QDomElement Listen = root.firstChildElement("Listen");

        QDomElement address = domDocument.createElement("Address");
        Listen.appendChild(address);
        QDomText addrtext = domDocument.createTextNode(addr);
        address.appendChild(addrtext);

        addrList.push_back(addr);
        write();
}

void
DataAccess::removeAddress(const QString& addr)
{
        QDomElement root = domDocument.documentElement();
        QDomElement Listen = root.firstChildElement("Listen");
        QDomElement child = Listen.firstChildElement("Address");
        while(!child.isNull())
        {
                QString tname = child.text();
                if (tname == addr)
                {
                        Listen.removeChild(child);
                }
                child = child.nextSiblingElement("Address");
        }

        addrList.removeAll(addr);
}

void
DataAccess::addRemoteUser(UserItem* uitem)
{
        QDomElement root = domDocument.documentElement();
        QDomElement remoteUser = root.firstChildElement("RemoteUser");
        if (remoteUser.isNull())
        {
                remoteUser = domDocument.createElement("RemoteUser");
                root.appendChild(remoteUser);
        }

        QDomElement child = remoteUser.firstChildElement("Item");
        while(!child.isNull())
        {
                QDomElement mac = child.firstChildElement("Mac");
                if (mac.text() == uitem->MacAddress())
                {
                        break;
                }
                child = child.nextSiblingElement("Item");
        }

        QDomElement item = domDocument.createElement("Item");

        QDomElement mac = domDocument.createElement("Mac");
        item.appendChild(mac);
        QDomText mactext = domDocument.createTextNode(uitem->MacAddress());
        mac.appendChild(mactext);

        QDomElement group = domDocument.createElement("Group");
        item.appendChild(group);
        QDomText grouptext = domDocument.createTextNode(uitem->GroupName());
        group.appendChild(grouptext);

        QDomElement ip = domDocument.createElement("IP");
        item.appendChild(ip);
        QDomText iptext = domDocument.createTextNode(uitem->IpAddress());
        ip.appendChild(iptext);

        if (!child.isNull())
        {
                remoteUser.replaceChild(item, child);
        }
        else
        {
                remoteUser.appendChild(item);
        }
}

void
DataAccess::removeRemoteUser(UserItem* uitem)
{
        QDomElement root = domDocument.documentElement();
        QDomElement remoteUser = root.firstChildElement("RemoteUser");
        QDomElement child = remoteUser.firstChildElement("Item");
        while(!child.isNull())
        {
                QDomElement mac = child.firstChildElement("Mac");
                if (mac.text() == uitem->MacAddress())
                {
                        remoteUser.removeChild(child);
                        break;
                }
                child = child.nextSiblingElement("Item");
        }
}

void
DataAccess::addState(int index, const QString& state)
{
        QDomElement root = domDocument.documentElement();
        QDomElement st = root.firstChildElement("State");
        QDomElement itm = domDocument.createElement("Item");
        itm.setAttribute("index", index);
        st.appendChild(itm);
        QDomText stitem = domDocument.createTextNode(state);
        itm.appendChild(stitem);
}

void
DataAccess::updateDefaultState(const QString& state)
{
        QDomElement root = domDocument.documentElement();
        QDomElement st = root.firstChildElement("State");
        st.setAttribute("default", state);
}

void
DataAccess::updateSaveDir(const QString& dir)
{
        QDomElement root = domDocument.documentElement();
        QDomElement Dir = root.firstChildElement("SaveDir");
        QDomElement sdir = domDocument.createElement("SaveDir");
        QDomText savedirtxt = domDocument.createTextNode(dir);
        sdir.appendChild(savedirtxt);
        root.replaceChild(sdir, Dir);
        m_histSaveDir = dir;
}

void
DataAccess::updateOpenDir(const QString& dir)
{
        QDomElement root = domDocument.documentElement();
        QDomElement Dir = root.firstChildElement("OpenDir");
        QDomElement odir = domDocument.createElement("OpenDir");
        QDomText odirtxt = domDocument.createTextNode(dir);
        odir.appendChild(odirtxt);
        root.replaceChild(odir, Dir);
        m_histOpenDir = dir;
}

QString
DataAccess::readSSTFile()
{
        QFile file(m_SSTFileName);
        if (!file.open(QFile::ReadOnly))
        {
                QMessageBox::warning(0, PROGRAM_NAME,
                        tr("Unable to read the file %1: %2")
                        .arg(file.fileName())
                        .arg(file.errorString()));
                return 0;
        }
        const QString& txt = QString::fromUtf8(file.readAll().data());
        file.close();
        return txt;
}

void
DataAccess::writeSSTFile(const QString& fName)
{
        QFile file(fName);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
                QMessageBox::warning(0, PROGRAM_NAME,
                        tr("Unable to save the file %1: %2")
                        .arg(file.fileName())
                        .arg(file.errorString()));
                return;
        }

        QFile filer(":/config/default.sst");
        if (!filer.open(QFile::ReadOnly | QFile::Text))
        {
                QMessageBox::warning(0, PROGRAM_NAME,
                        tr("Unable to read the file %1: %2")
                        .arg(filer.fileName())
                        .arg(filer.errorString()));
                return;
        }

        QByteArray sstArray = filer.readAll();
        filer.close();
        QString const& sst = QString::fromUtf8(sstArray.data(), sstArray.size()+1);

        file.write(sst.toUtf8());
        file.close();

}

FormTalk*
DataAccess::getFormTalk(const QString& key)
{
        UserItem* uitem = userItemCollection.Collection()->value(key, 0);
        if (!uitem)
        {
                return 0;
        }
        FormTalk* ft = talkDlgMap.value(key, 0);
        if (!ft)
        {
                ft = new FormTalk(*this, uitem);
                ft->setWindowTitle(uitem->LoginName());
                ft->setWindowIcon(QIcon(uitem->IconName()));
                talkDlgMap.insert(key, ft);
        }
        ft->show();
        ft->activateWindow();
        return ft;
}


QStringList
DataAccess::GetFileList_in(const QString& path, int* idx, int /*rootIndex*/)
{
        int* index = idx;
        QStringList fileList;
        QDir dir(path);
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::AllDirs);
        dir.setSorting(QDir::DirsFirst | QDir::Name);
        QList<QFileInfo> infoList = dir.entryInfoList();

        for (int i = 0; i < infoList.size(); i++)
        {
                QFileInfo const& finfo = infoList.at(i);
                QString const& fName = finfo.fileName();
                QString const& fPath = finfo.filePath();
                if (fName == "."
                        || fName == "..")
                {
                        continue;
                }
                (*index)++;
                int* thisIndex = index;

                if (finfo.isDir())
                {
                        fileList <<  QString("DIR|") + QString::number(finfo.size()) + "|\t" + fPath;
                        fileList << GetFileList_in(fPath, thisIndex, *thisIndex);
                }
                else
                {
                        fileList <<  QString("FIL|") + QString::number(finfo.size()) + "|\t" + fPath;
                }
        }
        return fileList;
}

QStringList
DataAccess::GetFileList(const QString& path, const int del, quint64* siz)
{
        QStringList fileList;
        QDir dir(path);
        dir.setFilter(QDir::Hidden | QDir::AllEntries | QDir::NoDotAndDotDot);
        dir.setSorting(QDir::DirsFirst | QDir::Name);
        QList<QFileInfo> infoList = dir.entryInfoList();

        for (int i = 0; i < infoList.size(); i++)
        {
                QFileInfo const& finfo = infoList.at(i);
                QString const& fName = finfo.fileName();
                QString fPath = finfo.filePath();
                if (fName == "."
                        || fName == "..")
                {
                        continue;
                }

                QString key = fPath;
                quint64 fsiz = finfo.size();
                if (finfo.isDir())
                {
                        fileList << (QString("DIR$") + QString::number(fsiz) + QString("$") + key.remove(0, del));
                        fileList << GetFileList(fPath, del, siz);
                }
                else
                {
                        fileList << (QString("FILE$") + QString::number(fsiz) + QString("$") + key.remove(0, del));
                        *siz +=fsiz;
                }
        }
        return fileList;
}

QStringList
DataAccess::GetFileList(const QString& path, quint64* siz)
{
        int del = path.size();
        quint64 s = 0;
        QStringList fileList = GetFileList(path, del, &s);
        fileList.prepend("DIR$" + QString::number(s) + "$" + path);
        *siz = s;
        return fileList;
}

QStringList
DataAccess::GetFileList(const QString& path)
{
        int index = 0;
        return GetFileList_in(path, &index, 0);
}
