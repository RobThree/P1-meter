async function fetchData() {
    const response = await fetch('/read');
    return await response.json();
}
async function updateUI(data) {
    let devicename, last, uptime, rssi;
    try {
        const data = await fetchData();
        devicename = data.devicename;
        last = data.p1.last;
        uptime = `Uptime: ${data.uptime}`;
        rssi = `RSSI: ${data.wifi.rssi} dBm`;
    } catch (error) {
        console.error('Error fetching sensor data:', error);
        devicename = last = uptime = rssi = 'Error';
    }
    document.getElementById('devicename').textContent = devicename;
    document.getElementById('last').textContent = last;
    document.getElementById('uptime').textContent = uptime;
    document.getElementById('rssi').textContent = rssi;
    setTimeout(updateUI, 10000);
}

async function showNotification(message, type) {
    const notification = document.getElementById('notification');
    notification.innerHTML = message;
    notification.className = `notification ${type}`;
    notification.style.display = 'block';

    setTimeout(() => {
        notification.style.display = 'none';
    }, 10000);
}

async function loadSettings() {
    try {
        const settings = await fetchData();
        const overlay = document.getElementById('loadingOverlay');

        document.getElementById('devicename').value = settings.devicename;
        document.getElementById('reporturl').value = settings.reporturl;
        document.getElementById('posttimeout').value = settings.posttimeout;
        overlay.classList.add('hidden');
    } catch (error) {
        console.error('Error loading settings:', error);
    }
}

async function saveSettings(formData) {
    const overlay = document.getElementById('loadingOverlay');
    overlay.classList.remove('hidden');
    try {
        const response = await fetch('/settings', { method: 'POST', body: formData });
        const data = await response.json();

        if (data.status === 'success') {
            showNotification('Settings saved successfully!', 'success');
        } else if (data.status === 'error' && data.errors.length > 0) {
            showNotification('Errors: <ul><li>' + data.errors.join('</li><li>') + '</li></ul>', 'error');
        } else {
            showNotification('Unexpected response from server', 'error');
        }
    } catch (error) {
        showNotification('Failed to save settings: ' + error.message, 'error');
    }
    overlay.classList.add('hidden');
}

function copyToClipboard() {
    const textarea = document.getElementById('last');
    textarea.select();
    document.execCommand('copy'); // Since we're not using https, we can't use navigator.clipboard.writeText
    textarea.setSelectionRange(textarea.selectionEnd, textarea.selectionEnd); // Sets the selection to the same position, effectively clearing it

    let button = document.querySelector('#copybutton');
    let oldcontent = button.innerHTML;
    button.innerHTML =
        '<svg width="16" viewBox="0 0 24 24"xmlns="http://www.w3.org/2000/svg"><path d="M20.285 2l-11.285 11.567-5.286-5.011-3.714 3.716 9 8.728 15-15.285z" fill="currentColor"/></svg> Copied!';
    setTimeout(() => {
        button.innerHTML = oldcontent;
    }, 2000);
}
