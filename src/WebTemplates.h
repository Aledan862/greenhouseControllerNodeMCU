// Web page structure is described as follows.
// It contains two tokens as {{STYLE}} and {{LEDIO}} also 'led'
//  parameter for GET method.
static const char PROGMEM _PAGE_LED[] = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8" name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv="refresh" content="5">
  <title>{{ARCH}} Control</title>
  <style type="text/css">
  body {
      -webkit-appearance:none;
      background-color: #222;
      }
p {
  font-family:'Arial',sans-serif;
  font-weight:bold;
  text-align:center;
  color: #ffffff;
}
h1 {
    text-align:center;
    color: #ffffff;
}
h3 {
  color: chartreuse;
}

.button {
  display:block;
  width:150px;
  margin:10px auto;
  padding:7px 13px;
  text-align:center;
  background:#668ad8;
  font-size:20px;
  color:#ffffff;
  white-space:nowrap;
  box-sizing:border-box;
  -webkit-box-sizing:border-box;
  -moz-box-sizing:border-box;
}
.button:active {
  font-weight:bold;
  vertical-align:top;
  padding:8px 13px 6px;
}
.one a {text-decoration:none;}
.img {text-align:center;}

.container {
  width: 400px;
  height: 200px;
  position: relative;
  top: 100px;
  left: 50%;
  overflow: hidden;
  text-align: center;
  transform: translate(-50%, -50%);
}

.gauge-a {
  z-index: 1;
  position: absolute;
  background-color: rgba(255,255,255,.2);
  width: 400px;
  height: 200px;
  top: 0%;
  border-radius: 250px 250px 0px 0px;
}

.gauge-b {
  z-index: 4;
  position: absolute;
  background-color: #222;
  width: 250px;
  height: 125px;
  top: 75px;
  margin-left: 75px;
  margin-right: auto;
  border-radius: 250px 250px 0px 0px;
}

.gauge-c {
  z-index: 2;
  position: absolute;
  background-color: #5664F9;
  width: 400px;
  height: 200px;
  top: 200px;
  margin-left: auto;
  margin-right: auto;
  border-radius: 0px 0px 200px 200px;
  transform-origin: center top;
  transition: all 1.3s ease-in-out;
  transform: rotate({{VAL}}deg)
}

.gauge-sp {
  z-index: 3;
  position: absolute;
  background-color: #56f95f;
  width: 200px;
  height: 10px;
  top: 195px;
  margin-left: auto;
  margin-right: auto;
  transform-origin: right center;
  transform: rotate({{SP}}deg)
}

.container:hover .gauge-data { color: rgba(255,255,255,1); }

.gauge-data {
  z-index: 4;
  color: rgba(255,255,255,.2);
  font-size: 1.5em;
  line-height: 25px;
  position: absolute;
  width: 400px;
  height: 200px;
  top: 90px;
  margin-left: auto;
  margin-right: auto;
  transition: all 1s ease-out;
}

  </style>
</head>
<body>
  <h1>Температура котла</h1>
<div class="container">
    <div class="gauge-a"></div>
    <div class="gauge-b"></div>
    <div class="gauge-c"></div>
    <div class="gauge-sp"></div>
    <div class="gauge-data">
        <h2 id="curent">{{VALUE}} &degC</h2>
        <h3 id="set">Уст: {{SETPOINT}} &degC</h3>
    </div>
</div>        
    
<div class="one">
    <p><a class="button" href="/?led=on">ON</a></p>
    <p><a class="button" href="/?led=off">OFF</a></p>
</div>
<div class="img">
    <img src="{{LEDIO}}"/>
</div>
</body>
</html>
)rawliteral";

