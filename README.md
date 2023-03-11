# DroneIdentification23

### Dependencies

- Qt 5.15 for MSVC compiler
- libtins 4.4 (expected to be in [project cwd]/libtins-master)


### RoadMap

- GUI
  - source log file choice +
  - output file choice +
  - status bar +
  - result summary
- Reading log file +
- Frame checksum
  - Exctract raw frame hex +
  - exctract FRC
  - compute CRC32
- Frame type detection (Wi-Fi 9.2)
- SSID analysis for Beacon-Frames
- Find supposed drone MAC-address
- Save all Beacon-frames with this MAC

### Add-ons
- logging +
- analysis progress tracking
- multithread
