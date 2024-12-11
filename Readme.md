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

https://forum.arduino.cc/t/arduino-and-ti-ads1298-way-of-data-out-suggestion/444389/2
void adsNormalElectrode()
{
adc_wreg(0x01, 0x04); //Config1 //0x04 500SPS
adc_wreg(0x02, 0x10); //Config2 //0x10
adc_wreg(0x03, 0xDC); //Config3 //0xDC
adc_wreg(0x04, 0x03); //LOFF //0x03
adc_wreg(0x05, 0x00); //CH1SET//0x00
adc_wreg(0x06, 0x00);
adc_wreg(0x07, 0x00);
adc_wreg(0x08, 0x00);
adc_wreg(0x09, 0x00);
adc_wreg(0x0A, 0x00);
adc_wreg(0x0B, 0x00);
adc_wreg(0x0C, 0x00); //CH8SET //0x00

adc_wreg(0x0D, 0x00); //RLD_SENSP //0x00
adc_wreg(0x0E, 0x00); //RLD_SENSN //0x00  
 adc_wreg(0x0F, 0xFF); //LOFF_SENSP //0xFF
adc_wreg(0x10, 0x02); //LOFF_SENSN //0x02
adc_wreg(0x11, 0x00); //LOFF_FLIP //0x00
adc_wreg(0x12, 0x00); //LOFF_STATP //0x00
adc_wreg(0x13, 0x00); //LOFF_STATN //0x00
adc_wreg(0x14, 0x00); //GPIO //0x00
adc_wreg(0x15, 0x00); //PACE //0x00
adc_wreg(0x16, 0x00); //RESP //0x00
adc_wreg(0x17, 0x00); //CONFIG4 //0x00
adc_wreg(0x18, 0x00); //WCT1 //0x00
adc_wreg(0x19, 0x00); //WCT2 //0x00
}
