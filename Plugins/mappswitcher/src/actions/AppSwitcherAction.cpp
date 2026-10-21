#include "actions/AppSwitcherAction.h"

#include "StreamDockSDK/HSDLogger.h"
#include <Windows.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <condition_variable>
#include <thread>

namespace {
INPUT KeyInput(WORD virtualKey, DWORD flags = 0) {
  INPUT input{};
  input.type = INPUT_KEYBOARD;
  input.ki.wVk = virtualKey;
  input.ki.dwFlags = flags;
  return input;
}

class AppSwitchSession {
 public:
  AppSwitchSession() : releaseThread_(&AppSwitchSession::ReleaseLoop, this) {}

  ~AppSwitchSession() {
    {
      std::lock_guard<std::mutex> lock(mutex_);
      stopping_ = true;
    }
    condition_.notify_one();
    releaseThread_.join();
  }

  bool Step(bool previous) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Do not trust altDown_ as the physical Windows key state. Focus changes or
    // interrupted input can release Alt behind our back while the session is
    // still active. Reassert Alt in the same SendInput batch as every Tab.
    std::array<INPUT, 5> inputs{};
    UINT inputCount = 0;
    inputs[inputCount++] = KeyInput(VK_MENU);
    if (previous) {
      inputs[inputCount++] = KeyInput(VK_SHIFT);
    }
    inputs[inputCount++] = KeyInput(VK_TAB);
    inputs[inputCount++] = KeyInput(VK_TAB, KEYEVENTF_KEYUP);
    if (previous) {
      inputs[inputCount++] = KeyInput(VK_SHIFT, KEYEVENTF_KEYUP);
    }

    if (SendInput(inputCount, inputs.data(), sizeof(INPUT)) != inputCount) {
      ReleaseModifiersAfterFailure();
      return false;
    }

    altDown_ = true;
    releaseAt_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(750);
    condition_.notify_one();
    return true;
  }

 private:
  void ReleaseLoop() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!stopping_) {
      if (!altDown_) {
        condition_.wait(lock, [this] { return stopping_ || altDown_; });
        continue;
      }

      if (condition_.wait_until(lock, releaseAt_) == std::cv_status::timeout &&
          std::chrono::steady_clock::now() >= releaseAt_) {
        if (!ReleaseAlt()) {
          releaseAt_ = std::chrono::steady_clock::now() +
                       std::chrono::milliseconds(100);
        }
      }
    }
    ReleaseAlt();
  }

  bool ReleaseAlt() {
    if (!altDown_) return true;
    INPUT altUp = KeyInput(VK_MENU, KEYEVENTF_KEYUP);
    if (SendInput(1, &altUp, sizeof(INPUT)) != 1) {
      HSDLogger::LogMessage("Failed to release Alt; Win32 error " +
                            std::to_string(GetLastError()));
      return false;
    }
    altDown_ = false;
    return true;
  }

  void ReleaseModifiersAfterFailure() {
    std::array<INPUT, 3> releases{
        KeyInput(VK_TAB, KEYEVENTF_KEYUP),
        KeyInput(VK_SHIFT, KEYEVENTF_KEYUP),
        KeyInput(VK_MENU, KEYEVENTF_KEYUP),
    };
    if (SendInput(static_cast<UINT>(releases.size()), releases.data(),
                  sizeof(INPUT)) == releases.size()) {
      altDown_ = false;
      return;
    }

    HSDLogger::LogMessage("Failed to clean up input modifiers; Win32 error " +
                          std::to_string(GetLastError()));
    altDown_ = true;
    releaseAt_ = std::chrono::steady_clock::now() +
                 std::chrono::milliseconds(100);
    condition_.notify_one();
  }

  std::mutex mutex_;
  std::condition_variable condition_;
  std::thread releaseThread_;
  std::chrono::steady_clock::time_point releaseAt_{};
  bool altDown_ = false;
  bool stopping_ = false;
};

AppSwitchSession& SwitchSession() {
  static AppSwitchSession session;
  return session;
}
}  // namespace

void AppSwitcherAction::DidReceiveSettings(const nlohmann::json& payload) {
  const Direction direction = ReadDirection(payload);
  std::lock_guard<std::mutex> lock(settingsMutex_);
  buttonDirection_ = direction;
}

void AppSwitcherAction::KeyUp(const nlohmann::json&) {
  Direction direction;
  {
    std::lock_guard<std::mutex> lock(settingsMutex_);
    direction = buttonDirection_;
  }
  Switch(direction);
}

void AppSwitcherAction::DialDown(const nlohmann::json&) {
  Direction direction;
  {
    std::lock_guard<std::mutex> lock(settingsMutex_);
    direction = buttonDirection_;
  }
  Switch(direction);
}

void AppSwitcherAction::RotateClockwise(const nlohmann::json&,
                                        unsigned int ticks, bool) {
  Switch(Direction::Next, ticks);
}

void AppSwitcherAction::RotateCounterClockwise(const nlohmann::json&,
                                               unsigned int ticks, bool) {
  Switch(Direction::Previous, ticks);
}

void AppSwitcherAction::WillAppear(const nlohmann::json& payload) {
  DidReceiveSettings(payload);
}

AppSwitcherAction::Direction AppSwitcherAction::ReadDirection(
    const nlohmann::json& payload) {
  if (payload.contains("settings") && payload["settings"].is_object()) {
    const auto& settings = payload["settings"];
    if (settings.value("buttonDirection", "next") == "previous") {
      return Direction::Previous;
    }
  }
  return Direction::Next;
}

void AppSwitcherAction::Switch(Direction direction, unsigned int count) {
  // A malformed host event must not generate an unbounded burst of input.
  count = std::min(count, 20u);
  for (unsigned int index = 0; index < count; ++index) {
    if (!SendAppSwitch(direction)) {
      HSDLogger::LogMessage("SendInput failed while switching applications");
      ShowAlert();
      return;
    }
  }
}

bool AppSwitcherAction::SendAppSwitch(Direction direction) {
  return SwitchSession().Step(direction == Direction::Previous);
}
