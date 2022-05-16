#ifndef _LOG_H_
#define _LOG_H_

#ifdef YACL_LOG
#define LOG do                                                                                                  \
{                                                                                                               \
	char str_line[16];                                                                                          \
	sprintf(str_line, "%d", __LINE__);                                                                          \
	printf("[%s, %d%s%*.*s -- ", __func__, __LINE__, "]", 15 - strlen(__func__) - strlen(str_line) + 4, 0, " ");\
} while(0)

#define LOG_DEBUG(...) LOG; printf(__VA_ARGS__)
#else
#define LOG_DEBUG(format, ...)
#endif

#endif //_LOG_H_
