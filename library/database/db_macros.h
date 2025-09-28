#ifndef DB_MACROS_H
#define DB_MACROS_H

#ifdef BUILD_DEBUG
#  define SQL_CHECK(EXPRESSION)                                                        \
    {                                                                                  \
      uint32_t result = (EXPRESSION);                                                  \
      if (result != SQLITE_OK) {                                                       \
        printf("%s failed with %s\n", #EXPRESSION, sqlite3_errmsg(s_database_handle)); \
        __debugbreak();                                                                \
      }                                                                                \
    }
#else
#  define SQL_CHECK(EXPRESSION) (EXPRESSION)
#endif // BUILD_DEBUG

#ifdef BUILD_DEBUG
#  define SQL_CHECK_STATUS(EXPRESSION, STATUS)                                         \
    {                                                                                  \
      uint32_t result = (EXPRESSION);                                                  \
      if (result != (STATUS)) {                                                        \
        printf("%s failed with %s\n", #EXPRESSION, sqlite3_errmsg(s_database_handle)); \
        __debugbreak();                                                                \
      }                                                                                \
    }
#else
#  define SQL_CHECK_STATUS(EXPRESSION, STATUS) (EXPRESSION)
#endif // BUILD_DEBUG

#endif // DB_MACROS_H
