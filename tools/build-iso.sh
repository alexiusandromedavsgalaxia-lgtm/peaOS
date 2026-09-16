#!/usr/bin/env bash
set -euo pipefail

# Build a bootable x86_64 ISO for QEMU/UTM.
# Required: g++, nasm, grub-mkrescue, xorriso, node.

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

make clean
make all

test -s build/peaOS-X90.bin
test -s build/peaOS-X90.iso

echo
echo "peaOS ISO ready: $ROOT_DIR/build/peaOS-X90.iso"
echo "Use an x86_64 VM in UTM and attach this ISO as the optical drive."
