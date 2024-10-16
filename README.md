## TVDM

Or **T**urbo **V**ison **D**isplay **M**anager.

Based on https://github.com/gsingh93/display-manager.

Some TurboVision configurations:

* `TV_BUILD_USING_GPM` (default is `ON`): Add mouse support on Linux (and more *NIXes)

* `TV_OPTIMIZE_BUILD` (default is `ON`): Optimize builds with precompiled headers (CMake 3.16+)

* `TV_LIBRARY_UNITY_BUILD` (default is `OFF`): Reduce build time of the main library (CMake 3.16+), the size can be increased.

See more in its CMakeLists.txt file.