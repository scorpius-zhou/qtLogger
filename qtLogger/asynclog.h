#ifndef ASYNCLOG_H
#define ASYNCLOG_H

#include <QObject>

class AsyncLoggerPrivate;
class AsyncLogger
{
private:
    AsyncLogger();
    AsyncLogger(const AsyncLogger &);
    AsyncLogger & operator =(const AsyncLogger &);

public:
    ~AsyncLogger();

    static AsyncLogger &instance();
    void setLogLevel(int level);
    void addLogFilter(const QString &tag, int level);

    void debug();
    void info();
    void warn();
    void error();
    void flush();

    AsyncLogger & operator<<(QChar t);
    AsyncLogger & operator<<(bool t);
    AsyncLogger & operator<<(char t);
    AsyncLogger & operator<<(signed short i);
    AsyncLogger & operator<<(unsigned short i);
    AsyncLogger & operator<<(signed int i);
    AsyncLogger & operator<<(unsigned int i);
    AsyncLogger & operator<<(signed long l);
    AsyncLogger & operator<<(unsigned long l);
    AsyncLogger & operator<<(qint64 i);
    AsyncLogger & operator<<(quint64 i);
    AsyncLogger & operator<<(float f);
    AsyncLogger & operator<<(double f);
    AsyncLogger & operator<<(const char * s);
    AsyncLogger & operator<<(const QString & s);
    AsyncLogger & operator<<(const QStringRef & s);
    AsyncLogger & operator<<(QLatin1String s);
    AsyncLogger & operator<<(const QByteArray & b);
    AsyncLogger & operator<<(const void * p);

private:
    AsyncLoggerPrivate *m_priv;
};

//  函数指针, 定义 AsyncLoggerFunction函数，参数为 AsyncLogger &， 返回值为 AsyncLogger &;
typedef AsyncLogger & (*AsyncLoggerFunction)(AsyncLogger &);

//  以下函数都符合 AsyncLoggerFunction
AsyncLogger & endl(AsyncLogger &s);
AsyncLogger & flush(AsyncLogger &s);
AsyncLogger & debug(AsyncLogger &s);
AsyncLogger & info(AsyncLogger &s);
AsyncLogger & warn(AsyncLogger &s);
AsyncLogger & error(AsyncLogger &s);


inline AsyncLogger &operator<<(AsyncLogger &s, AsyncLoggerFunction f) {
    return (*f)(s);
}

#define aLog() AsyncLogger::instance()
#define aDebug() AsyncLogger::instance() << debug
#define aInfo() AsyncLogger::instance() << info
#define aWarn() AsyncLogger::instance() << warn
#define aError() AsyncLogger::instance() << error

#endif // ASYNCLOG_H
