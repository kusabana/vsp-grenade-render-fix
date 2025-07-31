#include "plugin.hpp"
#include <iostream>

auto grenade_render_fix::load( valve::factory factory, valve::factory )
    -> bool {
  std::cout << " hello world from grenade_render_fix!" << std::endl;

  return false;
}
