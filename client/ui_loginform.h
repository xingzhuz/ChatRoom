/********************************************************************************
** Form generated from reading UI file 'loginform.ui'
**
** Created by: Qt User Interface Compiler version 5.12.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINFORM_H
#define UI_LOGINFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginForm
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_3;
    QWidget *widget_3;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLineEdit *passwordLineEdit;
    QLineEdit *userIdLineEdit;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer;
    QLabel *label;
    QSpacerItem *verticalSpacer;
    QWidget *widget;
    QGridLayout *gridLayout_2;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *registerPushButton;
    QPushButton *loginPushButton;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QWidget *LoginForm)
    {
        if (LoginForm->objectName().isEmpty())
            LoginForm->setObjectName(QString::fromUtf8("LoginForm"));
        LoginForm->resize(400, 300);
        LoginForm->setMinimumSize(QSize(400, 300));
        LoginForm->setMaximumSize(QSize(400, 300));
        verticalLayout = new QVBoxLayout(LoginForm);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_3);

        widget_3 = new QWidget(LoginForm);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_3->sizePolicy().hasHeightForWidth());
        widget_3->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(widget_3);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(widget_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 2, 1, 1, 1);

        passwordLineEdit = new QLineEdit(widget_3);
        passwordLineEdit->setObjectName(QString::fromUtf8("passwordLineEdit"));
        passwordLineEdit->setMaxLength(16);
        passwordLineEdit->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(passwordLineEdit, 2, 2, 1, 1);

        userIdLineEdit = new QLineEdit(widget_3);
        userIdLineEdit->setObjectName(QString::fromUtf8("userIdLineEdit"));
        userIdLineEdit->setMaxLength(6);

        gridLayout->addWidget(userIdLineEdit, 0, 2, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer, 0, 3, 1, 1);

        label = new QLabel(widget_3);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 1, 1, 1);

        verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 1, 2, 1, 1);


        verticalLayout->addWidget(widget_3);

        widget = new QWidget(LoginForm);
        widget->setObjectName(QString::fromUtf8("widget"));
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        gridLayout_2 = new QGridLayout(widget);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        horizontalSpacer_6 = new QSpacerItem(60, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_6, 0, 3, 1, 1);

        registerPushButton = new QPushButton(widget);
        registerPushButton->setObjectName(QString::fromUtf8("registerPushButton"));

        gridLayout_2->addWidget(registerPushButton, 0, 2, 1, 1);

        loginPushButton = new QPushButton(widget);
        loginPushButton->setObjectName(QString::fromUtf8("loginPushButton"));

        gridLayout_2->addWidget(loginPushButton, 0, 1, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(80, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_5, 0, 0, 1, 1);


        verticalLayout->addWidget(widget);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);


        retranslateUi(LoginForm);

        QMetaObject::connectSlotsByName(LoginForm);
    } // setupUi

    void retranslateUi(QWidget *LoginForm)
    {
        LoginForm->setWindowTitle(QApplication::translate("LoginForm", "\347\231\273\345\275\225\347\252\227\345\217\243", nullptr));
        label_2->setText(QApplication::translate("LoginForm", "\345\257\206\347\240\201", nullptr));
        label->setText(QApplication::translate("LoginForm", "\350\264\246\346\210\267ID", nullptr));
        registerPushButton->setText(QApplication::translate("LoginForm", "\346\263\250\345\206\214", nullptr));
        loginPushButton->setText(QApplication::translate("LoginForm", "\347\231\273\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginForm: public Ui_LoginForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINFORM_H
