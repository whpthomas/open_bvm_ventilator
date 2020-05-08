# Bill of Materials

This is the bill of materials for the [Open BVM Ventilator](https://github.com/whpthomas/open_bvm_ventilator) project. Where possible I have chosen widely available common off-the-shelf (COTS) components. However the machine screws I used are metric which may be harder to source in North America.

## Medical Supplies

Part|Description|Quantity
----|----|----
![Laerdal Bag II](Laerdal-Bag-II.jpg)|[Laerdal Bag II](https://www.laerdal.com/au/item/845111), [LSR](https://www.laerdal.com/au/item/87005133), or equivalent adult sized bag valve mask hand  resuscitator (e.g. Ambu Bag).|1
![PEEP Valve](peep-valve.jpg)|[PEEP Valve](https://www.laerdal.com/au/item/845240) (5-20 cm H2O).|1
![HEPA Filter](filter.jpg)|HEPA Filter.|1
![Extension Tube](extension-tube.jpg)|[Silicone Extension Tube](https://www.laerdal.com/au/item/871000).|1
![Manometer Connector](manometer-connector.jpg)|[Manometer connector](https://www.laerdal.com/au/item/850900). This barb requires a 6mm oxygen hose.|Option

## Hardware

Part|Description|Quantity
----|----|----
![Silicon Hose](silicon-hose.jpg)|5mm OD x 3mm ID x 250mm L food grade silicon hose.|1
![Hose Fitting](4mm_1-8_BSP-hose-barb.jpg)|1/8" BSP to 4mm barb hose fitting. This barb protrudes from the back of the housing base and is used to connect the manifold hose to the pressure sensor. If you decide to use the optional [manometer connector](https://www.laerdal.com/au/item/850900) a 1/8" BSP to **6mm** barb hose fitting should be used.|1
![Bearing](608-2RS-bearing.jpg)|608-2RS 22mm OD x 8mm ID x 7mm W skate bearings.|11
![M3x6mm Machine Screws](M3x6mm-socket-cap.jpg)|M3 x 6mm stainless socket head cap screws are used to attach the stepper motor to the ring mount, and the Arduino Uno, stepper driver and endstop circuit boards to the base.|13
![M3x12mm Machine Screws](M3x12mm-socket-cap.jpg)|M3 x 12mm stainless socket head cap screws are used to attach the interface front cover to the interface panel.|4
![M3x16mm Machine Screws](M3x16mm-socket-cap.jpg)|M3 x 16mm stainless socket head cap screws are used to attach the [Reprap Discount Full Graphic Smart Controller](https://reprap.org/wiki/RepRapDiscount_Full_Graphic_Smart_Controller) to the interface panel.|2
![M4x12mm Countersink Screws](M4x12mm-countersink.jpg)|M4 x 12mm stainless countersink screws are used to assemble the cradle, hinges and main housing.|13
![M4x16mm Machine Screws](M4x16mm-socket-cap.jpg)|M4 x 16mm stainless socket head cap screws are used to assemble the planetary gear module and attache it to the housing.|12
![M8x16mm Machine Screws](M8x20mm-socket-cap.jpg)|M8 x 16mm stainless socket head cap screws are used to attach bearings to the planet intermediate carrier to gang 1, bearings the planetary output carrier to gang 2 and bearings to the tie rod between the planetary output and press arm.|10


## Electronic components

Part|Description|Quantity
----|----|----
![Arduino Uno](arduino-uno-rev-3.jpg)|Arduino Uno Rev 3|1
![BME280 Sensor Module](bme280-sensor.jpg)|BME280 humidity, barometric pressure and temperature sensor module with 3 wire I2C interface.|1
![Makerbot endstop](makerbot-endstop.jpg)|Makerbot 3D printer endstop.|1
![Stepper Motor](Nema17-60mm-Stepper-Motor.jpg)|NEMA17 2 phase stepper motor with 1.8 degree step angle, 60mm motor length, and 1.5A current rating.|1
![Power Supply](power-supply.jpg)|12 volt 1 amp AC-DC power supply with 5.5mm OD 2.5mm ID center-positive DC plug.|1
![RGB LED](rcgb-led.png)|Common cathode 5mm RGB LED.|1
![Arduino Uno](reparp-full-graphic-smart.jpg)|[Reprap Discount Full Graphic Smart Controller](https://reprap.org/wiki/RepRapDiscount_Full_Graphic_Smart_Controller). This component is a widely available LCD interface controller commonly used on many 3D printers.|1
![Stepper Driver](TB6560.jpg)| TB6560 stepper driver.|1
![Jumper Wires](jumper_wires.jpg)|Bread board jumper wires|20 leads

## 3D printed parts

These parts can be printed in PLA or PETG on a 200mm W x 150mm D x 150mm H bed.

Part|Description|Quantity
----|----|----
![Base](3dp-base.jpg)|`base.stl` This part comprises the main body of the ventilator housing. The circuit boards and planetary drive are mounted inside this enclosure.|1
![Bearing Pin](3dp-bearing_pin.jpg)|`bearing_pin.stl`These press fitted parts connect the the 608 bearings in the press arm assembly to the left an right hinge plates.|2
![Control Knob](3dp-control_knob.jpg)|`control_knob.stl` This part is press fitted onto the encoder of the [Reprap Discount Full Graphic Smart Controller](https://reprap.org/wiki/RepRapDiscount_Full_Graphic_Smart_Controller)|1
![Cradle](3dp-cradle.jpg)|`cradle.stl` This part links the housing base and interface panel with the left and right hinge plates.|1
![Hex Link](3dp-hex_link.jpg)|`hex_link.stl` This part connects the Planet Intermediate carrier with the Planetary Gear module 2.|1
![Hinge Plate Right](3dp-hinge_plate_l.jpg)|`hinge_plate_l.stl` This part connects the press arm assembly to the the cradle and housing.|1![Hinge Plate Left](3dp-hinge_plate_r.jpg)|`hinge_plate_r.stl` This part connects the press arm assembly to the the cradle and housing.|1![Interface Front Cover](3dp-interface_front_cover.jpg)|`interface_front_cover.stl` This part attaches to the front interface panel and holds the [Reprap Discount Full Graphic Smart Controller](https://reprap.org/wiki/RepRapDiscount_Full_Graphic_Smart_Controller) in place.|1![Interface Panel](3dp-interface-panel.jpg)|`interface-panel.stl` This part is attached to the front of the base housing.|1![Mask Manifold](3dp-mask_manifold.jpg)|`mask_manifold.stl` This part attaches between the mask and valve and connects the opposite end of the 5mm silicon hose with the pressure sensor.|1![Motor Cover](3dp-motor_cover.jpg)|`motor_cover.stl` This part snaps onto the interface panel and covers the rear of the stepper motor.|1![Planet Intermediate](3dp-planet_intermediate.jpg)|`planet_intermediate.stl` This carrier holds the planetary gang 1 bearings in place and drives gang 2 via the hex link.|1![Planet Output](3dp-planet_output.jpg)|`planet_output.stl` This carrier holds the planetary gang 2 bearings in place and drives the tie rod.|1![Planetary Gear Module Gang 1](3dp-planetary_gang_1.jpg)|`planetary_gang_1_x-x.stl` This part is the gang 1 print-in-place planetary gear module. There are two versions of this file, one for a steeper motor with a 3.9mm keyway and the other for a 4.5mm keyway.|1![Planetary Gear Module Gang 2](3dp-planetary_gang_2.jpg)|`planetary_gang_2.stl` This part is the gang 2 print-in-place planetary gear module which is driven via the hex link by the intermediate carrier.|1![Press Bracket](3dp-press_bracket.jpg)|`press_bracket.stl` This bracket is attached to opposite side of the press arm.|1![Press Arm](3dp-press-arm.jpg)|`press-arm.stl` This part is attached via bearings and pins to the hinge plates.|1![Pressure Sensor Base](3dp-pressure_sensor_base.jpg)|`pressure_sensor_base.stl` This part encloses the BME280 sensor, that should be sealed with Sikaflex 227 (or equivalent).|1![Pressure Sensor Mount](3dp-pressure_sensor_mount.jpg)|`pressure_sensor_mount.stl` This part connects the BME280 sensor to the 4mm hose barb and is attached to the base with cyanoacrylate gel glue.|1![Ring Mount](3dp-ring_mount.jpg)|`ring_mount.stl` This part connects the planetary gear module to the stepper motor.|1![Ring Spacer](3dp-ring_spacer.jpg)|`ring_spacer.stl` This part connects the planetary gear gang 1 & 2 together.|1![Spacer](3dp-spacer.jpg)|`spacer.stl` This part separates the tie rod bearing from the planetary output carrier.|1![Stop Button](3dp-stop-button.jpg)|`stop-button.stl` This part links the tactile switch on the [Reprap Discount Full Graphic Smart Controller](https://reprap.org/wiki/RepRapDiscount_Full_Graphic_Smart_Controller) with the interface panel.|1
![Tie Rod](3dp-tie-rod.jpg)|`3dp-tie-rod.stl` This part links the planetary output carrier with the press arm.|1
