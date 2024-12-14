#ifndef _APP_H_
#define _APP_H_

#include "process.h"

Response listApp();

Response startApp(string path);

Response stopApp(int id);

#endif // !_APP_H_
