#!/bin/bash
# Test suite for Alias Manager
# Philosophy: Test behavior, not messages. Test contracts, not implementation.

set -o pipefail

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

# Test counter
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

# Temporary files for test isolation
TEST_DIR="/tmp/am_test_$$"
TEST_ALIAS_FILE="$TEST_DIR/aliases.txt"
export AM_ALIAS_FILE="$TEST_ALIAS_FILE"

# Cleanup function
cleanup() {
    rm -rf "$TEST_DIR"
}
trap cleanup EXIT

# Setup
setup() {
    mkdir -p "$TEST_DIR"
}

# Test helpers
pass() {
    echo -e "${GREEN}✓${NC} $1"
    TESTS_PASSED=$((TESTS_PASSED + 1))
    TESTS_RUN=$((TESTS_RUN + 1))
}

fail() {
    echo -e "${RED}✗${NC} $1"
    TESTS_FAILED=$((TESTS_FAILED + 1))
    TESTS_RUN=$((TESTS_RUN + 1))
}

# Check exit code
expect_success() {
    if [ $1 -eq 0 ]; then
        pass "$2"
        return 0
    else
        fail "$2 (exit $1)"
        return 1
    fi
}

expect_failure() {
    if [ $1 -ne 0 ]; then
        pass "$2"
        return 0
    else
        fail "$2 (should have failed)"
        return 1
    fi
}

# File content checks
file_contains() {
    grep -qF "$1" "$TEST_ALIAS_FILE" 2>/dev/null
}

file_not_contains() {
    ! grep -qF "$1" "$TEST_ALIAS_FILE" 2>/dev/null
}

echo "Running Alias Manager Tests..."
echo

# Build binary
if [ ! -f ./am ]; then
    echo "Building binary..."
    make clean && make || exit 1
fi

setup

echo "=== Basic Command Interface ==="

# Test: Help command succeeds
./am help >/dev/null 2>&1
expect_success $? "Help command succeeds" || true

# Test: Version command succeeds
./am version >/dev/null 2>&1
expect_success $? "Version command succeeds" || true

# Test: No arguments is not an error
./am >/dev/null 2>&1
expect_success $? "No arguments shows help" || true

# Test: Invalid command fails
./am this_command_does_not_exist >/dev/null 2>&1
expect_failure $? "Invalid command fails" || true

echo
echo "=== Init Command Behavior ==="

# Test: Init --hook produces executable shell code for bash
INIT_BASH=$(./am init --hook bash)
if [ $? -eq 0 ] && [ -n "$INIT_BASH" ]; then
    # Verify it's valid bash syntax
    bash -n <(echo "$INIT_BASH") 2>/dev/null
    expect_success $? "Init --hook bash produces valid bash syntax" || true
else
    fail "Init --hook bash produces valid bash syntax"
fi

# Test: Init --hook produces executable shell code for zsh
INIT_ZSH=$(./am init --hook zsh)
if [ $? -eq 0 ] && [ -n "$INIT_ZSH" ]; then
    # ZSH is bash-compatible enough for syntax check
    bash -n <(echo "$INIT_ZSH") 2>/dev/null
    expect_success $? "Init --hook zsh produces valid syntax" || true
else
    fail "Init --hook zsh produces valid syntax"
fi

# Test: Init --hook produces executable shell code for fish
INIT_FISH=$(./am init --hook fish)
if [ $? -eq 0 ] && [ -n "$INIT_FISH" ]; then
    # Just check it produces output (can't syntax check fish easily)
    pass "Init --hook fish produces output"
else
    fail "Init --hook fish produces output"
fi

# Test: Init with invalid shell fails
./am init --hook not_a_real_shell >/dev/null 2>&1
expect_failure $? "Init --hook with invalid shell fails" || true

# Test: Init code uses runtime XDG check (contains the expansion pattern)
if echo "$INIT_BASH" | grep -q '\${XDG_CONFIG_HOME:-'; then
    pass "Init --hook uses runtime XDG_CONFIG_HOME check"
else
    fail "Init --hook uses runtime XDG_CONFIG_HOME check"
