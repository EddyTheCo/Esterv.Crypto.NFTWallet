#pragma once

#include <QAbstractListModel>
#include<QObject>
#include<QString>
#include <QtQml/qqmlregistration.h>

#include<account.hpp>
#include"nodeConnection.hpp"
#include"outmonitor.hpp"
#include<QHash>




class NftBox :public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString  issuer READ issuer CONSTANT)
    Q_PROPERTY(QString  address READ address CONSTANT)
    Q_PROPERTY(QString  data READ data WRITE setData NOTIFY dataChanged)
    QML_ELEMENT

public:
    NftBox(QObject *parent = nullptr):QObject(parent){}
    NftBox(std::shared_ptr<Output> out,QObject *parent = nullptr);


    static QString get_addr(const c_array& var);

    static bool set_addr(QString var_str,c_array& var);

    QString data()const{return QString(data_);}
    QString issuer()const{return get_addr(issuer_);}
    QString address()const{return get_addr(address_);}
    c_array addr_array()const{return address_;}
    c_array data_array()const{return data_;}

    void setData(QString data);


signals:
    void issuerChanged();
    void addressChanged();
    void dataChanged();
private:
    c_array issuer_,address_,data_;

};

class BoxModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString cissuer READ cissuer WRITE setCissuer NOTIFY cIssuerChanged)
    QML_ELEMENT

public:
    enum ModelRoles {
        issuerRole = Qt::UserRole + 1,
        dataRole,addressRole};

    int count() const;
    explicit BoxModel(QObject *parent = nullptr);
    pvector<const Output> newNFTs(Node_info*,std::shared_ptr<const Unlock_Condition> unlock);
    void clearBoxes(bool emptyones=true);
    void setCissuer(QString addr)
    {
        if(NftBox::set_addr(addr,cIssuer_))emit cIssuerChanged();
    }
    QString cissuer()const
    {
        return NftBox::get_addr(cIssuer_);
    }
    Q_INVOKABLE void newBox(void)
    {
        addBox(new NftBox(this));
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

private:
    int m_count;
    QList<NftBox*> boxes;
    c_array cIssuer_;
};


class NFTCreator : public QObject
{

    Q_OBJECT

    Q_PROPERTY(quint64  funds READ funds  NOTIFY fundsChanged)
    Q_PROPERTY(BoxModel*  model READ model  CONSTANT)
    Q_PROPERTY(Stte  state READ state  NOTIFY stateChanged)
    QML_ELEMENT
    QML_SINGLETON

public:
    NFTCreator(QObject *parent = nullptr);
    enum Stte {
            Null,
            Ready,
        };
    Q_ENUM(Stte)
    Q_INVOKABLE void mint(void);
    Q_INVOKABLE void restart(void);
    Stte state()const{return state_;}
    void setState(Stte state_m){if(state_m!=state_){state_=state_m;emit stateChanged();}};

    quint64 funds(void)const{return funds_;}
    void setFunds(quint64 funds_m){if(funds_!=funds_m){funds_=funds_m;emit fundsChanged();}}
    BoxModel* model()const{return model_;}


signals:
    void fundsChanged();
    void created(void);
    void newBlock(QString);
    void notEnought(quint64);
    void stateChanged();

private:

    void checkOutputs(std::vector<qiota::Node_output>  outs);
    void createdBlock(qblocks::c_array);
    OutMonitor* monitor;
    quint64 funds_;
    QObject* receiver;
    QHash<QString,quint64> total_funds;
    BoxModel* model_;
    Stte state_;

};



