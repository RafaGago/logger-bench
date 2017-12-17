Logger benchmarks
=================

A project to place logger benchmarks. Previously they were polluting the
mini-async-log repository with many git submodules.

This will use CMake external projects instead.

The main motivator for change was to measure latency as a percentile. C++
Templates will be avoided as much as possible too (just for code readability).

This compiles only on Linux for now.

If malc is compiled, then there are dependencies on the meson and ninja build
systems. (Ubuntu/Debian):

> sudo apt install python3-pip ninja-build
> sudo -H pip3 install meson

The codebase tries to remain readable by sticking to the most basic C++ concepts
only (e.g. no templates). Contributions with new loggers or test method
improvements are welcome.

Implementation notes
====================

As no comparison is fair, this random list tries to maintain a non biased
summary of logger traits that can affect the performance.

malc
----

* Timestamping at the consumer side.

* Timestamps from boot time. Real calendar time has to be parsed from the file
  names (they contain the calendar and machine boot counter). If a conversion is
  desired the log lines must be processed by an external script.

* The format string has to be a literal _always_.

mal
---

* Timestamping at the consumer side.

* Timestamps from boot. Real calendar time has to be parsed from the file names
  (they contain the calendar and machine boot counter). If a conversion is
  desired the log lines must be processed by an external script.

* The format string has to be a literal _always_.

Nanolog
-------

* Buffer sizes of 8MB are not configurable, so if a comparison has to be kept
  fair all the other loggers have to use 8MB buffers Arbitrary memory usage.

* Logs without time zone.

spdlog
------

* No way to configure internal queue size (?). Arbitrary memory usage.

Glog
----

* This logger is not asynchronous if I remember correctly.

G3log
-----

* No remarks. I know almost nothing about the implementation.
