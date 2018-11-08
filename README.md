# AsyncLogger

```c++
#include "asynclog.h"
```

```c++
//download.log
AsyncLogger & logger = AsyncLogger::instance();
logger.addLogFilter("download", 3);

aInfo() << "download success" << endl;
```

```c++
//logger.log
aDebug() << "hello world" << endl;
```