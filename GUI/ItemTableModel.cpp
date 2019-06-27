#include "ItemTableModel.h"
#include <algorithm>

//--------------------------------------------------------------------------------------------------
// Name:   ItemTableModel(constructor)
// Action: Creates a  ItemTableModel
//--------------------------------------------------------------------------------------------------
ItemTableModel::ItemTableModel()
{
	tableSize = m_tableData.size();
}

//--------------------------------------------------------------------------------------------------
// Name  : getItemsData
// Output: isValid - return if data in the table is valid
// Return: a vector containing the table data
// Action: returns the data inside the table and whether it is valid 
//--------------------------------------------------------------------------------------------------
std::vector<Item>& ItemTableModel::getItemsData(bool& isValid)
{
	if (m_tableData.size() > 0)
		isValid = true;
	else
		isValid = false;
    
	for (Item& item : m_tableData)
	{
		if (item.dim.w == 0 || item.dim.h == 0 || item.dim.d == 0)
		{
			isValid = false;
			break;
		}
	}
 
	return m_tableData;
}

//--------------------------------------------------------------------------------------------------
// Name  : setItemsInTable
//--------------------------------------------------------------------------------------------------
void ItemTableModel::setItemsInTable(std::vector<Item>& newItems)
{
	removeRows(0, tableSize);
	int rowNum = newItems.size() - 1;
	beginInsertRows(QModelIndex(), 0, rowNum);
	m_tableData = newItems;
	tableSize = m_tableData.size();
	endInsertRows();
}

//--------------------------------------------------------------------------------------------------
// Name  : setItemsInTable
//--------------------------------------------------------------------------------------------------
void ItemTableModel::setItemsInTable(std::vector<Item>&& newItems)
{
	removeRows(0, tableSize);
	int rowNum = newItems.size() - 1;
	beginInsertRows(QModelIndex(), 0, rowNum);
	m_tableData = std::move(newItems);
	tableSize = m_tableData.size();
	endInsertRows();
}

//--------------------------------------------------------------------------------------------------
// Name  : resetTable
//--------------------------------------------------------------------------------------------------
void ItemTableModel::resetTable()
{
	removeRows(0, tableSize);
	addNewRow();
}

//--------------------------------------------------------------------------------------------------
// Name  : addNewRow
//--------------------------------------------------------------------------------------------------
void ItemTableModel::addNewRow()
{
	beginInsertRows(QModelIndex(), m_tableData.size(), m_tableData.size());
	int tableLastIndex;
	if (m_tableData.empty())
		tableLastIndex = 1;
	else
		tableLastIndex = m_tableData.back().id + 1;
	m_tableData.emplace_back(Dimensions(0,0,0), 0, tableLastIndex);
	tableSize++;
	endInsertRows();
}

//--------------------------------------------------------------------------------------------------
// Name  : rowCount
//--------------------------------------------------------------------------------------------------
int ItemTableModel::rowCount(const QModelIndex& parent/* = QModelIndex()*/) const
{
	return m_tableData.size();
}

//--------------------------------------------------------------------------------------------------
// Name  : columnCount
//--------------------------------------------------------------------------------------------------
int ItemTableModel::columnCount(const QModelIndex &parent/* = QModelIndex()*/) const
{
	return 5;
}

//--------------------------------------------------------------------------------------------------
// Name  : data
//--------------------------------------------------------------------------------------------------
QVariant ItemTableModel::data(const QModelIndex &index, int role/* = Qt::DisplayRole*/) const
{
	if (!index.isValid())
		return QVariant();
 
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;
    
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		if (index.column() < 5 && index.row() < m_tableData.size())
		{
			switch(index.column())
			{
				case 0:
					return m_tableData[index.row()].id;
                    
				case 1:
					return (int)m_tableData[index.row()].dim.w;
                                        
				case 2:
					return (int)m_tableData[index.row()].dim.h;
                    
				case 3:
					return (int)m_tableData[index.row()].dim.d;
                    
				case 4:
					return (int)m_tableData[index.row()].value;
			}
		}
	}
    
	return QVariant();
}

//--------------------------------------------------------------------------------------------------
// Name  : headerData
//--------------------------------------------------------------------------------------------------
QVariant ItemTableModel::headerData(int section, Qt::Orientation orientation, int role/* = Qt::DisplayRole*/) const
{    
	if (role != Qt::DisplayRole)
		return QVariant();
    
	if (orientation == Qt::Orientation::Horizontal)
	{
		switch(section)
		{
			case 0:
				return "Id";
                
			case 1:
				return "Width";
                
			case 2:
				return "Height";
                
			case 3:
				return "Depth";
                
			case 4:
				return "Value";
		}
	}
    
	if (orientation == Qt::Orientation::Vertical)
	{
		return QVariant(section + 1);
	}
    
	return QVariant();
}

