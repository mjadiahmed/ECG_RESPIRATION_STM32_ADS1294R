
void configADS1294R(void)
{
    //  MUST call begin() method before any other method of ADS129xSensor
    adsSensor.begin();

    // It is not necessary Only for example purposes
    Serial.println("Example: reset value for all registers without reset command");
    adsSensor.setAllRegisterToResetValuesWithoutResetCommand();

    // Read ADS129x ID:
    byte regValue = adsSensor.readRegister(ads::registers::id::REG_ADDR);

    Serial.print("chip ID register: ");
    printBits(adsSensor.readRegister(ads::registers::id::REG_ADDR));
    Serial.println("");

    Serial.print("Set sampling read to 1 kHz and low-power mode");
    Serial.print("Keep in mind that when config1 or resp registers are changed, internal reset is performed. See the datasheet, section Reset");
    // By default, ADS12xx is in low-power consumption and with a sample frequency of 250 Hz

    adsSensor.writeRegister(ads::registers::config1::REG_ADDR, ads::registers::config1::LOW_POWR_250_SPS);
    Serial.print("The new value CONFIG1 register is ");
    printBits(adsSensor.readRegister(ads::registers::config1::REG_ADDR));

    /**
     * Setup of my circuit. In my case, it hadn't external reference,
     * If you change inividual bits with constants B_xx, you must add with the RESERVED_BITS constant value to be sure that you will
     * write the right bits in the reserved bits in the register.
     * Remember to write all desired configuration in a register  simultaneously. When you write a register, you delete all previous values
     */

    Serial.println("Enabling internal reference buffer --> set PD_REFBUF to 1");
    adsSensor.writeRegister(ads::registers::config3::REG_ADDR, ads::registers::config3::RESERVED_BITS | ads::registers::config3::B_RLDREF_INT | ads::registers::config3::B_PD_RLD);

    // Wait for internal reference to wake up. See page 15, section Electrical Characteristicsm in the datasheet,
    delayMicroseconds(150);

    /**
     * Select test signal from chip
     * As example, this 2 methods will keep the SPI open for ADS129x chip for faster configuration. The difference It's not noticeable for humans
     * Be careful when you use this option. Read the documentation before using it.
     */
    // adsSensor.writeRegister(ads::registers::config2::REG_ADDR, ads::registers::config2::TEST_SOURCE_INTERNAL, true);
    // We will use the square signal at 4 Hz
    adsSensor.writeRegister(ads::registers::config2::REG_ADDR, ads::registers::config2::TEST_SOURCE_INTERNAL, true);

    Serial.println("Starting channels configuration");
    Serial.println("Channel 1: gain 6 and ELECTRODE input");
    adsSensor.enableChannelAndSetGain(1, ads::registers::chnSet::GAIN_6X, ads::registers::chnSet::ELECTRODE_INPUT);
    Serial.println("Channel 2: gain 6 and ELECTRODE input");
    adsSensor.enableChannelAndSetGain(2, ads::registers::chnSet::GAIN_6X, ads::registers::chnSet::ELECTRODE_INPUT);
    // Serial.println("Channel 3: power-down and its inputs shorted (as Texas Instruments recommends)");
    // adsSensor.disableChannel(3, true);
    Serial.println("Channel 3: gain 6 and ELECTRODE input");
    adsSensor.enableChannelAndSetGain(3, ads::registers::chnSet::GAIN_6X, ads::registers::chnSet::ELECTRODE_INPUT);

    Serial.println("Channel 4 : set gain 6 and ELECTRODE input");
    adsSensor.enableChannelAndSetGain(4, ads::registers::chnSet::GAIN_6X, ads::registers::chnSet::ELECTRODE_INPUT);

    // LEAD OFF
    adsSensor.writeRegister(ads::registers::loff::REG_ADDR, 0b00000000 | 0b00000010);

    /** To be tested  , config according to frank613055 */
    // adsSensor.writeRegister(ads::registers::rldSensp::REG_ADDR, ads::registers::rldSensp::RESERVED_BITS);
    // adsSensor.writeRegister(ads::registers::rldSensn::REG_ADDR, ads::registers::rldSensn::RESERVED_BITS);
    // adsSensor.writeRegister(ads::registers::loffSensp::REG_ADDR, 0xFF); // lead off activated for 8 inputs ?
    // adsSensor.writeRegister(ads::registers::loffSensn::REG_ADDR, 0x02);
    // adsSensor.writeRegister(0x0F, 0xFF); // LOFF_SENSP //0xFF
    // adsSensor.writeRegister(0x10, 0x02); // LOFF_SENSN //0x02
    // adsSensor.writeRegister(0x11, 0x00); // LOFF_FLIP  //0x00
    // adsSensor.writeRegister(0x12, 0x00); // LOFF_STATP //0x00
    // adsSensor.writeRegister(0x13, 0x00); // LOFF_STATN //0x00
    // adsSensor.writeRegister(0x14, 0x00); // GPIO       //0x00
    // adsSensor.writeRegister(0x15, 0x00); // PACE       //0x00
    // adsSensor.writeRegister(0x16, 0x00); // RESP       //0x00
    // adsSensor.writeRegister(0x17, 0x00); // CONFIG4    //0x00
    // adsSensor.writeRegister(0x18, 0x00); // WCT1       //0x00
    // adsSensor.writeRegister(0x19, 0x00); // WCT2       //0x00

    Serial.println("Starting channels configuration");
    adsSensor.sendSPICommandSTART();

    /**
     * We need to put ADS in DATA or RDATC mode to receive new data
     * Remember that in RDATAC mode, ADS ignores any SPI command sent if it is not SDATAC command
     */
    Serial.println("Set ADS chip in read data (RDATA) mode");
    adsSensor.sendSPICommandRDATA();

    /**
     * You can could the method end() to free GPIO used pins and resources. if you don't need any more de ADS
     * You have to call begin() if you want to use again the ADS
     * */

    // adsSensor.end()
}
