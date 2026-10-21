#pragma once

#include "StreamDockSDK/HSDAction.h"

#include <mutex>

class AppSwitcherAction final : public HSDAction {
 public:
  using HSDAction::HSDAction;

  void DidReceiveSettings(const nlohmann::json& payload) override;
  void KeyUp(const nlohmann::json& payload) override;
  void DialDown(const nlohmann::json& payload) override;
  void RotateClockwise(const nlohmann::json& payload, unsigned int ticks,
                       bool pressed) override;
  void RotateCounterClockwise(const nlohmann::json& payload,
                              unsigned int ticks, bool pressed) override;
  void WillAppear(const nlohmann::json& payload) override;

 private:
  enum class Direction { Next, Previous };

  static bool SendAppSwitch(Direction direction);
  static Direction ReadDirection(const nlohmann::json& payload);
  void Switch(Direction direction, unsigned int count = 1);

  std::mutex settingsMutex_;
  Direction buttonDirection_ = Direction::Next;
};
