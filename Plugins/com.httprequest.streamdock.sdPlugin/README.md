# HTTP Request — StreamDock Plugin

A StreamDock plugin that sends an HTTP request (GET, POST, PUT, DELETE, or PATCH) to any URL when a button is pressed. Perfect for triggering smart home devices, home automation endpoints, ESP32/Arduino projects, or any HTTP API without opening a browser.

## Requirements

- Windows 7+ or macOS 10.11+
- StreamDock software version 2.9+

## Installation

1. Download or clone this repository.
2. Copy the `com.httprequest.streamdock.sdPlugin` folder to your StreamDock plugins directory:
   - **Windows:** `%APPDATA%\HotSpot\StreamDock\plugins\`
   - **macOS:** `~/Library/Application Support/HotSpot/StreamDock/plugins/`
3. Restart the StreamDock application.
4. Drag the **HTTP Request** action from the action list onto any button.

## Configuration

Open the Property Inspector (right panel) after adding the action to a button:

| Field | Description |
|-------|-------------|
| **URL** | Full endpoint URL, e.g. `http://192.168.1.100/api/toggle` |
| **Method** | HTTP method: GET, POST, PUT, DELETE, or PATCH (default: GET) |
| **Headers** | Optional JSON object of request headers, e.g. `{"Authorization": "Bearer token"}` |
| **Body** | Optional request body for POST/PUT/PATCH (sent as-is, defaults to `application/json`) |
| **Timeout (ms)** | How long to wait before giving up (default: 10000) |
| **Show Status** | Display the HTTP status code on the button after the request |

## Button Feedback

| Result | Button shows |
|--------|-------------|
| Success (2xx) | HTTP status code + green flash |
| Client/server error (4xx/5xx) | HTTP status code + red alert |
| Network error | `Error` + red alert |
| Request timeout | `Timeout` + red alert |
| No URL configured | `No URL` + red alert |

## Use Cases

- Trigger an ESP32 or Arduino over Wi-Fi (`http://192.168.x.x/action`)
- Control Home Assistant via its REST API
- Toggle smart plugs or lights via local HTTP endpoints
- Send webhooks to services like IFTTT or n8n
- Any one-press HTTP action without opening a browser

## License

GPL-3.0 — see the [StreamDock-Plugins repository](https://github.com/MiraboxSpace/StreamDock-Plugins) for details.
