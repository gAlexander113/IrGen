#include "logindialogcl.h"

LoginDialogCl::LoginDialogCl(QWidget *parent) :
    QDialog(parent)
  , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    ui->lineEditPassword->setEchoMode(QLineEdit::Password);

    connect(ui->pushButtonOk, SIGNAL(clicked()), this, SLOT(slotgetdata()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(this, SIGNAL(sendAc()), SLOT(accept()));
}

LoginDialogCl::~LoginDialogCl()
{
    delete ui;
}

QString LoginDialogCl::getname() const
{
    return name;
}

QString LoginDialogCl::getpass() const
{
    return password;
}


void LoginDialogCl::slotgetdata()
{
    name = ui->lineEditLogin->text();
    password = ui->lineEditPassword->text();
    emit sendAc();
}
