#include "AppSwitcherPlugin.h"

#include "actions/AppSwitcherAction.h"
#include "StreamDockSDK/HSDLogger.h"

std::shared_ptr<HSDAction> AppSwitcherPlugin::GetOrCreateAction(
    const std::string& action, const std::string& context) {
  std::lock_guard<std::mutex> lock(actionsMutex_);

  if (const auto found = actions_.find(context); found != actions_.end()) {
    return found->second;
  }
  if (action != STREAMDOCK_ACTION_ID) {
    HSDLogger::LogMessage("Unknown action: " + action);
    return nullptr;
  }

  auto instance = std::make_shared<AppSwitcherAction>(mConnectionManager, action, context);
  actions_.emplace(context, instance);
  return instance;
}
