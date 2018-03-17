# Sample Application using IoT C++ SDK

## Overview
This sample application uses IoT C++ SDK to fill and send sample IoT data (Custom, Network and Error Events) to AppDynamics Collector. Sample Data reflects a Smart Car Application capturing usage information, network performance and errors.

## Steps

1. Build Sample Application using instructions given in [README](../README.md#Installation)
2. [Get EUM App Key](https://docs.appdynamics.com/display/PRO44/Set+Up+and+Access+IoT+Monitoring#SetUpandAccessIoTMonitoring-iot-app-key)
3. [Run the Sample App](#run-sample-app)
4. [Confirm IoT Data is reported to Collector](https://docs.appdynamics.com/display/PRO44/Confirm+the+IoT+Application+Reported+Data+to+the+Controller)
5. To Customize Instrumentation review [Getting Started](https://docs.appdynamics.com/pages/viewpage.action?pageId=42597347) and [SDK API docs](../docs)
6. Refer to [REST API docs](https://docs.appdynamics.com/javadocs/iot-rest-api/4.4/latest/)  for payload structure sent to IoT Collector


## <a id="run-sample-app"></a>Run the Sample App
You can find the executable `sample` in the build folder 

Below is usage information for the sample app. Provide AppKey as an input along with any options as needed.

```sh
USAGE: ./sample <appkey> [options]
options:
-collectorurl <url> Collector URL to which sdk will send events to.
-logfile <file>     Log file to which sdk log messages are written to.
-loglevel <level>   Log level set to one of the values:
                    1-error, 2-warn, 3-info, 4-debug, 5-verbose, 6-all.
-sendevent <event>  Sample event to be sent. Set to one of the following
                    event types: 1-custom, 2-network, 4-error, 7-all.
-url <url>          URL to trigger network request and capture network event.
-request <command>  Specify the request type to url. It is set to GET by default.
-data <data>        Data in JSON format that is to be sent in a POST request.
-help               Display available options
```

Below are the default values used by the sample app if above options are not given
```sh
DEFAULT PARAMS:
collectorurl = https://iot-col.eum-appdynamics.com
logfile = appd_iot_sdk.log
loglevel = 6
sendevent = 7
```

Here are some examples of using the sample app:

Display Usage Information
```sh
$ ./sample -h
```

Send Sample Custom, Network, and Error Events to default APPD Collector (https://iot-col.eum-appdynamics.com)
```sh
$ ./sample <appkey>
```

Send Sample Custom, Network, and Error Events to Custom Collector (http://localhost:9001)
```sh
$  ./sample <appkey> -c http://localhost:9001
```

Send Sample Custom, Network, and Error Events to Custom Collector (http://localhost:9001) and publish log messages to custom log file (mylog.log)
```sh
$  ./sample <appkey> -c http://localhost:9001 -f mylog.log
```

Send Sample Custom, Network, and Error Events to Custom Collector (http://localhost:9001) and publish log messages to mylog.log with loglevel set to warn
```sh
$  ./sample <appkey> -c http://localhost:9001 -f mylog.log -l 2
```

Send Sample Custom Event to Custom Collector (http://localhost:9001)
```sh
$  ./sample <appkey> -c http://localhost:9001 -s 1
```

Send Sample Network Event to Custom Collector (http://localhost:9001)
```sh
$  ./sample <appkey> -c http://localhost:9001 -s 2
```

Trigger POST Network Request to URL (http://yoururl.com) with data in JSON format. Capture and Send Network Event to Custom Collector (http://localhost:9001)
```sh
$  ./sample <appkey> -c http://localhost:9001 -s 2 -u http://yoururl.com -x POST -d '{"param1"="value1"}'
```

Send Sample Error Event to Custom Collector (http://localhost:9001)
```sh
$  ./sample <appkey> -c http://localhost:9001 -s 4
```
