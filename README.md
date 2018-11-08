# AsyncLogger

```c++
#include "asynclog.h"
```

```c++
//add download logger
AsyncLogger & logger = AsyncLogger::instance();
logger.addLogFilter("download", 3);

aInfo() << "download success" << endl;

/*
download.log:

yyyy-MM-dd hh:mm:ss.zzz[Info]: download success
*/
```

```c++
//normal logger
aDebug() << "hello world" << endl;

/*
logger.log:

yyyy-MM-dd hh:mm:ss.zzz[Debug]: hello world
*/
```
