
set(_PCAPNG "${PROJECT_SOURCE_DIR}/pcapxx/3rdParty/LightPcapNg/LightPcapNg")
add_library(pcapng
  ${_PCAPNG}/src/light_advanced.c
  ${_PCAPNG}/src/light_alloc.c
  ${_PCAPNG}/src/light_compression.c
  ${_PCAPNG}/src/light_internal.c
  ${_PCAPNG}/src/light_io.c
  ${_PCAPNG}/src/light_manipulate.c
  ${_PCAPNG}/src/light_null_compression.c
  ${_PCAPNG}/src/light_option.c
  ${_PCAPNG}/src/light_pcapng_cont.c
  ${_PCAPNG}/src/light_pcapng_ext.c
  ${_PCAPNG}/src/light_pcapng.c
  ${_PCAPNG}/src/light_platform.c
  ${_PCAPNG}/src/light_zstd_compression.c
)
target_include_directories(pcapng PUBLIC ${_PCAPNG}/include)

add_library(pcapxx INTERFACE)
find_library(_COMMONXX NAMES Common++ REQUIRED)
find_library(_PACKETXX NAMES Packet++ REQUIRED)
find_library(_PCAPXX NAMES Pcap++ REQUIRED)
find_library(_PCAP NAMES pcap REQUIRED)
target_link_libraries(pcapxx INTERFACE
  pcapng
  ${_PCAPXX}
  ${_PACKETXX}
  ${_COMMONXX}
  ${_PCAP}
)
find_path(_PCAP_include NAMES PcapFileDevice.h REQUIRED)
target_include_directories(pcapxx INTERFACE ${_PCAP_include})
