#ifndef TEST_UTILS_H
#define TEST_UTILS_H
#define ASSERT(condition, msg) _assert(condition, msg)

unsigned int _assert(unsigned int condition, const char* msg);

unsigned long now();
#endif