fi

echo
echo "=== Init File Creation ==="

# Test: Init creates alias file and directory
INIT_TEST_DIR="/tmp/am_init_test_$$"
INIT_ALIAS_FILE="$INIT_TEST_DIR/custom/nested/path/aliases.txt"
mkdir -p "$INIT_TEST_DIR"

# Run init with custom path (cancel interactive part)
HOME="$INIT_TEST_DIR" AM_ALIAS_FILE="$INIT_ALIAS_FILE" bash -c 'echo n | ./am init bash' >/dev/null 2>&1

if [ -d "$(dirname "$INIT_ALIAS_FILE")" ]; then
    pass "Init creates directory structure"
else
    fail "Init creates directory structure"
fi

if [ -f "$INIT_ALIAS_FILE" ]; then
    pass "Init creates alias file"
else
    fail "Init creates alias file"
fi

# Test: Init file has proper permissions
if [ -r "$INIT_ALIAS_FILE" ] && [ -w "$INIT_ALIAS_FILE" ]; then
    pass "Init file has correct permissions"
else
    fail "Init file has correct permissions"
fi

# Test: Init file contains header comments
if grep -q "# Alias Manager" "$INIT_ALIAS_FILE" 2>/dev/null; then
    pass "Init file contains header"
else
    fail "Init file contains header"
fi

# Test: Init is idempotent (running twice doesn't change file contents)
ORIG_CONTENT=$(cat "$INIT_ALIAS_FILE" 2>/dev/null)
HOME="$INIT_TEST_DIR" AM_ALIAS_FILE="$INIT_ALIAS_FILE" bash -c 'echo n | ./am init bash' >/dev/null 2>&1
NEW_CONTENT=$(cat "$INIT_ALIAS_FILE" 2>/dev/null)

if [ "$ORIG_CONTENT" = "$NEW_CONTENT" ]; then
    pass "Init is idempotent"
else
    fail "Init is idempotent"
fi

# Test: Init file works with am add
HOME="$INIT_TEST_DIR" AM_ALIAS_FILE="$INIT_ALIAS_FILE" ./am add inittest "echo test" >/dev/null 2>&1
if grep -q "alias inittest='echo test'" "$INIT_ALIAS_FILE" 2>/dev/null; then
    pass "Init file integrates with am add"
else
    fail "Init file integrates with am add"
fi

# Cleanup init test
rm -rf "$INIT_TEST_DIR"

echo
echo "=== Reload Command Behavior ==="

# Test: Reload produces executable shell code for bash
RELOAD_BASH=$(./am reload bash)
if [ $? -eq 0 ] && [ -n "$RELOAD_BASH" ]; then
    bash -n <(echo "$RELOAD_BASH") 2>/dev/null
    expect_success $? "Reload bash produces valid bash syntax" || true
else
    fail "Reload bash produces valid bash syntax"
fi

# Test: Reload produces executable shell code for zsh
RELOAD_ZSH=$(./am reload zsh)
if [ $? -eq 0 ] && [ -n "$RELOAD_ZSH" ]; then
    bash -n <(echo "$RELOAD_ZSH") 2>/dev/null
    expect_success $? "Reload zsh produces valid syntax" || true
else
    fail "Reload zsh produces valid syntax"
fi

# Test: Reload produces executable shell code for fish
RELOAD_FISH=$(./am reload fish)
if [ $? -eq 0 ] && [ -n "$RELOAD_FISH" ]; then
    pass "Reload fish produces output"
else
    fail "Reload fish produces output"
fi

# Test: Reload with invalid shell fails
./am reload not_a_real_shell >/dev/null 2>&1
expect_failure $? "Reload with invalid shell fails" || true

echo
echo "=== List Command Tests ==="

# Test: List with no alias file shows helpful message and path
rm -f "$TEST_ALIAS_FILE"
NO_FILE_OUT=$(./am ls 2>&1)
if [ $? -eq 0 ] && echo "$NO_FILE_OUT" | grep -qi "no aliases yet" && \
   echo "$NO_FILE_OUT" | grep -q "File:"; then
    pass "List with no file shows helpful message and path"
