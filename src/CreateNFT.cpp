#include"CreateNFT.hpp"
#include<QDataStream>
#include<QJsonDocument>
#include<QTimer>
using namespace qblocks;

QString NftBox::get_addr(const c_array& var)
{
    return (var.isNull())?QString():qencoding::qbech32::Iota::encode
                          (Node_Conection::rest_client->info()["protocol"].toObject()["bech32Hrp"].toString(),var);
}
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
NftBox::NftBox(std::shared_ptr<Output> out, QObject *parent):NftBox(parent)
{

    const auto issuefea=out->get_immutable_feature_(qblocks::Feature::Issuer_typ);
    if(issuefea)
    {
        issuer_=std::static_pointer_cast<const Issuer_Feature>(issuefea)->issuer()->addr();
    }
    const auto immetfea=out->get_immutable_feature_(qblocks::Feature::Metadata_typ);
    if(immetfea)
    {
        data_=std::static_pointer_cast<const Metadata_Feature>(immetfea)->data();
    }
    const auto id=out->get_id();
    address_=Address::NFT(id)->addr();
}

void NftBox::setMetdata(QString data_m)
{
    auto var=QJsonDocument::fromJson(data_m.toUtf8());
    c_array vararr;
    if(!var.isNull())
    {
        vararr=var.toJson();
    }
    else
    {
        vararr=c_array(data_m.toUtf8());
    }
    if(vararr!=data_)
    {
        data_=vararr;
        emit metdataChanged();
    }

};
BoxModel::BoxModel(QObject *parent)
    : QAbstractListModel(parent),newBoxes_(0){}

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

        if(boxes[j]->addr_array().isNull()==emptyones)
        {
            rmBox(j);
        }
        else
        {
            j++;
        }
    }
}
pvector<const Output> BoxModel::newNFTs(Node_info* info, std::shared_ptr<const Unlock_Condition> unlock)
{
    pvector<const Output> var;
    for (const auto& v:boxes)
    {
        if(v->addr_array().isNull())
        {

            pvector<const Feature> features;
            if(!cIssuer_.isNull())
            {
                const auto issuer=Feature::Issuer(Address::from_array(cIssuer_));
                features.push_back(issuer);
            }
            if(!v->data_array().isNull())
            {
                const auto metadata=Feature::Metadata(v->data_array());
                features.push_back(metadata);
            }
            auto out=Output::NFT(0,{unlock},{},features);
            out->amount_=Client::get_deposit(out,info);
            var.push_back(out);
        }
    }
    return var;
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
    beginInsertRows(QModelIndex(), i, i);
    boxes.append(o);
    emit countChanged(count());
    endInsertRows();

}
void BoxModel::rmBox(int i) {
    if(boxes.at(i)->address().isNull())
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
void BoxModel::rmBoxId(c_array addr)
{
    for(auto i=0;i<boxes.size();i++)
    {
        if(boxes[i]->addr_array()==addr)
        {
            beginRemoveRows(QModelIndex(), i, i);
            boxes.remove(i);
            emit countChanged(count());
            endRemoveRows();
            return;
        }
    }
}

NFTCreator::NFTCreator(QObject *parent):QObject(parent),funds_(0),receiver(nullptr),
    monitor(new OutMonitor(this)),model_(new BoxModel(this)),state_(Null)
{

}
void NFTCreator::restart (void)
{
    if(receiver)receiver->deleteLater();
    receiver=new QObject(this);
    monitor->restart();
    model_->clearBoxes(false);
    setFunds(0);
    if(Node_Conection::state()==Node_Conection::Connected)
    {
        auto info=Node_Conection::rest_client->get_api_core_v2_info();
        connect(info,&Node_info::finished,this,[=]( ){
            setState(Ready);
            connect(monitor,&OutMonitor::finished,receiver,[=](auto  outs,auto jsonOuts)
            {
                checkOutputs(outs);
            });
            connect(monitor,&OutMonitor::gotNewOutsMqtt,receiver,[=](auto  outs,auto jsonOuts)
            {
                checkOutputs(outs);
            });
            const auto address=Account::addr_bech32({0,0,0},info->bech32Hrp);
            monitor->setRestCalls(4);
            monitor->getRestBasicOuts("address="+address);
            monitor->getRestNftOuts("address="+address);
            monitor->getRestBasicOuts("expirationReturnAddress="+address);
            monitor->getRestNftOuts("expirationReturnAddress="+address);

            monitor->subscribe("outputs/unlock/address/"+address);

            info->deleteLater();
        });

    }
}
void NFTCreator::checkOutputs(std::vector<qiota::Node_output>  outs)
{

    quint64 total=0;
    if(outs.size()&&(outs.front().output()->type()==Output::Basic_typ||outs.front().output()->type()==Output::NFT_typ))
    {

        for(const auto& v:outs)
        {
            std::vector<Node_output> var{v};
            auto bundle= Account::get_addr({0,0,0});
            bundle.consume_outputs(var);

            if(bundle.amount)
            {
                total+=bundle.amount;
                total_funds.insert(v.metadata().outputid_.toHexString(),bundle.amount);
                auto resp=Node_Conection::mqtt_client->get_outputs_outputId(v.metadata().outputid_.toHexString());
                connect(resp,&ResponseMqtt::returned,receiver,[=](QJsonValue data){
                    const auto node_output=Node_output(data);


                    if(node_output.metadata().is_spent_)
                    {
                        auto it=total_funds.constFind(node_output.metadata().outputid_.toHexString());
                        if(it!=total_funds.cend())
                        {
                            setFunds(funds_-it.value());
                            total_funds.erase(it);
                            if(node_output.output()->type()==Output::NFT_typ)
                            {

                                node_output.output()->set_id(v.metadata().outputid_);
                                const auto id=node_output.output()->get_id();
                                model_->rmBoxId(Address::NFT(id)->addr());
                            }
                        }

                        resp->deleteLater();
                    }
                });
                if(bundle.nft_outputs.size())
                {
                    auto nbox=new NftBox(bundle.nft_outputs.front(),model_);
                    model_->addBox(nbox);
                }

            }
            if(bundle.to_unlock.size())
            {
                const auto unixtime=bundle.to_unlock.front();
                const auto triger=(unixtime-QDateTime::currentDateTime().toSecsSinceEpoch())*1000;
                QTimer::singleShot(triger+5000,receiver,[=](){
                    auto resp=Node_Conection::mqtt_client->get_outputs_outputId(v.metadata().outputid_.toHexString());
                    connect(resp,&ResponseMqtt::returned,receiver,[=](QJsonValue data){
                        const auto node_output=Node_output(data);
                        checkOutputs({node_output});
                        resp->deleteLater();
                    });
                });
            }
            if(bundle.to_expire.size())
            {
                const auto unixtime=bundle.to_expire.front();
                const auto triger=(unixtime-QDateTime::currentDateTime().toSecsSinceEpoch())*1000;
                QTimer::singleShot(triger,receiver,[=](){
                    auto it=total_funds.find(v.metadata().outputid_.toHexString());
                    if(it!=total_funds.end())
                    {
                        setFunds(funds_-it.value());
                        total_funds.erase(it);
                        if(v.output()->type()==Output::NFT_typ)
                        {
                            v.output()->set_id(v.metadata().outputid_);
                            const auto id=v.output()->get_id();
                            model_->rmBoxId(Address::NFT(id)->addr());
                        }
                    }
                });
            }


        }
        setFunds(funds_+total);
    }

}

void NFTCreator::mint(bool issend)
{
    setState(Null);
    QTimer::singleShot(15000,this,[=](){setState(Ready);});
    auto info=Node_Conection::rest_client->get_api_core_v2_info();
    QObject::connect(info,&Node_info::finished,this,[=]( ){

        auto mintmonitor=new OutMonitor(receiver);
        connect(mintmonitor,&OutMonitor::finished,receiver,[=](auto  outs,auto jsonOuts)
        {
            auto bundle=Account::get_addr({0,0,0});


            bundle.consume_outputs(outs);

            const auto eddAddr=bundle.get_address();

            auto addUnlcon=Unlock_Condition::Address(eddAddr);

            if(issend&&!recAddress_.isNull())
            {
                addUnlcon=Unlock_Condition::Address(Address::from_array(recAddress_));
            }

            pvector<const Output> nouts;

            if(!issend)nouts=model_->newNFTs(info,addUnlcon);

            if(nouts.size()||issend)
            {
                auto outAmount=0;
                for(const auto &v:nouts)outAmount+=v->amount_;
                for(auto & v: bundle.nft_outputs)
                {
                    v->unlock_conditions_={addUnlcon};
                    v->amount_=Client::get_deposit(v,info);
                    outAmount+=v->amount_;
                    nouts.push_back(v);
                }
                auto BaOut=Output::Basic(0,{addUnlcon},bundle.get_tokens());
                if(bundle.amount>=outAmount+Client::get_deposit(BaOut,info))
                {
                    model_->clearBoxes();
                    BaOut->amount_=bundle.amount-outAmount;
                    nouts.push_back(BaOut);
                    nouts.insert(nouts.end(), bundle.ret_outputs.begin(), bundle.ret_outputs.end());

                    auto Inputs_Commitment=Block::get_inputs_Commitment(bundle.Inputs_hash);

                    auto essence=Essence::Transaction(info->network_id_,bundle.inputs,Inputs_Commitment,nouts);

                    bundle.create_unlocks(essence->get_hash());

                    auto trpay=Payload::Transaction(essence,bundle.unlocks);

                    auto resp=Node_Conection::mqtt_client->get_subscription("transactions/"+trpay->get_id().toHexString() +"/included-block");
                    connect(resp,&ResponseMqtt::returned,this,[=](auto var){
                        setState(Ready);
                        resp->deleteLater();
                    });

                    auto block_=Block(trpay);
                    Node_Conection::rest_client->send_block(block_);
                }
                else
                {
                    setState(Ready);
                    emit notEnought(info->amount_json(outAmount+Client::get_deposit(BaOut,info)-bundle.amount));
                }

            }
            info->deleteLater();
            mintmonitor->deleteLater();

        });
        const auto address=Account::addr_bech32({0,0,0},info->bech32Hrp);
        mintmonitor->setRestCalls(4);
        mintmonitor->getRestBasicOuts("address="+address);
        mintmonitor->getRestNftOuts("address="+address);
        mintmonitor->getRestBasicOuts("expirationReturnAddress="+address);
        mintmonitor->getRestNftOuts("expirationReturnAddress="+address);


    });


}
void NFTCreator::setFunds(quint64 funds_m){

    if(funds_!=funds_m||funds_m==0)
    {
        funds_=funds_m;
        auto info=Node_Conection::rest_client->get_api_core_v2_info();
        QObject::connect(info,&Node_info::finished,receiver,[=]( ){
            funds_json=info->amount_json(funds_);
            emit fundsChanged();
            info->deleteLater();
        });
    }
}
