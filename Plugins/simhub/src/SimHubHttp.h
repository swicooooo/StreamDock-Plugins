#pragma once

#include <string>

namespace SimHubHttp {
bool PostForm(const std::wstring& path, const std::string& body);
std::string FormEncode(const std::string& value);
}
