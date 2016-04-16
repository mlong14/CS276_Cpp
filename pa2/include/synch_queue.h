#ifndef PA2_SYNCH_Q_H_
#define PA2_SYNCH_Q_H_

#include <queue>
#include <iostream>
#include <boost/thread.hpp>

template <class T>
class SynchronisedQueue {
	public:
		void Enqueue(const T &data) {
			boost::unique_lock<boost::mutex> lock(m_mutex);
			m_queue.push(data);
			m_cond.notify_one();
		}
		T Dequeue() {
			boost::unique_lock<boost::mutex> lock(m_mutex);
			while (m_queue.size() == 0) {
				m_cond.wait(lock);
			}
			T result = m_queue.front();
			m_queue.pop();
			return result;
		}
		int size() {
			return m_queue.size();
		}
	private:
		std::queue<T> m_queue;
		boost::mutex m_mutex;
		boost::condition_variable m_cond;
};

#endif
