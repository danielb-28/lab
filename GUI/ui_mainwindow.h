/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QFrame *horizontalFrame;
    QGridLayout *gridLayout;
    QPushButton *bt_parar;
    QLabel *label_4;
    QLabel *label_6;
    QLabel *label_baud;
    QSpacerItem *verticalSpacer_2;
    QComboBox *comboBox_amostras;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *verticalSpacer;
    QLabel *label_status;
    QLabel *label_clock;
    QLabel *label;
    QLabel *label_amostras;
    QLabel *label_5;
    QPushButton *bt_salvar;
    QComboBox *comboBox_clock;
    QLabel *label_8;
    QLabel *label_3;
    QComboBox *comboBox_porta;
    QLabel *label_fps;
    QCustomPlot *plot_widget;
    QLabel *label_7;
    QLabel *label_2;
    QSpacerItem *verticalSpacer_4;
    QPushButton *bt_inicio;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralwidget->sizePolicy().hasHeightForWidth());
        centralwidget->setSizePolicy(sizePolicy);
        horizontalFrame = new QFrame(centralwidget);
        horizontalFrame->setObjectName(QStringLiteral("horizontalFrame"));
        horizontalFrame->setGeometry(QRect(9, 9, 781, 581));
        sizePolicy.setHeightForWidth(horizontalFrame->sizePolicy().hasHeightForWidth());
        horizontalFrame->setSizePolicy(sizePolicy);
        horizontalFrame->setFrameShadow(QFrame::Plain);
        gridLayout = new QGridLayout(horizontalFrame);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        gridLayout->setContentsMargins(-1, -1, 20, -1);
        bt_parar = new QPushButton(horizontalFrame);
        bt_parar->setObjectName(QStringLiteral("bt_parar"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(bt_parar->sizePolicy().hasHeightForWidth());
        bt_parar->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(bt_parar, 15, 1, 1, 2);

        label_4 = new QLabel(horizontalFrame);
        label_4->setObjectName(QStringLiteral("label_4"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(label_4->sizePolicy().hasHeightForWidth());
        label_4->setSizePolicy(sizePolicy2);
        label_4->setMinimumSize(QSize(0, 0));

        gridLayout->addWidget(label_4, 8, 1, 1, 1);

        label_6 = new QLabel(horizontalFrame);
        label_6->setObjectName(QStringLiteral("label_6"));
        sizePolicy2.setHeightForWidth(label_6->sizePolicy().hasHeightForWidth());
        label_6->setSizePolicy(sizePolicy2);
        label_6->setMinimumSize(QSize(0, 0));

        gridLayout->addWidget(label_6, 11, 1, 1, 1);

        label_baud = new QLabel(horizontalFrame);
        label_baud->setObjectName(QStringLiteral("label_baud"));
        sizePolicy2.setHeightForWidth(label_baud->sizePolicy().hasHeightForWidth());
        label_baud->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_baud, 10, 2, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_2, 7, 1, 1, 2);

        comboBox_amostras = new QComboBox(horizontalFrame);
        comboBox_amostras->addItem(QString());
        comboBox_amostras->addItem(QString());
        comboBox_amostras->addItem(QString());
        comboBox_amostras->addItem(QString());
        comboBox_amostras->addItem(QString());
        comboBox_amostras->addItem(QString());
        comboBox_amostras->addItem(QString());
        comboBox_amostras->addItem(QString());
        comboBox_amostras->setObjectName(QStringLiteral("comboBox_amostras"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(comboBox_amostras->sizePolicy().hasHeightForWidth());
        comboBox_amostras->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(comboBox_amostras, 4, 1, 1, 2);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_3, 13, 1, 1, 2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Preferred);

        gridLayout->addItem(verticalSpacer, 17, 1, 1, 2);

        label_status = new QLabel(horizontalFrame);
        label_status->setObjectName(QStringLiteral("label_status"));
        sizePolicy2.setHeightForWidth(label_status->sizePolicy().hasHeightForWidth());
        label_status->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_status, 8, 2, 1, 1);

        label_clock = new QLabel(horizontalFrame);
        label_clock->setObjectName(QStringLiteral("label_clock"));
        sizePolicy2.setHeightForWidth(label_clock->sizePolicy().hasHeightForWidth());
        label_clock->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_clock, 12, 2, 1, 1);

        label = new QLabel(horizontalFrame);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy4(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy4);
        label->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(label, 1, 1, 1, 2);

        label_amostras = new QLabel(horizontalFrame);
        label_amostras->setObjectName(QStringLiteral("label_amostras"));
        sizePolicy2.setHeightForWidth(label_amostras->sizePolicy().hasHeightForWidth());
        label_amostras->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_amostras, 11, 2, 1, 1);

        label_5 = new QLabel(horizontalFrame);
        label_5->setObjectName(QStringLiteral("label_5"));
        sizePolicy2.setHeightForWidth(label_5->sizePolicy().hasHeightForWidth());
        label_5->setSizePolicy(sizePolicy2);
        label_5->setMinimumSize(QSize(0, 0));

        gridLayout->addWidget(label_5, 10, 1, 1, 1);

        bt_salvar = new QPushButton(horizontalFrame);
        bt_salvar->setObjectName(QStringLiteral("bt_salvar"));
        sizePolicy1.setHeightForWidth(bt_salvar->sizePolicy().hasHeightForWidth());
        bt_salvar->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(bt_salvar, 16, 1, 1, 2);

        comboBox_clock = new QComboBox(horizontalFrame);
        comboBox_clock->addItem(QString());
        comboBox_clock->addItem(QString());
        comboBox_clock->addItem(QString());
        comboBox_clock->addItem(QString());
        comboBox_clock->addItem(QString());
        comboBox_clock->addItem(QString());
        comboBox_clock->addItem(QString());
        comboBox_clock->addItem(QString());
        comboBox_clock->setObjectName(QStringLiteral("comboBox_clock"));
        sizePolicy3.setHeightForWidth(comboBox_clock->sizePolicy().hasHeightForWidth());
        comboBox_clock->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(comboBox_clock, 6, 1, 1, 2);

        label_8 = new QLabel(horizontalFrame);
        label_8->setObjectName(QStringLiteral("label_8"));
        sizePolicy2.setHeightForWidth(label_8->sizePolicy().hasHeightForWidth());
        label_8->setSizePolicy(sizePolicy2);
        label_8->setMinimumSize(QSize(0, 0));

        gridLayout->addWidget(label_8, 9, 1, 1, 1);

        label_3 = new QLabel(horizontalFrame);
        label_3->setObjectName(QStringLiteral("label_3"));
        sizePolicy4.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy4);
        label_3->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(label_3, 5, 1, 1, 2);

        comboBox_porta = new QComboBox(horizontalFrame);
        comboBox_porta->setObjectName(QStringLiteral("comboBox_porta"));
        sizePolicy3.setHeightForWidth(comboBox_porta->sizePolicy().hasHeightForWidth());
        comboBox_porta->setSizePolicy(sizePolicy3);

        gridLayout->addWidget(comboBox_porta, 2, 1, 1, 2);

        label_fps = new QLabel(horizontalFrame);
        label_fps->setObjectName(QStringLiteral("label_fps"));
        sizePolicy2.setHeightForWidth(label_fps->sizePolicy().hasHeightForWidth());
        label_fps->setSizePolicy(sizePolicy2);

        gridLayout->addWidget(label_fps, 9, 2, 1, 1);

        plot_widget = new QCustomPlot(horizontalFrame);
        plot_widget->setObjectName(QStringLiteral("plot_widget"));
        sizePolicy.setHeightForWidth(plot_widget->sizePolicy().hasHeightForWidth());
        plot_widget->setSizePolicy(sizePolicy);

        gridLayout->addWidget(plot_widget, 0, 0, 18, 1);

        label_7 = new QLabel(horizontalFrame);
        label_7->setObjectName(QStringLiteral("label_7"));
        sizePolicy2.setHeightForWidth(label_7->sizePolicy().hasHeightForWidth());
        label_7->setSizePolicy(sizePolicy2);
        label_7->setMinimumSize(QSize(0, 0));

        gridLayout->addWidget(label_7, 12, 1, 1, 1);

        label_2 = new QLabel(horizontalFrame);
        label_2->setObjectName(QStringLiteral("label_2"));
        sizePolicy4.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy4);
        label_2->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);

        gridLayout->addWidget(label_2, 3, 1, 1, 2);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer_4, 0, 1, 1, 2);

        bt_inicio = new QPushButton(horizontalFrame);
        bt_inicio->setObjectName(QStringLiteral("bt_inicio"));
        sizePolicy1.setHeightForWidth(bt_inicio->sizePolicy().hasHeightForWidth());
        bt_inicio->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(bt_inicio, 14, 1, 1, 2);

        MainWindow->setCentralWidget(centralwidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Interface ADC mbed", nullptr));
        bt_parar->setText(QApplication::translate("MainWindow", "Parar", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "Status / Porta: ", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "N\303\272mero de Amostras:", nullptr));
        label_baud->setText(QApplication::translate("MainWindow", "000000 baud", nullptr));
        comboBox_amostras->setItemText(0, QApplication::translate("MainWindow", "50", nullptr));
        comboBox_amostras->setItemText(1, QApplication::translate("MainWindow", "100", nullptr));
        comboBox_amostras->setItemText(2, QApplication::translate("MainWindow", "250", nullptr));
        comboBox_amostras->setItemText(3, QApplication::translate("MainWindow", "500", nullptr));
        comboBox_amostras->setItemText(4, QApplication::translate("MainWindow", "1000", nullptr));
        comboBox_amostras->setItemText(5, QApplication::translate("MainWindow", "2000", nullptr));
        comboBox_amostras->setItemText(6, QApplication::translate("MainWindow", "3000", nullptr));
        comboBox_amostras->setItemText(7, QApplication::translate("MainWindow", "4000", nullptr));

        label_status->setText(QApplication::translate("MainWindow", "Desconectado", nullptr));
        label_clock->setText(QApplication::translate("MainWindow", "0 MHz / 0 Ksps", nullptr));
        label->setText(QApplication::translate("MainWindow", "Porta Serial", nullptr));
        label_amostras->setText(QApplication::translate("MainWindow", "0", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "Baud Rate Serial :", nullptr));
        bt_salvar->setText(QApplication::translate("MainWindow", "Salvar", nullptr));
        comboBox_clock->setItemText(0, QApplication::translate("MainWindow", "12.0 MHz / 188 Ks/s", nullptr));
        comboBox_clock->setItemText(1, QApplication::translate("MainWindow", "6.0 MHz  / 94 Ks/s", nullptr));
        comboBox_clock->setItemText(2, QApplication::translate("MainWindow", "4.0 MHz / 63 Ks/s", nullptr));
        comboBox_clock->setItemText(3, QApplication::translate("MainWindow", "3.0 MHz / 47 Ks/s", nullptr));
        comboBox_clock->setItemText(4, QApplication::translate("MainWindow", "2.4 MHz / 38 Ks/s", nullptr));
        comboBox_clock->setItemText(5, QApplication::translate("MainWindow", "2.0 MHz / 31 Ks/s", nullptr));
        comboBox_clock->setItemText(6, QApplication::translate("MainWindow", "1.7 MHz / 27 Ks/s", nullptr));
        comboBox_clock->setItemText(7, QApplication::translate("MainWindow", "1.5 MHz / 23 Ks/s", nullptr));

        label_8->setText(QApplication::translate("MainWindow", "FPS Instant\303\242neo:", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "Clock / Taxa de Amostragem", nullptr));
        label_fps->setText(QApplication::translate("MainWindow", "00.00 FPS", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "Taxa de Amostragem:", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "N\303\272mero de Amostras", nullptr));
        bt_inicio->setText(QApplication::translate("MainWindow", "Iniciar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
