#include "valve.hpp"

struct prop_t
{
  char padding_0[0x3c];
  void* func;
  char padding_1[0x10];
};

struct send_table_t {
    prop_t* props;
};

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
  static auto send_proxy_override(
      const void *pProp,
      const void *pStruct,
      const void *pVarData,
      void *pOut,
      int iElement,
      int objectID ) -> void;

private:
  void *server_handle_;
  
  send_table_t *send_table_;
  decltype(prop_t::func) ehandle_to_int_;

  bool is_paused_;
};
