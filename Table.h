#include <string>
#include <utility>
#include <functional>
#include "DList.h"

#ifndef TABLE_H
#define TABLE_H

template <class TYPE>
class Table
{
public:
    Table() {}
    virtual bool update(const std::string& key, const TYPE& value) = 0;
    virtual bool remove(const std::string& key) = 0;
    virtual bool find(const std::string& key, TYPE& value) = 0;
    virtual int numRecords() const = 0;
    virtual bool isEmpty() const = 0;
    virtual int capacity() const = 0;
    virtual ~Table() {}
};

template <class TYPE>
class SimpleTable :public Table<TYPE>
{

    struct Record
    {
        TYPE data_;
        std::string key_;
        Record(const std::string& key, const TYPE& data) {
            key_ = key;
            data_ = data;
        }

    };

    Record** records_;   //the table
    int capacity_;       //capacity of the array


public:
    SimpleTable(int capacity);
    SimpleTable(const SimpleTable& rhs);
    SimpleTable(SimpleTable&& rhs);
    virtual bool update(const std::string& key, const TYPE& value);
    virtual bool remove(const std::string& key);
    virtual bool find(const std::string& key, TYPE& value);
    virtual const SimpleTable& operator=(const SimpleTable& rhs);
    virtual const SimpleTable& operator=(SimpleTable&& rhs);
    virtual ~SimpleTable();
    virtual bool isEmpty() const {
        return numRecords() == 0;
    }
    virtual int numRecords() const;
    virtual int capacity() const {
        return capacity_;
    }

};

template <class TYPE>
int SimpleTable<TYPE>::numRecords() const {
    int rc = 0;
    for (int i = 0; records_[i] != nullptr; i++) {
        rc++;
    }
    return rc;
}

template <class TYPE>
SimpleTable<TYPE>::SimpleTable(int capacity) : Table<TYPE>() {
    records_ = new Record * [capacity + 1];
    capacity_ = capacity;
    for (int i = 0; i < capacity_ + 1; i++) {
        records_[i] = nullptr;
    }
}

template <class TYPE>
SimpleTable<TYPE>::SimpleTable(const SimpleTable<TYPE>& rhs) {
    records_ = new Record * [rhs.capacity_ + 1];
    capacity_ = rhs.capacity_;
    for (int i = 0; i < capacity_ + 1; i++) {
        records_[i] = nullptr;
    }
    for (int i = 0; i < rhs.numRecords(); i++) {
        update(rhs.records_[i]->key_, rhs.records_[i]->data_);
    }
}
template <class TYPE>
SimpleTable<TYPE>::SimpleTable(SimpleTable<TYPE>&& rhs) {
    capacity_ = rhs.capacity_;
    records_ = rhs.records_;
    rhs.records_ = nullptr;
    rhs.capacity_ = 0;
}

template <class TYPE>
bool SimpleTable<TYPE>::update(const std::string& key, const TYPE& value) {
    int idx = -1;
    int sz = numRecords();
    bool rc = true;
    for (int i = 0; i < sz; i++) {
        if (records_[i]->key_ == key) {
            idx = i;
        }
    }
    if (idx == -1) {
        if (sz < capacity_) {
            records_[numRecords()] = new Record(key, value);
            for (int i = numRecords() - 1; i > 0 && records_[i]->key_ < records_[i - 1]->key_; i--) {
                Record* tmp = records_[i];
                records_[i] = records_[i - 1];
                records_[i - 1] = tmp;
            }
        }
        else {
            rc = false;
        }
    }
    else {
        records_[idx]->data_ = value;
    }
    return rc;
}

template <class TYPE>
bool SimpleTable<TYPE>::remove(const std::string& key) {
    int idx = -1;
    for (int i = 0; records_[i] != nullptr; i++) {
        if (records_[i]->key_ == key) {
            idx = i;
        }
    }
    if (idx != -1) {
        delete records_[idx];

        for (int i = idx; i < numRecords() - 1; i++) {
            records_[i] = records_[i + 1];
        }
        records_[numRecords() - 1] = nullptr;
        return true;
    }
    else {
        return false;
    }
}

template <class TYPE>
bool SimpleTable<TYPE>::find(const std::string& key, TYPE& value) {
    int idx = -1;
    for (int i = 0; i < numRecords(); i++) {
        if (records_[i]->key_ == key) {
            idx = i;
        }
    }
    if (idx == -1)
        return false;
    else {
        value = records_[idx]->data_;
        return true;
    }
}

template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(const SimpleTable<TYPE>& rhs) {
    if (this != &rhs) {
        if (records_) {
            while (numRecords() != 0) {
                remove(records_[0]->key_);
            }
            delete[] records_;
        }
        records_ = new Record * [rhs.capacity_ + 1];
        capacity_ = rhs.capacity_;
        for (int i = 0; i < capacity_; i++) {
            records_[i] = nullptr;
        }
        for (int i = 0; i < rhs.numRecords(); i++) {
            update(rhs.records_[i]->key_, rhs.records_[i]->data_);
        }

    }
    return *this;
}

template <class TYPE>
const SimpleTable<TYPE>& SimpleTable<TYPE>::operator=(SimpleTable<TYPE>&& rhs) {
    if (records_) {
        while (numRecords() != 0) {
            remove(records_[0]->key_);
        }
        delete[] records_;
    }
    records_ = rhs.records_;
    capacity_ = rhs.capacity_;
    rhs.records_ = nullptr;
    rhs.capacity_ = 0;

    return *this;
}

template <class TYPE>
SimpleTable<TYPE>::~SimpleTable() {
    if (records_) {
        int sz = numRecords();
        for (int i = 0; i < sz; i++) {
            remove(records_[0]->key_);
        }
        delete[] records_;
    }
}

#endif