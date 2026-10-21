#pragma once

#include "StreamDockSDK/HSDAction.h"

#include <mutex>
#include <string>

class ControlMapperAction final : public HSDAction {
 public:
  using HSDAction::HSDAction;
  void DidReceiveSettings(const nlohmann::json& payload) override;
  void WillAppear(const nlohmann::json& payload) override;
  void KeyDown(const nlohmann::json& payload) override;
  void KeyUp(const nlohmann::json& payload) override;

 private:
  std::string RoleName() const;
  void Send(const wchar_t* operation);
  mutable std::mutex mutex_;
  std::string roleName_;
};
