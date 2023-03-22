#ifndef KORS_WIFI_FCS_H
#define KORS_WIFI_FCS_H

#include <cstdint>
#include <cstdlib>

namespace kors {
namespace wifi {

uint32_t calcFcs( const uint8_t* data, std::size_t size );
uint8_t calcMpduCrc( uint16_t delimiter );

} // namespace wifi
} // namepspace kors

#endif // KORS_WIFI_FCS_H
