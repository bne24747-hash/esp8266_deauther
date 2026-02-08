/* =====================
   This software is licensed under the MIT License:
   https://github.com/spacehuhntech/esp8266_deauther
   ===================== 
   MODIFIED BY: GMPRO SYSTEM
   FEATURES: SSID GMpro2, MAX SIGNAL, PERSISTENT ATTACK, MIZER STYLE
   ===================== */

extern "C" {
  #include "user_interface.h"
}

#include "EEPROMHelper.h"

// --- GMPRO ADDITION ---
#include "A_webserver.h" // INI YANG TADI GUA LUPA TAMBAHIN
// ----------------------

#include "src/ArduinoJson-v5.13.5/ArduinoJson.h"
#if ARDUINOJSON_VERSION_MAJOR != 5
#error Please upgrade/downgrade ArduinoJSON library to version 5!
#endif 

#include "oui.h"
#include "language.h"
#include "functions.h"
#include "settings.h"
#include "Names.h"
#include "SSIDs.h"
#include "Scan.h"
#include "Attack.h"
#include "CLI.h"
#include "DisplayUI.h"
#include "A_config.h"
#include "led.h"

// Run-Time Variables //
Names names;
SSIDs ssids;
Accesspoints accesspoints;
Stations     stations;
Scan   scan;
Attack attack;
CLI    cli;
DisplayUI displayUI;

simplebutton::Button* resetButton;

#include "wifi.h"

uint32_t autosaveTime = 0;
uint32_t currentTime  = 0;
bool booted = false;

void setup() {
    randomSeed(os_random());
    Serial.begin(115200);

    prnt(SETUP_MOUNT_SPIFFS);
    LittleFS.begin();
    prntln(SETUP_OK);

    EEPROMHelper::begin(EEPROM_SIZE);

    currentTime = millis();

    settings::load();

    // ==========================================
    // --- GMPRO CUSTOM MODIFICATION START ---
    settings::setSSID("GMpro2");       // Sesuai SSID yang lu mau
    settings::setPassword("Sangkur87");
    settings::setWebEnabled(true);
    settings::setHidden(false); 
    
    system_phy_set_max_tpw(82); 
    WiFi.outputPower(20.5);
    
    settings::save(true);
    // --- GMPRO CUSTOM MODIFICATION END ---
    // ==========================================

    wifi::begin();
    wifi_set_promiscuous_rx_cb([](uint8_t* buf, uint16_t len) {
        scan.sniffer(buf, len);
    });

    if (settings::getDisplaySettings().enabled) {
        displayUI.setup();
        displayUI.mode = DISPLAY_MODE::INTRO;
    }

    names.load();
    ssids.load();
    cli.load();
    scan.setup();

    if (settings::getCLISettings().enabled) {
        cli.enable();
    }

    if (settings::getWebSettings().enabled) wifi::startAP();

    // --- GMPRO WEB HANDLER ACTIVATION ---
    setupWebHandlers(); // INI JUGA WAJIB ADA BIAR DASHBOARD MIZER JALAN
    // ------------------------------------

    led::setup();
    resetButton = new ButtonPullup(RESET_BUTTON);
    
    Serial.println("GMPRO SYSTEM ONLINE. READY TO RUSH.");
}

void loop() {
    currentTime = millis();

    led::update();   
    wifi::update();  
    attack.update(); 
    displayUI.update();
    cli.update();    
    scan.update();   
    ssids.update();  

    if (settings::getAutosaveSettings().enabled
        && (currentTime - autosaveTime > settings::getAutosaveSettings().time)) {
        autosaveTime = currentTime;
        names.save(false);
        ssids.save(false);
        settings::save(false);
    }

    if (!booted) {
        booted = true;
        EEPROMHelper::resetBootNum(BOOT_COUNTER_ADDR);
    }

    resetButton->update();
    if (resetButton->holding(5000)) {
        settings::reset();
        settings::setSSID("GMpro2");
        settings::setPassword("Sangkur87");
        settings::save(true);
        delay(2000);
        ESP.restart();
    }
}
