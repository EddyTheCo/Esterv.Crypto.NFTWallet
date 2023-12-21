#include"CreateNFT.hpp"
#include<QDataStream>
#include<QJsonDocument>
#include<QTimer>

#include "encoding/qbech32.hpp"
#include"nodeConnection.hpp"

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
NftBox::NftBox(std::shared_ptr<const Output> out, QObject *parent):NftBox(parent)
{
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
            emit metdataChanged();
        }
        const auto id=out->get_id();
        m_address=Address::NFT(id)->addr();
        m_addressBech32=qencoding::qbech32::Iota::encode(info->bech32Hrp,m_address);
        emit addressChanged();
        info->deleteLater();
    });

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
void BoxModel::rmBoxId(c_array addr)
{
    for(auto i=0;i<boxes.size();i++)
    {
        if(boxes[i]->addrArray()==addr)
        {
            beginRemoveRows(QModelIndex(), i, i);
            boxes.remove(i);
            emit countChanged(count());
            endRemoveRows();
            return;
        }
    }
}


