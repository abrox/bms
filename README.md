# Battery Management System
...Work in progress....

This project implements SOC based battery monitor.
And it's part of my [Vessel Monitor](https://github.com/abrox/vessel-monitor)



## SYSTEM CONFIGURATION
System configuration must be loaded trough serial connection.
Before take it in use. JSOn format is used.
	
	{
		"rev": {
			"maj": 1,
			"min": 1
		},
		"wlan": {
			"ssid": "MyNetworknŃame123456",
			"pass": "VerySecretPass123456"
		},
		"mqtt": {
			"id": "fooBar1234",
			"ip": "1111.222.333.444"
		}
	}

