async function fetchData() {
    const response = await fetch('/read');
    return await response.json()        
}
async function updateUI(data) {
    let rssi, devicename;
    try {
        const data = await fetchData();
        rssi = `RSSI: ${data.wifi.rssi} dBm`;
        devicename = data.devicename;
    } catch (error) {
        console.error('Error fetching sensor data:', error);
        telegram = age_ms = rssi = devicename = 'Error';
    }
    document.getElementById('devicename').textContent = devicename;
    document.getElementById('rssi').textContent = rssi;
    setTimeout(updateUI, 10000);
}

async function showNotification(message, type) {
    const notification = document.getElementById('notification');
    notification.innerHTML = message;
    notification.className = `notification ${type}`;
    notification.style.display = 'block';

    setTimeout(() => { notification.style.display = 'none'; }, 10000);
}

async function loadSettings() {
    try {
        const settings = await fetchData();

        document.getElementById('devicename').value = settings.devicename;
    } catch (error) {
        console.error('Error loading settings:', error);
    }
}

async function saveSettings(formData) {
    try {
        const response = await fetch('/settings', { method: 'POST', body: formData });
        const data = await response.json();

        if (data.status === "success") {
            showNotification("Settings saved successfully!", "success");
        } else if (data.status === "error" && data.errors.length > 0) {
            showNotification("Errors: <ul><li>" + data.errors.join("</li><li>") + "</li></ul>", "error");
        } else {
            showNotification("Unexpected response from server", "error");
        }
    } catch (error) {
        showNotification("Failed to save settings: " + error.message, "error");
    }
}