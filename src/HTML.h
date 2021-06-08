const char INDEX_HTML[] =
    "<!DOCTYPE HTML>"
    "<html>"
    "<head>"
    "<meta content=\"text/html; charset=ISO-8859-1\""
    " http-equiv=\"content-type\">"
    "<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
    "<title>ESP8266 Web Form Demo</title>"
    "<style>"
    "\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; text-align:center;}\""
    "</style>"
    "</head>"
    "<body>"
    "<h3>Enter your WiFi credentials</h3>"
    "<form action=\"/\" method=\"post\">"
    "<p>"
    "<label>SSID:&nbsp;</label>"
    "<input maxlength=\"30\" name=\"ssid\"><br>"
    "<label>Key:&nbsp;&nbsp;&nbsp;&nbsp;</label><input maxlength=\"30\" name=\"password\"><br>"
    "<input type=\"submit\" value=\"Save\">"
    "</p>"
    "</form>"
    "</body>"
    "</html>";

const char SCRIPT_UPDATE_VALUE[] PROGMEM = R"rawliteral(
    "<script>
        setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("temperature").innerHTML = this.responseText;
        }
        };
        xhttp.open("GET", "/temperature", true);
        xhttp.send();
        }, 10000 ) ;

        setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("humidity").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/humidity", true);
        xhttp.send();
        }, 10000 ) ;
        setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("dustDensity").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/dustDensity", true);
        xhttp.send();
        }, 10000 )
    </script>
</html>)rawliteral";

String sendHTML(float temperature, float humidity, float dustDensity)
{
    String str = "<!DOCTYPE html><html>\n";
    str += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
    str += "<link rel=\"icon\" href=\"data:,\">\n";
    str += "<style>body { text-align: center; font-family: \"Arial\", Arial;}\n";
    str += "table { border-collapse: collapse; width:40%; margin-left:auto; margin-right:auto;border-spacing: 2px;background-color: white;border: 4px solid green; }\n";
    str += "th { padding: 20px; background-color: #008000; color: white; }\n";
    str += "tr { border: 5px solid green; padding: 2px; }\n";
    str += "tr:hover { background-color:yellow; }\n";
    str += "td { border:4px; padding: 12px; }\n";
    str += ".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }\n";
    str += "</style></head><body><h1>ESP32 Web Server Reading sensor values</h1>\n";
    str += "<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>\n";
    str += "<tr><td>Temperature</td><td><span class=\"sensor\">\n";
    str += temperature;
    str += " &deg;C</span></td></tr>\n";
    str += "<tr><td>Humidity</td><td><span class=\"sensor\">\n";
    str += humidity;
    str += " %</span></td></tr>\n";
    str += "<tr><td>Dust Density</td><td><span class=\"sensor\">\n";
    str += dustDensity;
    str += " &mu;g/m³</span></td></tr>\n";
    // str += "<tr><td>CO</td><td><span class=\"sensor\">\n";
    // str += co;
    // str += " ppm</span></td></tr>\n";
    str += "</body>\n";
    str += "<script>\n";
    str += "setInterval(loadDoc,200);\n";
    str += "function loadDoc() {\n";
    str += "var xhttp = new XMLHttpRequest();\n";
    str += "xhttp.onreadystatechange = function() {\n";
    str += "if (this.readyState == 4 && this.status == 200) {\n";
    str += "document.getElementById(\"webpage\").innerHTML =this.responseText}\n";
    str += "};\n";
    str += "xhttp.open(\"GET\", \"/\", true);\n";
    str += "xhttp.send();\n";
    str += "}\n";
    str += "</script></html>";
    return str;
}
const char SHOW_SENSOR_VALUE[] PROGMEM = R"rawliteral(
    "<!DOCTYPE html>
    <html>
    <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <link rel="icon" href="data:,">
        <style>body { text-align: center; font-family: "Arial", Arial;}
            table { border-collapse: collapse; width:40%; margin-left:auto; margin-right:auto;border-spacing: 2px;background-color: white;border: 4px solid green; }
            th { padding: 20px; background-color: #008000; color: white; }
            tr { border: 5px solid green; padding: 2px; }
            tr:hover { background-color:yellow; }
            td { border:4px; padding: 12px; }
            .sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }
        </style>
    </head>
    <body>
        <h1>ESP32 Web Server Reading sensor values</h1>
        <table>
        <tr>
            <th>MEASUREMENT</th>
            <th>VALUE</th>
        </tr>
        <tr>
            <td>Temperature</td>
            <td><span class="sensor" id="temperature"> %temperature% &deg;C </span></td>
        </tr>
        <tr>
            <td>Humidity</td>
            <td><span class="sensor" id="humidity"> %humidity% % </span></td>
        </tr>
        <tr>
            <td>Dust Density</td>
            <td><span class="sensor" id="dustDensity"> %dustDensity% &Mu;/m³ </span></td>
        </tr>
    </body>
    <script>
        setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("temperature").innerHTML = this.responseText;
        }
        };
        xhttp.open("GET", "/temperature", true);
        xhttp.send();
        }, 10000 ) ;

        setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("humidity").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/humidity", true);
        xhttp.send();
        }, 10000 ) ;
        setInterval(function ( ) {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("dustDensity").innerHTML = this.responseText;
            }
        };
        xhttp.open("GET", "/dustDensity", true);
        xhttp.send();
        }, 10000 )
    </script>
</html>)rawliteral";
