EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A 11000 8500
encoding utf-8
Sheet 5 5
Title "Cell Monitor - Arduino based16-bit"
Date "2019-11-23"
Rev "A0"
Comp "simple-ee.com"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Wire Wire Line
	5800 4025 6050 4025
Wire Wire Line
	5800 3925 6050 3925
Wire Wire Line
	5800 3825 6000 3825
Text Label 6000 3825 2    50   ~ 0
GND
Text Label 6000 3925 2    50   ~ 0
SCL
Text Label 6000 4025 2    50   ~ 0
SDA
$Comp
L power:+5V #PWR0503
U 1 1 5D860C6F
P 5825 3575
F 0 "#PWR0503" H 5825 3425 50  0001 C CNN
F 1 "+5V" H 5825 3725 50  0000 C CNN
F 2 "" H 5825 3575 50  0001 C CNN
F 3 "" H 5825 3575 50  0001 C CNN
	1    5825 3575
	1    0    0    -1  
$EndComp
Wire Wire Line
	5825 3575 5825 3725
Wire Wire Line
	5825 3725 5800 3725
$Comp
L power:GND #PWR0501
U 1 1 5D98D54D
P 5750 3075
F 0 "#PWR0501" H 5750 2825 50  0001 C CNN
F 1 "GND" H 5750 2925 50  0000 C CNN
F 2 "" H 5750 3075 50  0001 C CNN
F 3 "" H 5750 3075 50  0001 C CNN
	1    5750 3075
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 3025 5750 3025
Wire Wire Line
	5750 3025 5750 3075
Wire Wire Line
	5700 2725 5950 2725
Wire Wire Line
	5700 2825 5950 2825
Wire Wire Line
	5700 2925 5950 2925
Text Label 5900 2825 2    50   ~ 0
A_RX
Text HLabel 5950 2825 2    50   Output ~ 0
A_RX
$Comp
L power:+5V #PWR0502
U 1 1 5E177008
P 5775 2475
F 0 "#PWR0502" H 5775 2325 50  0001 C CNN
F 1 "+5V" H 5775 2625 50  0000 C CNN
F 2 "" H 5775 2475 50  0001 C CNN
F 3 "" H 5775 2475 50  0001 C CNN
	1    5775 2475
	1    0    0    -1  
$EndComp
Wire Wire Line
	5700 2625 5775 2625
Wire Wire Line
	5775 2625 5775 2475
Text HLabel 6050 3925 2    50   BiDi ~ 0
SCL
Text HLabel 6050 4025 2    50   BiDi ~ 0
SDA
Text HLabel 5950 2925 2    50   Input ~ 0
A_TX
Text HLabel 5950 2725 2    50   BiDi ~ 0
DTR
$Comp
L Display:096OLED P502
U 1 1 5D852461
P 5750 3875
F 0 "P502" H 5600 4175 50  0000 C CNN
F 1 "096OLED" H 5600 3575 50  0000 C CNN
F 2 "Display:096OLED_NOMOUNTS" H 5950 3225 50  0001 C CNN
F 3 "" H 5775 3975 50  0000 C CNN
F 4 "3V3-5V" H 5700 3475 60  0001 C CNN "P_Rating_U"
F 5 "DIYMALL" H 6200 3475 50  0001 C CNN "Manufacturer"
F 6 "128x64" H 6150 3575 50  0001 C CNN "P_Value"
F 7 "###" H 5700 3375 50  0001 C CNN "P_Tolerance"
F 8 "TH" H 5750 3275 50  0001 C CNN "Package"
	1    5750 3875
	1    0    0    -1  
$EndComp
$Comp
L Connectors:S5B-PH-SM4-TB(LF)(SN) P501
U 1 1 5E17614C
P 5600 2975
F 0 "P501" H 5550 2525 50  0000 C CNN
F 1 "S5B-PH-SM4-TB(LF)(SN)" H 5550 2325 60  0001 C CNN
F 2 "Connectors_sg:S5B-PH-SM4-TB" V 6200 3425 60  0001 C CNN
F 3 "" H 5550 3175 60  0001 C CNN
F 4 "JST Sales America Inc." H 5500 3275 60  0001 C CNN "Manufacturer"
F 5 "5POS" H 5550 3125 50  0000 C CNN "P_Value"
F 6 "2A" H 5850 3425 50  0001 C CNN "P_Rating"
F 7 "SMD" H 5600 3575 60  0001 C CNN "Package"
	1    5600 2975
	1    0    0    1   
$EndComp
Text Label 5900 2925 2    50   ~ 0
A_TX
Text Label 5900 2725 2    50   ~ 0
DTR
Text HLabel 5875 4850 2    50   Input ~ 0
INT0
$Comp
L Connectors:SMD_2POS P503
U 1 1 5DE768A3
P 5700 4900
F 0 "P503" H 5650 5050 60  0000 C CNN
F 1 "SMD_2POS" H 5650 4500 50  0001 C CNN
F 2 "Connectors_sg:SMD_2POS" V 5850 4950 60  0001 C CNN
F 3 "" H 5650 5100 60  0001 C CNN
F 4 "CUSTOM" H 6050 5000 50  0001 C CNN "Manufacturer"
F 5 "2POS" H 5650 4750 50  0000 C CNN "P_Value"
F 6 "##" H 6050 4800 60  0001 C CNN "P_Rating_U"
F 7 "2.5MM" H 6050 4900 60  0001 C CNN "P_Tolerance"
F 8 "SMD" H 6050 4700 60  0001 C CNN "Package"
	1    5700 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 4850 5875 4850
$Comp
L power:GND #PWR0504
U 1 1 5DE7719B
P 5850 5000
F 0 "#PWR0504" H 5850 4750 50  0001 C CNN
F 1 "GND" H 5850 4850 50  0000 C CNN
F 2 "" H 5850 5000 50  0001 C CNN
F 3 "" H 5850 5000 50  0001 C CNN
	1    5850 5000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5800 4950 5850 4950
Wire Wire Line
	5850 4950 5850 5000
$EndSCHEMATC
