#pragma once
#include <cassert>
#include <cstdlib>
#include <algorithm>

const size_t N = 100;

template<typename T>
class Stack
{
public:
	explicit Stack(size_t n = N) : capacity(n), size(0), data((T*)calloc(n, sizeof(T))) {}
	Stack(const Stack<T>& other) : data((T*)calloc(other.size, sizeof(T))) 
	{
		for (int i = 0; i < other.size; i++)
			Push(other.data[i]);
	}
	void Push(const T& val) 
	{
		try
		{
			new (data + size) T(val);
			size++;
			if (size == capacity)
			{
				ChangeCapacity(2*capacity);
			}
		}
		catch (...)//(std::bad_alloc)
		{
			throw;
		}
	}
	T& Pop() 
	{ 
		size--; 
		return data[size];
	}
	Stack<T>& operator=(const Stack<T>& other)
	{
		Stack<T> tmp(other);
		if (this->capacity < tmp.capacity)
			this->ChangeCapacity(tmp.capacity);
		this->swap(tmp);
		return *this;
	}
	size_t Size() { return size; }
	T& Top() { return data[size - 1]; }
private:
	void ChangeCapacity(size_t newCap)
	{
		if (!(data = (T*)realloc(data, newCap * sizeof(T))))
			throw;
		capacity = newCap;
	}
	void swap(Stack<T>& other) throw()
	{
		for (size_t i = 0; i < other.size; i++)
		{
			std::swap<T>(this->data[i], other.data[i]);
		}
		size_t tmp = this->size;
		this->size = other.size;
		other.size = tmp;
	}
	size_t size;
	size_t capacity;
	T* data;
};