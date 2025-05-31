# =================================================================================================
# :: SQL Database Project Makefile ::
# =================================================================================================

PROJECT_NAME := sqldb
VERSION := 0.1.0

# Directories
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
TEST_DIR := tests
TOOLS_DIR := tools
DOCS_DIR := docs

# Compiler and Tools
CC := gcc
AR := ar
STRIP := strip
VALGRIND := valgrind

# Compiler Flags
CFLAGS := -Wall -Wextra -Werror -Wconversion -Wshadow -Wstrict-prototypes -std=c11
CFLAGS += -I$(INC_DIR)
LDFLAGS :=
LIBS := -lm

# Platform-specific settings
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    LIBS += -lpthread -lrt
    CFLAGS += -D_GNU_SOURCE
endif
ifeq ($(UNAME_S),Darwin)
    LIBS += -lpthread
    CFLAGS += -D_DARWIN_C_SOURCE
endif

# Build Configuration
DEBUG_FLAGS := -g -DDEBUG -O0 -fsanitize=address -fsanitize=undefined
RELEASE_FLAGS := -O2 -DNDEBUG -flto
PROFILE_FLAGS := -pg -g -O2

# Source Files
MAIN_SRC := $(SRC_DIR)/main.c

CORE_SRCS := $(wildcard $(SRC_DIR)/core/*.c)
STORAGE_SRCS := $(wildcard $(SRC_DIR)/storage/*.c)
QUERY_SRCS := $(wildcard $(SRC_DIR)/query/*.c)
NETWORK_SRCS := $(wildcard $(SRC_DIR)/network/*.c)
UTILS_SRCS := $(wildcard $(SRC_DIR)/utils/*.c)

# Library sources
LIB_SRCS := $(CORE_SRCS) $(STORAGE_SRCS) $(QUERY_SRCS) $(NETWORK_SRCS) $(UTILS_SRCS)
ALL_SRCS := $(MAIN_SRC) $(LIB_SRCS)

# Test Files
TEST_SRCS := $(wildcard $(TEST_DIR)/unit/*.c) $(wildcard $(TEST_DIR)/integration/*.c)
TEST_RUNNER_SRC := $(TEST_DIR)/test_runner.c

# Tool Files
TOOL_SRCS := $(wildcard $(TOOLS_DIR)/*.c)

# Object Files
DEBUG_OBJS := $(ALL_SRCS:%.c=$(BUILD_DIR)/debug/%.o)
RELEASE_OBJS := $(ALL_SRCS:%.c=$(BUILD_DIR)/release/%.o)
PROFILE_OBJS := $(ALL_SRCS:%.c=$(BUILD_DIR)/profile/%.o)

LIB_DEBUG_OBJS := $(LIB_SRCS:%.c=$(BUILD_DIR)/debug/%.o)
LIB_RELEASE_OBJS := $(LIB_SRCS:%.c=$(BUILD_DIR)/release/%.o)

TEST_OBJS := $(TEST_SRCS:%.c=$(BUILD_DIR)/debug/%.o) $(TEST_RUNNER_SRC:%.c=$(BUILD_DIR)/debug/%.o)

# Output Files
DEBUG_TARGET := $(BUILD_DIR)/debug/$(PROJECT_NAME)
RELEASE_TARGET := $(BUILD_DIR)/release/$(PROJECT_NAME)
PROFILE_TARGET := $(BUILD_DIR)/profile/$(PROJECT_NAME)
TEST_TARGET := $(BUILD_DIR)/debug/test_runner

STATIC_LIB := $(BUILD_DIR)/release/lib$(PROJECT_NAME).a
SHARED_LIB := $(BUILD_DIR)/release/lib$(PROJECT_NAME).so

# =================================================================================================
# :: Main Targets ::
# =================================================================================================

.PHONY: all debug release profile test clean install uninstall help format

all: debug 

debug: CFLAGS += $(DEBUG_FLAGS)
debug: LDFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_TARGET)

release: CFLAGS += $(RELEASE_FLAGS)
release: LDFLAGS += $(RELEASE_FLAGS)
release: $(RELEASE_TARGET)

profile: CFLAGS += $(PROFILE_FLAGS)
profile: LDFLAGS += $(PROFILE_FLAGS)
profile: $(PROFILE_TARGET)

# =================================================================================================
# :: Build Rules ::
# =================================================================================================

# Debug executable
$(DEBUG_TARGET): $(DEBUG_OBJS) | $(BUILD_DIR)/debug
	@echo "Linking debug executable: $@"
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

# Release executable
$(RELEASE_TARGET): $(RELEASE_OBJS) | $(BUILD_DIR)/release
	@echo "Linking release executable: $@"
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

# Profile executable
$(PROFILE_TARGET): $(PROFILE_OBJS) | $(BUILD_DIR)/profile
	@echo "Linking profile executable: $@"
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

# Static library
$(STATIC_LIB): $(LIB_RELEASE_OBJS) | $(BUILD_DIR)/release
	@echo "Creating static library: $@"
	$(AR) rcs $@ $^

# Shared library
$(SHARED_LIB): CFLAGS += -fPIC
$(SHARED_LIB): $(LIB_RELEASE_OBJS) | $(BUILD_DIR)/release
	@echo "Creating shared library: $@"
	$(CC) -shared $(LDFLAGS) -o $@ $^ $(LIBS)

# Test runner
$(TEST_TARGET): $(TEST_OBJS) $(LIB_DEBUG_OBJS) | $(BUILD_DIR)/debug
	@echo "Linking test runner: $@"
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

# =================================================================================================
# :: Object File Rules ::
# =================================================================================================

# Debug object files
$(BUILD_DIR)/debug/%.o: %.c | $(BUILD_DIR)/debug
	@mkdir -p $(dir $@)
	@echo "Compiling (debug): $<"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Release object files
$(BUILD_DIR)/release/%.o: %.c | $(BUILD_DIR)/release
	@mkdir -p $(dir $@)
	@echo "Compiling (release): $<"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# Profile object files
$(BUILD_DIR)/profile/%.o: %.c | $(BUILD_DIR)/profile
	@mkdir -p $(dir $@)
	@echo "Compiling (profile): $<"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

# =================================================================================================
# :: Directory Creation ::
# =================================================================================================

$(BUILD_DIR)/debug:
	@mkdir -p $@/src/core $@/src/storage $@/src/query $@/src/network $@/src/utils
	@mkdir -p $@/tests/unit $@/tests/integration

$(BUILD_DIR)/release:
	@mkdir -p $@/src/core $@/src/storage $@/src/query $@/src/network $@/src/utils

$(BUILD_DIR)/profile:
	@mkdir -p $@/src/core $@/src/storage $@/src/query $@/src/network $@/src/utils

# =================================================================================================
# :: Testing ::
# =================================================================================================

test: $(TEST_TARGET)
	@echo "Running tests..."
	./$(TEST_TARGET)

test-valgrind: $(TEST_TARGET)
	@echo "Running tests with Valgrind..."
	$(VALGRIND) --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_TARGET)

# =================================================================================================
# :: Tools and Utilities ::
# =================================================================================================

# Build all tools
tools: $(TOOL_SRCS:$(TOOLS_DIR)/%.c=$(BUILD_DIR)/release/tools/%)

$(BUILD_DIR)/release/tools/%: $(TOOLS_DIR)/%.c $(LIB_RELEASE_OBJS) | $(BUILD_DIR)/release
	@mkdir -p $(dir $@)
	@echo "Building tool: $@"
	$(CC) $(CFLAGS) $(RELEASE_FLAGS) $(CPPFLAGS) -o $@ $< $(LIB_RELEASE_OBJS) $(LIBS)

# Code formatting
format:
	@echo "Formatting source code..."
	@find $(SRC_DIR) $(INC_DIR) $(TEST_DIR) $(TOOLS_DIR) -name "*.c" -o -name "*.h" | \
		xargs clang-format -i -style=file

# Static analysis
analyze:
	@echo "Running static analysis..."
	@scan-build make debug

# Generate documentation
docs:
	@echo "Generating documentation..."
	@doxygen Doxyfile 2>/dev/null || echo "Doxygen not found, skipping documentation generation"

# =================================================================================================
# :: Installation ::
# =================================================================================================

PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin
LIBDIR := $(PREFIX)/lib
INCDIR := $(PREFIX)/include

install: release $(STATIC_LIB)
	@echo "Installing $(PROJECT_NAME)..."
	@install -d $(DESTDIR)$(BINDIR)
	@install -d $(DESTDIR)$(LIBDIR)
	@install -d $(DESTDIR)$(INCDIR)
	@install -m 755 $(RELEASE_TARGET) $(DESTDIR)$(BINDIR)/
	@install -m 644 $(STATIC_LIB) $(DESTDIR)$(LIBDIR)/
	@cp -r $(INC_DIR)/* $(DESTDIR)$(INCDIR)/
	@echo "Installation complete"

uninstall:
	@echo "Uninstalling $(PROJECT_NAME)..."
	@rm -f $(DESTDIR)$(BINDIR)/$(PROJECT_NAME)
	@rm -f $(DESTDIR)$(LIBDIR)/lib$(PROJECT_NAME).a
	@rm -rf $(DESTDIR)$(INCDIR)/sqldb
	@rm -f $(DESTDIR)$(INCDIR)/base.h
	@echo "Uninstallation complete"

# =================================================================================================
# :: Cleanup ::
# =================================================================================================

clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)

distclean: clean
	@echo "Cleaning all generated files..."
	@rm -rf $(DOCS_DIR)/html $(DOCS_DIR)/latex

# =================================================================================================
# :: Information ::
# =================================================================================================

help:
	@echo "$(PROJECT_NAME) v$(VERSION) - SQL Database Project"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build release version (default)"
	@echo "  debug     - Build debug version with sanitizers"
	@echo "  release   - Build optimized release version"
	@echo "  profile   - Build version for profiling"
	@echo "  test      - Build and run tests"
	@echo "  tools     - Build utility tools"
	@echo "  format    - Format source code"
	@echo "  analyze   - Run static analysis"
	@echo "  docs      - Generate documentation"
	@echo "  install   - Install to system (PREFIX=$(PREFIX))"
	@echo "  uninstall - Remove from system"
	@echo "  clean     - Remove build files"
	@echo "  distclean - Remove all generated files"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make debug"
	@echo "  make test"
	@echo "  make install PREFIX=/opt/sqldb"

# Show configuration
info:
	@echo "Build Configuration:"
	@echo "  CC:        $(CC)"
	@echo "  CFLAGS:    $(CFLAGS)"
	@echo "  LDFLAGS:   $(LDFLAGS)"
	@echo "  LIBS:      $(LIBS)"
	@echo "  Platform:  $(UNAME_S)"
	@echo "  Sources:   $(words $(ALL_SRCS)) files"

# =================================================================================================
# :: Dependencies ::
# =================================================================================================

# Generate dependency files
DEPS := $(ALL_SRCS:%.c=$(BUILD_DIR)/%.d) $(TEST_SRCS:%.c=$(BUILD_DIR)/%.d)

$(BUILD_DIR)/%.d: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(CPPFLAGS) -MM -MT $(@:%.d=%.o) $< > $@

# Include dependency files if they exist
-include $(DEPS)