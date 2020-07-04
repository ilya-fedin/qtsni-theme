# qtsni-theme

## Overview

QtSNI is a Qt platformtheme plugin that offers a flatpak-compatible SNI tray icon. It is based on [lxqt-qtplugin](https://github.com/lxqt/lxqt-qtplugin) and Qt's internal xdgdesktopportal platformtheme plugins.

## Installation

### Compiling source code

Runtime dependenciy is libdbusmenu-qt5.   
Additional build dependenciy is CMake, optionally Git to pull latest VCS checkouts.   

Code configuration is handled by CMake. CMake variable `CMAKE_INSTALL_PREFIX` has to be set to `/usr` on most operating systems.   

To build run `make`, to install `make install` which accepts variable `DESTDIR` as usual.   

## Configuration, Usage

To use the plugin in a flatpak package, just set `--env=QT_QPA_PLATFORMTHEME=qtsni` in `finish-args`. Then this package will get a working tray icon for a Qt5 program.   
If, for some unknown reasons, the plugin is not loaded, we can debug the plugin by exporting `QT_DEBUG_PLUGINS=1`. Then, Qt5 will print detailed information and error messages about all plugins in the console when running any Qt5 programs.
