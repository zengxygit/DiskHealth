#include "AttrModel.h"
#include "DiskInfoMgr.h"

QModelIndex AttrModel::parent(const QModelIndex & index) const
{
	return QModelIndex();
}

Qt::ItemFlags AttrModel::flags(const QModelIndex & index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex AttrModel::index(int row, int column, const QModelIndex & parent) const
{
	QModelIndex ret;
	do
	{
		ret = createIndex(row, column, nullptr);
	} while (0);
	return ret;
}

int AttrModel::rowCount(const QModelIndex & parent) const
{
	return m_pMgr->m_listRightInfo.size();
}

int AttrModel::columnCount(const QModelIndex & parent) const
{
	return roleNames().size();
}

QHash<int, QByteArray> AttrModel::roleNames() const
{
	QHash<int, QByteArray> names(QAbstractItemModel::roleNames());
	names[USER_STATUS] = "status";
	names[USER_ID] = "id";
	names[USER_ATTR] = "attribute";
	names[USER_COUNT] = "current";
	names[USER_HOLD] = "threshold";
	names[USER_RAW] = "raw";
	return names;
}

bool AttrModel::hasChildren(const QModelIndex & parent) const
{
	return false;
}

QVariant AttrModel::data(const QModelIndex & index, int role) const
{
	QVariant ret = "";
	int row = index.row();

	if (row >= 0 && row < m_pMgr->m_listRightInfo.size())
	{
		ListInfo &info = m_pMgr->m_listRightInfo[row];

		switch (role)
		{
		case USER_STATUS:
			ret = info.iconId;
			break;
		case USER_ID:
			ret = QString("%1").arg(info.attrId, 2, 16, QLatin1Char('0')).toUpper();
			break;
		case USER_ATTR:
			ret = info.attrName;
			break;
		case USER_COUNT:
			ret = (unsigned int)info.curValue;
			break;
		case USER_HOLD:
			ret = info.threshold;
			break;
		case USER_RAW:
			ret = info.raw;
			break;
		}
	}

	return ret;
}
