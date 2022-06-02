#ifndef CHAININGTABLE_H
#define CHAININGTABLE_H
#include "Table.h"

template <class TYPE>
class ChainingTable :public Table<TYPE>
{
    struct Record
    {
        TYPE data_;
        std::string key_;
        Record(const std::string& key = "", const TYPE& data = TYPE{}) {
            key_ = key;
            data_ = data;
        }

    };
    DList<Record>* listOfRecords_ = nullptr;   //the table
    int capacity_ = 0;         //capacity of the hash value
    int numOfRecords_ = 0;
    std::hash<std::string> hasFunction;

public:
    ChainingTable(int capacity);
    ChainingTable(const ChainingTable& other)noexcept;
    ChainingTable(ChainingTable&& other)noexcept;
    double loadFactor() const;
    virtual bool update(const std::string& key, const TYPE& value);
    virtual bool remove(const std::string& key);
    virtual bool find(const std::string& key, TYPE& value);
    virtual const ChainingTable& operator=(const ChainingTable& other)noexcept;
    virtual const ChainingTable& operator=(ChainingTable&& other)noexcept;
    virtual ~ChainingTable();
    virtual bool isEmpty() const;
    virtual int numRecords() const;
    virtual int capacity() const;

};

template <class TYPE>
ChainingTable<TYPE>::ChainingTable(int capacity) : Table<TYPE>() {
    capacity_ = capacity;
    listOfRecords_ = new DList<Record>[capacity_];
    numOfRecords_ = 0;
}

template <class TYPE>
ChainingTable<TYPE>::ChainingTable(const ChainingTable<TYPE>& other)noexcept {
    *this = other;
}

template <class TYPE>
ChainingTable<TYPE>::ChainingTable(ChainingTable<TYPE>&& other)noexcept {
    *this = std::move(other);
}

template <class TYPE>
bool ChainingTable<TYPE>::update(const std::string& key, const TYPE& value) {
    bool rc = false;
    size_t hash = hasFunction(key) % capacity_;
    if (listOfRecords_[hash].size()) {
        for (auto it = listOfRecords_[hash].begin(); it != listOfRecords_[hash].end(); ++it) {
            if ((*it).key_ == key) {
                if ((*it).data_ != value) {
                    (*it).data_ = value;
                    return true;
                }
                else
                    return false;
            }
        }
    }
    if (!rc) {
        auto it = listOfRecords_[hash].end();
        listOfRecords_[hash].insert(it, Record(key, value));
        numOfRecords_++;
        rc = true;
    }
    return rc;
}

template <class TYPE>
bool ChainingTable<TYPE>::remove(const std::string& key) {
    bool rc = false;
    size_t hash = hasFunction(key) % capacity_;
    if (listOfRecords_[hash].size()) {
        for (auto it = listOfRecords_[hash].begin(); it != listOfRecords_[hash].end(); ++it) {
            if ((*it).key_ == key) {
                listOfRecords_[hash].erase(it);
                numOfRecords_--;
                return true;
            }
        }
    }
    return rc;
}

template <class TYPE>
bool ChainingTable<TYPE>::find(const std::string& key, TYPE& value) {
    bool rc = false;
    size_t hash = hasFunction(key) % capacity_;
    if (listOfRecords_[hash].size()) {
        for (Record r : listOfRecords_[hash]) {
            if (r.key_ == key) {
                value = r.data_;
                return true;
            }
        }
    }
    return rc;
}

template <class TYPE>
const ChainingTable<TYPE>& ChainingTable<TYPE>::operator=(const ChainingTable<TYPE>& other) noexcept {
    if (this != &other) {
        delete[] listOfRecords_;
        listOfRecords_ = nullptr;
        capacity_ = other.capacity_;
        numOfRecords_ = other.numOfRecords_;
        listOfRecords_ = new DList<Record>[capacity_];
        for (int i = 0; i < capacity_; ++i)
            listOfRecords_[i] = other.listOfRecords_[i];
    }
    return *this;
}

template <class TYPE>
const ChainingTable<TYPE>& ChainingTable<TYPE>::operator=(ChainingTable<TYPE>&& other)noexcept {
    if (this != &other) {
        delete[] listOfRecords_;
        listOfRecords_ = nullptr;
        capacity_ = other.capacity_;
        other.capacity_ = 0;
        numOfRecords_ = other.numOfRecords_;
        other.numOfRecords_ = 0;
        listOfRecords_ = std::move(other.listOfRecords_);
        other.listOfRecords_ = nullptr;
    }
    return *this;
}

template <class TYPE>
ChainingTable<TYPE>::~ChainingTable() {
    delete[] listOfRecords_;
    listOfRecords_ = nullptr;
}

template <class TYPE>
bool ChainingTable<TYPE>::isEmpty() const {
    return numRecords() == 0;
}

template <class TYPE>
int ChainingTable<TYPE>::numRecords() const {
    return numOfRecords_;
}
template <class TYPE>
int ChainingTable<TYPE>::capacity() const {
    return capacity_;
}

template <class TYPE>
double ChainingTable<TYPE>::loadFactor() const {
    return numRecords() / capacity();
}

#endif