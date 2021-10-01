# LongQt-model

This repository contains the backend code for used by
LongQt and PyLongQt. This includes the electrophysiology cell models, the protocols, the
logic to run and save simulations, etc. This can be built as a standalone c++ library, or
added as a subdirectory to a project (the recommend approach for developing PyLongQt or
LongQt).

## Independent Build

### Requirements

- Qt version 5.12:  https://www.qt.io/
  - windows: install the MSVC compiled version
- c++17 or greater compliant compiler
  - linux:      gcc
  - windows:    MSVC 2019
  - OS X:       clang
- cmake
- optional:
  - QtCreator

LongQt-model needs to be built with different build systems depending on the platform, for windows use MSVC 2019,
for linux use gcc (or clang), and for mac use clang. On windows MSCV 2019 can be installed as
a standalone package or as a part of the visual studio IDE. Qt 5.12 should be used, although
other versions of Qt will likely work as well.

Building in QtCreator is straightforward, just add the project. There are no special
options that need selected. To install LongQt-model add the install target to the build
process (in QtCreator) or run cmake
with the install target (for command line). It is often best to set the install directory
manually, I install into a install subdirectory in my home folder. Set the CMAKE_INSTALL_PREFIX,
and ensure that it ends in LongQt-model. Once installed it can be added to other projects
as a static library using cmakes FindPackage (cmake may need the directory to search added
(using `LongQt-model_DIR`) or the install directory added to the path).
