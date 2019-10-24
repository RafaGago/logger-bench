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

Note that enablement of some loggers requires to turn on CMake options (see
CMakeLists.txt), eg:

> cmake .. -DNANOLOG=on -DSPDLOG=on -GLOG=on -DG3LOG=on

Metodology
==========

All: Tests are run with multiple threads. Each thread has a fixed CPU (affinity
is set). This benchmark, meant for asynchronous loggers, only measures the
performance at the call site.

Througput: The time spent on each thread is measured. Then all the times are
added and divided by the number of threads.

Latency: Every log entry is measured with the wall clock. Every sample is saved
to a contiguous array. An initial clock resolution test is done to see the
minimum delay to take the timestamps. This value is subtracted from each
sample.

For loggers with fixes queues or thread local storage, they try to be configured
to a size of 8MB, but unfortunately on most loggers the queue sizes are not
configurable, at least not in an obvious way. PR's are welcome. If some logger
shows faults try to edit the queue size on main.cpp:main, the variable is called
mem.

Implementation notes
====================

As no comparison is fair, this random list tries to maintain a non biased
summary of logger traits that one has to consider when looking at the results.

malc
----

* Timestamping at the consumer side.

* Timestamps from boot time. Real calendar time has to be parsed from the file
  names (they contain the calendar and machine boot counter). If a conversion
  to calendar time is desired the log lines must be processed by an external
  script.

* The format string has _always_ to be a literal.

* Configurable queue size. Shows faults on high message loads when not using
  the heap variant.

mal
---

* Timestamping at the consumer side.

* Timestamps from boot time. Real calendar time has to be parsed from the file
  names (they contain the calendar and machine boot counter). If a conversion
  to calendar time is desired, the log lines must be processed by an external
  script.

* The format string has to be a literal _always_.

* Configurable queue size. Shows faults on high message loads when not using
  the heap variant.

Nanolog
-------

NOTICE: this project is not Standford's University Nanolog, but another
reasonably fast logger with the same name that was available before:

https://github.com/Iyengar111/NanoLog.git

Remarks:

* Buffer sizes of 8MB are not configurable, so if a comparison has to be kept
  fair all the other loggers have to use 8MB buffers. Arbitrary memory usage.

* Logs without time zone (UTC).

spdlog
------

* No way to configure internal queue size (?). Arbitrary memory usage.

Glog
----

* No remarks. I know almost nothing about the implementation.

G3log
-----

* No remarks. I know almost nothing about the implementation.

Nanolog (Standford's university NanoLog)
----------------------------------------

This is the most famous NanoLog:

https://github.com/PlatformLab/NanoLog

It is internally refered as nanolog2 in every file and CMake config switch.

Remarks:

* No way to configure internal queue size (?). Arbitrary memory usage.

* Binary-only logger. Not human-readable logs.

* It has no explicit initialization/deinitialization, so it must launch a
  singleton thread that may affect the other logger's depending on what it does.
