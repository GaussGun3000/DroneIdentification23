# DroneIdentification23
Идентификация в потоке wi-fi трафика малого квадрокоптера DJI, работающего по протоколу IEEE802.11 (wi-fi)

### Dependencies

- Qt 5.15 for MSVC compiler

### RoadMap

- GUI
  - source log file choice +
  - output file choice +
  - status bar +
  - result summary + 
- Reading log file +
- Frame checksum
  - Exctract raw frame hex +
  - exctract FRC + 
  - compute CRC32 + 
  - refactor CRC methods into separate class + 
- Frame type detection (Beacon frames identified) +
- SSID analysis for Beacon-Frames +
- Find supposed drone MAC-address + 
- Save all Beacon-frames with this MAC +

- Autotest +

### Add-ons
- logging +

