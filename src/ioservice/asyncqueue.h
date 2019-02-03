#ifndef IOSERVICE_ASYNC_QUEUE_H
#define IOSERVICE_ASYNC_QUEUE_H

#include <QReadWriteLock>
#include <QQueue>

namespace ioservice
{

template <typename T>
class AsyncQueue
{
public:
    AsyncQueue() = default;

    T dequeue()
    {
        QWriteLocker wlock(&m_rwlock);
        return m_queue.dequeue();
    }

    void enqueue(const T& element)
    {
        QWriteLocker wlock(&m_rwlock);
        m_queue.enqueue(element);
    }

    bool isEmpty() const
    {
        QReadLocker rlock(&m_rwlock);
        return m_queue.isEmpty();
    }

private:
    mutable QReadWriteLock m_rwlock;
    QQueue<T> m_queue;

};

} // ioservice

#endif // IOSERVICE_ASYNC_QUEUE_H
