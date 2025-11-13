#pragma once

#include "datatable.hpp"
#include "valve.hpp"

class grenade_render_fix
    : public valve::interface_registry
    , valve::plugin_callbacks::v3 {
public:
  grenade_render_fix( ) noexcept
      : valve::interface_registry( "ISERVERPLUGINCALLBACKS003" ) {};

  auto description( ) -> const char * override { return "grenade-render-fix"; };

  auto load( valve::factory factory, valve::factory ) -> bool override;
  auto unload( ) -> void override;

  auto pause( ) -> void override { is_paused_ = true; }
  auto unpause( ) -> void override { is_paused_ = false; }

private:
  static auto invalid_ehandle_override(
      const void *pProp,
      const void *pStruct,
      const void *pVarData,
      void *pOut,
      int iElement,
      int objectID ) -> void;

private:
  void *server_handle_;

  prop_t *thrower_property_;
  prop_t *owner_property_;
  decltype( prop_t::proxy_fn ) ehandle_to_int_;

  bool is_paused_;
};
