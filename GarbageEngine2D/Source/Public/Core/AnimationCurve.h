#pragma once

#include "Core/Base.h"
#include "Core/Assert.h"
#include "Math/Color.h"
#include <vector>

class GARBAGE_API AnimationCurve
{
public:

	struct KeyFrame
	{
		float Time{ 0.0f };
		float Value{ 0.0f };
		float OutTangent{ 1.0f };
		float InTangent{ 1.0f };

		KeyFrame(float time, float value) : Time(time), Value(value) {}
	};

	AnimationCurve() = default;
	AnimationCurve(float minValue, float maxValue)
	{
		m_keyFrames.push_back(KeyFrame(0.0f, minValue));
		m_keyFrames.push_back(KeyFrame(1.0f, maxValue));
	}

	void AddKeyFrame(const KeyFrame& keyFrame)
	{
		if (m_keyFrames.size() > 0)
		{
			if (m_keyFrames[0].Time > keyFrame.Time)
			{
				m_keyFrames.insert(m_keyFrames.begin(), keyFrame);
			}
			else if (m_keyFrames.back().Time < keyFrame.Time)
			{
				m_keyFrames.push_back(keyFrame);
			}
			else
			{
				for (uint64 i = 1; i < m_keyFrames.size(); i++)
				{
					if (m_keyFrames[i].Time > keyFrame.Time)
					{
						m_keyFrames.insert(m_keyFrames.begin() + i, keyFrame);
						break;
					}
				}
			}
		}
		else m_keyFrames.push_back(keyFrame);
	}

	float Evaluate(float time) const
	{
		time = std::clamp(time, 0.0f, 1.0f);

		if (m_keyFrames.size() > 1)
		{
			if (time == 1.0f) return m_keyFrames.back().Value;

			for (uint32 i = 0; i < m_keyFrames.size(); i++)
			{
				if (m_keyFrames[i].Time <= time && m_keyFrames[i + 1].Time >= time)
				{
					return EvaluateBetween(time, m_keyFrames[i], m_keyFrames[i + 1]);
				}
			}

			return m_keyFrames.back().Value;
		}
		else if (m_keyFrames.size() == 1)
		{
			return m_keyFrames[0].Value;
		}
		else
		{
			return 0.0f;
		}
	}

	KeyFrame& GetKeyFrame(uint64 index)
	{
		return m_keyFrames[index];
	}

	std::vector<KeyFrame>& GetAllKeyFrames()
	{
		return m_keyFrames;
	}

	void ClearKeyFrames()
	{
		m_keyFrames.clear();
	}

private:

	std::vector<KeyFrame> m_keyFrames;

	float EvaluateBetween(float t, const KeyFrame& left, const KeyFrame& right) const
	{
		/*float dt = right.Time - left.Time;

		float m0 = left.OutTangent * dt;
		float m1 = right.InTangent * dt;

		float t2 = t * t;
		float t3 = t2 * t;

		float a = 2.0f * t3 - 3.0f * t2 + 1.0f;
		float b = t3 - 2.0f * t2 + t;
		float c = t3 - t2;
		float d = -2.0f * t3 + 3.0f * t2;

		return a * left.Value + b * m0 + c * m1 + d * right.Value;*/
		return Math::Lerp(left.Value, right.Value, (t - left.Time) / (right.Time - left.Time));
	}

};

class GARBAGE_API ColorAnimationCurve
{
public:

	struct KeyFrame
	{
		float Time{ 0.0f };
		Color Value{ 0.0f };
		float OutTangent{ 1.0f };
		float InTangent{ 1.0f };

		KeyFrame(float time, Color value) : Time(time), Value(value) {}
	};

	ColorAnimationCurve() = default;
	ColorAnimationCurve(Color minValue, Color maxValue)
	{
		m_keyFrames.push_back(KeyFrame(0.0f, minValue));
		m_keyFrames.push_back(KeyFrame(1.0f, maxValue));
	}

	void AddKeyFrame(const KeyFrame& keyFrame)
	{
		if (m_keyFrames.size() > 0)
		{
			if (m_keyFrames[0].Time > keyFrame.Time)
			{
				m_keyFrames.insert(m_keyFrames.begin(), keyFrame);
			}
			else if (m_keyFrames.back().Time < keyFrame.Time)
			{
				m_keyFrames.push_back(keyFrame);
			}
			else
			{
				for (uint64 i = 1; i < m_keyFrames.size(); i++)
				{
					if (m_keyFrames[i].Time > keyFrame.Time)
					{
						m_keyFrames.insert(m_keyFrames.begin() + i, keyFrame);
						break;
					}
				}
			}
		}
		else m_keyFrames.push_back(keyFrame);
	}

	Color Evaluate(float time) const
	{
		time = std::clamp(time, 0.0f, 1.0f);

		if (m_keyFrames.size() > 1)
		{
			if (time == 1.0f) return m_keyFrames.back().Value;

			for (uint32 i = 0; i < m_keyFrames.size(); i++)
			{
				if (m_keyFrames[i].Time <= time && m_keyFrames[i + 1].Time >= time)
				{
					return EvaluateBetween(time, m_keyFrames[i], m_keyFrames[i + 1]);
				}
			}

			return m_keyFrames.back().Value;
		}
		else if (m_keyFrames.size() == 1)
		{
			return m_keyFrames[0].Value;
		}
		else
		{
			return Color(0.0f);
		}
	}

	KeyFrame& GetKeyFrame(uint64 index)
	{
		return m_keyFrames[index];
	}

	std::vector<KeyFrame>& GetAllKeyFrames()
	{
		return m_keyFrames;
	}

	void ClearKeyFrames()
	{
		m_keyFrames.clear();
	}

private:

	std::vector<KeyFrame> m_keyFrames;

	Color EvaluateBetween(float t, const KeyFrame& left, const KeyFrame& right) const
	{
		/*float dt = right.Time - left.Time;

		float m0 = left.OutTangent * dt;
		float m1 = right.InTangent * dt;

		float t2 = t * t;
		float t3 = t2 * t;

		float a = 2.0f * t3 - 3.0f * t2 + 1.0f;
		float b = t3 - 2.0f * t2 + t;
		float c = t3 - t2;
		float d = -2.0f * t3 + 3.0f * t2;

		return a * left.Value + b * m0 + c * m1 + d * right.Value;*/

		const float time = (t - left.Time) / (right.Time - left.Time);
		return left.Value.Lerp(right.Value, time);
	}

};