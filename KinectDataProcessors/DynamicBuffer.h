#pragma once

template <class T>
class __declspec(dllexport) DynamicBuffer {
public:
	DynamicBuffer(const size_t& size);
	~DynamicBuffer();

	size_t GetBufferSize() const;
	void WriteNextValue(const T& val);

	T& First();
	T& Last();

	T& operator[](size_t index);
private:

	size_t GetNextIndex(size_t index);
	size_t GetPrevIndex(size_t index);

	T* m_buffer;
	size_t m_bufferSize;
	size_t m_currentIndex;
};

template<class T>
DynamicBuffer<T>::DynamicBuffer(const size_t& size)
{
	m_bufferSize = size;
	m_buffer = new T[m_bufferSize];
	m_currentIndex = 0;
}

template<class T>
DynamicBuffer<T>::~DynamicBuffer()
{
	delete[] m_buffer;
}

template<class T>
size_t DynamicBuffer<T>::GetBufferSize() const
{
	return m_bufferSize;
}

template<class T>
void DynamicBuffer<T>::WriteNextValue(const T& val)
{
	m_buffer[m_currentIndex] = val;
	m_currentIndex = GetNextIndex(m_currentIndex);
}

template<class T>
T& DynamicBuffer<T>::First()
{
	return m_buffer[0];
}

template<class T>
T& DynamicBuffer<T>::Last()
{
	return m_buffer[m_bufferSize - 1];
}

template<class T>
T& DynamicBuffer<T>::operator[](size_t index)
{
	if (index >= m_bufferSize)
		throw std::out_of_range("");

	auto startIndex = GetNextIndex(m_currentIndex);
	if (startIndex + index < m_bufferSize)
	{
		return m_buffer[index];
	}
	else
	{
		return m_buffer[index - startIndex];
	}
}

template<class T>
size_t DynamicBuffer<T>::GetNextIndex(size_t index)
{
	if (index + 1 != m_bufferSize)
	{
		return index + 1;
	}
	else
	{
		return 0;
	}
}

template<class T>
size_t DynamicBuffer<T>::GetPrevIndex(size_t index)
{
	if (index > 0)
	{
		return index - 1;
	}
	else
	{
		return m_bufferSize - 1;
	}
}