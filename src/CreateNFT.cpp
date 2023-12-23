#include"CreateNFT.hpp"
#include<QDataStream>
#include<QJsonDocument>
#include<QTimer>

#include "encoding/qbech32.hpp"
#include"nodeConnection.hpp"
#include "qaddr_bundle.hpp"
#include "qwallet.hpp"

bool NftBox::set_addr(QString var_str,c_array& var)
{
    const auto addr_pair=qencoding::qbech32::Iota::decode(var_str);
    if(addr_pair.second.size()&&addr_pair.second!=var)
    {
        var=addr_pair.second;
        return true;
    }
    return false;

}
NftBox::NftBox(std::shared_ptr<const Output> out, QObject *parent, c_array outId):NftBox(parent)
{
    m_outId=outId;
    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    connect(info,&Node_info::finished,this,[=]( ){
        const auto issuefea=out->get_immutable_feature_(qblocks::Feature::Issuer_typ);
        if(issuefea)
        {
            m_issuer=std::static_pointer_cast<const Issuer_Feature>(issuefea)->issuer()->addr();
            m_issuerBech32=qencoding::qbech32::Iota::encode(info->bech32Hrp,m_issuer);
            emit issuerChanged();
        }
        const auto immetfea=out->get_immutable_feature_(qblocks::Feature::Metadata_typ);
        if(immetfea)
        {
            m_data=std::static_pointer_cast<const Metadata_Feature>(immetfea)->data();
            auto var=QJsonDocument::fromJson(m_data);
            if(!var.isNull())
            {
                fillIRC27(var.object());
            }
            emit metdataChanged();
        }
        const auto id=out->get_id();
        m_address=Address::NFT(id)->addr();
        m_addressBech32=qencoding::qbech32::Iota::encode(info->bech32Hrp,m_address);
        emit addressChanged();
        info->deleteLater();
    });

}

void NftBox::mint()
{
    qDebug()<<"mint";
    setState(Minting);
    QTimer::singleShot(20000,this,[=](){setState(Ready);});
    auto info=NodeConnection::instance()->rest()->get_api_core_v2_info();
    connect(info,&Node_info::finished,this,[=]( ){
        qDebug()<<"Node_info::finished";
        qDebug()<<"addresses().size():"<<Wallet::instance()->addresses().size();
        if(Wallet::instance()->addresses().size())
        {

            //qDebug()<<"addr:"<<Wallet::instance()->addresses().begin()->second->getAddressBech32();
            const auto unlock=
                Unlock_Condition::Address(Wallet::instance()->addresses().begin()->second->getAddress());
            qDebug()<<"unlock:"<<unlock->get_Json();

            pset<const Feature> features;
            if(!m_issuer.isNull())
            {
                const auto issuer=Feature::Issuer(Address::from_array(m_issuer));
                features.insert(issuer);
                qDebug()<<"issuer:"<<issuer->get_Json();
            }
            if(!m_data.isNull())
            {
                const auto metadata=Feature::Metadata(m_data);
                features.insert(metadata);
                qDebug()<<"metadata:"<<metadata->get_Json();
            }
            auto NFTout=Output::NFT(0,{unlock},{},features);
            const auto deposit=NodeConnection::instance()->rest()->get_deposit(NFTout,info);
            NFTout->amount_=deposit;
            qDebug()<<"NFTout:"<<NFTout->get_Json();

            if(m_issuer.isNull()||Wallet::instance()->addresses().find(m_issuer)!=Wallet::instance()->addresses().cend())
            {
                InputSet inputSet;
                StateOutputs stateOutputs;
                quint64 consumedAmount=0;
                if(!m_issuer.isNull())
                {
                    const auto addB=Wallet::instance()->addresses().at(m_issuer);
                    const auto issuerAddr=addB->getAddress();
                    //qDebug()<<"addressunlock:"<<addB->getAddressBech32();
                    if(issuerAddr->type()==Address::NFT_typ||issuerAddr->type()==Address::Alias_typ)
                    {
                        consumedAmount+=Wallet::instance()->
                                          consume(inputSet,stateOutputs,deposit,{Output::All_typ},{addB->outId()});
                    }
                }

                quint64 stateAmount=0;
                for(const auto &v:std::as_const(stateOutputs))
                {
                    stateAmount+=v.amount;
                }
                quint64 requiredAmount=deposit+stateAmount;
                qDebug()<<"deposit:"<<deposit;
                qDebug()<<"stateAmount:"<<stateAmount;
                qDebug()<<"consumedAmount:"<<consumedAmount;
                if(consumedAmount<requiredAmount)
                {
                    consumedAmount+=Wallet::instance()->
                                      consume(inputSet,stateOutputs,requiredAmount-consumedAmount,{Output::Basic_typ},{});
                }
                pvector<const Output> theOutputs{NFTout};
                stateAmount=0;
                for(const auto &v:std::as_const(stateOutputs))
                {
                    stateAmount+=v.amount;
                    theOutputs.push_back(v.output);
                }
                requiredAmount=deposit+stateAmount;
                qDebug()<<"deposit:"<<deposit;
                qDebug()<<"stateAmount:"<<stateAmount;
                qDebug()<<"consumedAmount:"<<consumedAmount;
                if(consumedAmount>=requiredAmount)
                {
                    auto BaOut=Output::Basic(0,{unlock});
                    const auto minDeposit=Client::get_deposit(BaOut,info);
                    if(consumedAmount-requiredAmount>minDeposit)
                    {
                        BaOut->amount_=consumedAmount-requiredAmount;
                        theOutputs.push_back(BaOut);
                    }
                    else
                    {
                        NFTout->amount_+=consumedAmount-requiredAmount;
                    }
                    auto payloadusedids=Wallet::instance()->createTransaction(inputSet,info,theOutputs);
                    auto block=Block(payloadusedids.first);
                    const auto transactionid=payloadusedids.first->get_id().toHexString();
                    auto res=NodeConnection::instance()->mqtt()->get_subscription("transactions/"+transactionid +"/included-block");
                    QObject::connect(res,&ResponseMqtt::returned,this,[=](auto var){
                        setState(Ready);
                        res->deleteLater();
                    });

                    // Send the block to the node
                    qDebug().noquote()<<"block:\n"<<QString(QJsonDocument(block.get_Json()).toJson(QJsonDocument::Indented));
                    NodeConnection::instance()->rest()->send_block(block);

                }
                else
                {
                    qDebug()<<"notEnought";
                    this->setState(Ready);
                    //emit notEnought(qiota::Qml64(requiredAmount-consumedAmount).json());
                }
            }
            else
            {
                qDebug()<<"wrongissuer";
                setState(Ready);
                emit wrongIssuer(qencoding::qbech32::Iota::encode(info->bech32Hrp,m_issuer));
            }

        }

        info->deleteLater();
    });
}

