GeDoSaTo by Durante -- README
=============================

**Please read this whole document before running anything!**

What is it?
===========
The incredibly imaginatively named GeDoSaTo is a generalized downsampling tool for PC games.
Its goal is to obviate the need for (often limited and hardware dependent) driver modifications
to use downsampling in games.
GeDoSaTo also supports game-specific plugins, for example a Dark Souls 2 plugin.

How do I use it? (INSTALLATION & USAGE INSTRUCTIONS)
====================================================
1) Extract the contents of the zip to some permanent directory
2) Run GeDoSaToTool.exe *as Administrator* (you can use this to perform the following steps)
3) Adjust the settings and keybindings as desired
   - you can also store game-specific setting overrides, see "config/Blackguards" as an example
4) [optional] Add your game executable names to the whitelist (or user whitelist) 
   (Dark Souls 2 and a few others are already included!)
5) Select the downsampling resolution in-game if you want to use downsampling

What are the advantages and disadvantages compared to driver-level downsampling?
================================================================================
Advantages:
- Essentially no resolution limits (beyond those of the GPU)
- Downsampling from more than 4x the resolution is useful (multi-stage downsampling)
- Selection of downsampling methods, including bicubic and lanczos (not just linear sampling)
- Downsampling in linear color space
- Support downsampling to high-frequency (e.g. 120 Hz or 144 Hz) target modes
- Not limited by display hardware
- Downsampling in borderless windowed fullscreen mode
Disadvantages:
- Compatibility is limited compared to driver-level downsampling 
  (only DirectX9 and DirectX9Ex supported currently)
- Bugs :P

What additional features are there?
===================================
- Take screenshots of either the pre-downsampled full buffer or the actual image displayed on screen
  (automatically sorted in per-game folders!)
- Generic texture overriding for all textures loaded using D3DX
- Texture upsampling support using "smart" image processing algorithms
- Framerate limiting using double/triple/quadruple V-sync
- Forcing borderless fullscreen mode, hiding menus and mouse cursor hiding
- Frame timing information (both GPU and CPU)
- FXAA, SMAA and postprocessing injection
- Generic support for HuD-toggling and HuD-less postprocessing/screenshots by means of shader hash ids
- [Dark Souls 2] VSSAO2, Bokeh DoF, HDR Bloom and HuD-less screenshots

What is the Dark Souls 2 plugin?
================================
The Dark Souls 2 plugin adds several (optional) graphical effects to the game.
Note that you need to ENABLE IN-GAME ANTI ALIASING in order to use these effects.

Will it work?
=============
It currently works relatively reliably for a number of games, but fails to work for others.
Compatibility will hopefully improve with future versions.
*I can not and will not guarantee that it will work for anyone else, 
or not have any adverse effect on your game/computer. Use at your own risk!*

Are there known issues?
=======================
The mouse cursor position and/or click detection is broken in some games.

Will it cause performance problems?
===================================
That depends on your system configuration. 
Usually, performance scales rather linearly with framebuffer size. 

Can I donate to motivate further GeDoSaTo development?
======================================================
You can find donation links on my blog, where I'll also release new versions:
http://blog.metaclassofnil.com/
You can also simply Paypal to peter@metaclassofnil.com

How can I contribute?
=====================
GeDoSaTo is on github: https://github.com/PeterTh/gedosato

How can I uninstall it?
============================
Simply remove the files.
If you want to be ultra clean, also remove the registry key 
"HKEY_LOCAL_MACHINE\SOFTWARE\Durante\GeDoSaTo".

Contact information
===================
Contact me at peter@metaclassofnil.com
