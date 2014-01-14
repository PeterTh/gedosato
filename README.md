GeDoSaTo Developer Readme
=========================

General
=======

This file is intended for developers who want to improve GeDoSaTo or use it as a base for other modifications.

All the source code is released under the conditions of the GPLv3, except for
- SuperFastHash
- SMAA
which have their own licensing terms included.

Requirements
============

- Visual C++
- The DirectX SDK
- Microsoft Detours (Express) 3

File Overview
=============

General
-------

- The "pack" folder contains the files for distribution, including end-user documentation, .inis, effects and folders
- "main.*" includes the main function and a few utilities

Wrapping
--------

- The "d3d9*" files implement d3d wrapping
- "Detouring.*" files implement function overriding using the Detours library

Utilities
---------

- "Settings.*" files implement reading settings (defined in Settings.def) from .ini files and querying them
- "KeyActions.*" files implement keybindings, together with the Xmacro files "Keys.def" and "Actions.def"
- "WindowManager.*" files implement window management (e.g. borderless fullscreen)

Rendering
---------

- "RenderstateManager.*" is where most of the magic happens, implements detection and rerouting of the games' rendering pipeline state
- "SMAA.*", "VSSAO.*" and "GAUSS.*" are effects optionally used during rendering (derive from the base Effect)
- "Textures.def" is a database of known texture hashes

