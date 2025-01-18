# lua-cryptorandom

[![CI](https://github.com/luau-project/lua-cryptorandom/actions/workflows/ci.yml/badge.svg)](./.github/workflows/ci.yml) [![LuaRocks](https://img.shields.io/luarocks/v/luau-project/lua-cryptorandom?label=LuaRocks&color=2c3e67)](https://luarocks.org/modules/luau-project/lua-cryptorandom)

## Overview

**lua-cryptorandom** is a lightweight, native library for Lua aimed to generate cryptographically secure pseudo random numbers, using trusted sources of randomness provided by the operating system.

* On Unix-like distributions, it uses the ```OpenSSL``` library to generate random numbers;
* On Windows, it uses the WINAPI ```bcrypt``` library;
* On macOS / iOS, it uses the ```Security``` framework.

> [!NOTE]
> 
> ```lua-cryptorandom``` is implemented in C, and also compiles as C++.

## Table of Contents

* [Installation](#installation)
* [Usage](#usage)
    * [How to generate random bytes](#how-to-generate-random-bytes)
    * [How to generate a random float number](#how-to-generate-a-random-float-number)
    * [How to generate a random integer](#how-to-generate-a-random-integer)
* [Use cases](#use-cases)
* [Methods](#methods)
    * [bytes](#bytes)
    * [integer](#integer)
    * [number](#number)
    * [take](#take)
* [Known limitations](#known-limitations)
* [Change log](#change-log)
* [Future works](#future-works)

## Installation

> [!IMPORTANT]
> 
> On Unix-like operating systems (e.g.: Linux, BSD),  ```lua-cryptorandom``` depends on the ```OpenSSL``` library:
>  * On Debian-based (e.g.: Ubuntu) distributions:
> 
>      ```bash
>      sudo apt install -y libssl-dev
>      ```
> 
>  * On RedHat-based (e.g.: Fedora) distributions:
> 
>      ```bash
>      sudo dnf install openssl-devel
>      ```
> 
>  * On BSD-based (e.g.: FreeBSD) distributions:
> 
>      ```bash
>      pkg install openssl-devel
>      ```

Assuming that [LuaRocks](https://luarocks.org) is properly installed and configured on your system, execute the following command:

```bash
luarocks install lua-cryptorandom
```

## Usage

### How to generate random bytes

```lua
local random = require("lua-cryptorandom")

-- number of bytes to generate
local n = 10

local bytes, err = random.bytes(n)

if (bytes == nil) then
    print("error code: ", err)
else
    assert(n == #bytes, "Unexpected number of bytes")

    -- print each byte
    for i, b in ipairs(bytes) do
        print(i, ("0x%02X"):format(b))
    end
end
```

### How to generate a random float number

```lua
local random = require("lua-cryptorandom")

local number, err = random.number()

if (number == nil) then
    print("error code: ", err)
else
    print("number: ", number)
end
```

### How to generate a random integer

There are two functions to generate integers: [take](#take) and [integer](#integer).

* Generate integer (*usually in 32-bits range*):

    ```lua
    local random = require("lua-cryptorandom")

    local take, err = random.take()

    if (take == nil) then
        print("error code: ", err)
    else
        print("take: ", take)
    end
    ```

* Generate a potentially large integer (*in 64-bits range*):

    ```lua
    local random = require("lua-cryptorandom")

    local integer, err = random.integer()

    if (integer == nil) then
        print("error code: ", err)
    else
        print("integer: ", integer)
    end
    ```

## Use cases

Many security operations rely on high-quality randomization services to avoid reproducibility and remain resistant to reverse engineering:

* randomized password generation;
* nonces (numbers used once) generation;
* initialization vectors;
* salts in passwords before hashing;
* tokenization (*token generation*) to represent sensitive data;
* secure random sampling in statistical analysis.

## Methods

> [!IMPORTANT]
> 
> For each method below, always check whether the first returned value is ```nil``` or not. When the first value is ```nil```, there was an underlying error generating random values. It can fail because no trusted random source is available or the trusted entropy source temporarily fail to provide sufficient randomness material.

### bytes

* *Description*: Generates a sequence of random bytes
* *Signature*: ```bytes(n)```
    * *Parameters*:
        * *n*: the number of bytes to generate
    * *Return*: ```table | nil``` as first value, and ```nil | number``` as the second.
        * ```table | nil```: a table containing ```n``` bytes on success, or ```nil``` when an error occurred;
        * ```nil | integer```: an error code that is set to ```nil``` on success, or an ```integer``` representing the code used by the underlying library (```OpenSSL``` on Unix, ```bcrypt``` on Windows and ```Security``` framework on macOS / iOS).
* *Remark*: Here, a byte is meant as an integer in the range 0 - 255.
* *Usage*:

    ```lua
    local random = require("lua-cryptorandom")

    -- number of bytes to generate
    local n = 10

    local bytes, err = random.bytes(n)

    if (bytes == nil) then
        print("error code: ", err)
    else
        assert(n == #bytes, "Unexpected number of bytes")

        -- print each byte
        for i, b in ipairs(bytes) do
            print(i, ("0x%02X"):format(b))
        end
    end
    ```

### integer

* *Description*: Generates a random integer
* *Signature*: ```integer()```
    * *Return*: ```integer | nil``` as first value, and ```nil | integer``` as the second.
        * ```integer | nil```: the generated integer on success, or ```nil``` when an error occurred;
        * ```nil | integer```: an error code that is set to ```nil``` on success, or an ```integer``` representing the code used by the underlying library (```OpenSSL``` on Unix, ```bcrypt``` on Windows and ```Security``` framework on macOS / iOS).
* *Remark*: The generated integer can be any valid Lua integer, and such integer can span up to 64 bits. Use this function when you need a potentially large integer. For smaller integers, see [take](#take).
* *Usage*:

    ```lua
    local random = require("lua-cryptorandom")

    local integer, err = random.integer()

    if (integer == nil) then
        print("error code: ", err)
    else
        print("integer: ", integer)
    end
    ```

### number

* *Description*: Generates a random float number
* *Signature*: ```number()```
    * *Return*: ```number | nil``` as first value, and ```nil | integer``` as the second.
        * ```number | nil```: the generated float number on success, or ```nil``` when an error occurred;
        * ```nil | integer```: an error code that is set to ```nil``` on success, or an ```integer``` representing the code used by the underlying library (```OpenSSL``` on Unix, ```bcrypt``` on Windows and ```Security``` framework on macOS / iOS).
* *Remark*: since v0.0.2, in case of success, the returned number is not ```NaN``` or positive/negative infinity values.
* *Usage*:

    ```lua
    local random = require("lua-cryptorandom")

    local number, err = random.number()

    if (number == nil) then
        print("error code: ", err)
    else
        print("number: ", number)
    end
    ```

### take

* *Description*: Generates a random integer that spans at least 16 bits in size, but usually 32 bits in these-days-computers.
* *Signature*: ```take()```
    * *Return*: ```integer | nil``` as first value, and ```nil | integer``` as the second.
        * ```integer | nil```: the generated integer on success, or ```nil``` when an error occurred;
        * ```nil | integer```: an error code that is set to ```nil``` on success, or an ```integer``` representing the code used by the underlying library (```OpenSSL``` on Unix, ```bcrypt``` on Windows and ```Security``` framework on macOS / iOS).
* *Remark*: The generated integer has, at least, 16 bits in size, but it is usually a 32 bits integer in these-day-computers. The returned integer has the ```int``` data type in C. To generate potentially large integers, see [integer](#integer).
* *Usage*:

    ```lua
    local random = require("lua-cryptorandom")

    local take, err = random.take()

    if (take == nil) then
        print("error code: ", err)
    else
        print("take: ", take)
    end
    ```

## Known limitations

> [!WARNING]
> 
> This section mostly applies to users running a customized build of Lua.

* The error code (second return value) on each method might deliver a value different than the one returned by the underlying library. This condition might happen when the Lua type ```lua_Integer``` is shorter than an ```unsigned long``` in size. Even though it can be achieved on personalized builds of Lua (e.g.: Lua compiled as C89 on some platforms), the usual build of Lua should be safe for most users and platforms.

## Change log

* v0.0.3:
    * Using unions on [take](#take) to avoid alignment issues;
    * Added the [Usage](#usage) section on README.
* v0.0.2: Prevent the generation of ```NaN``` and positive/negative infinity values in the function [number](#number).
* v0.0.1: Initial release.

## Future works

* Add CMake as a build system.