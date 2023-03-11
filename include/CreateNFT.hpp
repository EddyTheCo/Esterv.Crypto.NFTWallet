#pragma once

#include<QObject>
#include<QString>
#include <QtQml/qqmlregistration.h>

#include<account.hpp>
#include"nodeConnection.hpp"



class NFTCreator : public QObject
{

    Q_OBJECT

    Q_PROPERTY(quint64  Lfunds READ Lfunds WRITE set_Lfunds NOTIFY lfundsChanged)
    Q_PROPERTY(QString  recaddr READ recaddr WRITE set_recaddr NOTIFY recaddrChanged)
    Q_PROPERTY(QString  immetadata READ immetadata WRITE set_immetadata NOTIFY immetadataChanged)
    Q_PROPERTY(QString  metadata READ metadata WRITE set_metadata NOTIFY metadataChanged)
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
    Q_INVOKABLE void restart(void);



    QString recaddr(void)const;
    QString immetadata(void)const;
    QString metadata(void)const;
    quint64 Lfunds(void)const{return Lfunds_;}



    void set_Lfunds(quint64 lfunds_m){if(Lfunds_!=lfunds_m){Lfunds_=lfunds_m;emit lfundsChanged();}}

    void set_recaddr(QString addr_m);
    void set_immetadata(QString met);
    void set_metadata(QString met);

signals:

    void immetadataChanged();
    void metadataChanged();
    void recaddrChanged();
    void lfundsChanged();
    void created(void);
     void newBlock(QString);

private:
    void createdBlock(qblocks::c_array);
    qblocks::c_array recaddr_,immetadata_,metadata_;
    quint64 Lfunds_;
    QObject* receiver;
};



