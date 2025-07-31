#include "valve.hpp"

class grenade_render_fix
    : public valve::interface_registry
    , valve::plugin_callbacks::v4 {
public:
  grenade_render_fix( ) noexcept
      : valve::interface_registry( "ISERVERPLUGINCALLBACKS004" ) {};

  auto description( ) -> const char * override { return "grenade-render-fix"; };

  auto load( valve::factory factory, valve::factory ) -> bool override;
};
