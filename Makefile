CFLAGS = -Wall -O0 -ffreestanding -nostdinc -nolibc -nostdlib -nostartfiles -I.

CC_SRC = $(wildcard *.cc)
S_SRC = $(wildcard *.S)
CC_OBJS = $(CC_SRC:.cc=.co)
S_OBJS = $(S_SRC:.S=.so)
OBJS = $(CC_OBJS) $(S_OBJS)

all: clean kernel8.img

%.co: %.cc
	aarch64-linux-gnu-g++ $(CFLAGS) -c $< -o $@

%.so: %.S
	aarch64-linux-gnu-g++ $(CFLAGS) -c $< -o $@

kernel8.img: $(OBJS) Makefile
	aarch64-linux-gnu-ld -nostdlib -nostartfiles $(OBJS) -T link.ld -o kernel8.elf
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

clean:
	rm kernel8.img kernel8.elf *.co *.so || true

run: kernel8.img
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -d in_asm

