let
  pkgs = import <nixpkgs> {};
  shell =
    { stdenv
    , lib
    , mkShell
    , qemu
    , OVMF
    }:

    let
      inherit (lib)
        optionalString
      ;
      archids = {
        x86_64-linux = { hostarch = "x86_64"; efiPlatform = "x64"; };
        i686-linux = rec { hostarch = "ia32"; efiPlatform = hostarch; };
        aarch64-linux = { hostarch = "aarch64"; efiPlatform = "aa64"; };
      };

      inherit
        (archids.${stdenv.hostPlatform.system} or (throw "unsupported system: ${stdenv.hostPlatform.system}"))
        hostarch efiPlatform;
    in
    mkShell {
      nativeBuildInputs = [
        qemu
      ];

      shellHook = ''
        qemu-uefi() {
          QEMU_ARGS=(
            qemu-system-${hostarch}
            -monitor none
            -m 2048
            -serial stdio
            ${{
              "aarch64" = ''
                -machine virt
                -cpu max
                -drive if=pflash,format=raw,unit=0,readonly=on,file=${OVMF.fd.firmware}
                -device virtio-gpu-pci
                -device usb-ehci,id=usb0
                -device usb-kbd
                -device usb-tablet
              '';
              "x86_64"  = ''
                -drive if=pflash,format=raw,unit=0,readonly=on,file=${OVMF.fd.firmware}
                -usb
                -device usb-tablet,bus=usb-bus.0
              '';
            }."${hostarch}"}
            "$@"
          )
          if [[ "$(uname -m)" == "${hostarch}" ]]; then
            QEMU_ARGS+=(
              -accel kvm
            )
          fi
          (
          PS4=" $ "
          set -x
          "''${QEMU_ARGS[@]}"
          )
        }
        run-result() {
          (
          set -e
          target="tmp"
          rm -rf "$target"
          mkdir -p "$target"/EFI/BOOT
          cp result/HackBGRT${efiPlatform}.efi "$target"/EFI/BOOT/BOOT${efiPlatform}.efi
          mkdir -p "$target"/EFI/HackBGRT/
          cp -t "$target"/EFI/HackBGRT/ config.txt splash.bmp
          chmod -R a+rw "$target"
          qemu-uefi -drive format=raw,file=fat:rw:"$target" "$@"
          )
        }
      '';
    }
  ;
in
  {
    "x86_64" = pkgs.callPackage shell {};
    "aarch64" = pkgs.pkgsCross.aarch64-multiplatform.callPackage shell {};
  }
