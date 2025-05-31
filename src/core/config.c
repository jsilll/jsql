#include "sqldb/database.h"

// =================================================================================================
// :: Private Helper Functions ::
// =================================================================================================

static void print_usage(const char *program_name);

// =================================================================================================
// :: Public API ::
// =================================================================================================

void db_config_init_defaults(DatabaseConfig *config) {
  ASSERT(config);
  config->db_file_path = DEFAULT_DB_FILE;
  config->page_size = DEFAULT_PAGE_SIZE;
  config->cache_size_mb = DEFAULT_CACHE_SIZE_MB;
  config->port = DEFAULT_PORT;
  config->enable_wal = false;
  config->read_only = false;
  config->log_level = LOG_LEVEL_INFO;
}

bool db_config_from_args(DatabaseConfig *config, int argc, char **argv) {
  ASSERT(config);

  for (int i = 1; i < argc; ++i) {
    const char *arg = argv[i];

    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
      print_usage(argv[0]);
      return false;
    } else if (strcmp(arg, "-f") == 0 || strcmp(arg, "--file") == 0) {
      if (++i >= argc) {
        LOG_ERROR("Option %s requires a value", arg);
        return false;
      }
      config->db_file_path = argv[i];
    } else if (strcmp(arg, "-p") == 0 || strcmp(arg, "--port") == 0) {
      if (++i >= argc) {
        LOG_ERROR("Option %s requires a value", arg);
        return false;
      }
      long port = strtol(argv[i], NULL, 10);
      if (port <= 0 || port > 65535) {
        LOG_ERROR("Invalid port number: %s", argv[i]);
        return false;
      }
      config->port = (u16)port;
    } else if (strcmp(arg, "-c") == 0 || strcmp(arg, "--cache") == 0) {
      if (++i >= argc) {
        LOG_ERROR("Option %s requires a value", arg);
        return false;
      }
      long cache_mb = strtol(argv[i], NULL, 10);
      if (cache_mb <= 0 || cache_mb > 8192) {
        LOG_ERROR("Invalid cache size: %s MB", argv[i]);
        return false;
      }
      config->cache_size_mb = (u32)cache_mb;
    } else if (strcmp(arg, "-s") == 0 || strcmp(arg, "--page-size") == 0) {
      if (++i >= argc) {
        LOG_ERROR("Option %s requires a value", arg);
        return false;
      }
      long page_size = strtol(argv[i], NULL, 10);
      if (page_size < 512 || page_size > 65536 ||
          (page_size & (page_size - 1)) != 0) {
        LOG_ERROR("Page size must be a power of 2 between 512 and 65536");
        return false;
      }
      config->page_size = (u32)page_size;
    } else if (strcmp(arg, "-r") == 0 || strcmp(arg, "--read-only") == 0) {
      config->read_only = true;
    } else if (strcmp(arg, "-w") == 0 || strcmp(arg, "--wal") == 0) {
      config->enable_wal = true;
    } else if (strcmp(arg, "-v") == 0 || strcmp(arg, "--verbose") == 0) {
      config->log_level = LOG_LEVEL_DEBUG;
    } else if (strcmp(arg, "-q") == 0 || strcmp(arg, "--quiet") == 0) {
      config->log_level = LOG_LEVEL_ERROR;
    } else {
      LOG_ERROR("Unknown option: %s", arg);
      return false;
    }
  }

  return true;
}

// =================================================================================================
// :: Private Helper Functions ::
// =================================================================================================

static void print_usage(const char *program_name) {
  printf("Usage: %s [OPTIONS]\n", program_name);
  printf("\nOptions:\n");
  printf("  -f, --file <path>       Database file path (default: %s)\n",
         DEFAULT_DB_FILE);
  printf("  -p, --port <port>       Server port (default: %d)\n", DEFAULT_PORT);
  printf("  -c, --cache <MB>        Cache size in MB (default: %d)\n",
         DEFAULT_CACHE_SIZE_MB);
  printf("  -s, --page-size <size>  Page size in bytes (default: %d)\n",
         DEFAULT_PAGE_SIZE);
  printf("  -r, --read-only         Open database in read-only mode\n");
  printf("  -w, --wal               Enable Write-Ahead Logging\n");
  printf("  -v, --verbose           Enable debug logging\n");
  printf("  -q, --quiet             Enable quiet mode (errors only)\n");
  printf("  -h, --help              Show this help message\n");
  printf("\nExamples:\n");
  printf("  %s -f mydb.db -p 8080 -c 128\n", program_name);
  printf("  %s --read-only --verbose\n", program_name);
}