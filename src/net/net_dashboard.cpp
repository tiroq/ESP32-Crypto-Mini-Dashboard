#include "net_dashboard.h"

#if ENABLE_OTA

#include "../app/app_model.h"
#include "../app/app_config.h"
#include <ArduinoJson.h>

// Web dashboard HTML (stored in PROGMEM)
const char dashboard_html[] PROGMEM = R"====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Crypto Dashboard</title>
  <style>
    * { margin: 0; padding: 0; box-sizing: border-box; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Arial, sans-serif;
      background: #0B0E11;
      color: #F0F0F0;
      padding: 20px;
    }
    .container { max-width: 1200px; margin: 0 auto; }
    header {
      background: #181A20;
      padding: 20px;
      border-radius: 8px;
      margin-bottom: 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    h1 { color: #F0B90B; font-size: 24px; }
    .nav { display: flex; gap: 10px; }
    .nav button {
      background: #2B3139;
      color: #F0F0F0;
      border: none;
      padding: 10px 20px;
      border-radius: 4px;
      cursor: pointer;
      font-size: 14px;
    }
    .nav button:hover { background: #3B4149; }
    .nav button.active { background: #F0B90B; color: #000; }
    .section {
      background: #181A20;
      padding: 20px;
      border-radius: 8px;
      margin-bottom: 20px;
      display: none;
    }
    .section.active { display: block; }
    .section h2 {
      color: #F0B90B;
      font-size: 18px;
      margin-bottom: 15px;
      border-bottom: 1px solid #2B3139;
      padding-bottom: 10px;
    }
    .crypto-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
      gap: 15px;
    }
    .crypto-card {
      background: #0B0E11;
      border: 1px solid #2B3139;
      padding: 15px;
      border-radius: 8px;
    }
    .crypto-name {
      color: #F0B90B;
      font-size: 16px;
      font-weight: bold;
      margin-bottom: 10px;
    }
    .price-row {
      display: flex;
      justify-content: space-between;
      margin: 8px 0;
      font-size: 14px;
    }
    .price-label { color: #8F8F8F; }
    .price-value { font-weight: bold; }
    .price-binance { color: #F0B90B; }
    .price-coinbase { color: #0ECB81; }
    .spread-positive { color: #0ECB81; }
    .spread-negative { color: #F6465D; }
    .funding-positive { color: #F0B90B; }
    .funding-negative { color: #F6465D; }
    .settings-grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
      gap: 20px;
    }
    .setting-group h3 {
      color: #F0B90B;
      font-size: 14px;
      margin-bottom: 10px;
    }
    .setting-item {
      margin: 10px 0;
    }
    .setting-item label {
      display: block;
      color: #8F8F8F;
      font-size: 13px;
      margin-bottom: 5px;
    }
    .setting-item input {
      width: 100%;
      background: #0B0E11;
      border: 1px solid #2B3139;
      color: #F0F0F0;
      padding: 8px;
      border-radius: 4px;
      font-size: 14px;
    }
    .setting-item input:focus {
      outline: none;
      border-color: #F0B90B;
    }
    .buttons {
      display: flex;
      gap: 10px;
      margin-top: 20px;
    }
    .btn {
      padding: 12px 30px;
      border: none;
      border-radius: 4px;
      font-size: 14px;
      cursor: pointer;
      font-weight: bold;
    }
    .btn-save {
      background: #0ECB81;
      color: #000;
    }
    .btn-save:hover { background: #0EDB91; }
    .btn-reset {
      background: #F6465D;
      color: #FFF;
    }
    .btn-reset:hover { background: #F65670; }
    .status {
      padding: 10px;
      border-radius: 4px;
      margin-top: 15px;
      display: none;
    }
    .status.success {
      background: rgba(14, 203, 129, 0.1);
      border: 1px solid #0ECB81;
      color: #0ECB81;
    }
    .status.error {
      background: rgba(246, 70, 93, 0.1);
      border: 1px solid #F6465D;
      color: #F6465D;
    }
    .last-update {
      color: #8F8F8F;
      font-size: 12px;
      text-align: center;
      margin-top: 10px;
    }
    @media (max-width: 600px) {
      .crypto-grid { grid-template-columns: 1fr; }
      .settings-grid { grid-template-columns: 1fr; }
      header { flex-direction: column; gap: 15px; }
    }
  </style>
</head>
<body>
  <div class="container">
    <header>
      <h1>ESP32 Crypto Dashboard</h1>
      <div class="nav">
        <button class="nav-btn active" data-section="prices">Prices</button>
        <button class="nav-btn" data-section="settings">Settings</button>
        <button class="nav-btn" data-section="ota">OTA Update</button>
      </div>
    </header>

    <div id="prices" class="section active">
      <h2>Real-Time Prices & Funding</h2>
      <div class="crypto-grid" id="cryptoGrid"></div>
      <div class="last-update">Last updated: <span id="lastUpdate">Never</span></div>
    </div>

    <div id="settings" class="section">
      <h2>Configuration</h2>
      <div class="settings-grid">
        <div class="setting-group">
          <h3>Alert Thresholds</h3>
          <div class="setting-item">
            <label>Spread Alert (%)</label>
            <input type="number" id="spreadThreshold" step="0.1" min="0">
          </div>
          <div class="setting-item">
            <label>Funding Alert (%)</label>
            <input type="number" id="fundingThreshold" step="0.01" min="0">
          </div>
        </div>
        <div class="setting-group">
          <h3>Refresh Intervals (seconds)</h3>
          <div class="setting-item">
            <label>Price Update Rate</label>
            <input type="number" id="priceInterval" step="1" min="1">
          </div>
          <div class="setting-item">
            <label>Funding Update Rate</label>
            <input type="number" id="fundingInterval" step="1" min="1">
          </div>
        </div>
      </div>
      <div class="buttons">
        <button class="btn btn-save" onclick="saveSettings()">Save Settings</button>
        <button class="btn btn-reset" onclick="resetSettings()">Reset to Defaults</button>
      </div>
      <div id="settingsStatus" class="status"></div>
    </div>

    <div id="ota" class="section">
      <h2>Firmware Update</h2>
      <p style="color: #8F8F8F; margin-bottom: 15px;">Upload new firmware (.bin file)</p>
      <form method="POST" action="/update" enctype="multipart/form-data">
        <input type="file" name="firmware" accept=".bin" required 
               style="background: #0B0E11; border: 1px solid #2B3139; color: #F0F0F0; padding: 10px; border-radius: 4px; margin-bottom: 15px;">
        <br>
        <button type="submit" class="btn btn-save">Upload Firmware</button>
      </form>
    </div>
  </div>

  <script>
    const symbols = ["BTC/USDT", "ETH/USDT"];
    let updateInterval;

    // Navigation
    document.querySelectorAll(".nav-btn").forEach(btn => {
      btn.addEventListener("click", () => {
        document.querySelectorAll(".nav-btn").forEach(b => b.classList.remove("active"));
        document.querySelectorAll(".section").forEach(s => s.classList.remove("active"));
        btn.classList.add("active");
        document.getElementById(btn.dataset.section).classList.add("active");
      });
    });

    // Fetch and display prices
    async function updatePrices() {
      try {
        const response = await fetch("/api/prices");
        const data = await response.json();
        
        const grid = document.getElementById("cryptoGrid");
        grid.innerHTML = "";
        
        data.symbols.forEach(symbol => {
          const card = document.createElement("div");
          card.className = "crypto-card";
          
          const spreadClass = symbol.spread_pct >= 0 ? "spread-positive" : "spread-negative";
          const fundingClass = symbol.funding_rate >= 0 ? "funding-positive" : "funding-negative";
          
          card.innerHTML = 
            "<div class='crypto-name'>" + symbol.name + "</div>" +
            "<div class='price-row'>" +
              "<span class='price-label'>Binance</span>" +
              "<span class='price-value price-binance'>$" + symbol.binance_price.toFixed(2) + "</span>" +
            "</div>" +
            "<div class='price-row'>" +
              "<span class='price-label'>Coinbase</span>" +
              "<span class='price-value price-coinbase'>$" + symbol.coinbase_price.toFixed(2) + "</span>" +
            "</div>" +
            "<div class='price-row'>" +
              "<span class='price-label'>Spread</span>" +
              "<span class='price-value " + spreadClass + "'>" + (symbol.spread_pct >= 0 ? "+" : "") + symbol.spread_pct.toFixed(3) + "%</span>" +
            "</div>" +
            "<div class='price-row'>" +
              "<span class='price-label'>Funding Rate</span>" +
              "<span class='price-value " + fundingClass + "'>" + (symbol.funding_rate >= 0 ? "+" : "") + (symbol.funding_rate * 100).toFixed(4) + "%</span>" +
            "</div>";
          grid.appendChild(card);
        });
        
        document.getElementById("lastUpdate").textContent = new Date().toLocaleTimeString();
      } catch (error) {
        console.error("Failed to fetch prices:", error);
      }
    }

    // Fetch and display settings
    async function loadSettings() {
      try {
        const response = await fetch("/api/settings");
        const data = await response.json();
        
        document.getElementById("spreadThreshold").value = data.spread_alert_threshold;
        document.getElementById("fundingThreshold").value = data.funding_alert_threshold;
        document.getElementById("priceInterval").value = data.price_update_interval_sec;
        document.getElementById("fundingInterval").value = data.funding_update_interval_sec;
      } catch (error) {
        console.error("Failed to load settings:", error);
      }
    }

    // Save settings
    async function saveSettings() {
      const settings = {
        spread_alert_threshold: parseFloat(document.getElementById("spreadThreshold").value),
        funding_alert_threshold: parseFloat(document.getElementById("fundingThreshold").value),
        price_update_interval_sec: parseInt(document.getElementById("priceInterval").value),
        funding_update_interval_sec: parseInt(document.getElementById("fundingInterval").value)
      };
      
      try {
        const response = await fetch("/api/settings", {
          method: "POST",
          headers: { "Content-Type": "application/json" },
          body: JSON.stringify(settings)
        });
        
        const result = await response.json();
        showStatus(result.success ? "success" : "error", result.message);
      } catch (error) {
        showStatus("error", "Failed to save settings");
      }
    }

    // Reset to defaults
    async function resetSettings() {
      if (!confirm("Reset all settings to factory defaults?")) return;
      
      try {
        const response = await fetch("/api/settings/reset", { method: "POST" });
        const result = await response.json();
        
        if (result.success) {
          await loadSettings();
          showStatus("success", result.message);
        } else {
          showStatus("error", result.message);
        }
      } catch (error) {
        showStatus("error", "Failed to reset settings");
      }
    }

    // Show status message
    function showStatus(type, message) {
      const status = document.getElementById("settingsStatus");
      status.className = "status " + type;
      status.textContent = message;
      status.style.display = "block";
      setTimeout(() => { status.style.display = "none"; }, 3000);
    }

    // Initialize
    loadSettings();
    updatePrices();
    updateInterval = setInterval(updatePrices, 5000); // Update every 5 seconds
  </script>
</body>
</html>
)====";

void dashboard_init(WebServer* server) {
    // Serve dashboard homepage
    server->on("/dashboard", HTTP_GET, [server]() {
        server->send_P(200, "text/html", dashboard_html);
    });

    // API: Get current prices
    server->on("/api/prices", HTTP_GET, [server]() {
        AppState state = model_snapshot();
        StaticJsonDocument<1024> doc;
        JsonArray symbols_array = doc.createNestedArray("symbols");
        
        for (int i = 0; i < 2; i++) {  // BTC and ETH
            JsonObject symbol = symbols_array.createNestedObject();
            symbol["name"] = state.symbols[i].symbol_name;
            symbol["binance_price"] = state.symbols[i].binance_quote.valid ? state.symbols[i].binance_quote.price : 0.0;
            symbol["coinbase_price"] = state.symbols[i].coinbase_quote.valid ? state.symbols[i].coinbase_quote.price : 0.0;
            symbol["spread_pct"] = state.symbols[i].spread_valid ? state.symbols[i].spread_pct : 0.0;
            symbol["funding_rate"] = state.symbols[i].funding.valid ? state.symbols[i].funding.rate : 0.0;
        }
        
        String response;
        serializeJson(doc, response);
        server->send(200, "application/json", response);
    });

    // API: Get settings
    server->on("/api/settings", HTTP_GET, [server]() {
        const AppConfig& cfg = config_get();
        StaticJsonDocument<256> doc;
        doc["spread_alert_threshold"] = cfg.spread_alert_pct;
        doc["funding_alert_threshold"] = cfg.funding_alert_pct;
        doc["price_update_interval_sec"] = cfg.price_refresh_ms / 1000;
        doc["funding_update_interval_sec"] = cfg.funding_refresh_ms / 1000;
        
        String response;
        serializeJson(doc, response);
        server->send(200, "application/json", response);
    });

    // API: Update settings
    server->on("/api/settings", HTTP_POST, [server]() {
        if (server->hasArg("plain")) {
            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, server->arg("plain"));
            
            if (!error) {
                config_set_spread_alert_pct(doc["spread_alert_threshold"]);
                config_set_funding_alert_pct(doc["funding_alert_threshold"]);
                config_set_price_refresh_ms(doc["price_update_interval_sec"].as<int>() * 1000);
                config_set_funding_refresh_ms(doc["funding_update_interval_sec"].as<int>() * 1000);
                
                config_save();
                
                StaticJsonDocument<128> response;
                response["success"] = true;
                response["message"] = "Settings saved successfully!";
                
                String json;
                serializeJson(response, json);
                server->send(200, "application/json", json);
                return;
            }
        }
        
        StaticJsonDocument<128> response;
        response["success"] = false;
        response["message"] = "Invalid request";
        
        String json;
        serializeJson(response, json);
        server->send(400, "application/json", json);
    });

    // API: Reset to defaults
    server->on("/api/settings/reset", HTTP_POST, [server]() {
        // Reset to default values
        config_set_spread_alert_pct(0.5);
        config_set_funding_alert_pct(0.01);
        config_set_price_refresh_ms(5000);
        config_set_funding_refresh_ms(60000);
        config_save();
        
        StaticJsonDocument<128> response;
        response["success"] = true;
        response["message"] = "Settings reset to defaults!";
        
        String json;
        serializeJson(response, json);
        server->send(200, "application/json", json);
    });
}

void dashboard_handle() {
    // Reserved for WebSocket handling if implemented
}

#endif // ENABLE_OTA
