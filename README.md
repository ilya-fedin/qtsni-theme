# qtsni-theme

## Overview

QtSNI is a Qt platformtheme plugin that offers a flatpak-compatible SNI tray icon. It is based on [lxqt-qtplugin](https://github.com/lxqt/lxqt-qtplugin) and Qt's internal xdgdesktopportal platformtheme plugins.

## Installation

### Compiling source code

Runtime dependenciy is libdbusmenu-qt5.   
Additional build dependenciy is CMake, optionally Git to pull latest VCS checkouts.   

Code configuration is handled by CMake. CMake variable `CMAKE_INSTALL_PREFIX` has to be set to `/usr` on most operating systems.   

To build run `make`, to install `make install` which accepts variable `DESTDIR` as usual.   

### Binary packages

There are `org.kde.PlatformTheme.QtSNI` package in Flathub that anyone can install and use with any Flatpak'ed application that uses `org.kde.Platform` as its base. Just use `flatpak override --env=QT_QPA_PLATFORMTHEME=qtsni`.

## Configuration, Usage

To use the plugin in a flatpak package, just set `--env=QT_QPA_PLATFORMTHEME=qtsni` and `--talk-name=org.kde.StatusNotifierWatcher` in `finish-args`. After that this package will get a working tray icon for a Qt5 program. You may also want to support Ubuntu's indicator-application-service, then you need `--talk-name=com.canonical.indicator.application` and `--talk-name=org.ayatana.indicator.application` permissions.   
If, for some unknown reasons, the plugin is not loaded, we can debug the plugin by exporting `QT_DEBUG_PLUGINS=1`. Then, Qt5 will print detailed information and error messages about all plugins in the console when running any Qt5 programs.
