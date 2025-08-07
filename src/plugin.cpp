#include "plugin.hpp"
#include "symbol.hpp"
#include <bit>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <elf.h>

grenade_render_fix plugin_instance;

auto grenade_render_fix::send_proxy_override(
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

  std::bit_cast< decltype( &grenade_render_fix::send_proxy_override ) >(
      plugin_instance.ehandle_to_int_ )(
      pProp, pStruct, pVarData, pOut, iElement, objectID );
}

auto grenade_render_fix::load( valve::factory factory, valve::factory )
    -> bool {
#if __x86_64__
  server_handle_ = dlopen( "cstrike/bin/linux64/server_srv.so", RTLD_NOW );
#else
  server_handle_ = dlopen( "cstrike/bin/server_srv.so", RTLD_NOW );
#endif
  if ( !server_handle_ )
    return false;

  send_table_ = std::bit_cast< send_table_t * >(
      sym::resolve( server_handle_, "_ZN14DT_BaseGrenade11g_SendTableE" ) );
  if ( !send_table_ )
    return false;

  ehandle_to_int_ = sym::resolve(
      server_handle_,
      "_Z22SendProxy_EHandleToIntPK8SendPropPKvS3_P8DVariantii" );
  if ( !ehandle_to_int_ )
    return false;

  assert( send_table_->props[ THROWER_PROP_INDEX ].func == ehandle_to_int_ );

  send_table_->props[ THROWER_PROP_INDEX ].func =
      std::bit_cast< void * >( &grenade_render_fix::send_proxy_override );

  return true;
}

auto grenade_render_fix::unload( ) -> void {
  if ( send_table_ && ehandle_to_int_ )
    send_table_->props[ THROWER_PROP_INDEX ].func = ehandle_to_int_;
  if ( server_handle_ )
    dlclose( server_handle_ );
}
