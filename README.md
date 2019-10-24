Logger benchmarks
=================

This is a project containing C/C++ log library benchmarks in a very easy to
build fashion.

As many other log library authors, I had my own tests polluting my repositories.
This drove me to create this separate project.

The codebase tries to remain trivial by sticking to the most basic C++ concepts
only. Contributions with new loggers or test method improvements are welcome.

Metodology
==========

* Tests performance at the calling site. Tailored for asynchronous loggers.

* Tests are run for different thread counts. Logger threads set CPU affinity.

* The throughput test is summing the time spent on each thread and dividing it
  by the number of messages.

* The latency tests measure each thread entry with the wall clock and calculate
  the 99.9% and 50% percentiles. On my machine these tests show a lot of
  variation between runs.

* For loggers with fixed queues or thread local storage a recommendation of
  memory usage is passed (8MB). If you see one library reporting faults it's
  because the queue is too small for the requested load. You can change the
  queue size recommendation at configure time:

> cmake .. -DLOGGER_MEMORY_KB=16384

Tested libraries
================

mini-async-log-c
----------------

A C11 low-latency wait-free producer (when using Thread Local Storage)
asynchronous data logger with type-safe strings.

https://github.com/RafaGago/mini-async-log-c

> cmake .. -DMALC=on

Requires: C11/C++, meson + ninja

Adds four variants to the tests:

* malc-tls: using thread local storage only.
* malc-fixed: using a bounded buffer for memory allocation.
* malc-fixed-cpu: using as many bounded buffers as CPUs for memory allocation.
* malc-heap: using the heap for memory allocation.

No hybrid variations are tested. The library supports them.

mini-async-log
--------------

A performant asynchronous data logger with acceptable feature-bloat (C++).

https://github.com/RafaGago/mini-async-log-c

> cmake .. -DMAL=on

Requires: partial C++11, cmake

Adds two variants to the tests:

* mal-fixed: using a bounded buffer for memory allocation.
* mal-heap: using the heap for memory allocation.

No hybrid variations are tested. The library supports them.

spdlog
------

Very fast, header-only/compiled, C++ logging library.

https://github.com/gabime/spdlog

> cmake .. -DSPDLOG=on

Requires: C++11, header only.

Adds two variants to the tests:

* spdlog-async: spdlog in asynchronous mode.
* spdlog-sync: spdlog in synchronous mode.

Google log (glog)
-----------------

C++ implementation of the Google logging module.

https://github.com/google/glog.git

> cmake .. -DGLOG=on

Requires: C++, make

NanoLog (Standford)
-------------------

Nanolog is an extremely performant nanosecond scale logging system for C++ that
exposes a simple printf-like API.

https://github.com/PlatformLab/NanoLog

> cmake .. -DNANOLOG2=on

Requires: C++17, make

g3log
-----

Asynchronous logger with Dynamic Sinks

https://github.com/KjellKod/g3log

> cmake .. -DG3LOG=on

Requires: C++14, cmake

llcpp
-----

Literal Logging for C++

https://github.com/blapid/llcpp

> cmake .. -DLLCP=on

Requires: C++17, header only.

Nanolog (Iyengar111)
--------------------

Low Latency C++11 Logging Library.

NOTE: this is not Standford's University NanoLog, but a logger with the same
name that already existed.

https://github.com/Iyengar111/NanoLog

> cmake .. -DNANOLOG=on

Requires: C++11, header only.

Compilation
===========

CMake is used only because of the convenience of "ExternalProject_Add" for this
use case. Unfortunately CMake is much more practical than meson for this as of
today.

This is the project I use when measuring my own logger. Note that enablement of
some loggers requires to turn on CMake options (see CMakeLists.txt), e.g.:

> cmake .. -DNANOLOG=on -DSPDLOG=on -GLOG=on -DG3LOG=on

mini-async-log* loggers can be turned off too:

> cmake .. -DMALC=off -DMAL=off

Execution
=========

> logger-bench <iterations> <messages/iteration> [loggers]...

Notes
=====

As no comparison is fair, this random list tries to maintain a non biased
summary of logger traits that one has to consider when looking at the results.

malc
----

* Timestamping at the consumer side (can be configured not to).

* The format string has to _always_ be a literal.

* Timestamps from boot time. Real calendar time has to be parsed from the file
  names (they contain the calendar time (from epoch) and the machine boot time
  at file  creation time). If a conversion to calendar time is desired the log
  lines must be processed by an external script.

* Configurable queue size. Shows faults on high message loads when not using
  the heap variant.

mal
---

* Timestamping at the consumer side (can be configured not to).

* The format string has to _always_ be a literal.

* Timestamps from boot time. Real calendar time has to be parsed from the file
  names (they contain the calendar time (from epoch) and the machine boot time
  at file  creation time). If a conversion to calendar time is desired the log
  lines must be processed by an external script.

* Configurable queue size. Shows faults on high message loads when not using
  the heap variant.

spdlog
------

* No way to configure internal queue size(?). Arbitrary memory usage.

glog
----

* No remarks. I know almost nothing about the implementation.

Nanolog (Standford)
-------------------

* No way to configure internal queue size (?). Arbitrary memory usage.

* Binary-only logger. Not human-readable logs. A tool to convert the logs is
  provided.

* It has no explicit initialization/deinitialization, so it must launch a
  singleton thread that may affect the other logger's depending on what it does.

G3log
-----

* No remarks. I know almost nothing about the implementation.

Nanolog (Iyengar111)
--------------------

* Buffer sizes of 8MB are not configurable, so if a comparison has to be kept
  fair all the other loggers have to use 8MB buffers. Arbitrary memory usage.

* Logs without time zone (UTC).
