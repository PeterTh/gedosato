GeDoSaTo DARK SOULS 2 edition by Durante -- README
==================================================

**Please read this whole document before running anything!**

What is it?
===========
The incredibly imaginatively named GeDoSaTo is a generalized downsampling tool for PC games.
Its goal is to obviate the need for (often limited and hardware dependent) driver modifications
to use downsampling in games.

What is the DARK SOULS 2 edition?
=================================
This version of GeDoSaTo is extended specifically for Dark Souls 2, and adds several visual
effects to the game.
Note that you need to ENABLE IN-GAME ANTI ALIASING in order to use these effects.

How do I use it? (INSTALLATION INSTRUCTIONS)
===========================================
1) Extract the contents of the zip to some permanent directory
2) Adjust the settings in GeDoSaTo.ini and GeDoSaToKeys.ini as desired
3) [optional] Add your game executable names to "whitelist.txt" in the "assets" folder 
   (dark souls 2 is already included!)
4) Run GeDoSaToTool.exe *as Administrator*
5) ENABLE IN-GAME ANTI-ALIASING in Dark Souls 2

What are the advantages and disadvantages compared to driver-level downsampling?
================================================================================
Advantages:
- Essentially no resolution limits (beyond those of the GPU)
- Downsampling from more than 4x the resolution is useful (multi-stage downsampling)
- Selection of downsampling methods (not just linear sampling)
- Downsampling in linear color space
- Support downsampling to high-frequency (e.g. 120 Hz or 144 Hz) target modes
- Not limited by display hardware
Disadvantages:
- Compatibility is limited compared to driver-level downsampling 
  (only DirectX9 and DirectX9Ex supported currently)
- Bugs :P

What additional features are there?
===================================
- take screenshots of either the pre-downsampled full buffer or the actual image displayed on screen
  (automatically sorted in per-game folders!)
- generic texture overriding for all textures loaded using D3DX
- framerate limiting using double/triple/quadruple V-sync
- [Dark Souls 2] VSAO2, Bokeh DoF, Postprocessing and SMAA

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
GeDoSaTo will be on github soon, if it isn't already.

How can I uninstall it?
============================
Simply remove the files.
If you want to be ultra clean, also remove the registry key "HKEY_LOCAL_MACHINE\SOFTWARE\Durante\GeDoSaTo".

Contact information
===================
Contact me at peter@metaclassofnil.com
