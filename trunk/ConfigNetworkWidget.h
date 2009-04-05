/*  filename: ConfigNetworkWidget.h    2009/02/07  */
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

#ifndef __ConfigNetworkWidget_h__
#define __ConfigNetworkWidget_h__

#include "defs.h"
#include "DataAccess.h"

class ConfigNetworkWidget : public QDialog
{
        Q_OBJECT

public:
        ConfigNetworkWidget(DataAccess& dacess, QWidget* parent = 0)
                : QDialog(parent)
                , m_DataAccess(dacess)
        {
                
                addrComboBox = new QComboBox;
                addrComboBox->setMinimumWidth(150);
                addrComboBox->setEditable(true);
                addrComboBox->addItems(m_DataAccess.addrList);

                int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
                QSize iconSize(size, size);

                QToolButton* addbut = new QToolButton;
                addbut->setIconSize(iconSize);
                addbut->setToolTip(tr("Add current IP Address to the login network list"));
                QToolButton* subbut = new QToolButton;
                subbut->setIconSize(iconSize);
                subbut->setToolTip(tr("Remove current IP Address from the login network list"));
                addbut->setIcon(QIcon(DataAccess::SkinPath() + "/add-line.png"));
                subbut->setIcon(QIcon(DataAccess::SkinPath() + "/delete.png"));
                connect(addbut, SIGNAL(clicked()), this, SLOT(slotAddAddress()));
                connect(subbut, SIGNAL(clicked()), this, SLOT(slotRemoveAddress()));

                QDialogButtonBox* dbutbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
                connect(dbutbox, SIGNAL(accepted()), this, SLOT(accept()));
                connect(dbutbox, SIGNAL(rejected()), this, SLOT(reject()));

                QGroupBox* groupBox = new QGroupBox(tr("Network Configure"));
                QHBoxLayout* hlayout = new QHBoxLayout(groupBox);
                hlayout->addWidget(addrComboBox);
                hlayout->addWidget(addbut);
                hlayout->addWidget(subbut);
                hlayout->addStretch();

                QLabel* tip = new QLabel("<b>" + tr("Tip") + tr(":") + " </b>" + tr("You need restart the program to take effect."));
                tip->setWordWrap(true);
                tip->setStyleSheet("color: red");
                QVBoxLayout* vlayout = new QVBoxLayout(this);
                vlayout->addWidget(groupBox);
                vlayout->addWidget(tip);
                vlayout->addStretch();
                vlayout->addWidget(dbutbox);

                setWindowTitle(tr("Network Configure"));
        }

public:
protected slots:
        void slotAddAddress();
        void slotRemoveAddress();
private:
        QComboBox* addrComboBox;
        DataAccess& m_DataAccess;
};

inline void
ConfigNetworkWidget::slotAddAddress()
{
        int idx = addrComboBox->findText(addrComboBox->currentText());
        if (idx == -1)
        {
                addrComboBox->addItem(addrComboBox->currentText());
                Address addr;
                //(0, addrComboBox->currentText().toAscii().data());
                if (addr.set((unsigned short)0, addrComboBox->currentText().toAscii().data()) != -1)
                {
                        m_DataAccess.addAddress(addrComboBox->currentText());
                }
        }
}

inline void
ConfigNetworkWidget::slotRemoveAddress()
{
        int idx = addrComboBox->findText(addrComboBox->currentText());
        if (idx != -1)
        {
                m_DataAccess.removeAddress(addrComboBox->currentText());
                addrComboBox->removeItem(idx);
        }
}

#endif // __ConfigNetworkWidget_h__
