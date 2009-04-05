/*  filename: DataAccess.h    2009/02/09  */
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

#ifndef __DataAccess_h__
#define __DataAccess_h__

#include "defs.h"
#include "UserItemCollection.h"

#include <QDomDocument>

namespace IPM
{
        extern bool delItemWhenFinished;
        extern bool updateDirFileItemInfo;
}

class EmotionWidget;
class FormTalk;
class SharingWidget;

class DataAccess : public QWidget
{
        Q_OBJECT

public:
        DataAccess(void);
        ~DataAccess(void);
public slots:
        void slotEmotionsAdded();
        void sendLoginMsg();
        void sendLogoutMsg();
        void slotSharingConfig();
public:
        QString readSSTFile();
        static void writeSSTFile(const QString& fName);
        static void sendMsg(Message_ptr m);
        static void MultiSendMsg(Message_ptr m, QList<UserItem*> const& toList);
        static void BCastMsg(Message_ptr m);
        static void sendTextMsg(const QString& text, const QString& toIP);
        static bool CreateTextMsg(const QString& text, Message_ptr&);
        bool CreateSignInMsg(Message_ptr&);

        static QStringList GetFileList(const QString& path);
        static QStringList GetFileList(const QString& path, quint64* siz);
        static QStringList GetFileList(const QString& path, const int del, quint64* siz);
        static QStringList GetFileList_in(const QString& path, int* idx, int rootIndex);

        QString StyleName() const { return m_StyleName; }
        void StyleName(QString val) { m_StyleName = val; }
        bool UseStandardPalette() const { return m_isUseStandardPalette; }
        void UseStandardPalette(bool val) { m_isUseStandardPalette = val; }

        static quint16 genTcpPort() { tcpPort++; if (tcpPort>65534) tcpPort = 50001; return tcpPort;}
        static QString genFileKey(const QHash<QString, QString>& hash, const QString& pathname, bool& alreadyExist);

public:
        void addFaces();
        void addEmotions();
        void updateUserNode(const QString& icon, const QString& name, const QString& pwd);
        void updateStyle(const QString& name, bool standardPal = true);
        void addAddress(const QString& addr);
        void removeAddress(const QString& addr);
        void addState(int index, const QString& state);
        void updateDefaultState(const QString& state);
        void updateSaveDir(const QString& dir);
        void updateOpenDir(const QString& dir);
        static QString SaveDir() { return m_histSaveDir; }
        static QString OpenDir() { return m_histOpenDir; }

        static QString ConfigPath() { return m_configPath; }
        static void ConfigPath(QString val) { m_configPath = val; }
        static QString LogPath() { return m_logPath; }
        static QString SkinPath() { return m_skinPath; }
        static void SkinPath(const QString& val) { m_skinPath = val; }
        static QString EmotionPath() { return m_emotionPath; }
        static void EmotionPath(QString val) { m_emotionPath = val; }
        static QString FacePath() { return m_facePath; }
        static void FacePath(QString val) { m_facePath = val; }
        static QString RecvImgPath() { return m_recvImgPath; }
        static void RecvImgPath(QString val) { m_recvImgPath = val; }
        static QString LogosPath() { return m_logosPath; }
        static void LogosPath(QString val) { m_logosPath = val; }

        static QString DefaultGroupName() { return m_defaultGroupName; }
        static void DefaultGroupName(QString val) { m_defaultGroupName = val; }
        bool AutoLogin() const { return m_autoLogin; }
        void AutoLogin(bool val);
        void addRemoteUser(UserItem* uitem);
        void removeRemoteUser(UserItem* uitem);

        QString Language() const { return m_language; }
        void Language(QString val);
        void SetTranslator(QString val);
        FormTalk* getFormTalk(const QString& key);

        UserItemCollection userItemCollection;

        static QStringList addrList;    // ������ַ
        QStringList nameList;           // ʹ�ù�������

        QHash<int, QString> stateMap;
        int defaultState;
        int currentState;

        QHash<QString, QIcon> faceMap;
        QHash<QString, QIcon> emotionMap;
        EmotionWidget* emotionView;
        EmotionWidget* faceView;

        QHash<QString, FormTalk*> talkDlgMap;

        QHash<QString, QString> sharingFilesMap;
        SharingWidget* sharingFilesWidget;

        QTimer m_secTimer;

private:
        void init();
        bool read();
        bool write();

private:
        QDomDocument domDocument;

        QString m_StyleName;
        bool m_isUseStandardPalette;

        bool m_autoLogin;
        static quint16 tcpPort;
        static QString m_defaultGroupName;

        static QString m_histOpenDir;
        static QString m_histSaveDir;

        static QString m_langPath;
        static QString m_configPath;
        static QString m_logPath;
        static QString m_skinPath;
        static QString m_logosPath;
        static QString m_emotionPath;
        static QString m_facePath;
        static QString m_recvImgPath;
        QString m_ConfigFileName;
        QString m_SSTFileName;

        QTranslator* translator;
        QTranslator* translatorQt;
        QString m_language;
};

#endif // __DataAccess_h__
