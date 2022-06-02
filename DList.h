#ifndef DLIST_H
#define DLIST_H

template <typename T>
class DList
{
	struct Node
	{
		T data_;
		Node* next_;
		Node* prev_;
		Node(const T& data = T{}, Node* next = nullptr, Node* prev = nullptr) {
			data_ = data;
			next_ = next;
			prev_ = prev;
		}
	};
	Node* front_ = nullptr;
	Node* back_ = nullptr;

public:
	class const_iterator
	{
	protected:
		friend class DList;
		Node* curr_ = nullptr;
		const DList* myList_ = nullptr;

	public:
		const_iterator() {}

		const_iterator(Node* n, const DList* list) {
			curr_ = n;
			myList_ = list;
		}

		const_iterator& operator++() {
			if (curr_)
				curr_ = curr_->next_;
			return *this;
		}

		const_iterator operator++(int) {
			const_iterator temp = *this;
			if (curr_)
				curr_ = curr_->next_;
			return temp;
		}

		const_iterator& operator--() {
			if (curr_)
				curr_ = curr_->prev_;
			return *this;
		}

		const_iterator operator--(int) {
			const_iterator temp = *this;
			if (curr_)
				curr_ = curr_->prev_;
			return temp;
		}

		bool operator==(const_iterator rhs) {
			return this->curr_ == rhs.curr_;
		}

		bool operator!=(const_iterator rhs) {
			return this->curr_ != rhs.curr_;
		}
		const T& operator*()const {
			return this->curr_->data_;
		}
	};


	class iterator :public const_iterator
	{
		friend class DList;
	public:
		iterator(Node* n, DList* list) :const_iterator(n, list) {};

		iterator() :const_iterator() {};

		iterator& operator++() {
			this->const_iterator::operator++();
			return *this;
		}

		iterator operator++(int) {
			iterator temp = *this;
			this->const_iterator::operator++();
			return temp;
		}

		iterator& operator--() {
			this->const_iterator::operator--();
			return *this;
		}

		iterator operator--(int) {
			iterator temp = *this;
			this->const_iterator::operator--();
			return temp;
		}

		T& operator*() {
			return const_cast<T&>(this->const_iterator::operator*());
		}

		const T& operator*()const {
			const_iterator iter(front_->next_);
			return iter;
		}
	};

	DList();
	~DList();
	DList(const DList& rhs);
	DList& operator=(const DList& rhs);
	DList(DList&& rhs)noexcept;
	DList& operator=(DList&& rhs)noexcept;
	iterator insert(iterator it, const T& data);
	iterator search(const T& data);
	iterator erase(iterator it);
	void sort(iterator first, iterator last);
	bool empty() const;
	int size() const;

	iterator begin();
	iterator end();
	const_iterator cbegin() const;
	const_iterator cend() const;
};

template <typename T>
DList<T>::DList() {
	front_ = new Node();
	back_ = new Node();
	front_->next_ = back_;
	back_->prev_ = front_;
}

template <typename T>
DList<T>::~DList() {
	while (front_->next_ != back_) {
		front_->next_ = front_->next_->next_;
		delete front_->next_->prev_;
		front_->next_->prev_ = front_;
	}
	delete front_;
	delete back_;
	front_ = nullptr;
	back_ = nullptr;
}

template <typename T>
DList<T>::DList(const DList& rhs) {
	*this = rhs;
}

template <typename T>
DList<T>& DList<T>::operator=(const DList& rhs) {
	if (this != &rhs) {
		if (front_ && back_) {

			while (front_->next_ != back_) {
				front_->next_ = front_->next_->next_;
				delete front_->next_->prev_;
				front_->next_->prev_ = front_;
			}
			front_->next_ = back_;
			back_->prev_ = front_;
		}
		else {
			front_ = new Node();
			back_ = new Node();
			front_->next_ = back_;
			back_->prev_ = front_;
		}

		if (rhs.front_->next_ != rhs.back_) {
			const_iterator iter = rhs.cbegin();
			while (iter != rhs.cend()) {
				back_->prev_->next_ = new Node(*iter, back_, back_->prev_);
				back_->prev_ = back_->prev_->next_;
				++iter;
			}
		}
	}
	return *this;
}

