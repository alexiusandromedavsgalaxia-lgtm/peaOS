CXX := g++
LD := ld
NASM := nasm
GRUB := grub-mkrescue

CXXFLAGS := -m32 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -fno-pie -fno-use-cxa-atexit -fno-builtin -O2 -Wall -Wextra -Ikernel
LDFLAGS := -m elf_i386 -T linker.ld

BUILD := build
ISO := $(BUILD)/peaOS.iso
KERNEL := $(BUILD)/peaOS.bin

CPP_SOURCES := kernel.cpp $(wildcard kernel/*.cpp)
CPP_OBJECTS := $(patsubst %.cpp,$(BUILD)/%.o,$(CPP_SOURCES))

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)/kernel

$(BUILD)/boot.o: boot.asm | $(BUILD)
	$(NASM) -f elf32 $< -o $@

$(BUILD)/%.o: %.cpp | $(BUILD)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(KERNEL): $(BUILD)/boot.o $(CPP_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(BUILD)/boot.o $(CPP_OBJECTS)

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
