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
		delete m_dynamicBuffer;
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

	HRESULT SinglePassExtractor::ProcessFile(std::vector<std::pair<time_t, time_t>>& segments)
	{
		if (!IsInit())
			return HCS_E_INVALID_STATE;

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

		// Sometimes due to the speed of the walking 
		// distance between points can cause single "direction switch"
		// That`s why we`ll track if direction is the same for the 5 following frames
		Direction currenDirection = GetWalkingDirection();
		Direction prevDirection = currenDirection;
		Direction direction = currenDirection;

		if(currenDirection != Direction::FORWARD)
			tempSegment = { 0, 0 };


		int directionChangeCounter = 0;
		bool isSegmentStart = false;
		bool isSegmentEnd = true;

		int counter = 0;

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
				directionChangeCounter = 0;
				switch (currenDirection)
				{
					case Direction::BACKWARDS:
					case Direction::STANDING_STILL:
						tempSegment.second = frameTime;
						segments.push_back(tempSegment);
						tempSegment = { 0, 0 };
						break;
					case Direction::FORWARD:
						tempSegment.first = frameTime;
						break;
				}
			}

			prevDirection = direction;

			counter += 1;
			printf_s("Iteration #%d Delta:%f\n", counter, m_dynamicBuffer->Last().Position.Z - m_dynamicBuffer->First().Position.Z);
		}

		if (currenDirection == Direction::FORWARD)
		{
			tempSegment.second = frameTime;
			segments.push_back(tempSegment);
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
		return delta >= 0 ? Direction::FORWARD : Direction::BACKWARDS;
		/*if (delta <= -0.1) {
			return Direction::FORWARD;
		}
		else if (-0.1 < delta && delta < 0.1)
		{
			return Direction::STANDING_STILL;
		}
		else
		{
			return Direction::BACKWARDS;
		}*/
	}
}