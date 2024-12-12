/**
 * TODO: RLD, LeadOFF detection,
 */

#include <Arduino.h>
#include <SPI.h>
#include "ads129xDriver.h"

/* ============== GPIO pins connected to ADS chip ===========*/
const int IPIN_PWDN = 5;
const int PIN_CLKSEL = 9;
const int IPIN_RESET = 6;
const int PIN_START = 7;
const int IPIN_DRDY = 8;
const int IPIN_CS = 10;
const int DAISY_IN = 4;

// extra defines
#define BIAS_SENSP 0x0D
#define BIAS_SENSN 0x0E
#define LOFF_SENSP 0x0F
#define LOFF_SENSN 0x10
#define LOFF_FLIP 0x11
#define LOFF_STATP 0x12
#define LOFF_STATN 0x13
#define GPIO 0x14
#define MISC1 0x15
#define MISC2 0x16
#define CONFIG4 0x17

// Constructor
ADS129xSensor adsSensor(IPIN_CS, IPIN_DRDY, IPIN_RESET); //); , PIN_START, IPIN_PWDN, PIN_CLKSEL);
/* Functions prototypes */
void printBits(byte myByte); // Helper function to print byte value in bits
void pinSetup();
void configADS1294R(void);

void setup()
{
  Serial.begin(115200);
  Serial.println("Starting the ADS setup");

  pinSetup();
  configADS1294R();
}

void loop()
{

  if (adsSensor.hasNewDataAvailable()) // Serial.println("New data available");
  {
    ads_data_t *adsData = adsSensor.getData();
    // byte[3] statusWord = adsData->formatedData.statusWord;
    // byte[3] channel1 = adsData->formatedData.channel[0];

    // Serial.println("Status word (in binary):");
    // printBits(adsData->formatedData.statusWord[0]);
    // Serial.print("\t");
    // printBits(adsData->formatedData.statusWord[1]);
    // Serial.print("\t");
    // printBits(adsData->formatedData.statusWord[2]);
    // Serial.println("");

    // Serial.println("Channel 1 sample:");
    // Serial.print(adsData->formatedData.channel[0].hi);
    // Serial.print("\t");
    // Serial.print(adsData->formatedData.channel[0].mid);
    // Serial.print("\t");
    // Serial.print(adsData->formatedData.channel[0].low);
    // Serial.print("\t");

    // Transform sample to voltage

    for (int i = 0; i < 4; i++)
    {
      byte *byteSample = (byte *)&adsData->formatedData.channel[i];
      // For an easy manipulation, transform twos complement format to binary offset. It is equivalent to an unsigned value
      // with V_Ref offset. The new zero is equivalent in the real world to maximum negative value possible
      byte mask = byteSample[0] & 0x80;
      mask ^= 0x80;
      byte offsetBinary = mask | (byteSample[0] & 0x7F);
      // Sample in binary offset of 24 bits. Remember, it has in MSB format. We can access with adsData->formatedData.channel[0].{hi,mid,low} fields
      int32_t sampleValue = (offsetBinary << 16) + (byteSample[1] << 8) + byteSample[2];

      Serial.printf(">cha%d:", i + 1);
      // Serial.println(sampleValue, DEC);
      // Remove offset and scale the variable before printing in the monitor
      float v_ref = 4; // V_ref for the ADS1294. In my setup, VREFP = 2.4V (see VREF_4V bit in config3) and VREFN = 0V (connected to ground)
      float channelGain = 6;
      float sampleInVoltsAndWithOffset = v_ref * ((float)sampleValue) / (pow(2, 23) - 1) / channelGain;
      float sampleInVolts = sampleInVoltsAndWithOffset - v_ref;

      Serial.println(sampleInVolts * 1e3, 5); // test with negative sign: (-1) *
    }
    // Serial.println();

    /*
        // Print in binary
        Serial.print("Original bits for channel 1: ");
        printBits(adsData->formatedData.channel[0].hi);
        printBits(adsData->formatedData.channel[0].mid);
        printBits(adsData->formatedData.channel[0].low);
        Serial.println("");
        Serial.print("Bits in offset binary for channel 1: ");
        Serial.println(sampleValue, BIN);
    */

    // Read new data. We aren't in RDATAC mode
    adsSensor.sendSPICommandRDATA();
  }

  // delay(1000);
}

void printBits(byte myByte)
{
  for (byte mask = 0x80; mask; mask >>= 1)
  {
    if (mask & myByte)
      Serial.print('1');
    else
      Serial.print('0');
  }
}

void pinSetup()
{
  pinMode(IPIN_CS, OUTPUT);
  pinMode(PIN_START, OUTPUT);
  pinMode(IPIN_DRDY, INPUT);
  pinMode(PIN_CLKSEL, OUTPUT);
  pinMode(IPIN_RESET, OUTPUT);
  pinMode(IPIN_PWDN, OUTPUT);

  // Start ADS1298
  delay(500); // wait for the ads129n to be ready - it can take a while to charge caps
  digitalWrite(PIN_CLKSEL, HIGH);
  delay(1);
  digitalWrite(IPIN_PWDN, HIGH);  // turn off power down mode (9.3.2.2 Power-Down Pin (PWDN))
  digitalWrite(IPIN_RESET, HIGH); // (9.3.2.3 Reset (RESET Pin and Reset Command))
  delay(100);
  digitalWrite(IPIN_RESET, LOW);
  delay(1);
  digitalWrite(IPIN_RESET, HIGH);
  delay(1);   // Wait for 18 tCLKs AKA 9 microseconds, we use 1 millisec
  delay(100); // pause to provide ads129n enough time to boot up...

  Serial.println("ADS1294 setup OK");
}

