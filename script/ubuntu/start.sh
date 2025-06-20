#! /bin/bash

set -euo pipefail

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

if [ $# -lt 1 ]; then
  echo "Use: $0 <input_file> [output_file_name.png]"
  exit 1
fi

INPUT="$1"
shift 1

RAW_OUTPUT="${1:-UnnamedCrochet}"
if [[ "$RAW_OUTPUT" == *.png ]]; then
  OUTPUT="$RAW_OUTPUT"
else
  OUTPUT="${RAW_OUTPUT}.png"
fi

cat "$INPUT" | build/Compiler "$@" > temp_generated_script.py

python3 temp_generated_script.py

sudo cp awesome_crochet.png "./$OUTPUT"
sudo rm awesome_crochet.png temp_generated_script.py

echo "¡Tu Crochet está listo! Generado como: $OUTPUT"