/********************************************************************************
** Form generated from reading UI file 'authentification.ui'
**
** Created by: Qt User Interface Compiler version 6.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTHENTIFICATION_H
#define UI_AUTHENTIFICATION_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QLineEdit *lineEdit_2;
    QLabel *label_3;
    QLabel *motdepasseoublier;
    QLabel *label_2;
    QLabel *label;
    QPushButton *add_employee_btn_3;
    QLabel *motdepasseoublier_2;
    QLineEdit *lineEdit;
    QPushButton *add_employee_btn_4;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(775, 563);
        lineEdit_2 = new QLineEdit(Dialog);
        lineEdit_2->setObjectName("lineEdit_2");
        lineEdit_2->setGeometry(QRect(280, 280, 231, 31));
        lineEdit_2->setMouseTracking(true);
        lineEdit_2->setTabletTracking(false);
        lineEdit_2->setAcceptDrops(true);
        lineEdit_2->setAutoFillBackground(false);
        lineEdit_2->setStyleSheet(QString::fromUtf8("background-color: rgb(9, 130, 230);\n"
"background-color: qlineargradient(spread:repeat, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(9, 130, 230 ,10) stop:1 rgba(255, 255, 255, 255));\n"
"border-radius:20px;\n"
"font-family: Lexend;\n"
"border-radius:5px;\n"
"color:#001F54;  /* Navy Blue */\n"
"\n"
"font-size:20px;\n"
"border-color: #001F54;  /* Navy Blue */\n"
"\n"
"border-style:20px solid #0982e6;;\n"
"\n"
"\n"
"QLineEdit\n"
"{\n"
"background:transparent;\n"
"border:none;\n"
"color:#0982e6;\n"
"border.bottum:1px solid #0982e6;\n"
"border-radius:25px;\n"
"}"));
        lineEdit_2->setFrame(true);
        lineEdit_2->setEchoMode(QLineEdit::EchoMode::PasswordEchoOnEdit);
        lineEdit_2->setDragEnabled(false);
        lineEdit_2->setReadOnly(false);
        lineEdit_2->setCursorMoveStyle(Qt::CursorMoveStyle::LogicalMoveStyle);
        lineEdit_2->setClearButtonEnabled(true);
        label_3 = new QLabel(Dialog);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(320, 70, 141, 131));
        label_3->setStyleSheet(QString::fromUtf8(""));
        label_3->setFrameShape(QFrame::Shape::StyledPanel);
        label_3->setPixmap(QPixmap(QString::fromUtf8("../\342\200\224Pngtree\342\200\224user name password data (3).png")));
        motdepasseoublier = new QLabel(Dialog);
        motdepasseoublier->setObjectName("motdepasseoublier");
        motdepasseoublier->setEnabled(true);
        motdepasseoublier->setGeometry(QRect(390, 340, 121, 16));
        motdepasseoublier->setMouseTracking(false);
        motdepasseoublier->setTabletTracking(false);
        motdepasseoublier->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
        motdepasseoublier->setAcceptDrops(false);
        motdepasseoublier->setAutoFillBackground(false);
        motdepasseoublier->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
        motdepasseoublier->setInputMethodHints(Qt::InputMethodHint::ImhNone);
        motdepasseoublier->setFrameShape(QFrame::Shape::Box);
        motdepasseoublier->setFrameShadow(QFrame::Shadow::Plain);
        motdepasseoublier->setLineWidth(1);
        motdepasseoublier->setMidLineWidth(4);
        motdepasseoublier->setTextFormat(Qt::TextFormat::AutoText);
        motdepasseoublier->setScaledContents(true);
        motdepasseoublier->setWordWrap(true);
        motdepasseoublier->setOpenExternalLinks(true);
        label_2 = new QLabel(Dialog);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(10, 0, 141, 121));
        label_2->setPixmap(QPixmap(QString::fromUtf8("../logo (2).png")));
        label = new QLabel(Dialog);
        label->setObjectName("label");
        label->setEnabled(true);
        label->setGeometry(QRect(0, 0, 775, 563));
        label->setAutoFillBackground(false);
        label->setStyleSheet(QString::fromUtf8(""));
        label->setFrameShape(QFrame::Shape::NoFrame);
        label->setScaledContents(true);
        add_employee_btn_3 = new QPushButton(Dialog);
        add_employee_btn_3->setObjectName("add_employee_btn_3");
        add_employee_btn_3->setGeometry(QRect(660, 460, 91, 51));
        add_employee_btn_3->setStyleSheet(QString::fromUtf8("*{\n"
"border-radius:25px;\n"
"color:#001F54;  /* Navy Blue */\n"
"font-family: Lexend;\n"
"font-size:16px;\n"
"border-color: #001F54;  /* Navy Blue */\n"
"border-width : 1.2px;\n"
"border-style:solid;\n"
"background-color:white;\n"
"background-color: qlineargradient(spread:repeat, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(9, 130, 230 ,10) stop:1 rgba(255, 255, 255, 255));\n"
"}\n"
"QPushButton:hover\n"
"{\n"
"background:white;\n"
"color:#001F54;  /* Navy Blue */\n"
"border-left:3px solid #00b521 ; \n"
"border-color:none;\n"
"border-radius:25px;\n"
"background-color: qlineargradient(spread:repeat, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(9, 130, 230 ,10) stop:1 rgba(255, 255, 255, 255));\n"
"}\n"
"QPushButton:pressed\n"
"{\n"
"background:#013220;\n"
"color:#001F54;  /* Navy Blue */\n"
"border-radius:25px;\n"
"font-family: Lexend;\n"
"font-size:16px;\n"
"border-left:3.2px solid #1BA019 ; \n"
"/*background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                stop: 0 #ffffff, stop: 1 #dcdcdc);*/}"
                        "\n"
"\n"
""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/images/add-circle.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
        add_employee_btn_3->setIcon(icon);
        add_employee_btn_3->setIconSize(QSize(200, 200));
        add_employee_btn_3->setAutoRepeatInterval(100);
        motdepasseoublier_2 = new QLabel(Dialog);
        motdepasseoublier_2->setObjectName("motdepasseoublier_2");
        motdepasseoublier_2->setEnabled(true);
        motdepasseoublier_2->setGeometry(QRect(430, 360, 61, 21));
        motdepasseoublier_2->setMouseTracking(true);
        motdepasseoublier_2->setTabletTracking(false);
        motdepasseoublier_2->setContextMenuPolicy(Qt::ContextMenuPolicy::DefaultContextMenu);
        motdepasseoublier_2->setAcceptDrops(false);
        motdepasseoublier_2->setAutoFillBackground(false);
        motdepasseoublier_2->setStyleSheet(QString::fromUtf8("color: rgb(255, 255, 255);"));
        motdepasseoublier_2->setInputMethodHints(Qt::InputMethodHint::ImhNone);
        motdepasseoublier_2->setFrameShape(QFrame::Shape::NoFrame);
        motdepasseoublier_2->setFrameShadow(QFrame::Shadow::Plain);
        motdepasseoublier_2->setLineWidth(1);
        motdepasseoublier_2->setMidLineWidth(4);
        motdepasseoublier_2->setTextFormat(Qt::TextFormat::AutoText);
        motdepasseoublier_2->setScaledContents(true);
        motdepasseoublier_2->setWordWrap(true);
        motdepasseoublier_2->setMargin(0);
        motdepasseoublier_2->setOpenExternalLinks(true);
        lineEdit = new QLineEdit(Dialog);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setGeometry(QRect(280, 210, 231, 31));
        lineEdit->setMouseTracking(true);
        lineEdit->setTabletTracking(false);
        lineEdit->setAcceptDrops(true);
        lineEdit->setAutoFillBackground(false);
        lineEdit->setStyleSheet(QString::fromUtf8("background-color: rgb(9, 130, 230);\n"
"background-color: qlineargradient(spread:repeat, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(9, 130, 230 ,10) stop:1 rgba(255, 255, 255, 255));\n"
"border-radius:20px;\n"
"font-family: Lexend;\n"
"border-radius:5px;\n"
"color:#001F54;  /* Navy Blue */\n"
"\n"
"font-size:20px;\n"
"border-color: #001F54;  /* Navy Blue */\n"
"\n"
"border-style:20px solid #0982e6;;\n"
"\n"
"\n"
"QLineEdit\n"
"{\n"
"background:transparent;\n"
"border:none;\n"
"color:#0982e6;\n"
"border.bottum:1px solid #0982e6;\n"
"border-radius:25px;\n"
"}"));
        lineEdit->setEchoMode(QLineEdit::EchoMode::NoEcho);
        lineEdit->setDragEnabled(false);
        lineEdit->setReadOnly(false);
        lineEdit->setCursorMoveStyle(Qt::CursorMoveStyle::LogicalMoveStyle);
        lineEdit->setClearButtonEnabled(true);
        add_employee_btn_4 = new QPushButton(Dialog);
        add_employee_btn_4->setObjectName("add_employee_btn_4");
        add_employee_btn_4->setGeometry(QRect(540, 460, 91, 51));
        add_employee_btn_4->setStyleSheet(QString::fromUtf8("*{\n"
"border-radius:25px;\n"
"color:#001F54;  /* Navy Blue */\n"
"font-family: Lexend;\n"
"font-size:16px;\n"
"border-color: #001F54;  /* Navy Blue */\n"
"border-width : 1.2px;\n"
"border-style:solid;\n"
"background-color:white;\n"
"background-color: qlineargradient(spread:repeat, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(9, 130, 230 ,10) stop:1 rgba(255, 255, 255, 255));\n"
"}\n"
"QPushButton:hover\n"
"{\n"
"background:white;\n"
"color:#001F54;  /* Navy Blue */\n"
"border-left:3px solid #00b521 ; \n"
"border-color:none;\n"
"border-radius:25px;\n"
"background-color: qlineargradient(spread:repeat, x1:0, y1:0, x2:1, y2:1, stop:0 rgba(9, 130, 230 ,10) stop:1 rgba(255, 255, 255, 255));\n"
"}\n"
"QPushButton:pressed\n"
"{\n"
"background:#013220;\n"
"color:#001F54;  /* Navy Blue */\n"
"border-radius:25px;\n"
"font-family: Lexend;\n"
"font-size:16px;\n"
"border-left:3.2px solid #1BA019 ; \n"
"/*background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,\n"
"                                stop: 0 #ffffff, stop: 1 #dcdcdc);*/}"
                        "\n"
"\n"
""));
        add_employee_btn_4->setIcon(icon);
        add_employee_btn_4->setIconSize(QSize(200, 200));
        add_employee_btn_4->setAutoRepeatInterval(100);
        label->raise();
        label_3->raise();
        lineEdit->raise();
        motdepasseoublier_2->raise();
        add_employee_btn_3->raise();
        add_employee_btn_4->raise();
        label_2->raise();
        lineEdit_2->raise();
        motdepasseoublier->raise();

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
        lineEdit_2->setText(QCoreApplication::translate("Dialog", "Mot de passe ", nullptr));
        label_3->setText(QString());
        motdepasseoublier->setText(QCoreApplication::translate("Dialog", "Mot de passe oublie?", nullptr));
        label_2->setText(QString());
        label->setText(QString());
        add_employee_btn_3->setText(QCoreApplication::translate("Dialog", "connexion", nullptr));
        motdepasseoublier_2->setText(QCoreApplication::translate("Dialog", " s'inscrire", nullptr));
        lineEdit->setText(QCoreApplication::translate("Dialog", "identifiant", nullptr));
        add_employee_btn_4->setText(QCoreApplication::translate("Dialog", "face ID ", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTHENTIFICATION_H
