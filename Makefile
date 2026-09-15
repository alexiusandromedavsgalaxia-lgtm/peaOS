CXX := g++
LD := ld
NASM := nasm
GRUB := grub-mkrescue
NODE := node

CXXFLAGS64 := -m64 -march=x86-64 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -fno-pie -fno-plt -fno-use-cxa-atexit -fno-builtin -fno-unwind-tables -fno-asynchronous-unwind-tables -mno-red-zone -mno-sse -mno-sse2 -O2 -Wall -Wextra -Ikernel -I. -I$(BUILD)/generated
LDFLAGS64 := -m elf_x86_64 -T arch/x86_64/linker.ld

BUILD := build
ISO := $(BUILD)/peaOS-X90.iso
KERNEL := $(BUILD)/peaOS-X90.bin
REGISTRY_HEADER := $(BUILD)/generated/certificate_registry.hpp

# Compile every kernel, hardware-driver and bundled-application translation unit.
CPP_SOURCES := arch/x86_64/kernel64.cpp $(wildcard kernel/*.cpp) $(wildcard kernel/drivers/*.cpp) $(wildcard apps/*/*.cpp)
CPP_OBJECTS := $(patsubst %.cpp,$(BUILD)/%.o,$(CPP_SOURCES))
DEPFILES := $(CPP_OBJECTS:.o=.d)

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)/arch/x86_64 $(BUILD)/kernel $(BUILD)/kernel/drivers $(BUILD)/generated

$(REGISTRY_HEADER): web-distribution/certificate-service/sync-registry.mjs | $(BUILD)
	$(NODE) $< $@

# Every kernel translation unit sees a freshly fetched authoritative registry snapshot.
$(CPP_OBJECTS): $(REGISTRY_HEADER)

$(BUILD)/arch/x86_64/boot.o: arch/x86_64/boot.asm | $(BUILD)
	mkdir -p $(dir $@)
	$(NASM) -f elf64 $< -o $@

$(BUILD)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS64) -MMD -MP -c $< -o $@

$(KERNEL): $(BUILD)/arch/x86_64/boot.o $(CPP_OBJECTS) arch/x86_64/linker.ld
	$(LD) $(LDFLAGS64) -o $@ $(BUILD)/arch/x86_64/boot.o $(CPP_OBJECTS)

$(ISO): $(KERNEL) arch/x86_64/grub.cfg
	mkdir -p $(BUILD)/iso/boot/grub
	cp $(KERNEL) $(BUILD)/iso/boot/peaOS-X90.bin
	cp arch/x86_64/grub.cfg $(BUILD)/iso/boot/grub/grub.cfg
	$(GRUB) -o $@ $(BUILD)/iso

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO)

sync-certificates:
	$(NODE) web-distribution/certificate-service/sync-registry.mjs $(REGISTRY_HEADER)

clean:
	rm -rf $(BUILD)

.PHONY: all run sync-certificates clean

-include $(DEPFILES)
