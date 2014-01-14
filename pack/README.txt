GeDoSaTo by Durante -- README
=============================

**Please read this whole document before running anything!**

What is it?
===========
The incredibly imaginatively named GeDoSaTo is a generalized downsampling tool for PC games.
Its goal is to obviate the need for (often limited and hardware dependent) driver modifications
to use downsampling in games.

How do I use it? (INSTALLATION INSTRUCTIONS)
===========================================
1) Extract the contents of the zip to some permanent directory
2) Adjust the settings in GeDoSaTo.ini and GeDoSaToKeys.ini as desired
3) Run GeDoSaToTool.exe (*as Administrator*)

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
- Compatibility is limited compared to driver-level downsampling (only DirectX9 supported currently)
- Bugs :P

Will it work?
=============
It's a very early release, so you tell me.
*I can not and will not guarantee that it will work for anyone else, 
or not have any adverse effect on your game/computer. Use at your own risk!*

Are there known issues?
=======================
The mouse cursor position and/or click detection is broken in some games. Still working on that.

Will it cause performance problems?
===================================
That depends on your system configuration. 
Usually, performance scales rather linearly with framebuffer size. 

Can I donate?
=============
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
