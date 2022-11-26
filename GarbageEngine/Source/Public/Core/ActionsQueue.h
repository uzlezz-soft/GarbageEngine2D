#pragma once

#include "Core/Base.h"
#include <queue>
#include <functional>
#include <mutex>

class ActionsQueue final
{
public:

	using Action = std::function<void()>;

	ActionsQueue() = default;

	FORCEINLINE void AddAction(Action action)
	{
		std::scoped_lock<std::mutex> scopedLock(m_mutex);
		m_queue.push(action);
	}

	void Execute()
	{
		std::scoped_lock<std::mutex> scopedLock(m_mutex);

		while (!m_queue.empty())
		{
			auto& action = m_queue.front();
			action();
			m_queue.pop();
		}
	}

private:

	std::queue<Action> m_queue;
	std::mutex m_mutex;

};