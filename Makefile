ARCH            = $(shell uname -m | sed s,i[3456789]86,ia32,)

TARGET          = HackBGRT.efi

_OBJS = main.o config.o types.o util.o
ODIR = obj
SDIR = src
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

EFIINC          = /usr/include/efi
EFIINCS         = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
LIB             = /usr/lib64
EFILIB          = /usr/lib64/gnuefi
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

CFLAGS          = $(EFIINCS)
CFLAGS += -std=c11
CFLAGS += -O2
CFLAGS += -fpic

CFLAGS += -fno-merge-all-constants
CFLAGS += -fno-stack-check
CFLAGS += -fno-stack-protector
CFLAGS += -fno-strict-aliasing
CFLAGS += -ffreestanding
CFLAGS += -fshort-wchar

CFLAGS += -maccumulate-outgoing-args
CFLAGS += -mno-red-zone
CFLAGS += -Wall
CFLAGS += -Wshadow
CFLAGS += -Wunused
CFLAGS += -Werror-implicit-function-declaration

CFLAGS += -Werror

ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

GIT_DESCRIBE = $(firstword $(shell git describe --tags) unknown)
CFLAGS += '-DGIT_DESCRIBE=L"$(GIT_DESCRIBE)"'

LDFLAGS = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
	-nostdlib --warn-common --no-undefined \
	--fatal-warnings --build-id=sha1 \
	-Bsymbolic -L $(EFILIB) -L $(LIB) $(EFI_CRT_OBJS)

all: $(TARGET)

$(ODIR):
	mkdir -p $(ODIR)

obj/%.o: src/%.c $(ODIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

HackBGRT.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) $^ $@
