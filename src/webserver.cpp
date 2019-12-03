#include "webserver.h"

// Get an architecture of compiled
String getArch(PageArgument& args) {

  return "ESP8266";
}

// This function is the logic for BUILTIN_LED on/off.
// It is called from the occurrence of the 'LEDIO' token by PageElement
//  'Button' declaration as following code.
String ledIO(PageArgument& args) {
  String ledImage = "";

  // Blinks BUILTIN_LED according to value of the http request parameter 'led'.
  if (args.hasArg("led")) {
    if (args.arg("led") == "on")
      digitalWrite(BUILTIN_LED, LOW);
    else if (args.arg("led") == "off")
      digitalWrite(BUILTIN_LED, HIGH);
  }
  delay(10);

  // Feedback the lighting icon depending on actual port value which
  //  indepent from the http request parameter.
  if (digitalRead(BUILTIN_LED) == LOW)
    ledImage = FPSTR(_PNG_LED);
  return ledImage;
}