#pragma once

#include<QObject>
#include<QString>
#include <QtQml/qqmlregistration.h>

#include<account.hpp>
#include"nodeConnection.hpp"



class NFTCreator : public QObject
{

    Q_OBJECT


    Q_PROPERTY(Stte  status READ status WRITE set_status NOTIFY status_changed)
    Q_PROPERTY(quint64  Lfunds READ Lfunds WRITE set_Lfunds NOTIFY lfundsChanged)
    Q_PROPERTY(QString  recaddr READ recaddr WRITE set_recaddr NOTIFY recaddrChanged)
    Q_PROPERTY(QString  immetadata READ immetadata WRITE set_immetadata NOTIFY immetadataChanged)
    Q_PROPERTY(QString  metadata READ metadata WRITE set_metadata NOTIFY metadataChanged)
    Q_PROPERTY(Account*   account READ account WRITE set_account  NOTIFY accountChanged)
    Q_PROPERTY(Node_Conection*   connection READ connection WRITE set_connection  NOTIFY connectionChanged)
    QML_ELEMENT
    QML_SINGLETON

public:
    NFTCreator();
    enum Stte {
            Null,
            Ready,
        };
    Q_ENUM(Stte)
    Q_INVOKABLE void tryToCreate(void);

    Stte status(void)const{return status_;}

    QString recaddr(void)const;
    QString immetadata(void)const;
    QString metadata(void)const;
    Account* account(void)const{return account_;}
    Node_Conection* connection(void)const{return connection_;}
    quint64 Lfunds(void)const{return Lfunds_;}


    void set_status(Stte st){if(st!=status_){status_=st;emit status_changed();}}
    void set_Lfunds(quint64 lfunds_m){if(Lfunds_!=lfunds_m){Lfunds_=lfunds_m;emit lfundsChanged();}}
    void set_recaddr(QString addr_m);
    void set_immetadata(QString met);
    void set_metadata(QString met);
    void set_connection(Node_Conection* con){connection_=con;emit connectionChanged();}
    void set_account(Account* acc){account_=acc;emit accountChanged();}

signals:
    void status_changed();
    void immetadataChanged();
    void metadataChanged();
    void accountChanged();
    void connectionChanged();
    void recaddrChanged();
    void lfundsChanged();
    void created(void);
     void newBlock(QString);

private:
    void createdBlock(qblocks::c_array);
    qblocks::c_array recaddr_,immetadata_,metadata_;
    QString hrp_;
    Account* account_;
    Node_Conection* connection_;
    Stte status_;
    quint64 Lfunds_;
};



