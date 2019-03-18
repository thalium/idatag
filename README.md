# IDATag - Tag explorer for IDA Pro

## Overview

IDATag is a tag plugin for [IDA Pro](https://www.hex-rays.com/products/ida/). The plugin leverages IDA as a platform to map, explore, and visualize collected tags.
Tags can come from multiple sources such as IDA itself or different other clients.

## Releases

* v0.0.1 -- Initial release

## Building

### IDA Pro (32-bit)

```sh
set IDASDK=C:\Program Files\IDA 7.2\idasdk72
set IDATARGET=IDA32
set QTHEADER=C:\Qt\Qt5.3.0\5.3\msvc2013\include
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
mkdir Ida32Build
cd Ida32Build
cmake -G"Visual Studio 15 2017 Win64" ..
cmake --build . --config RelWithDebInfo
```

The build binary can be found in `bin\IDATag_x64\` directory:
* `IDATag.dll` for IDA Pro 32-bit

### IDA Pro (64-bit)

```sh
set IDASDK=C:\Program Files\IDA 7.2\idasdk72
set IDATARGET=IDA64
set QTHEADER=C:\Qt\Qt5.3.0\5.3\msvc2013\include
"C:\Program Files (x86)\Microsoft Visual Studio\2017\Professional\VC\Auxiliary\Build\vcvars64.bat"
mkdir Ida64Build
cd Ida64Build
cmake -G"Visual Studio 15 2017 Win64" ..
cmake --build . --config RelWithDebInfo
```

> WARNING: no quotes for the IDASDK path!
> Example : 
> ```sh
> set IDASDK=C:\Program Files\IDA 7.2\idasdk72`
> ```

The build binary can be found in `bin\IDATag_x64\` directory:
* `IDATag64.dll` for IDA Pro 64-bit

## Installation

Just copy the built dll file in the IDA Pro plugin directory.

> REMARK: you can also create a symbolic link to th target dll. 
> for example, in an Admin prompt:
> ```
> mklink IDATag.dll c:\dev\IDATag\bin\IDATag_x64\IDATag.dll
> mklink IDATag64.dll c:\dev\IDATag\bin\IDATag_x64\IDATag64.dll
> ```

> This plugin targets IDA Pro version 7.0 and above only.

## Usage

IDATag loads automatically when an IDB is opened. To run the plugin, you may use the `Ctrl+Alt+T` shorcut.

## Tag Overview

The Tag Overview is a dockable widget that provides view of the referenced tags. 
These tags can come from three entry points:
* IDB functions
* IDB names
* External JSON tag file

The JSON tag file is a pivot to each IDATag client. Every tools that could create a JSON file with a specific structure is able to feed your IDA view. The JSON contract has been designed as minimalist.

For instance, the following JSON is used to tag a specific offset with two tags:

```
[
	{
		"tag": "crypto",
		"offset": 5065078,
		"feeder": "tagcrypt"
	},
	{
		"tag": "SHA-1",
		"offset": 5065078,
		"feeder": "tagcrypt"
	}
]
```

This table can be sorted by column, and entries can be double clicked to jump to their corresponding disassembly. Moreover, mulitple filters are available to facilitate the navigation:
* Remove not tagged offsets
* Regex filter
* Feeder filter

Also, it is possible to add tags from IDA by double-clicking the tag case of a specific offset.
Finally, all the tags may be modified by double-clicking on it.

## Context Menu

Right clicking the table in the Tag Overview will produce a context menu with a few basic amenities.

* Export Tags to a JSON file
* Filter by feeders (IDATag client)
* Refresh tags from files

## Configuration

The plugin can be configured with a specific folder to store tag files and username.

# Tag clients

* Tagcrypt
* TagAPI
* TagSource
* TagString
* TagRefs

