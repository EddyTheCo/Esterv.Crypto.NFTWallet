#include"CreateNFT.hpp"
#include<QDataStream>
#include<QJsonDocument>
#include<QTimer>

#include "encoding/qbech32.hpp"
#include"nodeConnection.hpp"
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
    const auto addrInputPair=Wallet::instance()->inputs().at(id).back();
    const auto inBox=addrInputPair.first->inputs().value(addrInputPair.second);
    const auto output=inBox.output;
    if(output->type()==qblocks::Output::NFT_typ)
    {
        auto nbox=new NftBox(output,this);
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
    connect(o,&NftBox::uriChanged,this,[=]{
        emit dataChanged(index(i),index(i),QList<int>{ModelRoles::uriRole});
    });
    beginInsertRows(QModelIndex(), i, i);
    boxes.append(o);
    emit countChanged(count());
    endInsertRows();

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


