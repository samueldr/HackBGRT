#include "util.h"

#include <efilib.h>

const CHAR16* TmpStr(CHAR8 *src, int length) {
	static CHAR16 arr[4][16];
	static int j;
	CHAR16* dest = arr[j = (j+1) % 4];
	int i;
	for (i = 0; i < length && i < 16-1 && src[i]; ++i) {
		dest[i] = src[i];
	}
	dest[i] = 0;
	return dest;
}

UINTN NullPrint(IN CONST CHAR16 *fmt, ...) {
	return 0;
}

const CHAR16* TrimLeft(const CHAR16* s) {
	// Skip white-space and BOM.
	while (s[0] == L'\xfeff' || s[0] == ' ' || s[0] == '\t') {
		++s;
	}
	return s;
}

const CHAR16* StrStr(const CHAR16* haystack, const CHAR16* needle) {
	int len = StrLen(needle);
	while (haystack && haystack[0]) {
		if (StrnCmp(haystack, needle, len) == 0) {
			return haystack;
		}
		++haystack;
	}
	return 0;
}

const CHAR16* StrStrAfter(const CHAR16* haystack, const CHAR16* needle) {
	return (haystack = StrStr(haystack, needle)) ? haystack + StrLen(needle) : 0;
}

void WaitKey(void) {
	uefi_call_wrapper(ST->ConIn->Reset, 2, ST->ConIn, FALSE);
	WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
}

EFI_INPUT_KEY ReadKey(void) {
	WaitKey();
	EFI_INPUT_KEY key = {0};
	uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
	return key;
}

void* LoadFileWithPadding(EFI_FILE_HANDLE dir, const CHAR16* path, UINTN* size_ptr, UINTN padding) {
	EFI_STATUS e;
	EFI_FILE_HANDLE handle;

	e = uefi_call_wrapper(dir->Open, 5, dir, &handle, (CHAR16*) path, EFI_FILE_MODE_READ, 0);
	if (EFI_ERROR(e)) {
		return 0;
	}

	EFI_FILE_INFO *info = LibFileInfo(handle);
	UINTN size = info->FileSize;
	FreePool(info);

	void* data = 0;
	e = uefi_call_wrapper(BS->AllocatePool, 3, EfiBootServicesData, size + padding, &data);
	if (EFI_ERROR(e)) {
		uefi_call_wrapper(handle->Close, 1, handle);
		return 0;
	}

	e = uefi_call_wrapper(handle->Read, 3, handle, &size, data);
	for (int i = 0; i < padding; ++i) {
		*((char*)data + size + i) = 0;
	}
	uefi_call_wrapper(handle->Close, 1, handle);
	if (EFI_ERROR(e)) {
		FreePool(data);
		return 0;
	}
	if (size_ptr) {
		*size_ptr = size;
	}
	return data;
}
