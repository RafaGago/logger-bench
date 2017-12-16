Logger benchmarks
=================

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

Implementation notes
====================

As no comparison is fair, this random list tries to maintain a non biased
summary of logger traits that can affect the performance.

malc
----

* Timestamping at the consumer side.

* Timestamps from boot. Real calendar time has to be parsed from the file names
  (they contain the calendar and machine boot counter). If a conversion is
  desired the log lines must be processed by an external script.

mal
---

* Timestamping at the consumer side.

* Timestamps from boot. Real calendar time has to be parsed from the file names
  (they contain the calendar and machine boot counter). If a conversion is
  desired the log lines must be processed by an external script.

Nanolog
-------

* Buffer sizes of 8MB are not configurable, so if a comparison has to be kept
  fair all the other loggers have to use 8MB buffers.

* It spawns a thread worker that can't be shat down without hacking with
  internals (it spins, so it can affect the other loggers when measuring).
