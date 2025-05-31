#include "sqldb/database.h"

bool db_init(Database *db, const DatabaseConfig *config) {
  ASSERT(db && config);
  LOG_INFO("Initializing database with file: %s", config->db_file_path);

  usize temp_arena_size = 1024 * 1024; // 1 MB for temporary allocations
  usize main_arena_size = config->cache_size_mb * 1024 * 1024;

  db->config = config;
  db->main_arena = arena_init(main_arena_size);
  db->temp_arena = arena_init(temp_arena_size);

  const char *mode = config->read_only ? "rb" : "rb+";
  db->db_file = fopen(config->db_file_path, mode);
  if (!db->db_file && !config->read_only) {
    LOG_INFO("Database file doesn't exist, creating new file: %s",
             config->db_file_path);
    db->db_file = fopen(config->db_file_path, "wb+");
  }

  if (!db->db_file) {
    LOG_ERROR("Failed to open database file: %s", config->db_file_path);
    arena_free_all(&db->temp_arena);
    arena_free_all(&db->main_arena);
    return false;
  }

  // TODO: Initialize page cache hash table
  // db->page_cache = ht_oa_init(256, page_hash_fn, page_equal_fn,
  // db->main_arena);

  db->is_initialized = true;
  LOG_INFO("Database initialized successfully");
  return true;
}

void db_shutdown(Database *db) {
  if (!db || !db->is_initialized) {
    return;
  }

  LOG_INFO("Shutting down database");

  if (db->db_file) {
    fclose(db->db_file);
    db->db_file = NULL;
  }

  ht_oa_free(&db->page_cache);
  arena_free_all(&db->main_arena);
  arena_free_all(&db->temp_arena);

  db->is_initialized = false;
  LOG_INFO("Database shutdown complete");
}