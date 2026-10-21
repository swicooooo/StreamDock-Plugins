#include "actions/ControlMapperAction.h"

#include "SimHubHttp.h"

void ControlMapperAction::DidReceiveSettings(const nlohmann::json& payload) {
  if (!payload.contains("settings") || !payload["settings"].is_object()) return;
  std::lock_guard<std::mutex> lock(mutex_);
  roleName_ = payload["settings"].value("rolename", "");
}

void ControlMapperAction::WillAppear(const nlohmann::json& payload) {
  DidReceiveSettings(payload);
}

void ControlMapperAction::KeyDown(const nlohmann::json&) { Send(L"StartRole"); }
void ControlMapperAction::KeyUp(const nlohmann::json&) { Send(L"StopRole"); }

std::string ControlMapperAction::RoleName() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return roleName_;
}

void ControlMapperAction::Send(const wchar_t* operation) {
  const std::string role = RoleName();
  if (role.empty()) { ShowAlert(); return; }
  const std::wstring path = std::wstring(L"/api/ControlMapper/") + operation + L"/";
  const std::string body = "roleName=" + SimHubHttp::FormEncode(role) +
      "&ownerId=" + SimHubHttp::FormEncode(GetContext());
  if (!SimHubHttp::PostForm(path, body)) ShowAlert();
}