static const char _PNG_LED[] PROGMEM = {
"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAJYAAACWCAYAAAA8AXHiAAAQHUlEQVR"
"4Xu2dC1RUxxnHF3ygIKIIiqiIgLwUAQFBUPCFCT7BqPhIWz1tba1Nc1Jjm5PTJibntOZhz0nSNqdJ"
"05q0VoUYARXREFSQxyIqL+X9UAIi8goo+IhCvwHWLOxddu/ufczAd88ZWWHmm2/+329nZ+fOzDVR4"
"IUKiKCAiQg20SQqoECwEAJRFECwRJEVjSJYyIAoCiBYosiKRhEsZEAUBRAsUWRFowgWMiCKAgiWKL"
"KiUQQLGRBFAQRLFFnRKIKFDIiiAIIliqxoFMFCBkRRAMESRVY0imAhA6IogGCJIisaRbCQAVEUQLB"
"EkRWNIljIgCgKIFh6yNqtUIw6ERj5G6uH9yxKrRwO7Uk79K0exYZ1FgRLR/h37vzIdm/KJ6Vzam5M"
"JFmbx03qypwdtGtdbuK/hjU5OhqPYOkQKN095OuQkoxw9Wz5jt7t3jfz7UC8BwgXtwII1iBkwEfgi"
"DSv0NrQwjQ79WwNVlMUU9oarEC8dgQLweLNAIA1smqKU7NTQ9V4jsLWAFYrb6PDpAD2WIP3WKOr7J"
"wane5wgjUJxGsZJpzwbiaCpRMsZwCrkqvHQrAG0Q7BGhysUX09lhVHNhsQr5n3W3mYFECwDO+xECz"
"ssQzrJsjEaKWdU5Mz9xgLwUKwDAYLBu9ax1i20N03GWZ56Jdi8qMQepLREJrvwXl4Kd4Fxs0ArLta"
"Bu+igwX1m0LrRkrRVqFVZAqsU14r3UeMHRnjVlPiWGnnXNLyZOS2LdeTKoUWRWVPB1iTQbxGseqO2"
"vPFpL0XPk22a6l3LnXwvPnoUdemDflnysSqT2i7zIBFeqm6SdPapjXXjVGJUGE/u+NTn0iX98+8f0"
"doYYg9ucDat2qf3Y+Kz5V5VRdYqtpVDm2dfbt8IgTsezHaKrRNZsA6sXDNvqis0+8NFOAbnxVV4Xn"
"fOAstTB9YltVTnGpncc+8T4c8t4X+OAaYTTI9Qm4GF2c4DGzTieB1r76QefIvYrRVaJvMgJXis2z/"
"srzzb3IJcHzR5o83pcfuMUYcCCjpCcceCv/p5qkt9UHWHc0e5p33XWc1VE+weNSpoRPc6mmvnzS18"
"oGZ+bcjFF1X7ppaHom+Gl8PGTuN8SMxYNWh"
"VTlndnDZSPELf3PF1eS3jbEvVVlmwHoz/Nf20aUpNR41xSMGitMxxqI7YcH6JdvTjqTxEQ5gGnt80"
"cZA6462V2y+u7vUub7C0uJhBx8T/fLWTHZ4XDrNPe/BWIuYdZlxn/G9SR0Tui109eWEi+CDRlxu2c"
"58OrPxFvkovGewgxIWZAYsosnhpT95Y/uFL97i0qd8mkvn7LoKcpvloS79Di3Z4Ti5s+kTGBgvnl9"
"xdayu/Ib8vXG8bVeR45zSu1ZTd2+6dDQD/HoymB3SY0IbmqEN5lz5jix5cf/2i4c5226If2KXYQos"
"IkbGnJDc4BsZPlzCpM4Lu7SkIDVUm2ixi7eG2TfXfOhTlTePq1cQS+wc14CmOptp70Rmxn+kbfB9E"
"XwPK0hdxOVDpmdIXkhRhq9Y/olhlzmwXt/w+tQdyuPls2+XWXAJciI4Ega48f0GuBc8lsw1GaWI9S"
"+74mHx8L4YOuplEwBrrJnq8vLG1KNH1Quc"
"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"
"jkw+bzwZvW3SANY9uBuJaQx+ruNOXUo8GP4+3/"
"lVIkGsEj7P4D0spxCDKG6r0NbfCDJ+pBMWsAi3w7zITkMoQDL1ZRQqPiSXJWr6qUFLOLP/"
"D5BzOUWheH6d4Pv5BaZ7BdNYBEx/CDFQMJbPPzQIHcvfksLVMR12sAiPo2H9DYkHHPpBxf52CPr2U"
"r1yy5NLhrBUrV8Ebz4JSSyQ8dRGjmYqaUFPM2ElADpMxq9phksdb3IuIsM7Mn2L1Z8FiPequ1fvQe"
"kUHwN5yBRHBb2XUOw2I8hlS1AsKgMC/tOIVjsx5DKFiBYVIaFfacQLPZjSGULECwqw8K+UwgW+zGk"
"sgUIFpVhYd8pBIv9GFLZAgSLyrCw7xSCxX4MqWwBgkVlWNh3CsFiP4ZUtgDBojIs7DuFYLEfQypbg"
"GBRGRb2nUKw2I8hlS1AsKgMC/tOIVjsx5DKFiBYVIaFfaf+DwPIBOI/NbGrAAAAAElFTkSuQmCC"
};
