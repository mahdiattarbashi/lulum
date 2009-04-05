/*  filename: ConfigStyleWidget.h    2009/02/07  */
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

#ifndef __ConfigStyleWidget_h__
#define __ConfigStyleWidget_h__

#include "defs.h"
#include "DataAccess.h"

class ConfigStyleWidget : public QDialog
{
        Q_OBJECT

public:
        ConfigStyleWidget(DataAccess& dacess, QWidget *parent = 0)
                : QDialog(parent)
                , m_DataAccess(dacess)
        {
                originalPalette = QApplication::palette();

                styleComboBox = new QComboBox;
                styleComboBox->setFixedWidth(150);
                styleComboBox->addItems(QStyleFactory::keys());
                connect(styleComboBox, SIGNAL(activated(const QString&)),
                        this, SLOT(changeStyle(const QString&)));

                useStylePaletteCheckBox = new QCheckBox(tr("&Use style's standard palette"));
                useStylePaletteCheckBox->setChecked(m_DataAccess.UseStandardPalette());
                connect(useStylePaletteCheckBox, SIGNAL(toggled(bool)),
                        this, SLOT(changePalette()));

                QDialogButtonBox* dbutbox = new QDialogButtonBox(QDialogButtonBox::Ok);
                connect(dbutbox, SIGNAL(accepted()), this, SLOT(accept()));

                QHBoxLayout *topLayout = new QHBoxLayout;
                topLayout->addWidget(styleComboBox);
                topLayout->addWidget(useStylePaletteCheckBox);
                topLayout->addStretch();

                QGroupBox* groupBox = new QGroupBox(tr("Style Configure"));
                groupBox->setLayout(topLayout);

                QVBoxLayout* vlayout = new QVBoxLayout(this);
                vlayout->addWidget(groupBox);
                vlayout->addWidget(dbutbox);

                setWindowTitle(tr("Style Configure"));
                int idx = styleComboBox->findText(m_DataAccess.StyleName());
                if (idx != -1)
                {
                        styleComboBox->setCurrentIndex(idx);
                } 

                changeStyle(styleComboBox->currentText());
                setWindowTitle(tr("Style Configure"));
        }

public slots:
        void changeStyle(const QString& styleName);
        void changePalette();

private:
        QPalette originalPalette;

        QLabel *styleLabel;
        QComboBox *styleComboBox;
        QCheckBox *useStylePaletteCheckBox;

        DataAccess& m_DataAccess;
};

inline void
ConfigStyleWidget::changeStyle(const QString& styleName)
{
        QApplication::setStyle(QStyleFactory::create(styleName));
        changePalette();

        m_DataAccess.updateStyle(styleName, useStylePaletteCheckBox->isChecked());
}

inline void
ConfigStyleWidget::changePalette()
{
        if (useStylePaletteCheckBox->isChecked())
                QApplication::setPalette(QApplication::style()->standardPalette());
        else
                QApplication::setPalette(originalPalette);
}

#endif // __ConfigStyleWidget_h__
