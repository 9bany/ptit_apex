#!/usr/bin/env bash
# Build Apex banking system
set -e

CXX="${CXX:-clang++}"
CXXFLAGS="-std=c++17 -Iinclude -pthread -O2 -Wall -Wextra"
SOURCES=$(find src -name '*.cpp' | tr '\n' ' ')

echo "[*] Building Apex..."
$CXX $CXXFLAGS $SOURCES -o apex
echo "[+] Done: ./apex"
echo ""
echo "Usage:"
echo "  ./apex           — interactive mode"
echo "  ./apex --demo    — pre-load sample data then open menu"
