#include "SimHubPlugin.h"

#include "StreamDockSDK/HSDLogger.h"
#include "actions/ControlMapperAction.h"
#include "actions/InputTriggerAction.h"

std::shared_ptr<HSDAction> SimHubPlugin::GetOrCreateAction(
    const std::string& action, const std::string& context) {
  std::lock_guard<std::mutex> lock(actionsMutex_);
  if (const auto found = actions_.find(context); found != actions_.end()) {
    return found->second;
  }

  std::shared_ptr<HSDAction> instance;
  if (action == CONTROL_MAPPER_ACTION_ID) {
    instance = std::make_shared<ControlMapperAction>(mConnectionManager, action, context);
  } else if (action == INPUT_TRIGGER_ACTION_ID) {
    instance = std::make_shared<InputTriggerAction>(mConnectionManager, action, context);
  } else {
    HSDLogger::LogMessage("Unknown SimHub action: " + action);
    return nullptr;
  }
  actions_.emplace(context, instance);
  return instance;
}
