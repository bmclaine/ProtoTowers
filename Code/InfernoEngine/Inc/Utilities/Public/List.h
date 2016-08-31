#pragma once

template <typename T>
class List{

private:
	unsigned int m_size;
	unsigned int m_capacity;
	T* m_item;

public:

	//Constructor
	List();

	//Destructor
	~List();

	//Copy Constructor
	List(const List& _rhs);

	List<T>& operator=(const List& _rhs);

	T& operator[](const unsigned int _index);

	const T& operator[](const unsigned int _index) const;

	//returns a pointer to the array
	const T* data() const;

	//returns the amount of items in the array
	unsigned int size() const;

	//returns the amount of items the array can hold before needing to be resized
	unsigned int capacity() const;

	//adds an item to the array
	void add(const T& item);

	//removes an item from the array 
	bool remove(const T& item);

	//removes an item from the array at a certain index
	bool removeAt(const unsigned int _index);

	//increases the capacity of the array
	void reserve(const unsigned int& _newCap = 0);

	//checks if the list contains a item
	bool contains(const T& item);

	//resets the array, size , and capacity
	void clear();
};

template <typename T>
List<T>::List(){
	m_size = m_capacity = 0;
	m_item = nullptr;
}

template <typename T>
List<T>::~List(){
	delete[] m_item;
	m_size = m_capacity = 0;
}

template <typename T>
List<T>::List(const List& _rhs){
	m_size = _rhs.m_size;
	m_capacity = _rhs.m_capacity;

	T* arr = new T[m_capacity];
	m_item = arr;

	for (unsigned int i = 0; i < m_size; ++i){
		m_item[i] = _rhs.m_item[i];
	}
}

template <typename T>
List<T>& List<T>::operator=(const List& _rhs){
	if (this != &_rhs){
		clear();

		m_size = _rhs.m_size;
		m_capacity = _rhs.m_capacity;

		T* arr = new T[m_capacity];
		m_item = arr;

		for (unsigned int i = 0; i < m_size; ++i){
			m_item[i] = _rhs.m_item[i];
		}
	}

	return *this;
}

template <typename T>
T& List<T>::operator[](const unsigned int _index){
	return *(m_item + _index);
}

template <typename T>
const T& List<T>::operator[](const unsigned int _index) const{
	return *(m_item + _index);
}

template <typename T>
unsigned int List<T>::size() const{
	return m_size;
}

template <typename T>
unsigned int List<T>::capacity() const{
	return m_capacity;
}

template <typename T>
void List<T>::add(const T& _item){
	if (m_capacity == 0){
		reserve();
		m_item[m_size] = _item;
		++m_size;
		return;
	}

	if (m_size == m_capacity)
		reserve();

	m_item[m_size] = _item;
	++m_size;
}

template <typename T>
bool List<T>::remove(const T& _item){
	if (m_size == 0) return false;

	bool found = false;
	unsigned int i = 0;
	for (; i < m_size; ++i){
		if (m_item[i] == _item){
			found = true;
			break;
		}
	}

	if (found == false) return false;

	for (; i < m_size - 1; ++i){
		m_item[i] = m_item[(i+1)];
	}

	--m_size;

	return true;
}

template <typename T>
bool List<T>::removeAt(const unsigned int _index){
	if (_index >= m_size) return false;

	for (unsigned int i = _index; i < m_size - 1; ++i){
		m_item[i] = m_item[(i + 1)];
	}

	--m_size;

	return true;
}

template <typename T>
void List<T>::reserve(const unsigned int& _newCap = 0){
	if (_newCap == 0){
		if (m_capacity == 0)
			m_capacity = 1;
		else
			m_capacity *= 2;
	}
	else if (_newCap < m_size)
		return;
	else
		m_capacity = _newCap;

	T* arr = new T[m_capacity];

	for (unsigned int i = 0; i < m_size; ++i){
		arr[i] = m_item[i];
	}

	delete[] m_item;

	m_item = arr;

}

template <typename T>
bool List<T>::contains(const T& item){
	for (unsigned int i = 0; i < m_size; ++i){
		if (m_item[i] == item)
			return true;
	}

	return false;
}

template <typename T>
void List<T>::clear(){
	m_size = m_capacity = 0;
	delete[] m_item;
	m_item = nullptr;
}

template <typename T>
const T* List<T>::data() const{
	const T* _data = m_Item;
	return _data;
}
