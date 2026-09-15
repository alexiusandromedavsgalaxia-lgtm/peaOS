CXX := g++
LD := ld
NASM := nasm
GRUB := grub-mkrescue

CXXFLAGS := -m32 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -fno-pie -O2 -Wall -Wextra
LDFLAGS := -m elf_i386 -T linker.ld

BUILD := build
ISO := $(BUILD)/peaOS.iso
KERNEL := $(BUILD)/peaOS.bin

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: boot.asm | $(BUILD)
	$(NASM) -f elf32 $< -o $@

$(BUILD)/kernel.o: kernel.cpp | $(BUILD)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(KERNEL): $(BUILD)/boot.o $(BUILD)/kernel.o linker.ld
	$(LD) $(LDFLAGS) -o $@ $(BUILD)/boot.o $(BUILD)/kernel.o

$(ISO): $(KERNEL) grub.cfg
	mkdir -p $(BUILD)/iso/boot/grub
	cp $(KERNEL) $(BUILD)/iso/boot/peaOS.bin
	cp grub.cfg $(BUILD)/iso/boot/grub/grub.cfg
	$(GRUB) -o $@ $(BUILD)/iso

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

clean:
	rm -rf $(BUILD)

.PHONY: all run clean
