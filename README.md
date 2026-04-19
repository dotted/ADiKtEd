# ADiKtEd

ADiKtEd is a Dungeon Keeper map editing project that contains both the classic terminal-style editor and a reusable C library for reading, modifying, and writing map data.

This repository currently builds:

- `libadikted`, a C library with installed headers, CMake package metadata, and pkg-config metadata
- `map`, the `mapslang`-based ADiKtEd editor frontend
- optional SDL-based example programs that demonstrate library usage

## What This Repo Contains

- [`libadikted/`](libadikted/) contains the core map editing library and installed public headers
- [`mapslang/`](mapslang/) contains the `map` executable, a text UI frontend built on S-Lang
- [`examples/`](examples/) contains optional SDL-based sample programs such as `putgems`, `puttrain`, `viewmap`, and `putemple`
- [`docs/`](docs/) contains manuals and reference material for ADiKtEd and Dungeon Keeper level scripting
- [`cmake/`](cmake/) contains packaging helpers and dependency logic, including S-Lang resolution for `mapslang`

## Build

Configure from the repository root:

```sh
cmake -S . -B build
cmake --build build
cmake --install build
```

Important CMake options:

- `-DMAPSLANG_BUILD=OFF` builds only `libadikted`
- `-DADIKTED_BUILD_EXAMPLES=ON` builds the SDL example programs
- `-DMAPSLANG_FETCH_SLANG=ON` allows CMake to fetch and build S-Lang 2.3.2 when `mapslang` cannot find a compatible installation
- `-DMAPSLANG_SLANG_SOURCE=release` or `git` selects the source used by the S-Lang fallback

By default, the project installs into `build/install` unless `CMAKE_INSTALL_PREFIX` is set explicitly.

## Dependencies

Required:

- a C99-capable compiler
- CMake 3.15 or newer
- S-Lang 2.3.2 for the `map` editor, unless you disable `MAPSLANG_BUILD`

Optional:

- SDL for the example programs enabled with `ADIKTED_BUILD_EXAMPLES`

The build first tries to find a system S-Lang installation for `mapslang`. When enabled, the fallback fetch path is mainly intended for MinGW-based Windows builds and for Unix-like environments that do not package S-Lang.

## Install And Use `libadikted`

After installation, external CMake projects can consume the library with:

```cmake
find_package(libadikted CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE libadikted::adikted)
```

Headers are installed under `include/libadikted`, so consumers can include:

```c
#include <libadikted/adikted.h>
```

The install also provides pkg-config metadata as `libadikted.pc`.

## Using ADiKtEd

The editor frontend built from [`mapslang/`](mapslang/) installs as `map`.

The historical command-line interface follows the form:

```sh
map [mapfile] [options]
```

The detailed editor workflow, keyboard help, map installation guidance, and scripting background are better covered by the bundled manuals than by the top-level README. If you are approaching ADiKtEd as an end user rather than a library consumer, start with the editor manual and installation guide linked below.

## Examples

The example programs are optional and are not built by default. They are primarily useful as small integration samples for `libadikted`.

- `putgems` loads a level and changes slabs
- `puttrain` demonstrates editing a room area and using the internal message system
- `viewmap` renders maps graphically with SDL
- `putemple` demonstrates interactive drawing and fast rendering routines

These targets require SDL and are enabled with `-DADIKTED_BUILD_EXAMPLES=ON`.

## Documentation

Bundled local references:

- [`docs/dk_adikted_manual.htm`](docs/dk_adikted_manual.htm) for the ADiKtEd editor manual and quick start
- [`docs/dk_editor_hdinst.htm`](docs/dk_editor_hdinst.htm) for installation and playing edited maps
- [`docs/dk_scripting_ref.htm`](docs/dk_scripting_ref.htm) for Dungeon Keeper level scripting reference

Additional external reference:

- [Dungeon Keeper documentation mirror](https://keeper.lubiki.pl/dk1_docs/)

The external mirror appears to include fan-maintained and expanded documentation, especially around scripting and editor usage. It is a helpful cross-reference when the bundled docs look incomplete or stale, but the repository build files remain authoritative for how this project is configured and packaged today.

## History And Maintainers

- Original creator: [Jon Skeet](https://jonskeet.uk/dk/)
- Former maintainer: [mefistotelis](https://github.com/mefistotelis)
- Current maintainers: [Dungeon Keeper Fans and Contributors](https://github.com/dkfans)

This repository preserves a long-lived community toolchain around Dungeon Keeper map editing while also exposing the reusable `libadikted` library for automation and integration work.

## License

ADiKtEd is distributed under the terms included in [`LICENSE`](LICENSE).
