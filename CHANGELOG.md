# Change Log

## lua-cryptorandom v0.1.1

* Refactored the source code to adhere to ISO C89 rules. This turns the code compatible to a wider range of C compilers.

## lua-cryptorandom v0.1.0

* Added [CMake](https://cmake.org/) as build system;
* Added the process to report code coverage to [CodeCov](https://codecov.io/gh/luau-project/lua-cryptorandom) using `GCC` and `gcov`;
* Added docs providing guidance to publish a new release;
* Added docs to build `lua-cryptorandom` through `CMake`;
* Updated release automation (`publish.yml`) to also deploy artifacts to GitHub Releases.

## lua-cryptorandom v0.0.6

* Changed the dynamic memory allocator function provided by the system to use Lua's memory allocator function;
* In rare cases (systems such that `sizeof(unsigned char)` is not 1), the library will not use addresses out of ranges as it could happen in earlier versions;
* Added a field to inform the version of the library. See [version](#version) for more information;
* Upload of rockspec to LuaRocks website was split to live on its own workflow. This new [publish workflow](.github/workflows/publish.yml) must be run manually by the library's owner. Going this way, in case of intermitent upload failures (connection issues or temporarily unavailable services), the library's owner can run it later again without the need to run it locally on the owner's computer.

## lua-cryptorandom v0.0.5

* On Apple platforms, moved from `Security` framework to `CommonCrypto` in order to use the `builtin` rockspec build type;
* Removed Makefile that was used to build on Apple operating systems.

## lua-cryptorandom v0.0.4

* Added the possibility for all Unix-like distributions to build and install `lua-cryptorandom` using the binding for `OpenSSL`;
* Added a CI job to build and test `lua-cryptorandom` on [Cygwin](https://www.cygwin.com/);
* Now, as a Unix-like distribution, Cygwin builds as a Unix distro. Thus, in order to build on Cygwin, you need to install the package `libssl-devel`.

## lua-cryptorandom v0.0.3

* Using unions on [take](#take) to avoid alignment issues;
* Added the [Usage](#usage) section on README.

## lua-cryptorandom v0.0.2

* Prevent the generation of `NaN` and positive/negative infinity values in the function [number](#number).

## lua-cryptorandom v0.0.1

* Initial release.
