#include "SimHubHttp.h"

#include <Windows.h>
#include <winhttp.h>

#include <iomanip>
#include <sstream>

namespace {
class InternetHandle {
 public:
  explicit InternetHandle(HINTERNET value = nullptr) : value_(value) {}
  ~InternetHandle() { if (value_) WinHttpCloseHandle(value_); }
  operator HINTERNET() const { return value_; }
  bool valid() const { return value_ != nullptr; }
 private:
  HINTERNET value_;
};
}

std::string SimHubHttp::FormEncode(const std::string& value) {
  std::ostringstream encoded;
  encoded << std::uppercase << std::hex;
  for (const unsigned char ch : value) {
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
        (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
      encoded << ch;
    } else {
      encoded << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
    }
  }
  return encoded.str();
}

bool SimHubHttp::PostForm(const std::wstring& path, const std::string& body) {
  InternetHandle session(WinHttpOpen(L"StreamDock SimHub/1.0",
      WINHTTP_ACCESS_TYPE_NO_PROXY, WINHTTP_NO_PROXY_NAME,
      WINHTTP_NO_PROXY_BYPASS, 0));
  if (!session.valid()) return false;
  WinHttpSetTimeouts(session, 1500, 1500, 1500, 1500);

  InternetHandle connection(WinHttpConnect(session, L"127.0.0.1", 8888, 0));
  if (!connection.valid()) return false;
  InternetHandle request(WinHttpOpenRequest(connection, L"POST", path.c_str(),
      nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0));
  if (!request.valid()) return false;

  const wchar_t* headers = L"Content-Type: application/x-www-form-urlencoded\r\n";
  if (!WinHttpSendRequest(request, headers, static_cast<DWORD>(-1L),
      const_cast<char*>(body.data()), static_cast<DWORD>(body.size()),
      static_cast<DWORD>(body.size()), 0) || !WinHttpReceiveResponse(request, nullptr)) {
    return false;
  }

  DWORD status = 0;
  DWORD size = sizeof(status);
  if (!WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
      WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX)) {
    return false;
  }
  return status >= 200 && status < 300;
}
