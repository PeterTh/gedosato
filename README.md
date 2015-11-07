GeDoSaTo Developer Readme
=========================

General
=======

This file is intended for developers who want to improve GeDoSaTo or use it as a base for other modifications.

If you are interested in creating custom game profiles, an in-depth guide is available here:
http://www.pcgamer.com/2014/07/24/how-to-create-sweetfx-style-shaders-that-dont-affect-the-hud-with-durantes-gedosato/

All the source code is released under the conditions of the GPLv3, except for
- SuperFastHash
- SMAA and FXAA
which have their own licensing terms included.

Requirements & Dependencies
===========================

- Visual C++ (2015 or later)
- The DirectX SDK
- Boost (http://www.boost.org/)
- MinHook (this fork: https://github.com/RaMMicHaeL/minhook)
- Ruby (just for the script which updates the build numbers/dates -- you can do without that if you want to)
- NVAPI
- The Steamworks API

Contributing
============

*The best way to contribute is to send a Github pull request.*

If you want to add a major new feature (e.g. DX11 support), I'd appreciate it if you were to contact me before,
so that we can discuss the design a bit.

If you want to contribute a game-specific plugin, look at Dark Souls 2 or the generic plugins as examples.

File Overview
=============

General
-------

- The "pack" folder contains the files for distribution, including end-user documentation, .inis, effects and folders
- "main.*" includes the main function and a few utilities
- The "plugins" folder contain game-specific plugin headers and sources (plugin mappings are defined in "plugins.def")

Wrapping
--------

- The "d3d9*" files implement d3d wrapping
- "detouring.*" files implement function overriding using the Detours library

Utilities
---------

- "settings.*" files implement reading settings (defined in settings.def) from .ini files and querying them
- "key_actions.*" files implement keybindings, together with the Xmacro files "keys.def" and "actions.def"
- "window_manager.*" files implement window management (e.g. borderless fullscreen)
- "d3dutil", string_utils" and "utils" contain various utility functions
- "blacklist.*" manage blacklisting (and whitelisting)

Rendering
---------

- "RenderstateManager.*" is where most of the magic happens, implements detection and rerouting of the games' rendering pipeline state
- "SMAA.*", "VSSAO.*" and "GAUSS.*" are effects optionally used during rendering (derive from the base Effect)
- "Textures.def" is a database of known texture hashes
- "Shaders.def" is a database of known shader hashes
