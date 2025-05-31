#ifndef SQLDB_DATABASE_H
#define SQLDB_DATABASE_H

#include "base.h"

// =================================================================================================
// :: Database Configuration ::
// =================================================================================================

#define DEFAULT_DB_FILE "database.db"
#define DEFAULT_PAGE_SIZE 4096
#define DEFAULT_CACHE_SIZE_MB 64
#define DEFAULT_PORT 5432

typedef struct {
  char *db_file_path;
  u32 page_size;
  u32 cache_size_mb;
  u16 port;
  bool enable_wal;
  bool read_only;
  LogLevel log_level;
} DatabaseConfig;

void db_config_init_defaults(DatabaseConfig *config);

bool db_config_from_args(DatabaseConfig *config, int argc, char **argv);

// =================================================================================================
// :: Database Core Structures ::
// =================================================================================================

typedef struct {
  FILE *db_file;
  Arena main_arena;
  Arena temp_arena;
  BaseHashTableOA page_cache;
  bool is_initialized;
  const DatabaseConfig *config;
} Database;

bool db_init(Database *db, const DatabaseConfig *config);

void db_shutdown(Database *db);

#endif // SQLDB_DATABASE_H
