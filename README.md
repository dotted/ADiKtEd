# Adikted Dungeon Keeper Map Editor

Note - there is now a manual available for ADiKtEd.
It even includes a basic tutorial to quickly learn the program.
A version of it is included within this distribution,
called `dk_adikted_manual.htm`. You may wish to print this out.

## Build

### Root CMake workflow

Configure from the repository root:

```sh
cmake -S . -B build
cmake --build build
cmake --install build
```

Inside an MSYS2 `MINGW32` or `MINGW64` shell, use the default `Unix Makefiles` generator and point CMake at the MinGW compiler if needed:

```sh
cmake -S . -B build-mingw32 -DCMAKE_C_COMPILER=/mingw32/bin/gcc
cmake --build build-mingw32
cmake --install build-mingw32
```

Optional switches:

- `-DADIKTED_BUILD_EXAMPLES=ON` builds the example programs.
- `-DMAPSLANG_BUILD=OFF` builds only `libadikted`.
- `-DMAPSLANG_FETCH_SLANG=ON` fetches and builds S-Lang 2.3.2 when `mapslang` cannot find a system installation. This fallback supports MinGW-based Windows builds plus Unix-like builds on Linux and macOS.

### S-Lang for `mapslang`

`mapslang` depends on Jedsoft S-Lang 2.3.2.
The CMake build first looks for a system installation and can fall back to fetching either:

- the release tarball: `https://www.jedsoft.org/releases/slang/slang-2.3.2.tar.bz2`
- the upstream Git tag: `git://git.jedsoft.org/git/slang.git` at `v2.3.2`

The fallback is mainly intended for MinGW-based Windows builds and environments without packaged S-Lang.
System S-Lang is preferred on every platform.
For non-MinGW Windows builds, either provide a compatible S-Lang installation or configure with `-DMAPSLANG_BUILD=OFF` to build only `libadikted`.

## Using `libadikted` from another CMake project

After installing `libadikted`, external CMake projects can consume the library with:

```cmake
find_package(libadikted CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE libadikted::adikted)
```

Headers are installed under `include/libadikted`, so consumers can include:

```c
#include <libadikted/adikted.h>
```

## Usage

Run `map [mapfile] [-m <logfile>] [-v] [-r] [-n] [-s [mapfile]] [-q]`

When ADiKtEd saves a map, it will ask you what you wish to call it
(unless you're not using quick save). I suggest you don't save
directly over the Dungeon Keeper original levels, but keep it
in the current directory until you're finished.
Then, at end, save it on `map00001` to access it easily in the game.

You'll need a level script for your newly created level. You may be
able to get by with the script which comes with the original level 1
- ie just copy it and paste into TXT file of your new map - but
if not, study the level scripts reference from `dk_scripting_ref.htm`.
You can also try looking at the original DK and DD levels for examples.

Press F1 for help.

## TODO before final

- Fixations in room things parameters (height,other)
- Fixations in room corner graphics

## Author

Jon Skeet, skeet@pobox.com

Dev-C++ IDE version,
rewritten most of the code:
Tomasz Lis
