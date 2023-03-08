#include "config.h"
#include "util.h"

#include <efilib.h>

BOOLEAN ReadConfigFile(struct HackBGRT_config* config, EFI_FILE_HANDLE root_dir, const CHAR16* path) {
	void* data = 0;
	UINTN data_bytes = 0;
	data = LoadFileWithPadding(root_dir, path, &data_bytes, 4);
	if (!data) {
		Print(L"HackBGRT: Failed to load configuration (%s)!\n", path);
		return FALSE;
	}
	CHAR16* str;
	UINTN str_len;
	if (*(CHAR16*)data == 0xfeff) {
		// UCS-2
		str = data;
		str_len = data_bytes / sizeof(*str);
	} else {
		// UTF-8 -> UCS-2
		EFI_STATUS e = uefi_call_wrapper(BS->AllocatePool, 3, EfiBootServicesData, data_bytes * 2 + 2, (void**)&str);
		if (EFI_ERROR(e)) {
			FreePool(data);
			return FALSE;
		}
		UINT8* str0 = data;
		for (UINTN i = str_len = 0; i < data_bytes;) {
			UINTN unicode = 0xfffd;
			if (str0[i] < 0x80) {
				unicode = str0[i];
				i += 1;
			} else if (str0[i] < 0xc0) {
				i += 1;
			} else if (str0[i] < 0xe0) {
				unicode = ((str0[i] & 0x1f) << 6) | ((str0[i+1] & 0x3f) << 0);
				i += 2;
			} else if (str0[i] < 0xf0) {
				unicode = ((str0[i] & 0x0f) << 12) | ((str0[i+1] & 0x3f) << 6) | ((str0[i+2] & 0x3f) << 0);
				i += 3;
			} else if (str0[i] < 0xf8) {
				unicode = ((str0[i] & 0x07) << 18) | ((str0[i+1] & 0x3f) << 12) | ((str0[i+2] & 0x3f) << 6) | ((str0[i+3] & 0x3f) << 0);
				i += 4;
			} else {
				i += 1;
			}
			if (unicode <= 0xffff) {
				str[str_len++] = unicode;
			} else {
				str[str_len++] = 0xfffd;
			}
		}
		str[str_len] = 0;
		FreePool(data);
	}

	for (int i = 0; i < str_len;) {
		int j = i;
		while (j < str_len && str[j] != '\r' && str[j] != '\n') {
			++j;
		}
		while (j < str_len && (str[j] == '\r' || str[j] == '\n')) {
			str[j] = 0;
			++j;
		}
		ReadConfigLine(config, root_dir, &str[i]);
		i = j;
	}
	// NOTICE: string is not freed, because paths are not copied.
	return TRUE;
}

static void SetBMP(struct HackBGRT_config* config, enum HackBGRT_action action, int x, int y, const CHAR16* path) {
	config->action = action;
	config->image_path = path;
	config->image_x = x;
	config->image_y = y;
}

static int ParseCoordinate(const CHAR16* str, enum HackBGRT_action action) {
	if (str && L'0' <= str[0] && str[0] <= L'9') {
		return Atoi(str);
	}
	if ((str && StrnCmp(str, L"native", 6) == 0) || action == HackBGRT_KEEP) {
		return HackBGRT_coord_native;
	}
	return HackBGRT_coord_auto;
}

static void ReadConfigImage(struct HackBGRT_config* config, const CHAR16* line) {
	const CHAR16* x = StrStrAfter(line, L"x=");
	const CHAR16* y = StrStrAfter(line, L"y=");
	const CHAR16* f = StrStrAfter(line, L"path=");
	enum HackBGRT_action action = HackBGRT_KEEP;
	if (f) {
		action = HackBGRT_REPLACE;
	} else if (StrStr(line, L"remove")) {
		action = HackBGRT_REMOVE;
	} else if (StrStr(line, L"black")) {
		action = HackBGRT_REPLACE;
	} else if (StrStr(line, L"keep")) {
		action = HackBGRT_KEEP;
	} else {
		Print(L"HackBGRT: Invalid image line: %s\n", line);
		return;
	}
	SetBMP(config, action, ParseCoordinate(x, action), ParseCoordinate(y, action), f);
}

static void ReadConfigResolution(struct HackBGRT_config* config, const CHAR16* line) {
	const CHAR16* x = line;
	const CHAR16* y = StrStrAfter(line, L"x");
	if (x && *x && y && *y) {
		config->resolution_x = *x == '-' ? -(int)Atoi(x+1) : (int)Atoi(x);
		config->resolution_y = *y == '-' ? -(int)Atoi(y+1) : (int)Atoi(y);
	} else {
		Print(L"HackBGRT: Invalid resolution line: %s\n", line);
	}
}

void ReadConfigLine(struct HackBGRT_config* config, EFI_FILE_HANDLE root_dir, const CHAR16* line) {
	line = TrimLeft(line);
	if (line[0] == L'#' || line[0] == 0) {
		return;
	}

	if (StrnCmp(line, L"debug=", 6) == 0) {
		config->debug = (StrCmp(line, L"debug=1") == 0);
		return;
	}
	if (StrnCmp(line, L"image=", 6) == 0) {
		ReadConfigImage(config, line + 6);
		return;
	}
	if (StrnCmp(line, L"boot=", 5) == 0) {
		config->boot_path = line + 5;
		return;
	}
	if (StrnCmp(line, L"config=", 7) == 0) {
		ReadConfigFile(config, root_dir, line + 7);
		return;
	}
	if (StrnCmp(line, L"resolution=", 11) == 0) {
		ReadConfigResolution(config, line + 11);
		return;
	}
	Print(L"Unknown configuration directive: %s\n", line);
}
