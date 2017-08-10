// MyMap.h

// Skeleton for the MyMap class template.  You must implement the first six
// member functions.

#ifndef MYMAP_H
#define MYMAP_H

#include "support.h"

template<typename KeyType, typename ValueType>
class MyMap
{
public:
	MyMap();
	~MyMap();
	void clear();
	int size() const;
	void associate(const KeyType& key, const ValueType& value);

	  // for a map that can't be modified, return a pointer to const ValueType
	const ValueType* find(const KeyType& key) const;

	  // for a modifiable map, return a pointer to modifiable ValueType
	ValueType* find(const KeyType& key)
	{
		return const_cast<ValueType*>(const_cast<const MyMap*>(this)->find(key));
	}

	  // C++11 syntax for preventing copying and assignment
	MyMap(const MyMap&) = delete;
	MyMap& operator=(const MyMap&) = delete;

private:
	struct Node {
		Node(KeyType key, ValueType value) {
			m_key = key;
			m_value = value;
			m_left = nullptr;
			m_right = nullptr;
		}
		KeyType m_key;
		ValueType m_value;

		Node* m_left;
		Node* m_right;
	};

	Node* m_root;
	int m_size;

	void deleteTree(Node*& current);
};

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::MyMap() {
	m_root = nullptr;
	m_size = 0;
}

template<typename KeyType, typename ValueType>
MyMap<KeyType, ValueType>::~MyMap() {
	clear();
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::clear() {
    if(m_root == nullptr) {
        return;
    }
	deleteTree(m_root);
    m_size = 0;
}

template<typename KeyType, typename ValueType>
int MyMap<KeyType, ValueType>::size() const {
	return m_size;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::associate(const KeyType& key, const ValueType& value) {
	if(m_root == nullptr) {
		m_root = new Node(key, value);
        m_size++;
        return;
	}
	else {
		Node* current = m_root;
		while(current != nullptr) {
			if(current->m_key == key) {
				current->m_value = value;
				return;
			}
            else if(key < current->m_key) {
                if(current->m_left != nullptr) {
                    current = current->m_left;
                }
                else {
                    current->m_left = new Node(key, value);
                    m_size++;
                    return;
                }
            }
            else if(key > current->m_key) {
                if(current->m_right != nullptr) {
                    current = current->m_right;
                }
                else {
                    current->m_right = new Node(key, value);
                    m_size++;
                    return;
                }
            }
		}
	}
}

template<typename KeyType, typename ValueType>
const ValueType* MyMap<KeyType, ValueType>::find(const KeyType& key) const {
	if(m_root == nullptr) {
		return nullptr;
	}

	Node* current = m_root;
	while(current != nullptr) {
		if(current->m_key == key) {
			return &(current->m_value);
		}
        else if(key < current->m_key) {
            current = current->m_left;
        }
        else if(key > current->m_key) {
            current = current->m_right;
        }
	}

	return nullptr;
}

template<typename KeyType, typename ValueType>
void MyMap<KeyType, ValueType>::deleteTree(Node*& current) {
	if(current == nullptr) {
		return;
	}

	deleteTree(current->m_left);
	deleteTree(current->m_right);

	delete current;
    current = nullptr;
}

#endif