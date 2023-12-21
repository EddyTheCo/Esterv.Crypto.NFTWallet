#pragma once

#include <QAbstractListModel>
#include<QObject>
#include<QString>
#include <QtQml/qqmlregistration.h>

#include<QHash>

#include "block/qoutputs.hpp"


using namespace  qiota;
using namespace  qblocks;

class NftBox :public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString  issuer READ issuerBech32 WRITE setIssuer NOTIFY issuerChanged)
    Q_PROPERTY(QString  address READ addressBech32 NOTIFY addressChanged)
    Q_PROPERTY(QString  metdata READ metdata WRITE setMetdata NOTIFY metdataChanged)
    QML_ELEMENT

public:
    NftBox(QObject *parent = nullptr):QObject(parent){}
    NftBox(std::shared_ptr<const Output> out, QObject *parent = nullptr);

    static bool set_addr(QString var_str,c_array& var);

    QString metdata()const{return (m_data.isEmpty())?QString():QString(m_data);}
    QString issuerBech32()const{return m_issuerBech32;}
    QString addressBech32()const{return m_addressBech32;}
    c_array addrArray()const{return m_address;}
    c_array issuerArray()const{return m_issuer;}
    c_array dataArray()const{return m_data;}

    void setMetdata(QString data);
    void setIssuer(QString addr)
    {
        if(NftBox::set_addr(addr,m_issuer))emit issuerChanged();
    }

signals:
    void issuerChanged();
    void addressChanged();
    void metdataChanged();
private:
    c_array m_issuer,m_address,m_data;
    QString m_addressBech32,m_issuerBech32;

};

class BoxModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int newBoxes READ newBoxes NOTIFY newBoxesChanged)
    QML_ELEMENT
    QML_SINGLETON

public:
    enum ModelRoles {
        issuerRole = Qt::UserRole + 1,
        metdataRole,addressRole};

    int count() const;
    int newBoxes()const{return newBoxes_;}
    BoxModel(QObject *parent = nullptr);

    Q_INVOKABLE void clearBoxes(bool emptyones=true);
    Q_INVOKABLE void mint(){};

    Q_INVOKABLE void newBox(void)
    {
        addBox(new NftBox(this));
        newBoxes_++;
        emit newBoxesChanged();
    };
    void addBox(NftBox* nbox);
    Q_INVOKABLE void rmBox(int i);
    void rmBoxId(c_array addr);

    Q_INVOKABLE bool setProperty(int i, QString role, const QVariant value);


    int rowCount(const QModelIndex &p) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;
    QHash<int, QByteArray> roleNames() const;

signals:
    void countChanged(int count);
    void cIssuerChanged();
    void newBoxesChanged();

private:
    int m_count,newBoxes_;
    QList<NftBox*> boxes;
};






