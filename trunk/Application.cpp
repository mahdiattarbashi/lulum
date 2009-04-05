/*  filename: Application.cpp    2008/12/26  */
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

#include "Application.h"
#include "FormMain.h"
#include "DataAccess.h"

QString PROGRAM_NAME;
QString PROGRAM_VER = "2.0.1";
QString COMPUTER_OS;
QString COMPUTER_UserName;
QString COMPUTER_UserDomain;


Application::Application(int &argc, char **argv)
: QApplication(argc, argv)
{
        QString locale = QLocale::system().name();
        m_DataAccess = new DataAccess();

        //QTranslator* translator = new QTranslator;
        //translator->load(":/config/" + QString("IPM_") + locale + ".qm");
        //installTranslator(translator);
        //translator = new QTranslator;
        //translator->load(":/config/" + QString("qt_") + locale + ".qm");
        //installTranslator(translator);

        mainWindow = new FormMain(*m_DataAccess);
        mainWindow->show();

        setStyleSheet(m_DataAccess->readSSTFile());
}

Application::~Application(void)
{
        delete mainWindow;
        delete m_DataAccess;
}
