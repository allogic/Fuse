file (GLOB SRC_FILES "*.c")

target_sources (library PRIVATE ${SRC_FILES} "${VENDOR_DIR}/sqlite3/src/sqlite3.c")