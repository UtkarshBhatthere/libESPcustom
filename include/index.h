#ifndef _INDEX_H_
#define _INDEX_H_

#include "string.h"

char* webpage = "<!DOCTYPE html>\\
<html>\\  
    <title>Microphone Setup</title>\\
    <body>\\
        <form action=# method='post'>\\
            <div>\\
              <label for='SSID'>Enter ssid here</label>\\
              <input type='text' name='SSID'>\\
            </div>\\
            <div>\\
              <label for='Password'>Enter password here</label>\\
              <input type='password' name='Password'>\\
            </div>\\
            <div>\\ 
              <button>Submit</button>\\
            </div>\\
        </form>\\
    </body>\\
</html>";

char* page = "<!DOCTYPE html>\\
<html>\\
<head>\\
<style>\\
body {font-family: Arial, Helvetica, sans-serif;}\\
form {border: 3px solid #f1f1f1;}\\
input[type=text], input[type=password] {\\
  width: 100%;\\
  padding: 12px 20px;\\
  margin: 8px 0;\\
  display: inline-block;\\
  border: 1px solid #ccc;\\
  box-sizing: border-box;\\
}\\
button {\\
  background-color: #4CAF50;\\
  color: white;\\
  padding: 14px 20px;\\
  margin: 8px 0;\\
  border: none;\\
  cursor: pointer;\\
  width: 100%;\\
}\\
button:hover {\\
  opacity: 0.8;\\
}\\
.container {\\
  padding: 16px;\\
}\\
span.psw {\\
  float: right;\\
  padding-top: 16px;\\
}\\
</style>\\
</head>\\
<body><h2>MiFi Setup</h2>\\
<form  action=# method='post'>\\
  <div class='container'>\\
    <label for='ssid'><b>WiFi ID:</b></label>\\
    <input type='text' placeholder='SSID' name='SSID' required>\\
    <label for='password'><b>Password</b></label>\\
    <input type='password' placeholder='Enter Password' name='password' required>\\
    <label for='IP'> <b>Target IP</b> </label>\\
    <input type='text' placeholder='xxx.xxx.xxx.xxx' name='IP' required>\\
    <button type='submit'>Setup</button>\\
  </div>\\
  </div>\\
</form>\\
</body>\\
</html>";

int webpage_len = strlen(webpage);
int page_len = strlen(page);
#endif /* @_INDEX_H_ */