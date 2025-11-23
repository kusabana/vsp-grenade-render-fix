#include <bit>
#include <cassert>
#include <cstring>
#include <dlfcn.h>

#include "plugin.hpp"
#include "symbol.hpp"

grenade_render_fix plugin_instance;

auto grenade_render_fix::invalid_ehandle_override(
    const void *pProp,
    const void *pStruct,
    const void *pVarData,
    void *pOut,
    int iElement,
    int objectID ) -> void {
  // Set pVarData to nullptr so EHandleToInt will serialize to
  // INVALID_NETWORKED_EHANDLE_VALUE
  if ( !plugin_instance.is_paused_ )
    pVarData = nullptr;

  std::bit_cast< decltype( prop_t::proxy_fn ) >(
      plugin_instance.ehandle_to_int_ )(
      pProp, pStruct, pVarData, pOut, iElement, objectID );
}

auto grenade_render_fix::load(
    source::interface_factory_fn factory, source::interface_factory_fn )
    -> bool {
#if __x86_64__
  server_handle_ = dlopen( "cstrike/bin/linux64/server_srv.so", RTLD_NOW );
#else
  server_handle_ = dlopen( "cstrike/bin/server_srv.so", RTLD_NOW );
#endif
  if ( !server_handle_ )
    return false;

  const auto send_table = std::bit_cast< data_table * >(
      sym::resolve( server_handle_, "_ZN14DT_BaseGrenade11g_SendTableE" ) );
  if ( !send_table )
    return false;

  ehandle_to_int_ = ( decltype( prop_t::proxy_fn ) ) sym::resolve(
      server_handle_,
      "_Z22SendProxy_EHandleToIntPK8SendPropPKvS3_P8DVariantii" );
  if ( !ehandle_to_int_ )
    return false;

  thrower_property_ = send_table->find_prop_recursive( "m_hThrower" );
  owner_property_ = send_table->find_prop_recursive( "m_hOwnerEntity" );

  if ( !thrower_property_ || !owner_property_ )
    return false;

  if ( thrower_property_->proxy_fn != ehandle_to_int_ ||
       owner_property_->proxy_fn != ehandle_to_int_ )
    return false;

  thrower_property_->proxy_fn = grenade_render_fix::invalid_ehandle_override;
  owner_property_->proxy_fn = grenade_render_fix::invalid_ehandle_override;

  return true;
}

auto grenade_render_fix::unload( ) -> void {
  if ( thrower_property_ )
    thrower_property_->proxy_fn = ehandle_to_int_;
  if ( owner_property_ )
    owner_property_->proxy_fn = ehandle_to_int_;

  if ( server_handle_ )
    dlclose( server_handle_ );
}
