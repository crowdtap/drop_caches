Drop Caches
===========

Introduction
-------------

drop\_caches allows to drop caches for specific files or block devices.
It does it with `posix_fadvise` and `POSIX_FADV_DONTNEED` on the whole file
range.

Installation
------------

`make`

Usage
------

    Usage: drop_caches [OPTIONS]... FILE...
    Drop the page caches of the given files.
    
      -h            show help
      -i INTERVAL   drop caches every INTERVAL seconds
      -D            daemonize
