

### Lab Entry: February 17, 2024 - Power Subsystem Initial Design and Testing
**Objective:** Design and test the Power Subsystem to ensure stable voltage supply for all components.

**Design Considerations:**
- Chose a power adapter capable of delivering 5V to 36V to accommodate various component needs.
- Integrated a voltage regulator (LM1117T-3.3) to step down voltage to 3.3V, necessary for microcontroller and sensor operations.

**Equations and Formulas:**
- Voltage Output: \( V_{out} = V_{in} \times \left(\frac{R2}{R1 + R2}\right) \) where \( V_{in} \) is the input from the power adapter, R1 and R2 are resistances in the voltage divider circuit.
  
**Bibliographic References:**
- LM1117 800-mA Low-Dropout Linear Regulator Datasheet, Texas Instruments. [Accessed: February 17, 2024].

**Testing and Debugging:**
- Setup included the power adapter connected to a breadboard with multimeter readings confirming output stability.
- Identified an issue with voltage fluctuation; added capacitors to stabilize output.

**Results and Analysis:**
- Post modification, stable 3.3V output achieved with ±0.05V tolerance.
- Test setup and multimeter readings documented for future reference.

### Lab Entry: February 28, 2024 - Alarm Subsystem Initial Design and Testing
**Objective:** Design and test the Alarm Subsystem to ensure it triggers appropriately when the user does not place keys in the designated area.

**Design Considerations:**
- Selected CMS-251472-24SP speaker for its optimal sound pressure level and frequency response for household environments.
- Integrated with ATtiny84A microcontroller to manage alarm logic based on input from the proximity detection subsystem.

**Equations and Formulas:**
- Sound Pressure Level (SPL) calculation to ensure audibility: \( SPL = 20 \log \left(\frac{P}{P_{ref}}\right) \) where \( P \) is the sound pressure and \( P_{ref} \) is the reference sound pressure.

**Bibliographic References:**
- CMS-251472-24SP Datasheet, CUI Devices. [Accessed: February 24, 2024].

**Testing and Debugging:**
- Alarm subsystem circuit set up on breadboard with connections to microcontroller and speaker.
- Tested alarm activation by simulating conditions where keys are not placed: alarm triggered as expected.

**Results and Analysis:**
- Alarm effectively audible at 80 dB across standard testing room, conforming to design specifications.
- Documented all testing setups, circuit diagrams, and results for inclusion in the final report.


### Lab Entry: March 21, 2024 - Further Testing of the Power Subsystem
**Objective:** Conduct extensive load testing on the Power Subsystem to ensure reliability under full operational conditions.

**Design Considerations:**
- Continued use of the initial power adapter and voltage regulator setup to evaluate long-term stability and performance under variable loads.

**Equations and Formulas:**
- Calculation of expected power draw: \( P = V \times I \), ensuring that total power does not exceed the capacity of the power adapter.

**Bibliographic References:**
- "Understanding Power Supplies and Simple Circuits," IEEE Spectrum, 2024. Provides insights into best practices for power stability and efficiency.

**Testing and Debugging:**
- Setup included multiple components drawing power simultaneously to simulate real-world usage.
- Monitored voltage drops and regulator heat dissipation over extended periods.

**Results and Analysis:**
- Observed minimal voltage fluctuation within acceptable limits of ±0.1V during extended testing.
- Heat dissipation was within safe limits; however, noted the need for potential heat sinks if ambient temperatures increase.

### Lab Entry: March 30, 2024 - Prototype Housing Development and Testing
**Objective:** Design and test a prototype housing for the key station to enhance aesthetic appeal and functional reliability.

**Design Considerations:**
- Explore materials that are both aesthetically pleasing and functional, such as acrylic and ABS plastic, for the prototype housing.
- Design must accommodate all internal components securely while allowing easy access for maintenance and adjustments.

**Bibliographic References:**
- "Modern Materials for Product Design," Materials Today, 2024. Discusses the properties of various polymers and composites used in consumer electronics housing.

