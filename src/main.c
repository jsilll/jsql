#define BASE_IMPLEMENTATION
#include "sqldb/core.h"

#include <signal.h>
#include <unistd.h>

// =================================================================================================
// :: Global Variables ::
// =================================================================================================

static volatile bool g_shutdown_requested = false;

// =================================================================================================
// :: Signal Handling ::
// =================================================================================================

static void signal_handler(int sig) {
  (void)sig;
  g_shutdown_requested = true;
}

// =================================================================================================
// :: Main Loop ::
// =================================================================================================

static int run_database_server(Database *db) {
  ASSERT(db && db->is_initialized);
  LOG_INFO("Starting database server on port %d", db->config->port);
  LOG_INFO("Database file: %s", db->config->db_file_path);
  LOG_INFO("Page size: %u bytes", db->config->page_size);
  LOG_INFO("Cache size: %u MB", db->config->cache_size_mb);
  LOG_INFO("Read-only mode: %s",
           db->config->read_only ? "enabled" : "disabled");
  LOG_INFO("WAL mode: %s", db->config->enable_wal ? "enabled" : "disabled");
  while (!g_shutdown_requested) {
    // TODO: Implement main server logic
    // - Accept client connections
    // - Parse SQL queries
    // - Execute queries
    // - Return results
    usleep(100000); // 100ms
  }
  LOG_INFO("Server loop exited");
  return EXIT_SUCCESS;
}

// =================================================================================================
// :: Entry Point ::
// =================================================================================================

int main(int argc, char **argv) {
  DatabaseConfig config = {0};
  db_config_init_defaults(&config);
  if (!db_config_from_args(&config, argc, argv)) {
    return EXIT_FAILURE;
  }

  g_log_level = config.log_level;
  if (g_log_level == LOG_LEVEL_DEBUG) {
    LOG_DEBUG("Debug logging enabled");
  }

  LOG_INFO("Setting up signal handlers for graceful shutdown");
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

  LOG_INFO("Starting SQL Database Server");
  Database db = {0};
  if (!db_init(&db, &config)) {
    LOG_FATAL("Failed to initialize database");
  }

  int exit_code = run_database_server(&db);
  db_shutdown(&db);

  LOG_INFO("Database server exited with code %d", exit_code);
  return exit_code;
}