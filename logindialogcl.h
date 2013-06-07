#ifndef LOGINDIALOGCL_H
#define LOGINDIALOGCL_H

#include "main.h"
#include "ui_logindialog.h"

class LoginDialogCl : public QDialog
{
    Q_OBJECT
private:
    Ui::LoginDialog *ui;
    QString name;
    QString password; // =) todo must do normal pass
public:
    explicit LoginDialogCl(QWidget *parent = 0);
    ~LoginDialogCl();

    QString getname() const;
    QString getpass() const;

signals:
    void sendAc();
private slots:
    void slotgetdata();
};

#endif // LOGINDIALOGCL_H
