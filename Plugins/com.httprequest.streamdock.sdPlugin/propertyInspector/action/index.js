// Called by action.js after settings are received
function populateForm(settings) {
  document.getElementById('url').value = settings.url || '';
  document.getElementById('method').value = settings.method || 'GET';
  document.getElementById('headers').value = settings.headers || '';
  document.getElementById('body').value = settings.body || '';
  document.getElementById('timeout').value = settings.timeout || 10000;
  document.getElementById('showStatus').checked = settings.showStatus !== false;
}

// Wire up all inputs to save on change
function bindInputs() {
  const fields = ['url', 'method', 'headers', 'body', 'timeout'];
  fields.forEach(id => {
    document.getElementById(id).addEventListener('input', () => {
      $settings[id] = document.getElementById(id).value;
      saveSettings();
    });
  });

  document.getElementById('showStatus').addEventListener('change', () => {
    $settings.showStatus = document.getElementById('showStatus').checked;
    saveSettings();
  });
}

bindInputs();
