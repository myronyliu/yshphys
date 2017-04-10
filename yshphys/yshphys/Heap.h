#pragma once
template <class T>
class Heap_t
{
public:
	Heap_t(bool(*LessThan)(const T&, const T&));
	virtual ~Heap_t();

	void Push(const T& element);
	T Pop();
	T Top() const;

protected:
	T* m_elements;
	unsigned int m_nElements;
	unsigned int m_capacity;
	bool(*m_lessThan)(const T&, const T&);
	
	void HeapifyUp(unsigned int i);
	void HeapifyDown(unsigned int i);
	void DeleteElement(unsigned int i);
};

