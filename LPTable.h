#ifndef LPTABLE_H
#define LPTABLE_H
#include "Table.h"

template <class TYPE>
class LPTable :public Table<TYPE>
{
    struct Record
    {
        TYPE data_;
        std::string key_;
        Record(const std::string& key = "", const TYPE& data = TYPE{}) {
            key_ = key;
            data_ = data;
        }
        Record& operator=(const Record& other)noexcept {
            key_ = other.key_;
            data_ = other.data_;
            return *this;
        }
    };

    Record* records_ = nullptr;   //the table
    int capacity_ = 0;      //capacity of the array
    int numOfRecords_ = 0;
    std::hash<std::string> hasFunction;
public:
    LPTable(int capacity);
    LPTable(const LPTable& other)noexcept;
    LPTable(LPTable&& other)noexcept;
    double loadFactor() const;
    virtual bool update(const std::string& key, const TYPE& value);
    virtual bool remove(const std::string& key);
    virtual bool find(const std::string& key, TYPE& value);
    virtual const LPTable& operator=(const LPTable& other)noexcept;
    virtual const LPTable& operator=(LPTable&& other)noexcept;
    virtual ~LPTable();
    virtual bool isEmpty() const;
    virtual int numRecords() const;
    virtual int capacity() const;
};

template <class TYPE>
LPTable<TYPE>::LPTable(int capacity) : Table<TYPE>() {
    capacity_ = capacity;
    numOfRecords_ = 0;
    records_ = new Record[capacity_ + 1];
}

template <class TYPE>
LPTable<TYPE>::LPTable(const LPTable<TYPE>& other) noexcept {
    *this = other;
}

template <class TYPE>
LPTable<TYPE>::LPTable(LPTable<TYPE>&& other)noexcept {
    *this = std::move(other);
}

template <class TYPE>
bool LPTable<TYPE>::update(const std::string& key, const TYPE& value) {
    bool rc = false;
    size_t hash = hasFunction(key) % capacity_;
    for (int i = hash; records_[i].key_ != ""; i = (i + 1) % capacity_) {
        if (records_[i].key_ == key)
            rc = true;
    }
    if (rc || (!rc && numOfRecords_ < capacity_ - 1)) {
        Record record = Record(key, value);
        int i;
        for (i = hash; records_[i].key_ != ""; i = (i + 1) % capacity_) {
            if (records_[i].key_ == key) {
                records_[i] = record;
                return true;
            }
            else if (hasFunction(records_[i].key_) % capacity_ != i && hasFunction(record.key_) % capacity_ == i) {
                Record temp = records_[i];
                records_[i] = record;
                record = temp;
            }
        }
        records_[i] = record;
        if (!rc)
            numOfRecords_++;
        rc = true;
    }
    return rc;
}

template <class TYPE>
bool LPTable<TYPE>::remove(const std::string& key) {
    bool rc = false;
    size_t hash = hasFunction(key) % capacity_;
    int count = 0;
    for (int i = hash; records_[i].key_ != ""; i = (i + 1) % capacity_) {
        int h = hasFunction(records_[i].key_) % capacity_;
        if (rc && h != i) {
            Record temp = records_[i];
            records_[i] = Record();
            while (records_[h].key_ != "")
                h = (h + 1) % capacity_;
            records_[h] = temp;
        }
        if (records_[i].key_ == key) {
            rc = true;
            numOfRecords_--;
            records_[i] = Record();
        }
    }
    return rc;
}

template <class TYPE>
bool LPTable<TYPE>::find(const std::string& key, TYPE& value) {
    bool rc = false;
    size_t hash = hasFunction(key) % capacity_;
    int i;
    for (i = hash; records_[i].key_ != ""; i = (i + 1) % capacity_) {
        if (records_[i].key_ == key) {
            value = records_[i].data_;
            return true;
        }
    }
    return rc;
}

template <class TYPE>
const LPTable<TYPE>& LPTable<TYPE>::operator=(const LPTable<TYPE>& other)noexcept {
    if (this != &other) {
        delete[] records_;
        capacity_ = other.capacity_;
        numOfRecords_ = other.numOfRecords_;
        records_ = new Record[capacity_ + 1];
        for (int i = 0; i < capacity_ + 1; ++i) {
            if (other.records_[i].key_ != "")
                records_[i] = Record(other.records_[i].key_, other.records_[i].data_);
        }
    }
    return *this;
}

template <class TYPE>
const LPTable<TYPE>& LPTable<TYPE>::operator=(LPTable<TYPE>&& other) noexcept {
    if (this != &other) {
        delete[] records_;
        capacity_ = other.capacity_;
        records_ = std::move(other.records_);
        numOfRecords_ = other.numOfRecords_;
        other.records_ = nullptr;
        other.capacity_ = 0;
        other.numOfRecords_ = 0;
    }
    return *this;
}

template <class TYPE>
LPTable<TYPE>::~LPTable() {
    delete[] records_;
    records_ = nullptr;
}

template <class TYPE>
bool LPTable<TYPE>::isEmpty() const {
    return numRecords() == 0;
}

template <class TYPE>
int LPTable<TYPE>::numRecords() const {
    return numOfRecords_;
}
template <class TYPE>
int LPTable<TYPE>::capacity() const {
    return capacity_;
}
template <class TYPE>
double LPTable<TYPE>::loadFactor() const {
    return numRecords() / capacity();
}

#endif