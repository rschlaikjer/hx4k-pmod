EESchema Schematic File Version 4
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 16 16
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Oscillator:ASE-xxxMHz X?
U 1 1 5ED38F4C
P 1550 1450
AR Path="/5E2CE2B8/5ED38F4C" Ref="X?"  Part="1" 
AR Path="/5E3186DD/5ED38F4C" Ref="X?"  Part="1" 
AR Path="/5E6E6211/5ED38F4C" Ref="X?"  Part="1" 
AR Path="/5ED38F4C" Ref="X?"  Part="1" 
AR Path="/5ED383BF/5ED38F4C" Ref="X1"  Part="1" 
F 0 "X1" H 1700 1700 50  0000 L CNN
F 1 "ECS-2520MV-120" H 1800 1350 50  0000 L CNN
F 2 "_Oscillators:XTAL_OSC_ECS_2520" H 2250 1100 50  0001 C CNN
F 3 "http://www.abracon.com/Oscillators/ASV.pdf" H 1450 1450 50  0001 C CNN
F 4 "XC2749DKR-ND" H 1800 1250 50  0000 L CNN "DigiKey PN"
F 5 "ECS-2520MV-120-BN-TR" H 1550 1450 50  0001 C CNN "Manufacturer PN"
	1    1550 1450
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR?
U 1 1 5ED38F53
P 1550 1000
AR Path="/5E2CE2B8/5ED38F53" Ref="#PWR?"  Part="1" 
AR Path="/5E3186DD/5ED38F53" Ref="#PWR?"  Part="1" 
AR Path="/5E6E6211/5ED38F53" Ref="#PWR?"  Part="1" 
AR Path="/5ED38F53" Ref="#PWR?"  Part="1" 
AR Path="/5ED383BF/5ED38F53" Ref="#PWR0164"  Part="1" 
F 0 "#PWR0164" H 1550 850 50  0001 C CNN
F 1 "+3.3V" H 1565 1173 50  0000 C CNN
F 2 "" H 1550 1000 50  0001 C CNN
F 3 "" H 1550 1000 50  0001 C CNN
	1    1550 1000
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5ED38F59
P 1200 1250
AR Path="/5E2CE2B8/5ED38F59" Ref="R?"  Part="1" 
AR Path="/5E3186DD/5ED38F59" Ref="R?"  Part="1" 
AR Path="/5E6E6211/5ED38F59" Ref="R?"  Part="1" 
AR Path="/5ED38F59" Ref="R?"  Part="1" 
AR Path="/5ED383BF/5ED38F59" Ref="R1"  Part="1" 
F 0 "R1" H 1130 1204 50  0000 R CNN
F 1 "10k" H 1130 1295 50  0000 R CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 1130 1250 50  0001 C CNN
F 3 "~" H 1200 1250 50  0001 C CNN
F 4 "RMCF0805FT10K0TR-ND" H 0   0   50  0001 C CNN "DigiKey PN"
F 5 "RMCF0805FT10K0" H 0   0   50  0001 C CNN "Manufacturer PN"
	1    1200 1250
	1    0    0    1   
$EndComp
Wire Wire Line
	1200 1400 1200 1450
Wire Wire Line
	1200 1450 1250 1450
Wire Wire Line
	1200 1100 1200 1050
Wire Wire Line
	1200 1050 1550 1050
Wire Wire Line
	1550 1050 1550 1000
Wire Wire Line
	1550 1050 1550 1150
Connection ~ 1550 1050
$Comp
L power:GND #PWR?
U 1 1 5ED38F67
P 1550 1750
AR Path="/5E2CE2B8/5ED38F67" Ref="#PWR?"  Part="1" 
AR Path="/5E3186DD/5ED38F67" Ref="#PWR?"  Part="1" 
AR Path="/5E6E6211/5ED38F67" Ref="#PWR?"  Part="1" 
AR Path="/5ED38F67" Ref="#PWR?"  Part="1" 
AR Path="/5ED383BF/5ED38F67" Ref="#PWR0165"  Part="1" 
F 0 "#PWR0165" H 1550 1500 50  0001 C CNN
F 1 "GND" H 1555 1577 50  0000 C CNN
F 2 "" H 1550 1750 50  0001 C CNN
F 3 "" H 1550 1750 50  0001 C CNN
	1    1550 1750
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5ED38F6D
P 900 1250
AR Path="/5E2CE2B8/5ED38F6D" Ref="C?"  Part="1" 
AR Path="/5E3186DD/5ED38F6D" Ref="C?"  Part="1" 
AR Path="/5E6E6211/5ED38F6D" Ref="C?"  Part="1" 
AR Path="/5ED38F6D" Ref="C?"  Part="1" 
AR Path="/5ED383BF/5ED38F6D" Ref="C1"  Part="1" 
F 0 "C1" H 1014 1204 50  0000 L CNN
F 1 "0.1uF" H 1014 1295 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 938 1100 50  0001 C CNN
F 3 "~" H 900 1250 50  0001 C CNN
F 4 "1276-1003-1-ND" H 0   0   50  0001 C CNN "DigiKey PN"
F 5 "CL21B104KBCNNNC" H 0   0   50  0001 C CNN "Manufacturer PN"
	1    900  1250
	-1   0    0    1   
$EndComp
Wire Wire Line
	1850 1450 2300 1450
Wire Wire Line
	900  1100 900  1050
Wire Wire Line
	900  1050 1200 1050
Connection ~ 1200 1050
$Comp
L power:GND #PWR?
U 1 1 5ED38F79
P 900 1400
AR Path="/5E2CE2B8/5ED38F79" Ref="#PWR?"  Part="1" 
AR Path="/5E3186DD/5ED38F79" Ref="#PWR?"  Part="1" 
AR Path="/5E6E6211/5ED38F79" Ref="#PWR?"  Part="1" 
AR Path="/5ED38F79" Ref="#PWR?"  Part="1" 
AR Path="/5ED383BF/5ED38F79" Ref="#PWR0166"  Part="1" 
F 0 "#PWR0166" H 900 1150 50  0001 C CNN
F 1 "GND" H 905 1227 50  0000 C CNN
F 2 "" H 900 1400 50  0001 C CNN
F 3 "" H 900 1400 50  0001 C CNN
	1    900  1400
	1    0    0    -1  
$EndComp
Text HLabel 2300 1450 2    50   Output ~ 0
CLOCK_12MHZ
Text Notes 600  750  0    100  ~ 0
12MHz Oscillator
Wire Notes Line
	550  550  550  2000
Wire Notes Line
	550  550  2950 550 
Wire Notes Line
	2950 550  2950 2000
Wire Notes Line
	550  2000 2950 2000
$EndSCHEMATC
