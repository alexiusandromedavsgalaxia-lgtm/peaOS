#!/usr/bin/env bash
set -euo pipefail

# Temporary, realistic QEMU hardware profiles for peaOS driver bring-up.
# Devices exist only for the lifetime of the QEMU process.

PROFILE="${1:-all}"
ISO="${2:-build/peaOS-X90.iso}"
QEMU_DISPLAY="${QEMU_DISPLAY:-gtk}"

if [[ ! -f "$ISO" ]]; then
  echo "ISO not found: $ISO" >&2
  echo "Run: make" >&2
  exit 1
fi

COMMON=(
  -machine q35,accel=tcg
  -cpu max
  -smp 2
  -m 2048
  -boot order=d
  -drive file="$ISO",media=cdrom,readonly=on
  -display "$QEMU_DISPLAY"
  -serial stdio
)

run_qemu() {
  exec qemu-system-x86_64 "${COMMON[@]}" "$@"
}

case "$PROFILE" in
  rtl8139)
    run_qemu -device rtl8139,netdev=net0 -netdev user,id=net0,ipv4=on,ipv6=on
    ;;
  e1000)
    run_qemu -device e1000,netdev=net0 -netdev user,id=net0,ipv4=on,ipv6=on
    ;;
  virtio-net)
    run_qemu -device virtio-net-pci,netdev=net0 -netdev user,id=net0,ipv4=on,ipv6=on
    ;;
  usb-xhci)
    run_qemu -device qemu-xhci,id=xhci -device usb-kbd,bus=xhci.0 -device usb-tablet,bus=xhci.0
    ;;
  all)
    run_qemu \
      -device rtl8139,netdev=net0 \
      -device e1000,netdev=net1 \
      -device virtio-net-pci,netdev=net2 \
      -netdev user,id=net0,ipv4=on,ipv6=on \
      -netdev user,id=net1,ipv4=on,ipv6=on \
      -netdev user,id=net2,ipv4=on,ipv6=on \
      -device qemu-xhci,id=xhci \
      -device usb-kbd,bus=xhci.0 \
      -device usb-tablet,bus=xhci.0
    ;;
  matrix)
    echo "peaOS driver matrix: each profile opens QEMU; close it to continue."
    for profile in rtl8139 e1000 virtio-net usb-xhci; do
      echo "=== temporary QEMU hardware: $profile ==="
      "$0" "$profile" "$ISO"
    done
    ;;
  *)
    echo "Usage: $0 {rtl8139|e1000|virtio-net|usb-xhci|all|matrix} [iso]" >&2
    exit 2
    ;;
esac
