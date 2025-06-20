#! /bin/bash

set -euxo pipefail

BASE_PATH="$(dirname "$0")/../.."
cd "$BASE_PATH"

sudo apt-get install bison --yes
sudo apt-get install cmake --yes
sudo apt-get install flex --yes
sudo apt-get install gcc --yes
sudo apt-get install make --yes
sudo apt-get update --yes
sudo apt-get upgrade --yes
sudo apt-get install python3 python3-pip --yes
sudo apt install python3-pillow

echo "All done."
