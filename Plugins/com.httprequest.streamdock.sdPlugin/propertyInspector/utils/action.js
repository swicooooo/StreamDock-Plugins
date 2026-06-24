let $websocket, $uuid, $action, $context, $settings = {};

const $propEvent = {};

WebSocket.prototype.sendToPlugin = function (payload) {
  this.send(JSON.stringify({ event: "sendToPlugin", action: $action, context: $uuid, payload }));
};

WebSocket.prototype.saveData = $.debounce(function (payload) {
  this.send(JSON.stringify({ event: "setSettings", context: $uuid, payload }));
}, 300);

async function connectElgatoStreamDeckSocket(port, uuid, event, app, info) {
  info = JSON.parse(info);
  $uuid = uuid;
  $action = info.action;
  $context = info.context;

  $websocket = new WebSocket('ws://127.0.0.1:' + port);
  $websocket.onopen = () => $websocket.send(JSON.stringify({ event, uuid }));

  $websocket.onmessage = e => {
    const data = JSON.parse(e.data);
    if (data.event === 'didReceiveSettings') {
      $settings = data.payload.settings || {};
      populateForm($settings);
      document.querySelector('.sdpi-wrapper').style.display = 'block';
    }
    $propEvent[data.event]?.(data.payload);
  };
}

function saveSettings() {
  $websocket?.saveData($settings);
}
