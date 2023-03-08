# HackBGRT

HackBGRT is intended as a boot logo changer for UEFI-based systems.

This is a fork of the original [HackBGRT](https://github.com/Metabolix/HackBGRT).

## Summary

When booting on a UEFI-based computer, many operating systems will show a
vendor-defined logo which is stored on the UEFI firmware in a section called
Boot Graphics Resource Table (BGRT). It's usually very difficult to change the
image permanently, but a custom UEFI application may be used to overwrite it
during the boot. HackBGRT does exactly that.

## Usage

**Important:** If you mess up the installation, your system may become 
unbootable! Create a rescue disk before use. This software comes with no 
warranty. Use at your own risk.

* Make sure that your computer is booting with UEFI.
* Make sure that Secure Boot is disabled, unless you know how to sign EFI applications.

### Multi-boot configurations

If you only need HackBGRT for Windows:

 * Extract the latest release to `[EFI System Partition]\EFI\HackBGRT\`.
 * Set `boot=\EFI\Microsoft\Boot\bootmgfw.efi` in `config.txt`.
 * Point your boot loader to `\EFI\HackBGRT\bootx64.efi`.

If you need it for other systems as well:

 * Extract the latest release to `[EFI System Partition]\EFI\HackBGRT\`.
 * Set `boot=\EFI\your-actual-boot-loader.efi` in `config.txt`.
 * Set `\EFI\HackBGRT\bootx64.efi` as your default boot loader with `efibootmgr` or some other EFI boot manager tool.

On 32-bit machines, use `bootia32.efi` instead of `bootx64.efi`.

## Configuration

The configuration options are described in `config.txt`, which should be 
stored in `[EFI System Partition]\EFI\HackBGRT\config.txt`.

## Images

The image path can be changed in the configuration file. The default path is 
`[EFI System Partition]\EFI\HackBGRT\splash.bmp`.

The image must be a 24-bit BMP file with a 54-byte header. That's a TrueColor 
BMP3 in Imagemagick, or 24-bit BMP/DIB in Microsoft Paint.

## Recovery

*TODO*

## Building

*TODO*