else
    fail "List with no file shows helpful message and path"
fi

# Test: List with empty file
touch "$TEST_ALIAS_FILE"
LIST_EMPTY=$(./am ls 2>&1)
if [ $? -eq 0 ] && [ -z "$LIST_EMPTY" ]; then
    pass "List with empty file shows nothing"
else
    fail "List with empty file shows nothing"
fi

# Test: Filter with no matches shows helpful message
./am add something "echo test" >/dev/null 2>&1
NO_MATCH=$(./am ls nonexistent 2>&1)
if echo "$NO_MATCH" | grep -qi "no aliases found"; then
    pass "Filter with no matches shows helpful message"
else
    fail "Filter with no matches shows helpful message"
fi

echo
echo "=== Core Functionality ==="

# Test: Add alias creates file
./am add testcmd "echo test" >/dev/null 2>&1
if [ $? -eq 0 ] && [ -f "$TEST_ALIAS_FILE" ]; then
    pass "Add creates alias file"
else
    fail "Add creates alias file"
fi

# Test: Alias file contains proper shell syntax
if file_contains "alias testcmd=" && file_contains "echo test"; then
    pass "Alias stored with correct syntax"
else
    fail "Alias stored with correct syntax"
fi

# Test: Generated alias is valid bash syntax
bash -n "$TEST_ALIAS_FILE" 2>/dev/null
expect_success $? "Alias file is valid bash syntax" || true

# Test: List shows added alias
LIST_OUT=$(./am ls)
if [ $? -eq 0 ] && echo "$LIST_OUT" | grep -q "testcmd"; then
    pass "List shows added alias"
else
    fail "List shows added alias"
fi

# Test: List output format is correct (name='command')
if echo "$LIST_OUT" | grep -qE "^testcmd='echo test'$"; then
    pass "List output format is correct"
else
    fail "List output format is correct"
fi

# Test: Search filters aliases by name
./am add another "echo other" >/dev/null 2>&1
SEARCH_OUT=$(./am ls test)
if echo "$SEARCH_OUT" | grep -q "testcmd" && \
   echo "$SEARCH_OUT" | grep -qv "another"; then
    pass "Search filters by alias name"
else
    fail "Search filters by alias name"
fi

# Test: Search matches command content too
./am add myalias "echo testword" >/dev/null 2>&1
SEARCH_CMD=$(./am ls testword)
if echo "$SEARCH_CMD" | grep -q "myalias"; then
    pass "Search matches command content"
else
    fail "Search matches command content"
fi

# Test: Search is case-sensitive
./am add UPPERCASE "echo caps" >/dev/null 2>&1
SEARCH_CASE=$(./am ls upper)
if echo "$SEARCH_CASE" | grep -qv "UPPERCASE"; then
    pass "Search is case-sensitive"
else
    fail "Search is case-sensitive"
fi

# Test: Update replaces command
./am add testcmd "echo updated" >/dev/null 2>&1
if grep -q "^alias testcmd='echo updated'$" "$TEST_ALIAS_FILE" 2>/dev/null && \
   ! grep -q "^alias testcmd='echo test'$" "$TEST_ALIAS_FILE" 2>/dev/null; then
    pass "Update replaces old command"
else
    fail "Update replaces old command"
fi

# Test: Remove deletes alias
./am rm testcmd -f >/dev/null 2>&1
if file_not_contains "testcmd"; then
    pass "Remove deletes alias"
else
    fail "Remove deletes alias"
fi

echo
echo "=== Edge Cases ==="

# Test: Complex commands with pipes and quotes
./am add complex "ps aux | grep -v grep | awk '{print \$1}'" >/dev/null 2>&1
if [ $? -eq 0 ] && file_contains "complex"; then
    # Verify it's still valid shell syntax
    bash -n "$TEST_ALIAS_FILE" 2>/dev/null
    expect_success $? "Complex command stored correctly" || true
else
    fail "Complex command stored correctly"
fi

