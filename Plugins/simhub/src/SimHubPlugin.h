#pragma once

#include "StreamDockSDK/HSDPlugin.h"

#include <map>
#include <memory>
#include <mutex>
#include <string>

class SimHubPlugin final : public HSDPlugin {
 protected:
  std::shared_ptr<HSDAction> GetOrCreateAction(
      const std::string& action, const std::string& context) override;

 private:
  std::mutex actionsMutex_;
  std::map<std::string, std::shared_ptr<HSDAction>> actions_;
};
