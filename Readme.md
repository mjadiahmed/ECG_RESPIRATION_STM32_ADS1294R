![Capture d'écran 2024-12-12 172749](https://github.com/user-attachments/assets/fb33b076-e0e3-48b5-9656-291b1b83f54d)

![Capture d'écran 2024-12-12 172540](https://github.com/user-attachments/assets/5f2ac5b1-e95a-4731-9e89-3c8d51c96cfa)


# Channel Settings (Table 22. Individual Channel Settings (n = 1 to 8) Field Descriptions)

set to input short by setting the appropriate MUXn[2:0] = 001 of the CHnSET register.

## Channel input

These bits determine the channel input selection.

- 000 = Normal electrode input
- 001 = Input shorted (for offset or noise measurements)
- 010 = Used in conjunction with RLD_MEAS bit for RLD measurements. See the Right Leg Drive (RLD) DC Bias Circuit subsection of the ECG-Specific Functions section for more details.
- 011 = MVDD for supply measurement
- 100 = Temperature sensor
- 101 = Test signal
- 110 = RLD_DRP (positive electrode is the driver)
- 111 = RLD_DRN (negative electrode is the driver)


# make ECG simulator

https://www.instructables.com/ECG-Simulator/