template <typename T>
DList<T>& DList<T>::operator=(DList<T>&& rhs)noexcept {
	if (this != &rhs) {
		if (front_ && back_) {
			while (front_->next_ != back_) {
				front_->next_ = front_->next_->next_;
				delete front_->next_->prev_;
				front_->next_->prev_ = front_;
			}
			front_->next_ = back_;
			back_->prev_ = front_;
		}

		else {
			front_ = new Node();
			back_ = new Node();
			front_->next_ = back_;
			back_->prev_ = front_;
		}

		if (rhs.front_->next_ != rhs.back_) {
			front_->next_ = rhs.front_->next_;
			back_->prev_ = rhs.back_->prev_;
			front_->next_->prev_ = front_;
			back_->prev_->next_ = back_;
			rhs.front_->next_ = rhs.back_;
			rhs.back_->prev_ = rhs.front_;
		}
	}
	return *this;
}

template <typename T>
DList<T>::DList(DList<T>&& rhs) noexcept {
	*this = std::move(rhs);
}

template <typename T>
typename DList<T>::iterator DList<T>::insert(iterator it, const T& data) {
	Node* temp = front_->next_;
	iterator iter = this->begin();
	while (iter != it) {
		temp = temp->next_;
		++iter;
	}
	temp->prev_->next_ = new Node(data, temp, temp->prev_);
	temp->prev_ = temp->prev_->next_;
	return --it;
}

template <typename T>
typename DList<T>::iterator DList<T>::search(const T& data) {
	Node* temp = front_->next_;
	iterator iter = this->begin();
	while (*iter != data && iter != this->end()) {
		temp = temp->next_;
		++iter;
	}
	return iter;
}

template <typename T>
typename DList<T>::iterator DList<T>::erase(iterator it) {
	Node* temp = front_->next_;
	iterator iter = this->begin();
	while (iter != it) {
		temp = temp->next_;
		++iter;
	}
	++iter;
	temp->prev_->next_ = temp->next_;
	temp->next_->prev_ = temp->prev_;
	delete temp;
	return iter;
}

template <typename T>
void DList<T>::sort(iterator first, iterator last) {
	Node* firstNode = front_->next_;

	iterator iter = this->begin();
	unsigned int count = 0;

	while (iter != first) {
		firstNode = firstNode->next_;
		++iter;
	}

	iterator i = first;
	while (i != last) {
		++i;
		++count;
	}

	for (unsigned i = 0; i < count - 1; ++i) {
		Node* innerNode = firstNode;
		bool isFirst = true;
		iterator inner = iterator(firstNode, this);
		for (unsigned p = i; p < count - 1; ++p) {
			T currentVal = *inner;
			T nextVal = *(++inner);
			if (currentVal.count_ > nextVal.count_) {
				if (isFirst)
					firstNode = firstNode->next_;
				innerNode->prev_->next_ = innerNode->next_;
				innerNode->next_ = innerNode->prev_->next_->next_;
				innerNode->prev_->next_->next_ = innerNode;
				innerNode->prev_->next_->prev_ = innerNode->prev_;
				innerNode->next_->prev_ = innerNode;
				innerNode->prev_ = innerNode->prev_->next_;
			}
			else
				innerNode = innerNode->next_;

			inner = iterator(innerNode, this);
			isFirst = false;
		}
	}
}

template <typename T>
bool DList<T>::empty() const {
	return front_->next_ == back_;
}

template <typename T>
int DList<T>::size() const {
	int count = 0;
	for (const_iterator i = this->cbegin(); i != this->cend(); ++i)
		++count;
	return count;
}

template <typename T>
typename DList<T>::iterator DList<T>::begin() {
	iterator iter(front_->next_, this);
	return iter;
}

template <typename T>
typename DList<T>::const_iterator DList<T>::cbegin() const {
	const_iterator iter(front_->next_, this);
	return iter;
}

template <typename T>
typename DList<T>::const_iterator DList<T>::cend() const {
	const_iterator iter(back_, this);
	return iter;
}

template <typename T>
typename DList<T>::iterator DList<T>::end() {
	iterator iter(back_, this);
	return iter;
}

#endif