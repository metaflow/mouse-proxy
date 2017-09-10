#include <HID-Project.h>
#include <HID-Settings.h>

#include "hidboot.h"
#include "usbhub.h"
// #include <Mouse.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

// Uncomment for debug to Serial.
//#define __SERIAL

const uint8_t multiplier = 3;

class MouseRptParser : public MouseReportParser {
public:
  MouseRptParser() : changeSensetivity(false) {}
protected:
  void OnMouseMove  (MOUSEINFO *mi);
  void Debug(bool is_rpt_id, uint8_t len, uint8_t *buf) override;
  void OnButtonDown(uint8_t button) override;
  void OnButtonUp(uint8_t button) override;
  bool changeSensetivity;
};

void MouseRptParser::OnMouseMove(MOUSEINFO *mi)
{
  int16_t dx = mi->dX;
  int16_t dy = mi->dY;
#if defined(__SERIAL)
//  Serial.print("x ");
//  Serial.print(mi->dX);
//  Serial.print(" y ");
//  Serial.println(mi->dY);
#endif
  if (changeSensetivity) {
    dx *= multiplier;    
    dy *= multiplier;
  }    
  Mouse.move(dx, dy, mi->wheel);
};

void MouseRptParser::OnButtonDown(uint8_t button) {
  if (button == (1u << 3)) {
    changeSensetivity = true;
    return;
  }
  //Serial.print("D ");
  //Serial.println(button);
  Mouse.press(button);
};

void MouseRptParser::OnButtonUp(uint8_t button) {
  if (button == (1u << 3)) {
    changeSensetivity = false;
    return;
  }
  //Serial.print("U ");
  //Serial.println(button);
  Mouse.release(button);
};

void MouseRptParser::Debug(bool is_rpt_id, uint8_t len, uint8_t *buf) {
#if not defined(__SERIAL)
  return;
#endif
  return;
  Serial.print("DEBUG: ");
//  Serial.println(is_rpt_id ? "true" : "false");
//  Serial.println(len);
  for (int i = 0; i < len; i++) {
    Serial.print(buf[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
};

USB Usb;
USBHub Hub(&Usb);
HIDBoot<USB_HID_PROTOCOL_MOUSE> HidMouse(&Usb, true);
uint32_t next_time;
MouseRptParser Prs;

void setup()
{
  Mouse.begin();
  Mouse.move(10,10,0);
#if defined(__SERIAL)
  Serial.begin(9600);
#if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
   Serial.println("Start");
   Serial.println(MOUSE_LEFT);
   Serial.println(MOUSE_RIGHT);
   Serial.println(MOUSE_PREV);
#endif   

    if (Usb.Init() == -1) {
//        Serial.println("OSC did not start.");
    }

    delay(200);
    next_time = millis() + 5000;
    HidMouse.SetReportParser(0, &Prs);
}

void loop()
{
  Usb.Task();
  //Mouse.move(1,1,0);
//  delay(100);
}