void configADS1294R(void)
{
  //  MUST call begin() method before any other method of ADS129xSensor
  adsSensor.begin();

  // It is not necessary Only for example purposes
  Serial.println("Example: reset value for all registers without reset command");
  adsSensor.setAllRegisterToResetValuesWithoutResetCommand();
  delayMicroseconds(500);

  // Read ADS129x ID:
  byte regValue = adsSensor.readRegister(ads::registers::id::REG_ADDR);

  Serial.print("chip ID register: ");
  printBits(adsSensor.readRegister(ads::registers::id::REG_ADDR));
  Serial.println("");

  Serial.print("Set sampling read to 1 kHz and low-power mode");
  Serial.print("Keep in mind that when config1 or resp registers are changed, internal reset is performed. See the datasheet, section Reset");

  /**
   * CONFIG
   */
  // By default, ADS12xx is in low-power consumption and with a sample frequency of 250 Hz
  adsSensor.writeRegister(ads::registers::config1::REG_ADDR, ads::registers::config1::LOW_POWR_250_SPS); // LOW_POWR_250_SPS is the best
  delayMicroseconds(150);

  Serial.print("The new value CONFIG1 register is ");
  printBits(adsSensor.readRegister(ads::registers::config1::REG_ADDR));

  /**
   * Select test signal from chip
   * As example, this 2 methods will keep the SPI open for ADS129x chip for faster configuration. The difference It's not noticeable for humans
   * Be careful when you use this option. Read the documentation before using it.
   */
  adsSensor.writeRegister(ads::registers::config2::REG_ADDR, ads::registers::config2::TEST_SOURCE_INTERNAL, true);
  // We will use the square signal at 4 Hz
  adsSensor.writeRegister(ads::registers::config2::REG_ADDR, ads::registers::config2::TEST_FREQ_4HZ, true);
  delayMicroseconds(150);

  /**
   * Setup of my circuit. In my case, it hadn't external reference,
   * If you change inividual bits with constants B_xx, you must add with the RESERVED_BITS constant value to be sure that you will
   * write the right bits in the reserved bits in the register.
   * Remember to write all desired configuration in a register  simultaneously. When you write a register, you delete all previous values
   */
  Serial.println("Enabling internal reference buffer --> set PD_REFBUF to 1");
  adsSensor.writeRegister(ads::registers::config3::REG_ADDR, 0xDC); // ads::registers::config3::B_PD_REFBUF | ads::registers::config3::B_VREF_4V | ads::registers::config3::RESERVED_BITS  //| ads::registers::config3::B_VREF_4V

  // Wait for internal reference to wake up. See page 15, section Electrical Characteristicsm in the datasheet,
  delayMicroseconds(150);

  Serial.println("Starting channels configuration");
  Serial.println("Channel 1: gain 6 and ELECTRODE input");
  adsSensor.enableChannelAndSetGain(1, ads::registers::chnSet::GAIN_6X, ads::registers::chnSet::TEST_SIGNAL);
  delay(10);
  Serial.println("Channel 2: gain 6 and ELECTRODE input");
  adsSensor.enableChannelAndSetGain(2, ads::registers::chnSet::GAIN_6X, 0x50); // ads::registers::chnSet::ELECTRODE_INPUT
  delay(10);
  // Serial.println("Channel 3: power-down and its inputs shorted (as Texas Instruments recommends)");
  // adsSensor.disableChannel(3, true);
  Serial.println("Channel 3: gain 6 and ELECTRODE input");
  adsSensor.enableChannelAndSetGain(3, ads::registers::chnSet::GAIN_6X, 0x50); // ads::registers::chnSet::ELECTRODE_INPUT
  delay(10);

  Serial.println("Channel 4 : set gain 6 and ELECTRODE input");
  adsSensor.enableChannelAndSetGain(4, ads::registers::chnSet::GAIN_6X, 0x50); // ads::registers::chnSet::ELECTRODE_INPUT
  delay(10);
  // to do : bias &value of channel;

  adsSensor.writeRegister(BIAS_SENSP, 0x00);
  delay(10);
  adsSensor.writeRegister(BIAS_SENSN, 0x00);
  delay(10);

  adsSensor.writeRegister(LOFF_SENSP, 0xFF);
  delay(10);
  adsSensor.writeRegister(LOFF_SENSN, 0x02);
  delay(10);

  adsSensor.writeRegister(LOFF_FLIP, 0x00);
  delay(10);

  adsSensor.writeRegister(LOFF_STATP, 0xF1);
  delay(10);
  adsSensor.writeRegister(LOFF_STATN, 0x00);
  delay(10);

  adsSensor.writeRegister(GPIO, 0x00);
  delay(10);

  adsSensor.writeRegister(MISC1, 0x00);
  delay(10);
  adsSensor.writeRegister(MISC2, 0xF0);
  delay(10);

  adsSensor.writeRegister(ads::registers::config4::REG_ADDR, 0x22);
  delay(10);

  adsSensor.writeRegister(0x18, 0x0A);
  delay(10);

  adsSensor.writeRegister(0x19, 0xE3);
  delay(10);

  adsSensor.writeRegister(0, 17);
  delay(1000);

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
