let
  pkgs = import <nixpkgs> {};
in
{
  "x86_64" = pkgs.callPackage ./hackbgrt.nix {};
  "aarch64" = pkgs.pkgsCross.aarch64-multiplatform.callPackage ./hackbgrt.nix {};
}