//--------------------------------------------------------------------------------------------------
// Name  : setData
//--------------------------------------------------------------------------------------------------
bool ItemTableModel::setData(const QModelIndex &index, const QVariant &value, int role/* = Qt::EditRole*/)
{
	if (!index.isValid())
		return false;
    
	if (index.column() < 5 && index.row() < m_tableData.size())
	{
		bool canConvertToInt;
		int intValue = value.toInt(&canConvertToInt);
		if (canConvertToInt)
		{
			switch (index.column())
			{
				case 0:
					m_tableData[index.row()].id = intValue;
					break;
                
				case 1:
					m_tableData[index.row()].dim.w = intValue;
					break;
                    
				case 2:
					m_tableData[index.row()].dim.h = intValue;
					break;
                    
				case 3:
					m_tableData[index.row()].dim.d = intValue;
					break;
                    
				case 4:
					m_tableData[index.row()].value = intValue;
					break;
			}
			dataChanged(index, index);
			return true;
		}
	}
    
	return false;
}

//--------------------------------------------------------------------------------------------------
// Name  : flags
//--------------------------------------------------------------------------------------------------
Qt::ItemFlags ItemTableModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

//--------------------------------------------------------------------------------------------------
// Name  : insertRows
//--------------------------------------------------------------------------------------------------
bool ItemTableModel::insertRows(int row, int count, const QModelIndex &parent/* = QModelIndex()*/)
{
	if (row < m_tableData.size() && row > 0)
	{
		beginInsertRows(parent, row, row + count);
		m_tableData.insert(m_tableData.begin() + row, count, Item(Dimensions(0,0,0),0, 0));
		tableSize += count;
		endInsertRows();
		return true;
	}
	else
		return false;
}

//--------------------------------------------------------------------------------------------------
// Name  : removeRows
//--------------------------------------------------------------------------------------------------
bool ItemTableModel::removeRows(int row, int count, const QModelIndex &parent/* = QModelIndex()*/)
{
	if (row < tableSize && row >= 0)
	{
		beginRemoveRows(parent, row, row + count - 1);
		if (row < m_tableData.size() && row >= 0)
			m_tableData.erase(m_tableData.begin() + row, m_tableData.begin() + row + count);
		tableSize -= count;
		endRemoveRows();
		return true;
	}
	else
		return false;
}

//--------------------------------------------------------------------------------------------------
// Name  : sort
//--------------------------------------------------------------------------------------------------
void ItemTableModel::sort(int column, Qt::SortOrder order/* = Qt::AscendingOrder*/)
{
	switch (column)
	{
		case 0:
			if (order == Qt::AscendingOrder)
				std::sort(m_tableData.begin(), m_tableData.end(), [](const Item& a, const Item& b) {return a.id < b.id;});
			else
				std::sort(m_tableData.begin(), m_tableData.end(), [](const Item& a, const Item& b) {return a.id > b.id;});
			dataChanged( createIndex(0,0),  createIndex(m_tableData.size() -1, 4));
			break;
                
		case 1:
			if (order == Qt::AscendingOrder)
				std::sort(m_tableData.begin(), m_tableData.end(), [](const Item& a, const Item& b) {return a.dim.w < b.dim.w;});
			else
				std::sort(m_tableData.begin(), m_tableData.end(), [](const Item& a, const Item& b) {return a.dim.w > b.dim.w;});
			dataChanged( createIndex(0,0),  createIndex(m_tableData.size() -1, 4));
			break;
                    
		case 2:
			if (order == Qt::AscendingOrder)
				std::sort(m_tableData.begin(), m_tableData.end(), [](const Item& a, const Item& b) {return a.dim.h < b.dim.h;});
			else
				std::sort(m_tableData.begin(), m_tableData.end(), [](const Item& a, const Item& b) {return a.dim.h > b.dim.h;});
			dataChanged( createIndex(0,0),  createIndex(m_tableData.size() -1, 4));
			break;
                    
		case 3:
			if (order == Qt::AscendingOrder)
				std::sort(m_tableData.begin(), m_tableData.end(), [](const Item& a, const Item& b) {return a.dim.d < b.dim.d;});
			else
				std::sort(m_tableData.begin(), m_tableData.end(), [](const Item& a, const Item& b) {return a.dim.d > b.dim.d;});
			dataChanged( createIndex(0,0),  createIndex(m_tableData.size() -1, 4));
			break;
                    
		case 4:
			if (order == Qt::AscendingOrder)
				std::sort(m_tableData.begin(), m_tableData.end(), [](const Item& a, const Item& b) {return a.value < b.value;});
			else
				std::sort(m_tableData.begin(), m_tableData.end(), [](const Item& a, const Item& b) {return a.value > b.value;});
			dataChanged( createIndex(0,0),  createIndex(m_tableData.size() -1, 4));
			break;
	} 
}
