#include "asynclog.h"
#include "logger.h"
#include <QEvent>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>

class AsyncLoggerEvent : public QEvent
{
public:
    AsyncLoggerEvent(QString &content, int level)
        : QEvent(eventType()), m_content(content), m_level(level)
    {

    }

    static Type eventType() {
        if(s_evType == QEvent::None) {
            s_evType = (QEvent::Type)registerEventType();
        }
        return s_evType;
    }

    static Type s_evType;
    QString m_content;
    int m_level;
};
QEvent::Type AsyncLoggerEvent::s_evType = QEvent::None;

class LogBuffer
{
public:
    LogBuffer(QObject *receiver)
        : m_receiver(receiver)
        ,m_level(log_error),m_maxLen(4096)
    {
        m_content.reserve(m_maxLen);
    }

    void flush()
    {
        if( m_content.length() > 0)
        {
            QCoreApplication::postEvent(m_receiver, new AsyncLoggerEvent(m_content, m_level));
            m_content.clear();
        }
    }

    void checkLogLevelChange(int level)
    {
        flush();
        if( level != m_level ) m_level = level;
    }

    void debug()
    {
        checkLogLevelChange(log_debug);
    }

    void info()
    {
        checkLogLevelChange(log_info);
    }

    void warn()
    {
        checkLogLevelChange(log_warn);
    }

    void error()
    {
        checkLogLevelChange(log_error);
    }

    void checkFlush()
    {
        if(m_content.length() > 4000)
        {
            QCoreApplication::postEvent(m_receiver, new AsyncLoggerEvent(m_content, m_level));
            m_content.clear();
        }
    }

    void putQChar(QChar t)
    {
        m_content += t;
        checkFlush();
    }

    void putChar(char t)
    {
        m_content += t;
        checkFlush();
    }

    void putCString(const char *t)
    {
        m_content += t;
        checkFlush();
    }

    void putQString(const QString &t)
    {
        m_content += t;
        checkFlush();
    }

    void putLatin1(const QLatin1String &t)
    {
        m_content += t;
        checkFlush();
    }

    void putByteArray(const QByteArray &t)
    {
        m_content += t;
        checkFlush();
    }

    void putStringRef(const QStringRef &t)
    {
        m_content += t;
        checkFlush();
    }

    QObject *m_receiver;
    int m_level;
    int m_maxLen;
    QString m_content;
};


typedef QHash<Qt::HANDLE, LogBuffer*> LOGBUFFER;
class AsyncLoggerPrivate : public QObject
{
public:
    AsyncLoggerPrivate()
        : m_log("logger.log")
    {
        m_log.setMaxSize(524288);
    }
    ~AsyncLoggerPrivate(){}

    bool event(QEvent *e)
    {
        if(e->type() == AsyncLoggerEvent::s_evType)
        {
            AsyncLoggerEvent* ale = (AsyncLoggerEvent*)e;
            QHash<QString,Logger*>::iterator it = m_logFilters.begin();
            while(it != m_logFilters.end())
            {
                if(ale->m_content.startsWith(it.key())) break;
                ++it;
            }
            if(it != m_logFilters.end())
            {
                it.value()->log(ale->m_level, ale->m_content);
            }
            else
            {
                m_log.log(ale->m_level, ale->m_content);
            }
            return true;
        }

        return QObject::event(e);
    }

    LogBuffer *getLogBuffer(Qt::HANDLE tid)
    {
        LOGBUFFER::iterator it = m_logBuffers.find(tid);
        LogBuffer *lb;
        if(it == m_logBuffers.end())
        {
            lb = new LogBuffer(this);
            m_logBuffers.insert( tid, lb );
        }
        else
        {
            lb = it.value();
        }
        return lb;
    }

    Logger m_log;
    QHash<Qt::HANDLE, LogBuffer*> m_logBuffers;
    QHash<QString, Logger*> m_logFilters;
};

AsyncLogger::AsyncLogger()
    :m_priv(new AsyncLoggerPrivate)
{
    AsyncLoggerEvent::eventType();
}

AsyncLogger::~AsyncLogger()
{
    delete m_priv;
}

AsyncLogger &AsyncLogger::instance()
{
    static AsyncLogger logger;
    return logger;
}

void AsyncLogger::setLogLevel(int level)
{
    m_priv->m_log.setLogLevel(level);
}

void AsyncLogger::addLogFilter(const QString &tag, int level)
{
//    QString logFile = QString("%1/%2").arg(g_log_options->m_logdir)
//            .arg(tag);
    Logger *logger = new Logger(tag + ".log", level);
    logger->setMaxSize(524288);
    m_priv->m_logFilters.insert(tag, logger);
}

void AsyncLogger::debug()
{
    m_priv->getLogBuffer(QThread::currentThreadId())->debug();
}

void AsyncLogger::info()
{
    m_priv->getLogBuffer(QThread::currentThreadId())->info();
}

void AsyncLogger::warn()
{
    m_priv->getLogBuffer(QThread::currentThreadId())->warn();
}

void AsyncLogger::error()
{
    m_priv->getLogBuffer(QThread::currentThreadId())->error();
}

void AsyncLogger::flush()
{
    m_priv->getLogBuffer(QThread::currentThreadId())->flush();
}

AsyncLogger & AsyncLogger::operator<<(QChar t)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQChar(t);
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(bool t)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putCString(t ? "true" : "false");
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(char t)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putChar(t);
    return *this;
}

AsyncLogger & AsyncLogger::operator<<(signed short i)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(QString::number(i));
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(unsigned short i)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(QString::number(i));
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(signed int i)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(QString::number(i));
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(unsigned int i)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(QString::number(i));
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(signed long l)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(QString::number(l));
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(unsigned long l)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(QString::number(l));
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(qint64 i)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(QString::number(i));
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(quint64 i)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(QString::number(i));
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(float f)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(QString::number(f));
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(double f)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(QString::number(f));
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(const char * s)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putCString(s);
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(const QString & s)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putQString(s);
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(const QStringRef & s)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putStringRef(s);
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(QLatin1String s)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putLatin1(s);
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(const QByteArray & b)
{
    m_priv->getLogBuffer(QThread::currentThreadId())->putByteArray(b);
    return *this;
}
AsyncLogger & AsyncLogger::operator<<(const void * p)
{
    char buf[16];
    sprintf(buf, "%p", p);
    m_priv->getLogBuffer(QThread::currentThreadId())->putCString(buf);
    return *this;
}

//
AsyncLogger & debug(AsyncLogger &logger)
{
    logger.debug();
    return logger;
}

AsyncLogger & info(AsyncLogger &logger)
{
    logger.info();
    return logger;
}

AsyncLogger & warn(AsyncLogger &logger)
{
    logger.warn();
    return logger;
}

AsyncLogger & error(AsyncLogger &logger)
{
    logger.error();
    return logger;
}

AsyncLogger & endl(AsyncLogger &logger)
{
    logger.flush();
    return logger;
}
AsyncLogger & flush(AsyncLogger &logger)
{
    logger.flush();
    return logger;
}
