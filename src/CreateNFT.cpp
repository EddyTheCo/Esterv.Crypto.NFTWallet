#include"CreateNFT.hpp"
#include<QDataStream>
#include<QJsonDocument>

using namespace qblocks;
NFTCreator::NFTCreator():connection_(nullptr),account_(nullptr),status_(Stte::Null),Lfunds_(0)
{
    QObject::connect(this,&NFTCreator::connectionChanged,this,[=](){

        connect(connection_,&Node_Conection::connected,this,[=](){
            connect(connection_->rest_client,&qiota::Client::last_blockid,this,&NFTCreator::createdBlock);
            auto info=connection_->rest_client->get_api_core_v2_info();
            QObject::connect(info,&Node_info::finished,this,[=]( ){
                hrp_=info->bech32Hrp;
                set_status(Stte::Ready);
                info->deleteLater();
            });
            QObject::connect(connection_->mqtt_client,&QMqttClient::stateChanged,this,[=](QMqttClient::ClientState state ){
                if(connection_->mqtt_client->state()==QMqttClient::Connected)
                {
                    auto resp=connection_->mqtt_client->
                            get_outputs_unlock_condition_address("address/"+account_->addr({0,0,0}));
                    QObject::connect(resp,&ResponseMqtt::returned,this,[=](QJsonValue data){
                        set_Lfunds(0);
                    });
                }
            });

        });

    });
}
void NFTCreator::createdBlock(qblocks::c_array bid)
{
    emit newBlock(QString(bid.toHexString()));
}
void NFTCreator::set_recaddr(QString addr_m)
{
    if(!hrp_.isNull())
    {
        const auto addr_pair=qencoding::qbech32::Iota::decode(addr_m);
        if(addr_pair.second.size()&&addr_pair.second!=recaddr_)
        {
            recaddr_=addr_pair.second;

            emit recaddrChanged();

        }

    }
}
void NFTCreator::tryToCreate(void)
{

    if(!recaddr_.isNull())
    {
        auto info=connection_->rest_client->get_api_core_v2_info();
        QObject::connect(info,&Node_info::finished,connection_->rest_client,[=]( ){

            auto addr_bundle=account_->get_addr({0,0,0});
            const auto address=addr_bundle.get_address<Address::Ed25519_typ>();
            auto varadres=recaddr_;
            auto fundAddr=std::shared_ptr<Address>(new Ed25519_Address(addr_bundle.get_hash()));

            auto buffer=QDataStream(&varadres,QIODevice::ReadOnly);
            buffer.setByteOrder(QDataStream::LittleEndian);
            auto RAddr=qblocks::Address::from_<QDataStream>(buffer);

            auto node_outputs_=new Node_outputs();
            connection_->rest_client->get_basic_outputs(node_outputs_,"address="+address);

            QObject::connect(node_outputs_,&Node_outputs::finished,this,[=]( ){

                auto addr=addr_bundle;
                c_array Inputs_Commitments;
                quint64 amount=0;
                std::vector<std::shared_ptr<qblocks::Output>> ret_outputs;
                std::vector<std::shared_ptr<qblocks::Input>> inputs;

                addr.consume_outputs(node_outputs_->outs_,0,Inputs_Commitments,amount,ret_outputs,inputs);

                auto metFea=std::shared_ptr<qblocks::Feature>(new Metadata_Feature(fl_array<quint16>(metadata_)));
                auto immetFea=std::shared_ptr<qblocks::Feature>(new Metadata_Feature(fl_array<quint16>(immetadata_)));

                auto addUnlcon=std::shared_ptr<qblocks::Unlock_Condition>(new Address_Unlock_Condition(RAddr));
                std::vector<std::shared_ptr<Feature>> features;
                if(!metadata_.isNull())features.push_back(metFea);

                std::vector<std::shared_ptr<Feature>> imfeatures;
                if(!immetadata_.isNull())imfeatures.push_back(immetFea);


                auto NFTOut= std::shared_ptr<qblocks::Output>
                        (new NFT_Output(0,{addUnlcon}, features,{},imfeatures));

                auto storage_deposit= NFTOut->min_deposit_of_output(info->vByteFactorKey,info->vByteFactorData,info->vByteCost);
                qDebug()<<"amount:"<<amount;
                qDebug()<<"storage_deposit:"<<storage_deposit;

                if(amount>=storage_deposit)
                {
                    auto BaOut=std::shared_ptr<qblocks::Output>(new Basic_Output(0,{addUnlcon},{},{}));
                    const auto min_deposit=BaOut->min_deposit_of_output(info->vByteFactorKey,info->vByteFactorData,info->vByteCost);
                    qDebug()<<"min_deposit:"<<min_deposit;
                    quint64 retamount=0;
                    if(amount>storage_deposit&&min_deposit>amount-storage_deposit)
                    {
                        quint64 baamount=0;
                        addr.consume_outputs(
                                    node_outputs_->outs_,min_deposit-(amount-storage_deposit),Inputs_Commitments,baamount,ret_outputs,inputs );
                        qDebug()<<"baamount:"<<baamount;
                        if(baamount>=min_deposit-(amount-storage_deposit))
                        {
                            BaOut->amount_=baamount+amount-storage_deposit;
                        }
                        else
                        {
                            retamount+=amount-storage_deposit;
                        }
                    }
                    else
                    {
                        BaOut->amount_=amount-storage_deposit;
                    }
                    NFTOut->amount_=storage_deposit+retamount;  //one could add a storage return unlock here. here the user will pay more than the min deposit for creating th nft.

                    std::vector<std::shared_ptr<qblocks::Output>> the_outputs_{NFTOut};
                    if(BaOut->amount_)the_outputs_.push_back(BaOut);
                    the_outputs_.insert(the_outputs_.end(), ret_outputs.begin(), ret_outputs.end());

                    auto Inputs_Commitment=c_array(QCryptographicHash::hash(Inputs_Commitments, QCryptographicHash::Blake2b_256));
                    auto essence=std::shared_ptr<qblocks::Essence>(
                                new Transaction_Essence(info->network_id_,inputs,Inputs_Commitment,the_outputs_,nullptr));
                    c_array serializedEssence;
                    serializedEssence.from_object<Essence>(*essence);
                    auto essence_hash=QCryptographicHash::hash(serializedEssence, QCryptographicHash::Blake2b_256);
                    std::vector<std::shared_ptr<qblocks::Unlock>> unlocks;
                    addr.create_unlocks<qblocks::Reference_Unlock>(essence_hash,unlocks);
                    auto trpay=std::shared_ptr<qblocks::Payload>(new Transaction_Payload(essence,{unlocks}));
                    auto block_=Block(trpay);
                    connection_->rest_client->send_block(block_);

                }
                else
                {
                    set_Lfunds(storage_deposit-amount);
                }

                info->deleteLater();
            });
        });
    }

}
QString NFTCreator::recaddr(void)const
{
    return qencoding::qbech32::Iota::encode(hrp_,recaddr_);
}
void NFTCreator::set_metadata(QString met)
{
    qDebug()<<"metadata:"<<met;
    auto var=QJsonDocument::fromJson(met.toUtf8());
    c_array vararr;
    if(!var.isNull())
    {
        vararr=var.toJson();
    }
    else
    {
        vararr=c_array(met.toUtf8());
    }
    if(vararr!=metadata_)
    {
        metadata_=vararr;
        emit metadataChanged();
    }

};
void NFTCreator::set_immetadata(QString met)
{
    qDebug()<<"immetadata:"<<met;
    auto var=QJsonDocument::fromJson(met.toUtf8());
    c_array vararr;
    if(!var.isNull())
    {
        vararr=var.toJson();
    }
    else
    {
        vararr=c_array(met.toUtf8());
    }
    if(vararr!=immetadata_)
    {
        immetadata_=vararr;
        emit immetadataChanged();
    }

};
QString NFTCreator::immetadata(void)const
{
    return QString(immetadata_);
}
QString NFTCreator::metadata(void)const
{
    return QString(metadata_);
}
