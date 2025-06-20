#! /bin/bash

set -euo pipefail

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

if [ $# -lt 1 ]; then
  echo "Use: $0 <input_file> [output_file_name.py]"
  exit 1
fi

INPUT="$1"
shift 1

OUTPUT="${1:-UnnamedCrochet.py}"

cat "$INPUT" | build/Compiler "$@" > "$OUTPUT"

python3 "$OUTPUT"