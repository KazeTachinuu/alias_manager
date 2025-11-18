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

# Test: Init produces executable shell code for bash
INIT_BASH=$(./am init bash)
if [ $? -eq 0 ] && [ -n "$INIT_BASH" ]; then
    # Verify it's valid bash syntax
    bash -n <(echo "$INIT_BASH") 2>/dev/null
    expect_success $? "Init bash produces valid bash syntax" || true
else
    fail "Init bash produces valid bash syntax"
fi

# Test: Init produces executable shell code for zsh
INIT_ZSH=$(./am init zsh)
if [ $? -eq 0 ] && [ -n "$INIT_ZSH" ]; then
    # ZSH is bash-compatible enough for syntax check
    bash -n <(echo "$INIT_ZSH") 2>/dev/null
    expect_success $? "Init zsh produces valid syntax" || true
else
    fail "Init zsh produces valid syntax"
fi

# Test: Init produces executable shell code for fish
INIT_FISH=$(./am init fish)
if [ $? -eq 0 ] && [ -n "$INIT_FISH" ]; then
    # Just check it produces output (can't syntax check fish easily)
    pass "Init fish produces output"
else
    fail "Init fish produces output"
fi

# Test: Init with invalid shell fails
./am init not_a_real_shell >/dev/null 2>&1
expect_failure $? "Init with invalid shell fails" || true

# Test: Init code uses runtime XDG check (contains the expansion pattern)
if echo "$INIT_BASH" | grep -q '\${XDG_CONFIG_HOME:-'; then
    pass "Init uses runtime XDG_CONFIG_HOME check"
else
    fail "Init uses runtime XDG_CONFIG_HOME check"
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

# Test: Search filters aliases
./am add another "echo other" >/dev/null 2>&1
SEARCH_OUT=$(./am ls test)
if echo "$SEARCH_OUT" | grep -q "testcmd" && \
   echo "$SEARCH_OUT" | grep -qv "another"; then
    pass "Search filters by pattern"
else
    fail "Search filters by pattern"
fi

# Test: Update replaces command
./am add testcmd "echo updated" >/dev/null 2>&1
if file_contains "echo updated" && file_not_contains "echo test"; then
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
