#include "QTransfersModel.h"
#include "MegaApplication.h"

using namespace mega;

QTransfersModel::QTransfersModel(MegaTransferList *transfers, int type, QObject *parent) :
    QAbstractItemModel(parent)
{
    MegaApi *api =  ((MegaApplication *)qApp)->getMegaApi();
    this->type = type;
    delegateListener = new QTMegaTransferListener(api, this);
    api->addTransferListener(delegateListener);

    setupModelTransfers(transfers);
}

TransferItem *QTransfersModel::transferFromIndex(const QModelIndex &index) const
{
    if(index.isValid())
    {
        return static_cast<TransferItem *>(index.internalPointer());
    }else
    {
        return NULL;
    }
}

int QTransfersModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant QTransfersModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (index.row() < 0 || transfers.count() <= index.row()))
    {
        return QVariant();
    }

    if (role == Qt::DisplayRole)
    {
        return QVariant::fromValue(static_cast<TransferItem *>(index.internalPointer()));
    }

    return QVariant();
}

QModelIndex QTransfersModel::parent(const QModelIndex &index) const
{
    return QModelIndex();

}

QModelIndex QTransfersModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    return createIndex(row, column, transfers.value(transfersOrder.at(row)));
}

void QTransfersModel::insertTransfer(MegaTransfer *transfer, const QModelIndex &parent)
{
    beginInsertRows(QModelIndex(), transfers.size(), transfers.size());
    transfers.insert(transfer->getTag(), new TransferItem());
    transfersOrder.append(transfer->getTag());
    endInsertRows();
}

void QTransfersModel::removeTransfer(MegaTransfer *transfer, const QModelIndex &parent)
{
    int row = transfersOrder.indexOf(transfer->getTag());
    beginRemoveRows(QModelIndex(), row, row);
    transfers.remove(transfer->getTag());
    transfersOrder.removeAt(row);
    endRemoveRows();
}

int QTransfersModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return transfersOrder.size();
}

QTransfersModel::~QTransfersModel()
{
    qDeleteAll(transfers);
    delete delegateListener;
}

void QTransfersModel::onTransferStart(MegaApi *api, MegaTransfer *transfer)
{
    if (type == TYPE_ALL || transfer->getType() == type)
    {
        insertTransfer(transfer, QModelIndex());
        updateTransferInfo(transfer);
    }

}

void QTransfersModel::onTransferFinish(MegaApi *api, MegaTransfer *transfer, MegaError *e)
{
    if (type == TYPE_ALL || transfer->getType() == type)
    {
        if (transfers.contains(transfer->getTag()))
        {
            TransferItem *item = transfers.value(transfer->getTag());
            item->finishTransfer();
            removeTransfer(transfer, QModelIndex());

            //Update the view
            emit dataChanged(QModelIndex(), QModelIndex());

        }
    }
}

void QTransfersModel::onTransferUpdate(MegaApi *api, MegaTransfer *transfer)
{
    if (type == TYPE_ALL || transfer->getType() == type)
    {
        if (transfers.contains(transfer->getTag()))
        {
            updateTransferInfo(transfer);
        }
    }

}

void QTransfersModel::onTransferTemporaryError(MegaApi *api, MegaTransfer *transfer, MegaError *e)
{

}

void QTransfersModel::setupModelTransfers(MegaTransferList *transfers)
{
    for (int i = 0; i < transfers->size(); i++)
    {
        MegaTransfer *t = transfers->get(i);
        insertTransfer(t, QModelIndex());
    }   
}

void QTransfersModel::updateTransferInfo(MegaTransfer *transfer)
{
    TransferItem *item = transfers.value(transfer->getTag());
    item->setFileName(QString::fromUtf8(transfer->getFileName()));
    item->setType(transfer->getType());
    item->setSpeed(transfer->getSpeed());
    item->setTotalSize(transfer->getTotalBytes());
    item->setTransferredBytes(transfer->getTransferredBytes());
    item->updateTransfer();

    //Update the view
    emit dataChanged(QModelIndex(), QModelIndex());
}