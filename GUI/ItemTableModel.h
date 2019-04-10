#ifndef DATATABLE_H
#define DATATABLE_H

#include <vector>
#include <QAbstractTableModel>
#include "../GA/Item.h"

class ItemTableModel : public QAbstractTableModel
{
public:
    ItemTableModel();
    
    std::vector<Item>& getItemsData(bool& isValid);
    void setItemsInTable(std::vector<Item>& newItems);
    void setItemsInTable(std::vector<Item>&& newItems);
    void resetTable();
    
    void addNewRow();
    // read only model actions
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    // write model actions 
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    
private:
    std::vector<Item> m_tableData;
    int tableSize;
};

#endif // DATATABLE_H
