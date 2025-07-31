#include "valve.hpp"
#include <string>
#include <unordered_map>

namespace valve {
  enum class return_code : bool { FAILED, OK };

  extern "C" __attribute__( ( used ) ) inline auto
  CreateInterface( const char *name, return_code *ret ) -> void * {
    auto *interface = interface_registry::find( name );

    if ( ret != nullptr )
      *ret = static_cast< return_code >( interface != nullptr );

    return interface;
  }

  std::unordered_map< std::string, interface_registry * >
      interface_registry::registry_ = { };

} // namespace valve
