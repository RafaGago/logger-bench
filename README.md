Logger benchmarks
------------------

A project to place logger benchmarks. Previously they were polluting the
mini-async-log repository with many git submodules.

This will use Cmake external projects instead.

The main motivator for change was to measure latency as a percentile. C++
Templates will be avoided as much as possible too (just for code readability).

This now only compiles on Linux.

If malc is compiled, then there are dependencies on the meson and ninja build
systems. (Ubuntu/Debian):

> sudo apt install python3-pip ninja-build
> sudo -H pip3 install meson