**Testing and Debugging:**
- Constructed an initial prototype using 3D-printed ABS plastic to assess fit and ease of assembly.
- Conducted drop tests and stress tests to evaluate the durability of the materials and the integrity of the housing.

**Results and Analysis:**
- The prototype housing adequately protected internal components from impacts and provided satisfactory thermal management.
- Identified issues with assembly alignment; adjustments to design tolerances were noted for the next prototype iteration.


### Lab Entry: April 6, 2024 - CAD Design Refinement and Component Layout
**Objective:** Refine the CAD design of the key station's housing to optimize space and improve component arrangement.

**Design Considerations:**
- Utilize CAD software to create detailed 3D models of the housing, ensuring all components fit precisely with efficient use of space.
- Design features such as snap-fit connections for easier assembly and maintenance.

**Equations and Formulas:**
- Calculation of enclosure dimensions based on component sizes and necessary clearances: \( V = l \times w \times h \) where \( l, w, h \) are the length, width, and height of the housing.

**Bibliographic References:**
- "CAD Techniques in Modern Product Design," Journal of Engineering and Industrial Design, 2024. Discusses advanced CAD modeling strategies for electronic enclosures.

**Testing and Debugging:**
- Digital simulations of the assembly process to identify potential interference or alignment issues.
- Stress analysis on critical points to ensure the structure can withstand everyday use.

**Results and Analysis:**
- Adjustments made to the internal layout to reduce interference and improve airflow around heat-generating components.
- Identified the need for additional supports around the snooze button and speaker areas to prevent flexing during use.

### Lab Entry: April 9, 2024 - Prototype Iteration and User Interface Design
**Objective:** Iterate on the prototype design based on initial feedback and integrate user interface elements for better usability.

**Design Considerations:**
- Enhance the visual and tactile aspects of the user interface, such as the snooze button and LED indicators, using CAD to model these features precisely.
- Incorporate feedback mechanisms visually represented in the CAD model to assist with user interaction.

**Bibliographic References:**
- "Ergonomics in Design: User Interface and Aesthetics," Human Factors and Ergonomics, 2024. Provides guidelines for designing user-friendly interfaces and controls.

**Testing and Debugging:**
- Conduct user testing with the new prototype to gather feedback on the interface and overall usability.
- Use CAD simulations to visualize operation sequences and ensure all moving parts operate without binding.

**Results and Analysis:**
- User feedback highlighted the need for larger, more tactile buttons and clearer LED indicators, which were incorporated into the design.
- CAD revisions made to improve ergonomics, reducing the force required to activate controls and enhancing user interaction comfort.


### Lab Entry: April 15, 2024 - Final Power System Redesign
**Objective:** Implement a final redesign of the Power Subsystem due to issues with the original power adapter and to accommodate changes in voltage requirements.

**Design Considerations:**
- Switched to using an Arduino Uno as a stable 5V power source after the power adapter failed to provide consistent outputs.
- Adjusted voltage requirements across subsystems to ensure compatibility with the Arduino's output capabilities.

**Equations and Formulas:**
- Verification of power supply requirements: \( I = \frac{P}{V} \), where \( P \) is power in watts, \( V \) is voltage in volts, and \( I \) is current in amperes. This equation ensures that the Arduino can sufficiently power all subsystems without exceeding its current limitations.

**Bibliographic References:**
- Arduino Power Supply Capabilities, Arduino Official Documentation, 2024.

**Testing and Debugging:**
- Re-tested the entire system using the Arduino to ensure all subsystems function correctly with the new power setup.
- Monitored system stability and temperature to assess the impact of continuous operation on the Arduino.

**Results and Analysis:**
- The Arduino provided a stable and reliable power source, with all subsystems operating within normal parameters.
- Noted improvements in system reliability and a reduction in complexity by eliminating the external power adapter.

