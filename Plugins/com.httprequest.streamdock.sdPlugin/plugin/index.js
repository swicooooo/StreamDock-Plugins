/// <reference path="./utils/common.js" />

const plugin = new Plugins("httprequest");

plugin.action = new Actions({
  default: {
    url: '',
    method: 'GET',
    headers: '',
    body: '',
    timeout: 10000,
    showStatus: true
  },

  async _willAppear({ context }) {
    const settings = this.data[context];
    if (settings.url) {
      window.socket.setTitle(context, labelFor(settings));
    }
  },

  async keyDown(data) {
    const { context } = data;
    const settings = this.data[context] || {};
    const url = settings.url?.trim();

    if (!url) {
      window.socket.setTitle(context, 'No URL');
      window.socket.showAlert(context);
      return;
    }

    window.socket.setTitle(context, '...');

    const method = (settings.method || 'GET').toUpperCase();
    const timeout = parseInt(settings.timeout) || 10000;

    // Parse custom headers
    let headers = {};
    if (settings.headers?.trim()) {
      try {
        headers = JSON.parse(settings.headers);
      } catch (e) {
        window.socket.setTitle(context, 'Bad Hdr');
        window.socket.showAlert(context);
        return;
      }
    }

    // Build fetch options
    const fetchOptions = { method, headers };
    if (method !== 'GET' && method !== 'HEAD' && settings.body?.trim()) {
      fetchOptions.body = settings.body;
      if (!headers['Content-Type'] && !headers['content-type']) {
        fetchOptions.headers['Content-Type'] = 'application/json';
      }
    }

    const controller = new AbortController();
    const timer = setTimeout(() => controller.abort(), timeout);
    fetchOptions.signal = controller.signal;

    try {
      const response = await fetch(url, fetchOptions);
      clearTimeout(timer);

      if (settings.showStatus) {
        window.socket.setTitle(context, String(response.status));
      }

      if (response.ok) {
        window.socket.showOk(context);
      } else {
        window.socket.showAlert(context);
      }
    } catch (err) {
      clearTimeout(timer);
      const label = err.name === 'AbortError' ? 'Timeout' : 'Error';
      window.socket.setTitle(context, label);
      window.socket.showAlert(context);
    }
  },

  keyUp(data) {},

  _didReceiveSettings({ context }) {
    const settings = this.data[context];
    window.socket.setTitle(context, labelFor(settings));
  }
});

function labelFor(settings) {
  if (!settings?.url) return '';
  try {
    const u = new URL(settings.url);
    return (settings.method || 'GET') + '\n' + u.hostname;
  } catch {
    return settings.url.slice(0, 12);
  }
}
