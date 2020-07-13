idf_component_register(SRCS 	"main.c"
				"src/bluetooth.c"
			        "src/memorymanager.c"
				"src/sensormanager.c"
				"src/wifimanager.c"
                    INCLUDE_DIRS "include")

idf_component_register(
SRCS 				"main.c"
				"src/bluetooth.c"
				"src/wifimanager.c"
				"src/memorymanager.c"
				"src/httpmanager.c"
				"src/sensormanager.c"




INCLUDE_DIRS 			"include"
			)