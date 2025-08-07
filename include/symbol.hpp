#pragma once

// Resolve symbol from loaded module
auto resolve_symbol( void *handle, const char *name ) -> void *;
