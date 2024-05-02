# Marsh's Project Working Log: Habit Forming Key Station

[[_TOC_]]

# 2024-03-05 - Initial PCB Design

Activities:

* Began initial design of the PCB tailored for supporting both subsystems.
* Selected and placed components on the schematic, focusing on a compact and efficient layout.

Progress:

* Completed the initial draft of the PCB schematic.

Changes Made:

* Switched from using a general-purpose microcontroller to a more power-efficient model better suited for real-time processing requirements.


# 2024-03-10 - PCB Layout Adjustments

Activities:

* Adjusted the PCB layout to optimize the routing of traces for power distribution and signal integrity.

Progress:

* Identified optimal trace widths and clearances to minimize noise and cross-talk between high-frequency components.

Changes Made:

* Moved the location of the power regulator to reduce electromagnetic interference with the Proximity Detection Subsystem.

![schematic of our keydish PCB design](key_dish_pcb.png)



![Front-side of our Keydish PCB design](keydish.png)


And here are the schematic and front-side view of our keychain PCB:

![](keychain_pcb.png)
![](keychain.png)



# 2024-03-17 - Confirmation Subsystem Component Tuning

I have contacted several custom battery builders in the esk8 (custom electric longboard builders) forum, located [here](https://forum.esk8.news/).  Builder Zach Tetra from [Black Fox Builds](https://forum.esk8.news/t/black-fox-boards-east-coast-battery-building-services/37402) has the following pair of 6s4p batteries that he can send immediately:

![](blackfox_battery.png)

For our project, I would prefer a 12s battery for adequate performance. The pack above is made up of Samsung 30Q cells, I found more info [here](https://lygte-info.dk/review/batteries2012/Samsung%20INR18650-30Q%203000mAh%20(Pink)%20UK.html). We can convert the 6s4p pack into a 12s2p pack, which will be nearly 200Whr.
This will be plenty, for up to 10 miles of range. I have never worked on batteries before. When researching how to do so, I found this over-an-hour-long video [here]( https://www.youtube.com/watch?v=7QjO90LG67g), that clearly depicts and describes everything required to safely build a reliable battery pack.

I will need access to a spot welder to complete this - we’ve reached out to Illini Solar Car, as they have claimed the spot welder on campus.

# 2021-02-18 - Battery Update

Received the pair of 6s4p batteries, along with battery building supplies such as a Smart BMS, nickel strips, fishpaper, 10 gauge wire, and more. Upon closer inspection, splitting the 6s4p pack into a 12s2p pack is difficult, as there is no leverage to remove the existing H-shaped nickel strips.

The pair of 6s4p packs take up a lot less space than I expected. It would be easier to make a 12s4p pack out of them instead, which will provide us the same power as I required from earlier at nearly 500 Whr. Additionally, I have gotten in contact with a coworker who has a battery spot welder I can use.

Once I have planned out the build and assembled the materials, we should be good to go to complete the battery.

# 2021-02-24 - Investigation acquiring RPM data from VESCs

I have done some investigating of the VESC hardware and software and discovered that you can query them for RPM telemetry data. This removes the need for us to develop RPM sensing
around the motorized wheels.

To accomplish this however, we may need to talk to each VESC individually. Having a dual VESC is convenient, but this may interfere with our ability to query data from them properly. Some dual VESCs have dual MCUs, other more recent designs have single MCUs as shown earlier in this journal.

# 2021-03-01 - Parts update

From further discussions about the issue of communicating to both VESCs individually or to a ‘master’ VESC to both, we have decided to go with a pair of single VESCs. For example, we could attach a CANBUS cable between the pair of VESCs in order to allow them to communicate with each other, or simply split two PPM signals to pass throttle information individually, among various other choices. In other words, the VESCs can work in tandem with one another or can be independent from each other.

I have assembled a significant portion of the board at this point, as seen here in this picture below:

![](parts_update.png)

Some issues:

- The motor pulley is scraping on the motor mount. This is an issue of tolerances, as the
motor mount I am using is 6mm wide and the Motor shaft has a tolerance of 6.2mm +-
0.5mm. In theory, we should have 0.2mm of space, but the margin of error was not in our
favor.
  ![](motor_pulley_scraping.png)
- The threads on the ends of the trucks are not deep enough. We need bearing spacers to
push the wheel out.
  ![](bearing_spacer.png)

