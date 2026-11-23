#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PYTHON="${PYTHON:-python3}"
RUNNER="$ROOT/tools/validation/run.py"

usage() {
  cat <<'EOF'
Usage: tools/test.sh [--graphics|--sanitizer|--performance|--all] [--milestone R0.x] [--dry-run] [--job ID]

Lightweight wrapper around tools/validation/run.py.
  (no args)         Run R0.6 mandatory synthetic validation (default)
  --graphics        Run graphics-category jobs (default milestone R0.4)
  --sanitizer       Run sanitizer-category jobs (default milestone R0.5)
  --performance     Run performance-category jobs (default milestone R0.5)
  --all             Run all milestone jobs
  --milestone R0.x  Select milestone explicitly
  --dry-run         Print selected jobs without executing
  --job ID          Run a specific job id (repeatable)
  -h, --help        Show help
EOF
}

milestone=""
categories=()
jobs=()
dry_run=0
all=0

while [[ $# -gt 0 ]]; do
  case "$1" in
    --graphics) categories+=(--category graphics); [[ -z "$milestone" ]] && milestone="R0.4" ;;
    --sanitizer) categories+=(--category sanitizer); [[ -z "$milestone" ]] && milestone="R0.5" ;;
    --performance) categories+=(--category performance); [[ -z "$milestone" ]] && milestone="R0.5" ;;
    --all) all=1; milestone="all" ;;
    --milestone) shift; milestone="${1:-}" ;;
    --dry-run) dry_run=1 ;;
    --job) shift; jobs+=(--job "${1:-}") ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown argument: $1" >&2; usage >&2; exit 2 ;;
  esac
  shift
done

if [[ $all -eq 0 && -z "$milestone" ]]; then
  milestone="R0.6"
fi

cmd=("$PYTHON" "$RUNNER" "$milestone")
if [[ ${#categories[@]} -gt 0 ]]; then cmd+=("${categories[@]}"); fi
if [[ ${#jobs[@]} -gt 0 ]]; then cmd+=("${jobs[@]}"); fi
if [[ $dry_run -eq 1 ]]; then cmd+=(--dry-run); fi

echo "+ ${cmd[*]}" >&2
exec "${cmd[@]}"