void NftBox::fillIRC27(QJsonObject data)
{

    const auto name=data["name"].toString();
    if(!name.isNull()&&name!=m_name)
    {
        m_name=name;
        emit nameChanged();
    }
    const auto uri=QUrl(data["uri"].toString());
    if(uri.isValid()&&uri!=m_uri)
    {
        m_uri=uri;
        emit uriChanged();
    }

}
void NftBox::setMetdata(QString data)
{
    auto var=QJsonDocument::fromJson(data.toUtf8());
    c_array vararr;
    if(!var.isNull())
    {
        vararr=var.toJson();
    }
    else
    {
        vararr=c_array(data.toUtf8());
    }
    if(vararr!=m_data)
    {
        m_data=vararr;
        if(!var.isNull())
        {

            fillIRC27(var.object());
        }
        emit metdataChanged();
    }

};
BoxModel::BoxModel(QObject *parent)
    : QAbstractListModel(parent),newBoxes_(0){

    connect(Wallet::instance(),&Wallet::inputAdded,this,&BoxModel::gotInput);
    connect(Wallet::instance(),&Wallet::inputRemoved,this,&BoxModel::rmBoxId);
}
void BoxModel::gotInput(c_array id)
{
    const auto inBox=Wallet::instance()->getInput(id);
    const auto output=inBox.output;

    if(output->type()==qblocks::Output::NFT_typ)
    {
        auto nbox=new NftBox(output,this,id);
        addBox(nbox);
    }

}

int BoxModel::count() const
{
    return boxes.size();
}
void BoxModel::clearBoxes(bool emptyones)
{
    size_t j=0;
    const auto var=boxes.size();
    for (auto i=0;i<var;i++)
    {

        if(boxes[j]->addrArray().isNull()==emptyones)
        {
            rmBox(j);
        }
        else
        {
            j++;
        }
    }
}

int BoxModel::rowCount(const QModelIndex &p) const
{
    Q_UNUSED(p)
    return boxes.size();
}
QHash<int, QByteArray> BoxModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[issuerRole] = "issuer";
    roles[metdataRole] = "metdata";
    roles[addressRole] = "address";
    roles[uriRole] = "uri";
    roles[nameRole] = "name";
    roles[stateRole] = "state";
    return roles;
}
QVariant BoxModel::data(const QModelIndex &index, int role) const
{
    return boxes[index.row()]->property(roleNames().value(role));
}
bool BoxModel::setData(const QModelIndex &index, const QVariant &value, int role )
{

    const auto re=boxes[index.row()]->setProperty(roleNames().value(role),value);

    if(re)
    {
        emit dataChanged(index,index,QList<int>{role});
        return true;
    }
    return false;
}
bool BoxModel::setProperty(int i,QString role,const QVariant value)
{
    const auto ind=index(i);
    const auto rol=roleNames().keys(role.toUtf8());
    return setData(ind,value,rol.front());
}
QModelIndex BoxModel::index(int row, int column , const QModelIndex &parent ) const
{
    return createIndex(row,column);
}
void BoxModel::addBox(NftBox* o)
{

    int i = boxes.size();
    connect(o,&NftBox::nameChanged,this,[=]{
        emit dataChanged(index(i),index(i),QList<int>{ModelRoles::nameRole});
    });
    connect(o,&NftBox::stateChanged,this,[=]{
        emit dataChanged(index(i),index(i),QList<int>{ModelRoles::stateRole});
    });
    connect(o,&NftBox::uriChanged,this,[=]{
        emit dataChanged(index(i),index(i),QList<int>{ModelRoles::uriRole});
    });
    beginInsertRows(QModelIndex(), i, i);
    boxes.append(o);
    emit countChanged(count());
    endInsertRows();

}

void BoxModel::mint(int i)
{
    if(boxes.at(i)->addressBech32().isNull())
    {
        boxes.at(i)->mint();
    }
}
void BoxModel::rmBox(int i) {
    if(boxes.at(i)->addressBech32().isNull())
    {
        newBoxes_--;
        emit newBoxesChanged();
    }

    beginRemoveRows(QModelIndex(),i,i);
    boxes[i]->deleteLater();
    boxes.remove(i);
    emit countChanged(count());
    endRemoveRows();
}
void BoxModel::rmBoxId(c_array outId)
{
    for(auto i=0;i<boxes.size();i++)
    {
        if(boxes[i]->outId()==outId)
        {
            beginRemoveRows(QModelIndex(), i, i);
            boxes.remove(i);
            emit countChanged(count());
            endRemoveRows();
            return;
        }
    }
}


