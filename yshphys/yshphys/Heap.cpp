#include "stdafx.h"
#include "Heap.h"

#define HEAP_INIT_CAPACITY 256

template <class T>
Heap_t<T>::Heap_t(bool(*LessThan)(const T&, const T&)) : m_capacity(HEAP_INIT_CAPACITY), m_nElements(0), m_lessThan(LessThan)
{
	m_elements = new T[HEAP_INIT_CAPACITY];
}

template <class T>
Heap_t<T>::~Heap_t()
{
	delete[] m_elements;
}

template <class T>
void Heap_t<T>::Push(const T& element)
{
	if (m_nElements = m_capacity)
	{
		unsigned int newCapacity = 2 * m_capacity;
		T* newElements = new T[newCapacity];
		std::memcpy(newElements, m_elements, m_capacity);
		delete[] m_elements;
		m_elements = newElements;
		m_capacity = newCapacity;
	}
	int i = m_nElements++;
	m_elements[i] = element;

	HeapifyUp(i);
}

template <class T>
void Heap_t<T>::HeapifyUp(unsigned int index)
{
	int i = index;
	int j = (i + 1) / 2 - 1; // index of i's parent

	while (m_lessThan(i, j) && i > 0)
	{
		T swp = m_elements[i];
		m_elements[i] = m_elements[j];
		m_elements[j] = swp;
		i = j;
		j = (i + 1) / 2 - 1;
	}
}

template <class T>
void Heap_t<T>::HeapifyDown(unsigned int index)
{
	int i = index;

	while (true)
	{
		unsigned int iLeft = 2 * (i + 1) - 1;
		unsigned int iRight = iLeft + 1;
		if (iRight > m_nElements)
		{
			break;
		}
		else
		{
			int j;
			if (iRight == m_nElements)
			{
				j = iLeft;
			}
			else // ... if (iRight < m_nElements)
			{
				j = m_lessThan(iRight, iLeft) ? iRight: iLeft; // i.e. if (left <= right) then left, else right
			}

			if (m_lessThan(j, i))
			{
				T swp = m_elements[i];
				m_elements[i] = m_elements[j];
				m_elements[j] = swp;

				i = j;
			}
			else
			{
				break;
			}
		}
	}
}

template <class T>
T Heap_t<T>::Pop()
{
	T top = m_elements[0];
	DeleteElement(0);
	return top;
}

template <class T>
T Heap_t<T>::Top() const
{
	return m_elements[0];
}

template <class T>
void Heap_t<T>::DeleteElement(unsigned int index)
{
	m_elements[index] = m_elements[--m_nElements];
	HeapifyDown(index);
}

template class Heap_t<int>;
