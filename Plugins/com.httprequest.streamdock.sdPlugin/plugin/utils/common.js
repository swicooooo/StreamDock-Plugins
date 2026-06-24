const Timer = new Worker('./utils/worker.js');
const TimerSubscribe = { setTimeout: {}, setInterval: {} };

Timer.addEventListener('message', function ({ data }) {
  TimerSubscribe[data.event][data.id]?.();
});

class Actions {
  constructor(data) {
    this.data = {};
    this.default = {};
    Object.assign(this, data);
  }

  static currentAction = null;
  static currentContext = null;

  propertyInspectorDidAppear({ action, context }) {
    Actions.currentAction = action;
    Actions.currentContext = context;
    this._propertyInspectorDidAppear?.({ action, context });
  }

  willAppear(data) {
    const { context, payload: { settings } } = data;
    this.data[context] = Object.assign({ ...this.default }, settings);
    this._willAppear?.(data);
  }

  willDisappear(data) {
    this._willDisappear?.(data);
    delete this.data[data.context];
  }

  didReceiveSettings(data) {
    const { context, payload: { settings } } = data;
    this.data[context] = Object.assign(this.data[context] || { ...this.default }, settings);
    this._didReceiveSettings?.(data);
  }
}

class Plugins {
  constructor(name) {
    this.name = name;
  }

  clearTimeout(id) { Timer.postMessage({ event: 'clearTimeout', id }); }
  setTimeout(id, callback, delay) {
    this.clearTimeout(id);
    TimerSubscribe.setTimeout[id] = callback;
    Timer.postMessage({ event: 'setTimeout', id, delay });
  }
  clearInterval(id) { Timer.postMessage({ event: 'clearInterval', id }); }
  setInterval(id, callback, delay) {
    this.clearInterval(id);
    TimerSubscribe.setInterval[id] = callback;
    Timer.postMessage({ event: 'setInterval', id, delay });
  }
}

window.connectElgatoStreamDeckSocket = function () {
  const uuid = arguments[1], event = arguments[2];
  window.info = JSON.parse(arguments[3]);
  window.socket = new WebSocket("ws://127.0.0.1:" + arguments[0]);

  WebSocket.prototype.openUrl = function (url) {
    this.send(JSON.stringify({ event: "openUrl", payload: { url } }));
  };

  WebSocket.prototype.sendToPropertyInspector = function (payload) {
    this.send(JSON.stringify({
      payload,
      event: "sendToPropertyInspector",
      action: Actions.currentAction,
      context: Actions.currentContext
    }));
  };

  WebSocket.prototype.setSettings = function (context, payload) {
    this.send(JSON.stringify({ event: "setSettings", context, payload }));
  };

  WebSocket.prototype.setImage = function (context, url) {
    const image = new Image();
    image.src = url;
    image.onload = () => {
      const canvas = document.createElement("canvas");
      canvas.width = image.naturalWidth;
      canvas.height = image.naturalHeight;
      const ctx = canvas.getContext("2d");
      ctx.drawImage(image, 0, 0);
      this.send(JSON.stringify({
        event: "setImage",
        context,
        payload: { target: 0, image: canvas.toDataURL("image/png") }
      }));
    };
  };

  WebSocket.prototype.setTitle = function (context, str) {
    this.send(JSON.stringify({
      event: "setTitle",
      context,
      payload: { target: 0, title: str }
    }));
  };

  WebSocket.prototype.setState = function (context, state) {
    this.send(JSON.stringify({ event: "setState", payload: { state }, context }));
  };

  WebSocket.prototype.showAlert = function (context) {
    this.send(JSON.stringify({ event: "showAlert", context }));
  };

  WebSocket.prototype.showOk = function (context) {
    this.send(JSON.stringify({ event: "showOk", context }));
  };

  window.socket.onopen = () => window.socket.send(JSON.stringify({ uuid, event }));

  window.socket.onmessage = e => {
    const data = JSON.parse(e.data);
    plugin[data.action?.split('.').pop()]?.[data.event]?.(data);
    plugin[data.event]?.(data);
  };
};
