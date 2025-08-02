#include "plugin.hpp"
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <dlfcn.h>
#include <elf.h>
#include <link.h>
#include <sys/mman.h>
#include <sys/stat.h>

grenade_render_fix plugin_instance;

constexpr std::size_t THROWER_DT_INDEX = 4;

auto resolve_symbol( void *handle, const char *name ) -> void * {
  const struct link_map *lm;
  void *result = nullptr;
  void *base = nullptr;
  FILE *f = nullptr;
  struct stat s;

  unsigned int i, symcount = 0;
  ElfW( Ehdr ) * e;
  ElfW( Shdr ) * shdr;
  ElfW( Sym ) * sym;
  char *strtab;

  if ( dlinfo( handle, RTLD_DI_LINKMAP, &lm ) != 0 )
    goto cleanup;

  f = fopen( lm->l_name, "r" );
  if ( !f )
    goto cleanup;

  if ( fstat( fileno( f ), &s ) != 0 )
    goto cleanup;

  base = mmap( nullptr, s.st_size, PROT_READ, MAP_PRIVATE, fileno( f ), 0 );
  if ( base == MAP_FAILED ) {
    base = nullptr;
    goto cleanup;
  }

  e = ( ElfW( Ehdr ) * ) base;
  shdr = ( ElfW( Shdr ) * ) ( ( char * ) base + e->e_shoff );
  sym = nullptr;
  strtab = nullptr;

  for ( i = 0; i < e->e_shnum; i++ ) {
    if ( shdr[ i ].sh_type == SHT_SYMTAB ) {
      sym = ( ElfW( Sym ) * ) ( ( char * ) base + shdr[ i ].sh_offset );
      symcount = shdr[ i ].sh_size / sizeof( ElfW( Sym ) );
      strtab = ( char * ) base + shdr[ shdr[ i ].sh_link ].sh_offset;

      break;
    }
  }

  if ( !strtab || !sym || !symcount )
    goto cleanup;

  for ( i = 0; i < symcount; i++ ) {
    if ( !strcmp( &strtab[ sym[ i ].st_name ], name ) ) {
      result = ( void * ) ( lm->l_addr + sym[ i ].st_value );
      break;
    }
  }

cleanup:
  if ( base )
    munmap( base, s.st_size );
  if ( f )
    fclose( f );
  return result;
}

auto grenade_render_fix::send_proxy_override(
    const void *pProp,
    const void *pStruct,
    const void *pVarData,
    void *pOut,
    int iElement,
    int objectID ) -> void {
  if ( !plugin_instance.is_paused_ )
    pVarData = nullptr;

  std::bit_cast< decltype( &grenade_render_fix::send_proxy_override ) >(
      plugin_instance.ehandle_to_int_ )(
      pProp, pStruct, pVarData, pOut, iElement, objectID );
}

auto grenade_render_fix::load( valve::factory factory, valve::factory )
    -> bool {
  server_handle_ = dlopen( "cstrike/bin/server_srv.so", RTLD_NOW );
  if ( !server_handle_ )
    return false;

  send_table_ = std::bit_cast< send_table_t * >(
      resolve_symbol( server_handle_, "_ZN14DT_BaseGrenade11g_SendTableE" ) );
  if ( !send_table_ )
    return false;

  ehandle_to_int_ = resolve_symbol(
      server_handle_,
      "_Z22SendProxy_EHandleToIntPK8SendPropPKvS3_P8DVariantii" );
  if ( !ehandle_to_int_ )
    return false;

  if ( send_table_->props[ THROWER_DT_INDEX ].func != ehandle_to_int_ )
    return false;

  send_table_->props[ THROWER_DT_INDEX ].func =
      std::bit_cast< void * >( &grenade_render_fix::send_proxy_override );

  return true;
}

auto grenade_render_fix::unload( ) -> void {
  if ( send_table_ )
    send_table_->props[ THROWER_DT_INDEX ].func = ehandle_to_int_;
  if ( server_handle_ )
    dlclose( server_handle_ );
}
