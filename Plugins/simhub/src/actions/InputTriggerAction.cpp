#include "actions/InputTriggerAction.h"

#include "SimHubHttp.h"

void InputTriggerAction::DidReceiveSettings(const nlohmann::json& payload) {
  if (!payload.contains("settings") || !payload["settings"].is_object()) return;
  std::lock_guard<std::mutex> lock(mutex_);
  inputName_ = payload["settings"].value("input_name", "");
}

void InputTriggerAction::WillAppear(const nlohmann::json& payload) {
  DidReceiveSettings(payload);
}

void InputTriggerAction::KeyDown(const nlohmann::json&) { Send(L"Press"); }
void InputTriggerAction::KeyUp(const nlohmann::json&) { Send(L"Release"); }

std::string InputTriggerAction::InputName() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return inputName_;
}

void InputTriggerAction::Send(const wchar_t* operation) {
  const std::string input = InputName();
  if (input.empty()) { ShowAlert(); return; }
  const std::wstring path = std::wstring(L"/api/StreamDeckInput/") + operation + L"/";
  const std::string body = "inputName=" + SimHubHttp::FormEncode(input) +
      "&ownerId=" + SimHubHttp::FormEncode(GetContext());
  if (!SimHubHttp::PostForm(path, body)) ShowAlert();
}
