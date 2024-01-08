#include "KinectDataProcessors.h"

namespace KinectDataProcessors {

	SinglePassExtractor::SinglePassExtractor()
	{
		m_isInitiated = FALSE;
		m_isFileProcessed = FALSE;
		m_dynamicBuffer = new DynamicBuffer<Joint>(10);
	}

	SinglePassExtractor::~SinglePassExtractor()
	{
		delete[] m_dynamicBuffer;
	}

	HRESULT SinglePassExtractor::Init(const wchar_t* targetDir)
	{
		HRESULT hr = S_OK;

		if (IsInit())
			hr = E_NOT_VALID_STATE;

		if (SUCCEEDED(hr))
		{
			m_is.open(targetDir, std::ios::in | std::ios::binary);
			hr = m_is.is_open() ? S_OK : E_HANDLE;
		}

		if (SUCCEEDED(hr))
			m_isInitiated = TRUE;

		return hr;
	}

	HRESULT SinglePassExtractor::GetPassSegments()
	{
		if (!IsInit())
			return HCS_E_INVALID_STATE;

		return S_OK;
	}

	HRESULT SinglePassExtractor::ProcessFile()
	{
		if (!IsInit())
			return HCS_E_INVALID_STATE;

		std::vector<std::pair<time_t, time_t>> segments;

		//Allocate temporary storage
		Joint joints[JointType_Count];
		time_t frameTime;
		std::pair<time_t, time_t> tempSegment = { 0, 0 };

		//fill the buffer
		for (int i = 0; i < m_dynamicBuffer->GetBufferSize(); i++)
		{
			if (!m_is.eof()) {
				m_is.read(reinterpret_cast<char*>(&joints), sizeof(Joint) * JointType_Count);
				m_is.read(reinterpret_cast<char*>(&frameTime), sizeof(time_t));
				m_dynamicBuffer->WriteNextValue(joints[JointType_SpineBase]);
				tempSegment.second = frameTime;
			}
			else
			{
				return E_ABORT;
			}
		}
		segments.push_back(tempSegment);

		// Sometimes due to the speed of the walking 
		// distance between points can cause single "direction switch"
		// That`s why we`ll track if direction is the same for the 5 following frames
		Direction currenDirection = GetWalkingDirection();
		Direction prevDirection = currenDirection;
		Direction direction = currenDirection;

		int directionChangeCounter = 0;
		bool isSegmentStart = false;
		bool isSegmentEnd = true;

		tempSegment = { 0, 0 };

		while (!m_is.eof())
		{
			m_is.read(reinterpret_cast<char*>(&joints), sizeof(Joint) * JointType_Count);
			m_is.read(reinterpret_cast<char*>(&frameTime), sizeof(time_t));
			m_dynamicBuffer->WriteNextValue(joints[JointType_SpineBase]);

			direction = GetWalkingDirection();
			if (direction != currenDirection)
			{
				if (direction == prevDirection)
					directionChangeCounter += 1;
				else
					directionChangeCounter = 0;
			}

			if (directionChangeCounter == 5)
			{
				currenDirection = direction;
				directionChangeCounter = 0
			}

			prevDirection = direction;
		}

		return S_OK;
	}

	BOOL SinglePassExtractor::IsInit() const
	{
		return m_isInitiated;
	}

	Direction SinglePassExtractor::GetWalkingDirection()
	{
		float delta = m_dynamicBuffer->Last().Position.Z - m_dynamicBuffer->First().Position.Z;
		if (delta <= -0.1) {
			return Direction::FORWARD;
		}
		else if (-0.1 < delta && delta < 0.1)
		{
			return Direction::STANDING_STILL;
		}
		else
		{
			return Direction::BACKWARDS;
		}
	}

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
			return m_bufferSize-1;
		}
	}
}
