Known issues
============

- The whole frame doesn't fit in a 802.15.4 package. Tested with the
  remote-revb we have about 128 bytes of payload. For now we have commented out
  the last sensor in the Qt Py driver (VL53L1X).

  Do like we do with the waspmote and split a frame into several packages.  Or
  maybe the package size can be increased. Or maybe if we switch to a higher
  layer protocol fragmentation will be handled automatically.
