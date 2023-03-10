{ stdenv
, fetchFromGitHub
, binutils-unwrapped
, gnu-efi
}:

let
  archids = {
    x86_64-linux = { hostarch = "x86_64"; efiPlatform = "x64"; };
    i686-linux = rec { hostarch = "ia32"; efiPlatform = hostarch; };
    aarch64-linux = { hostarch = "aarch64"; efiPlatform = "aa64"; };
  };

  inherit
    (archids.${stdenv.hostPlatform.system} or (throw "unsupported system: ${stdenv.hostPlatform.system}"))
    hostarch efiPlatform;
in

stdenv.mkDerivation rec {
  pname = "hackbgrt";
  version = "wip";

  src = builtins.fetchGit ./.;

  buildInputs = [
    gnu-efi
  ];

  makeFlags = [
    "EFIINC=${gnu-efi}/include/efi"
    "EFILIB=${gnu-efi}/lib"
    "GNUEFILIB=${gnu-efi}/lib"
    "EFICRT0=${gnu-efi}/lib"
    "HOSTARCH=${hostarch}"
    "ARCH=${hostarch}"
  ];

  installPhase = ''
    runHook preInstall

    mkdir -p $out/
    cp HackBGRT.efi $out/HackBGRT${efiPlatform}.efi

    runHook postInstall
  '';

  hardeningDisable = [ "all" ];
}
