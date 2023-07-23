//
// Created by 神奇bug在哪里 on 2023/1/2.
//

#ifndef FINAL_PROJECT_SETTINGS_H
#define FINAL_PROJECT_SETTINGS_H

#define LOG_FILE_ENABLE 1
#define GLOBAL_LOG_LEVEL error
#define ERROR_COLOR "\033[0;32;31m" //红色
#define WARNING_COLOR "\033[1;33m"  //黄色
#define INFO_COLOR "\033[0;32;32m"  //绿色


#if LOG_FILE_ENABLE == 1
#define LOG_FILE_NAME "backend.log"
#define LOG_FILE_LEVEL info
#endif



#endif //FINAL_PROJECT_SETTINGS_H