# Test: Alias with quotes in command
./am add quoted "echo 'hello world'" >/dev/null 2>&1
expect_success $? "Quotes in command handled" || true
bash -n "$TEST_ALIAS_FILE" 2>/dev/null
expect_success $? "Quoted command is valid syntax" || true

# Test: Add without arguments fails
./am add >/dev/null 2>&1
expect_failure $? "Add without arguments fails" || true

# Test: Add with only name fails
./am add nameonly >/dev/null 2>&1
expect_failure $? "Add with only name fails" || true

# Test: Remove without arguments fails
./am rm >/dev/null 2>&1
expect_failure $? "Remove without arguments fails" || true

# Test: Remove non-existent doesn't fail (graceful)
./am rm doesnotexist >/dev/null 2>&1
expect_success $? "Remove non-existent is graceful" || true

echo
echo "=== File Format Validation ==="

# Test: Every line starts with 'alias '
if grep -v "^alias " "$TEST_ALIAS_FILE" | grep -q .; then
    fail "All lines start with 'alias '"
else
    pass "All lines start with 'alias '"
fi

# Test: Sourcing the file doesn't cause errors
(source "$TEST_ALIAS_FILE" 2>/dev/null)
expect_success $? "Alias file can be sourced" || true

# Test: Alias actually works when sourced
# Note: We just test that the alias is defined after sourcing
if (bash -c "source '$TEST_ALIAS_FILE' && alias complex" >/dev/null 2>&1); then
    pass "Sourced aliases are functional"
else
    fail "Sourced aliases are functional"
fi

echo
echo "=== Path Command ==="

# Test: Path command returns the configured path
PATH_OUT=$(./am path)
if [ $? -eq 0 ] && [ "$PATH_OUT" = "$TEST_ALIAS_FILE" ]; then
    pass "Path command returns AM_ALIAS_FILE path"
else
    fail "Path command returns AM_ALIAS_FILE path"
fi

# Test: Path command with custom file
CUSTOM_PATH_OUT=$(AM_ALIAS_FILE="$TEST_DIR/custom.txt" ./am path)
if [ $? -eq 0 ] && [ "$CUSTOM_PATH_OUT" = "$TEST_DIR/custom.txt" ]; then
    pass "Path command respects AM_ALIAS_FILE"
else
    fail "Path command respects AM_ALIAS_FILE"
fi

echo
echo "=== Custom Configuration ==="

# Test: Custom file path via environment
CUSTOM_FILE="$TEST_DIR/custom.txt"
AM_ALIAS_FILE="$CUSTOM_FILE" ./am add custom "echo test" >/dev/null 2>&1
if [ -f "$CUSTOM_FILE" ] && grep -q "custom" "$CUSTOM_FILE"; then
    pass "Custom file path works"
else
    fail "Custom file path works"
fi

echo
echo "=== Integration Test ==="

# Test: Full workflow - add multiple, list, remove, verify
rm -f "$TEST_ALIAS_FILE"
./am add a1 "echo 1" >/dev/null 2>&1
./am add a2 "echo 2" >/dev/null 2>&1
./am add a3 "echo 3" >/dev/null 2>&1

LIST_ALL=$(./am ls)
if echo "$LIST_ALL" | grep -q "a1" && \
   echo "$LIST_ALL" | grep -q "a2" && \
   echo "$LIST_ALL" | grep -q "a3"; then
    pass "Multiple aliases managed correctly"
else
    fail "Multiple aliases managed correctly"
fi

./am rm a2 -f >/dev/null 2>&1
if file_contains "a1" && file_not_contains "a2" && file_contains "a3"; then
    pass "Selective removal works"
else
    fail "Selective removal works"
fi

# Final syntax check
bash -n "$TEST_ALIAS_FILE" 2>/dev/null
expect_success $? "Final file is valid after operations" || true

echo
echo "================================"
echo "Tests: $TESTS_PASSED/$TESTS_RUN passed, $TESTS_FAILED failed"
echo "================================"

if [ $TESTS_PASSED -eq $TESTS_RUN ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    exit 0
else
    echo -e "${RED}Some tests failed${NC}"
    exit 1
fi
