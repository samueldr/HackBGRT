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

CFLAGS += -Wall
CFLAGS += -Wshadow
CFLAGS += -Wunused
CFLAGS += -Werror-implicit-function-declaration

CFLAGS += -Werror

LDFLAGS =-T $(EFI_LDS)
LDFLAGS += -nostdlib
LDFLAGS += -znocombreloc
LDFLAGS += -shared
LDFLAGS += -nostdlib
LDFLAGS += --warn-common
LDFLAGS += --no-undefined
LDFLAGS += --fatal-warnings
LDFLAGS += --build-id=sha1
LDFLAGS += -Bsymbolic

LDFLAGS += -L $(EFILIB)
LDFLAGS += -L $(LIB)
LDFLAGS += $(EFI_CRT_OBJS)

FORMAT = --target=efi-app-$(ARCH)

ifeq ($(ARCH),x86_64)
  CFLAGS += -maccumulate-outgoing-args
  CFLAGS += -mno-red-zone
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

ifeq ($(ARCH),aarch64)
  FORMAT = -O binary

  LDFLAGS += --no-warn-rwx-segments
  LDFLAGS += -defsym=EFI_SUBSYSTEM=0xa

  CFLAGS += -DEFIAARCH64
  CFLAGS += -ffreestanding
  CFLAGS += -fno-merge-constants
  CFLAGS += -fno-stack-check
endif

GIT_DESCRIBE = $(firstword $(shell git describe --tags) unknown)
CFLAGS += '-DGIT_DESCRIBE=L"$(GIT_DESCRIBE)"'

all: $(TARGET)

$(ODIR):
	mkdir -p $(ODIR)

obj/%.o: src/%.c $(ODIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

HackBGRT.so: $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

%.efi: %.so
	$(OBJCOPY) -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel -j .rela -j .reloc \
		$(FORMAT) $^ $@
