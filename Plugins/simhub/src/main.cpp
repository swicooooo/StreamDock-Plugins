#include "SimHubPlugin.h"
#include "StreamDockSDK/HSDMain.h"

#include <memory>

int main(int argc, const char** argv) {
  auto plugin = std::make_unique<SimHubPlugin>();
  return esd_main(argc, argv, plugin.get());
}
