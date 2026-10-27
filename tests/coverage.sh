#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

BUILD="$ROOT/tests/.coverage"
REPORT="$BUILD/report"

rm -rf "$BUILD"
mkdir -p "$BUILD" "$REPORT"

CC=${CC:-gcc}
GCOV=${GCOV:-}
if [ -z "$GCOV" ]; then
    case "$CC" in
        gcc-*) GCOV="gcov-${CC#gcc-}" ;;
        *) GCOV="gcov" ;;
    esac
fi
COV_FLAGS="-Wall -Wextra -std=c11 -O0 -g --coverage -Iinclude"

src_objs=()
for src in src/*.c; do
    name=$(basename "$src" .c)
    obj="$BUILD/$name.o"
    $CC $COV_FLAGS -c "$src" -o "$obj"
    src_objs+=("$obj")
done

$CC $COV_FLAGS -c tests/unit_tests.c -o "$BUILD/unit_tests.o"

test_objs=()
for obj in "${src_objs[@]}"; do
    if [ "$(basename "$obj")" != "main.o" ]; then
        test_objs+=("$obj")
    fi
done

$CC --coverage -o "$BUILD/bank_cov" "${src_objs[@]}"
$CC --coverage -o "$BUILD/unit_tests" "${test_objs[@]}" "$BUILD/unit_tests.o"

echo "== unit tests =="
"$BUILD/unit_tests"

echo
echo "== CLI coverage scenarios =="
cli_dir=$(mktemp -d "${TMPDIR:-/tmp}/bank-cli-coverage-XXXXXX")
trap 'rm -rf "$cli_dir"' EXIT
bash "$ROOT/tests/coverage_scenarios.sh" "$BUILD/bank_cov" "$cli_dir"

echo
echo "== gcov line and branch report =="
(
    cd "$BUILD"
    for src in "$ROOT"/src/*.c; do
        "$GCOV" -i -b -o . "$src" >/dev/null
    done
)

python3 "$ROOT/tests/coverage_summary.py" "$BUILD" \
    --output "$REPORT/summary.md" \
    --fail-under-lines 80 \
    --fail-under-branches 70
