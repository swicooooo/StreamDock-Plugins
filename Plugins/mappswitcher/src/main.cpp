#include "AppSwitcherPlugin.h"
#include "StreamDockSDK/HSDMain.h"

#include <memory>

int main(int argc, const char** argv) {
  auto plugin = std::make_unique<AppSwitcherPlugin>();
  return esd_main(argc, argv, plugin.get());
}
