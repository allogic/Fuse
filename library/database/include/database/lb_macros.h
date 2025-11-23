#ifndef LB_DATABASE_MACROS_H
#define LB_DATABASE_MACROS_H

#ifdef BUILD_DEBUG
#  define LB_SQL_CHECK(EXPRESSION)                                                     \
    {                                                                                  \
      uint32_t result = (EXPRESSION);                                                  \
      if (result != SQLITE_OK) {                                                       \
        printf("%s failed with %s\n", #EXPRESSION, sqlite3_errmsg(s_database_handle)); \
        __debugbreak();                                                                \
      }                                                                                \
    }
#else
#  define LB_SQL_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#ifdef BUILD_DEBUG
#  define LB_SQL_CHECK_STATUS(EXPRESSION, STATUS)                                      \
    {                                                                                  \
      uint32_t result = (EXPRESSION);                                                  \
      if (result != (STATUS)) {                                                        \
        printf("%s failed with %s\n", #EXPRESSION, sqlite3_errmsg(s_database_handle)); \
        __debugbreak();                                                                \
      }                                                                                \
    }
#else
#  define LB_SQL_CHECK_STATUS(EXPRESSION, STATUS) (EXPRESSION)
#endif // BUILD_DEBUG

#endif // LB_DATABASE_MACROS_H
