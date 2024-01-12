#include "KinectDataProcessors.h"
#include <cmath>

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
			hr = m_reader.Init(targetDir);
			hr = m_reader.IsInit() ? S_OK : E_HANDLE;
		}

		if (SUCCEEDED(hr))
			m_isInitiated = TRUE;

		return hr;
	}

	HRESULT SinglePassExtractor::GetPassSegments()
	{
		if (!IsInit())
			return E_NOT_VALID_STATE;

		return S_OK;
	}

	HRESULT SinglePassExtractor::ProcessFile(std::vector<std::pair<time_t, time_t>>& segments)
	{
		if (!IsInit())
			return E_NOT_VALID_STATE;

		//Allocate temporary storage
		SkeletonModeFrameData skeletonData(nullptr);
		skeletonData.ReserveBufferMemory();
		time_t frameTime;
		std::pair<time_t, time_t> tempSegment = { 0, 0 };

		//fill the buffer
		for (int i = 0; i < m_dynamicBuffer->GetBufferSize(); i++)
		{
			if (!m_reader.IsEOF()) {
				m_reader.ReadFrame(&skeletonData);
				Joint* pJoints = reinterpret_cast<Joint*>(skeletonData.pBuffer);
				m_dynamicBuffer->WriteNextValue((pJoints[JointType_SpineBase]));
				frameTime = skeletonData.timestamp;
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

		while (!m_reader.IsEOF())
		{
			m_reader.ReadFrame(&skeletonData);
			Joint* pJoints = reinterpret_cast<Joint*>(skeletonData.pBuffer);
			m_dynamicBuffer->WriteNextValue(pJoints[JointType_SpineBase]);
			frameTime = skeletonData.timestamp;

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
		
		//TODO remove code if nothing gonna change
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

	AxisRotator::AxisRotator()
	{
	}

	AxisRotator::~AxisRotator()
	{
	}

	HRESULT AxisRotator::Init(const wchar_t* filePath)
	{
		if (IsInit())
			return E_NOT_VALID_STATE;

		m_reader.Init(filePath);
		if (!m_reader.IsInit())
			return E_HANDLE;

		m_isInit = TRUE;
		return S_OK;
	}

	HRESULT AxisRotator::CalculateRotationAngles(std::vector<std::pair<time_t, time_t>>& segments)
	{
		if (m_wereAnglesCalculated)
			return E_NOT_VALID_STATE;

		SkeletonModeFrameData skeletonModeFrameData(nullptr);
		skeletonModeFrameData.ReserveBufferMemory();

		for (const auto& segment : segments)
		{
			auto startTime = segment.first;
			auto endTime = segment.second;
			bool isFirstRead = true;
			CameraSpacePoint start = { -1, -1, -1 };
			CameraSpacePoint end = { -1, -1, -1 };

			while (!m_reader.IsEOF())
			{
				HRESULT hr = m_reader.ReadFrame(&skeletonModeFrameData);
				if (FAILED(hr))
					throw std::runtime_error("Unexpected error during rotation angles calculation");
				
				if (skeletonModeFrameData.timestamp > segment.second)
					break;

				Joint* pJoints = reinterpret_cast<Joint*>(skeletonModeFrameData.pBuffer);
				if (isFirstRead)
				{
					start = pJoints[JointType_SpineBase].Position;
					isFirstRead = false;
				}
				else
				{
					start = pJoints[JointType_SpineBase].Position;
				}
			}

			if (IsValidSpacePoint(start))
				return E_FAIL;
			
			if (IsValidSpacePoint(end))
				return E_FAIL;

			auto delta_z = end.Z - start.Z;
			auto delta_x = end.X - start.X;

			auto angle = std::atan2(delta_x, delta_z);
			m_rotationAngles.push_back(angle);
		}

		m_wereAnglesCalculated = TRUE;
		return S_OK;
	}

	HRESULT AxisRotator::GetRotationAngles(std::vector<float>& angles)
	{
		if (!m_wereAnglesCalculated)
			return E_NOT_VALID_STATE;
		angles = m_rotationAngles;
	}

	HRESULT AxisRotator::CreateFileWithRotatedAxis()
	{
		return S_OK;
	}

	BOOL AxisRotator::IsInit() const
	{
		return m_isInit;
	}

	BOOL AxisRotator::IsValidSpacePoint(const CameraSpacePoint& spacePoint) const
	{
		return spacePoint.X == -1 && spacePoint.Y == -1 && spacePoint.Z == -1;
	}
}