#!/usr/bin/env bash
set -e

REPO_URL="https://github.com/bokshi-gh/http-codec.git"
LIB_NAME="http_codec"
PREFIX="/usr/local"

TMP_DIR=$(mktemp -d)

echo "==> Cloning repository..."
git clone --depth 1 "$REPO_URL" "$TMP_DIR"
cd "$TMP_DIR"

echo "==> Building $LIB_NAME..."
make

echo "==> Installing $LIB_NAME system-wide to $PREFIX..."
make install PREFIX="$PREFIX"

cd /
rm -rf "$TMP_DIR"

echo ""
echo "==> Installation complete!"
echo ""
echo "System-wide headers installed at: $PREFIX/include/$LIB_NAME"
echo "System-wide library installed at: $PREFIX/lib/lib$LIB_NAME.a"
echo ""
echo "You can now compile your projects with:"
echo "  g++ main.cpp -l$LIB_NAME -o my_app"
echo ""
echo "And include headers in your code like this:"
echo "  #include <$LIB_NAME/http_codec.hpp>"
echo ""
echo "On Linux, you may need to run 'sudo ldconfig' to update the linker cache."
