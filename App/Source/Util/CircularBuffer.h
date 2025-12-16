#pragma once

template<typename T, size_t Capacity>
class CircularBuffer
{
public:
	void push(const T& value)
	{
		buffer[head] = value;
		head = (head + 1) % Capacity;

		if (size < Capacity)
			size++;
		else
			tail = (tail + 1) % Capacity;
	}

	T& operator[](size_t index)
	{
		return buffer[(tail + index) % Capacity];
	}

	const T& operator[](size_t index) const
	{
		return buffer[(tail + index) % Capacity];
	}

	size_t getSize() const { return size; }

	void clear()
	{
		head = 0;
		tail = 0;
		size = 0;
	}

private:
	T buffer[Capacity];
	size_t head = 0;
	size_t tail = 0;
	size_t size = 0;
};
