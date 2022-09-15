#ifndef HASHMAP_MACROS_H
#define HASHMAP_MACROS_H
#ifndef OR
#define OR(a, b) (a ? a : b)
#endif


#define HASHMAP_WARNING_RETURN(ret, ...)                                          \
  {                                                                            \
    printf("\n****\n");                                                        \
    printf("(HASHMAP)(Warning)(%s): \n", __func__);   \
    fprintf(__VA_ARGS__);                                                      \
    printf("\n****\n");                                                        \
    return ret;                                                                \
  }

#endif
