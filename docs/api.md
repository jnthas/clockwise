# Clockwise API

Clockwise comes with a small built in server that provides a few calls to control your device. 

We're documenting these here for easy referencing and access, but you can also use our [openapi.yml](/docs/openapi.yml) file with your OpenAPI tools of choice.

For all requests (except "GET /") you will receive the HTTP status code 204 - No Content when the call was successfull.

## GET /

This call returns the HTML for the settings page of your Clockwise device.

## GET /get

Returns all clockwise related information about the device as "X-"-headers:
- `X-displayBright`: Display brightness
- `X-autoBrightMin`: Auto brightness minimum
- `X-autoBrightMax`: Auto brightness maximum
- `X-swapBlueGreen`: Is RBG used instead of RGB (blue & green are switched)?
- `X-use24hFormat`: Use the 24h format instead of 12h for the clock
- `X-ldrPin`: Which GPIO pin is currently defined for the LDR
- `X-timeZone`: Currently selected time zone
- `X-wifiSsid`: SSID the device is connected to
- `X-ntpServer`: URL of the NTP server the device is connected to
- `X-canvasFile`: Name of the canvas file that is currently being used
- `X-canvasServer`: Address of the canvas server that is currently being used
- `X-CW_FW_VERSION`: Clockwise firmware version
- `X-CW_FW_NAME`: Clockwise firmware name
- `X-CLOCKFACE_NAME`: Clockface name (e.g. `cw-cf-0x01`)

## GET /read

Getting pin information

## POST /restart

Can be used to restart the device.

## POST /set

You can set specific attributes of the device by using the following query parameters:

- `displayBright`: Change display brightness
- `wifiSsid`:  Wifi SSID to be used
- `wifiPwd`: Wifi password to be used
- `autoBright`: Set min and max values for autoBrightness (e.g. 0010,0800)
- `swapBlueGreen`: Swap the blue and green color. RBG instead of RGB ("1" = true)
- `use24hFormat`: Use the 24h format ("1" = true)
- `ldrPin`: Define which GPIO pin the LDR is connected to (default = 35)
- `timeZone`: Set time zone name to be used
- `ntpServer`: Define URL of NTP server to be used
- `canvasFile`: Name of the canvas file that should be used (e.g. my-theme.json)
- `canvasServer`: URL of the server where the canvas file is saved
- `manualPosix`: Avoid remote lookups of ezTime, by providing a posix string

So for example if you want to set the brightness to 80 and your device has the IP 192.168.0.42, the easiest way to set the display brightness would be to cURL your device like this:
```
curl -X POST http://192.168.0.42/set?displayBright=80
```

**Be advised** 

Right now you can only update a single attribute within a single request and afterwards your device needs to be restarted!