#pragma once

#include <QAbstractItemModel>

class DiskInfoMgr;

class AttrModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	enum ItemRoles
	{
		USER_STATUS = Qt::UserRole + 1,
		USER_ID,
		USER_ATTR,
		USER_COUNT,
		USER_HOLD,
		USER_RAW,
	};

	AttrModel(DiskInfoMgr *parent) { m_pMgr = parent; }

	void reset()
	{
		beginResetModel();
		endResetModel();
	}

protected:
	QModelIndex parent(const QModelIndex &index) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QHash<int, QByteArray> roleNames() const;
	virtual bool hasChildren(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
protected:
	DiskInfoMgr *m_pMgr = nullptr;
};



