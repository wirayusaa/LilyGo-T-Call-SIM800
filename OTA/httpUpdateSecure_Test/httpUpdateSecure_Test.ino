/**
   httpUpdateSecure.ino

    Created on: 16.10.2018 as an adaptation of the ESP8266 version of httpUpdate.ino

*/

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>
#include <HTTPUpdate.h>

#include <time.h>

WiFiMulti WiFiMulti;

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(7*3600, 3*3600, "pool.ntp.org");  // UTC

  Serial.print(F("Waiting for NTP time sync: "));
  time_t now = time(nullptr);
  Serial.print("Now  ");
  Serial.println(now);
  while (now < 8 * 3600 * 2) {
    yield();
    delay(500);
    Serial.print(F("."));
    now = time(nullptr);
  }

  Serial.println(F(""));
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

/**
 * This is lets-encrypt-x3-cross-signed.pem
 */
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIGaTCCBVGgAwIBAgIQF3QeaQZFEugMSyGx0hIeZDANBgkqhkiG9w0BAQsFADBy\n" \
"MQswCQYDVQQGEwJVUzELMAkGA1UECBMCVFgxEDAOBgNVBAcTB0hvdXN0b24xFTAT\n" \
"BgNVBAoTDGNQYW5lbCwgSW5jLjEtMCsGA1UEAxMkY1BhbmVsLCBJbmMuIENlcnRp\n" \
"ZmljYXRpb24gQXV0aG9yaXR5MB4XDTIxMDEzMTAwMDAwMFoXDTIxMDUwMTIzNTk1\n" \
"OVowGTEXMBUGA1UEAxMOZWxlZmFudGUuY28uaWQwggEiMA0GCSqGSIb3DQEBAQUA\n" \
"A4IBDwAwggEKAoIBAQCynOCEdLZUzWZJV0dHdXdRU159Vwo82JxI9b3+74H/lMRZ\n" \
"RdRia+IGWaH6Epld5zdOQNa63JufTZpg8SU6Zu/RPco9wnhZkfufD6ds2t2/qGlk\n" \
"ZrpTZd46PnGdm9AzCSQXgXZAK1xrJg1YLSwxBdmLJ62VcBz7NB1CKR4PZ4tIfKlN\n" \
"RSLOBdwqJhiyQSpOx8GBmrxD0us3LyWUzqJg8P7lJL2YEviftAnh8OVl2jUkepJo\n" \
"18wi3WJcTpUARR0KWrA3VJPXe6EzBMpebvl8yMALnKwd1j5im73jBIB+lnRixmfh\n" \
"Kd/sTlaD4k8dUZnyWAqA5eMC0dFsWdnyEj4MKk5/AgMBAAGjggNSMIIDTjAfBgNV\n" \
"HSMEGDAWgBR+A1plQWunfgrhuJ0I6h2OHWrHZTAdBgNVHQ4EFgQUANh/XcJ1lEF+\n" \
"L46sj9RTrR6LNikwDgYDVR0PAQH/BAQDAgWgMAwGA1UdEwEB/wQCMAAwHQYDVR0l\n" \
"BBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMEkGA1UdIARCMEAwNAYLKwYBBAGyMQEC\n" \
"AjQwJTAjBggrBgEFBQcCARYXaHR0cHM6Ly9zZWN0aWdvLmNvbS9DUFMwCAYGZ4EM\n" \
"AQIBMEwGA1UdHwRFMEMwQaA/oD2GO2h0dHA6Ly9jcmwuY29tb2RvY2EuY29tL2NQ\n" \
"YW5lbEluY0NlcnRpZmljYXRpb25BdXRob3JpdHkuY3JsMH0GCCsGAQUFBwEBBHEw\n" \
"bzBHBggrBgEFBQcwAoY7aHR0cDovL2NydC5jb21vZG9jYS5jb20vY1BhbmVsSW5j\n" \
"Q2VydGlmaWNhdGlvbkF1dGhvcml0eS5jcnQwJAYIKwYBBQUHMAGGGGh0dHA6Ly9v\n" \
"Y3NwLmNvbW9kb2NhLmNvbTCCAQUGCisGAQQB1nkCBAIEgfYEgfMA8QB2AH0+8viP\n" \
"/4hVaCTCwMqeUol5K8UOeAl/LmqXaJl+IvDXAAABd1njaGAAAAQDAEcwRQIhAMkL\n" \
"ygLmnTNM+QkN0HuJ1It/HtL7ZnVNZL8+6Hbxlp0AAiB4r4ZRyViSeHsUX1jEki0p\n" \
"frKjeEbVckLG+Jmb4RiLwQB3AJQgvB6O1Y1siHMfgosiLA3R2k1ebE+UPWHbTi9Y\n" \
"TaLCAAABd1njaIUAAAQDAEgwRgIhAKNim5p87+KMBaBWROF8ukc+8QH7xhDhAQAS\n" \
"h/62bnwpAiEAvN7xJybz8vCJTWPj0Mm0CUIiEyIBI9nPxlOfK+HOlnkwga0GA1Ud\n" \
"EQSBpTCBooIOZWxlZmFudGUuY28uaWSCFWNwYW5lbC5lbGVmYW50ZS5jby5pZIIa\n" \
"Y3BjYWxlbmRhcnMuZWxlZmFudGUuY28uaWSCGWNwY29udGFjdHMuZWxlZmFudGUu\n" \
"Y28uaWSCFndlYmRpc2suZWxlZmFudGUuY28uaWSCFndlYm1haWwuZWxlZmFudGUu\n" \
"Y28uaWSCEnd3dy5lbGVmYW50ZS5jby5pZDANBgkqhkiG9w0BAQsFAAOCAQEAPOoz\n" \
"qcYifObak2lWQ/gBZZ8dnysxXgzE2fAQ6Fbcr24Dq508v/I7n0qEal+jB6JiEKDg\n" \
"SZnCwJfM909vLQ0o7KwR7Lou2d3QePjmNqju0FAC1H8veqBrVGn4hOxq5CtDnk5O\n" \
"jPzzyH7kKIH3ldbdBAkrL3POUi3C/T1ikVbVsInlHnSBhzS+5NUYQf6xZ1vSfDkW\n" \
"W8d6lef80sOABfjHWNhX9iJl3+k8UTeXZUQ6pJgFNXc3g9pH3vUFCjfyQjlhZXmX\n" \
"ZRJ2gSuXmRRIiGyDXMFY2GwN+3Wy6HtlkPCZtAaxMikiv21CnSGGN8CZutb5GUMe\n" \
"6HmXYJ7jGON7RuaHYg==\n" \
"-----END CERTIFICATE-----\n";

void setup() {

  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  Serial.println();
  Serial.println();
  Serial.println();

  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Elefante_Network", "Elefante.123");
}

void loop() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    setClock();

    WiFiClientSecure client;
    client.setCACert(rootCACertificate);

    // Reading data over SSL may be slow, use an adequate timeout
    client.setTimeout(12000 / 1000); // timeout argument is defined in seconds for setTimeout

    // The line below is optional. It can be used to blink the LED on the board during flashing
    // The LED will be on during download of one buffer of data from the network. The LED will
    // be off during writing that buffer to flash
    // On a good connection the LED should flash regularly. On a bad connection the LED will be
    // on much longer than it will be off. Other pins than LED_BUILTIN may be used. The second
    // value is used to put the LED on. If the LED is on with HIGH, that value should be passed
    // httpUpdate.setLedPin(LED_BUILTIN, HIGH);

    t_httpUpdate_return ret = httpUpdate.update(client, "https://elefante.co.id/firmware/firmware1.bin");
    // Or:
    //t_httpUpdate_return ret = httpUpdate.update(client, "server", 443, "file.bin");


    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
}