### Lab Entry: April 17, 2024 - Final System Assembly and Verification Testing
**Objective:** Assemble the final version of the habit-forming key station and conduct comprehensive verification testing to validate all design requirements and functionality.

**Design Considerations:**
- Integrated all subsystems within the newly designed housing, ensuring secure mounting and optimal placement for user interaction.
- Enhanced the alarm subsystem by switching from a traditional speaker to a piezo buzzer for better reliability and integration on the breadboard setup.

**Bibliographic References:**
- "Standards for Electronic Test and Verification," IEEE Standard Association, 2024.

**Testing and Debugging:**
- Conducted a series of stress tests, including power cycling and long-duration operational tests, to ensure durability and reliability.
- Systematic testing of the proximity detection, alarm functionality, and user interface responsiveness.

**Results and Analysis:**
- All subsystems met or exceeded their performance criteria, with successful integration demonstrated through seamless operation during testing.
- Feedback from user trials was overwhelmingly positive, with particular praise for the system's responsiveness and reliability.

**Additional Notes:**
- Prepared detailed documentation of all tests, including setups, results, and any anomalies, for inclusion in the final project report.
- Developed a user manual based on the testing outcomes and operational characteristics of the key station.

## Lab Entry: April 20, 2024 - Preparation of Presentation and Final Paper Outlines
### Objective
Develop comprehensive outlines for an upcoming presentation and the final project report, summarizing the project's scope, key findings, and conclusions.

### Design Considerations
- Organize content to flow logically, beginning with an introduction to the problem statement, followed by a detailed discussion of the design process, testing methodologies, and final outcomes.
- Emphasize key innovations, challenges overcome, and the practical applications of the habit-forming key station.

### Presentation Outline
1. **Introduction**
   - Overview of the project and its objectives.
   - Importance of key management and habit formation.

2. **Design Overview**
   - Description of the habit-forming key station and its components.
   - Detailed view of the subsystems: Power, Alarm, Proximity Detection, and Confirmation.

3. **Testing and Results**
   - Summary of testing phases for each subsystem.
   - Presentation of critical data and findings.
   - Discussion on the stability and efficiency improvements in the final design.

4. **Challenges and Solutions**
   - Highlight significant challenges faced during the design and testing phases.
   - Discuss the solutions implemented to overcome these challenges.

5. **Conclusion and Future Work**
   - Summary of the project’s impact on habitual behavior related to key placement.
   - Suggestions for future enhancements or related products.

6. **Q&A**
   - Open the floor for questions and provide thoughtful responses to engage the audience.

### Final Paper Outline
1. **Abstract**
   - Brief summary of the entire project and its outcomes.

2. **Introduction**
   - Detailed problem statement and the project's scope.

3. **Methodology**
   - Explanation of the design process and testing methodologies used.

4. **System Design**
   - Comprehensive details of each subsystem, including schematics and component specifications.

5. **Results**
   - Analysis of testing results, demonstrating the efficacy of the system.
   - Statistical data and graphs showcasing performance metrics.

6. **Discussion**
   - Interpretation of results.
   - Evaluation of the system’s performance against initial objectives.

7. **Challenges and Solutions**
   - Detailed account of obstacles encountered and their resolutions.

8. **Conclusion and Recommendations**
   - Conclusions drawn from the project.
   - Recommendations for future research and potential product development.

9. **References**
   - Cite all sources used in the research and development of the project.

### Testing and Debugging
- Conduct peer reviews of the outlines to ensure completeness and clarity.
- Simulate the presentation to a small audience for feedback on content delivery and comprehension.

### Results and Analysis
- Adjustments made to the presentation content based on feedback, improving clarity and engagement.
- Finalization of the paper outline to ensure it meets academic standards and thoroughly documents the project lifecycle.

### Additional Notes
- Prepare visual aids and supplementary materials for the presentation to enhance audience understanding.
- Schedule further review sessions to refine the presentation delivery and ensure all team members are prepared.
