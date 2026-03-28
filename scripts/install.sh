#!/usr/bin/env bash
set -e

REPO_URL="https://github.com/bokshi-gh/http-codec.git"
LIB_NAME="http_codec"
PREFIX="/usr/local"

TMP_DIR=$(mktemp -d)

echo "Cloning repository..."
git clone --depth 1 "$REPO_URL" "$TMP_DIR"
cd "$TMP_DIR"

echo ""
echo "Building $LIB_NAME..."
make

echo ""
echo "Installing $LIB_NAME system-wide to $PREFIX..."
make install PREFIX="$PREFIX"

cd /
rm -rf "$TMP_DIR"

echo ""
echo "Installation complete!"
