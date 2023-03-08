#pragma once

#include <efi.h>

/**
 * Convert a short ASCII string to UCS2, store in a static array.
 *
 * @param src The ASCII string. Will be truncated to 15 characters + null.
 * @param length The maximum length, if the string is not null-terminated.
 * @return The UCS2 string, statically allocated, null-terminated.
 */
extern const CHAR16* TmpStr(CHAR8 *src, int length);

/**
 * Empty function that has the same signature as Print.
 */
extern UINTN NullPrint(IN CONST CHAR16 *fmt, ...);

/**
 * Return the greater of two numbers.
 */
static inline int max(int a, int b) {
	return a > b ? a : b;
}

/**
 * Return the smaller of two numbers.
 */
static inline int min(int a, int b) {
	return a < b ? a : b;
}

/**
 * Trim BOM, spaces and tabs from the beginning of a string.
 *
 * @param s The string.
 * @return Pointer to the first acceptable character.
 */
extern const CHAR16* TrimLeft(const CHAR16* s);

/**
 * Find the position of another string within a string.
 *
 * @param haystack The full text.
 * @param needle The string to look for.
 * @return Pointer to the first occurence of needle in the haystack, or 0.
 */
extern const CHAR16* StrStr(const CHAR16* haystack, const CHAR16* needle);


/**
 * Find the position after another string within a string.
 *
 * @param haystack The full text.
 * @param needle The string to look for.
 * @return Pointer after the first occurence of needle in the haystack, or 0.
 */
extern const CHAR16* StrStrAfter(const CHAR16* haystack, const CHAR16* needle);

/**
 * Rotate left a 64-bit value.
 */
static inline UINT64 rotl(const UINT64 x, int k) {
	return (x << k) | (x >> (64 - k));
}

/**
 * Wait for a key press. It will still remain in the buffer.
 */
extern void WaitKey(void);

/**
 * Wait for a key press and read it.
 *
 * @return The pressed key.
 */
extern EFI_INPUT_KEY ReadKey(void);

/**
 * Load a file, allocate some extra bytes as well.
 */
extern void* LoadFileWithPadding(EFI_FILE_HANDLE dir, const CHAR16* path, UINTN* size_ptr, UINTN padding);

/**
 * Load a file.
 */
static inline void* LoadFile(EFI_FILE_HANDLE dir, const CHAR16* path, UINTN* size_ptr) {
	return LoadFileWithPadding(dir, path, size_ptr, 0);
}

