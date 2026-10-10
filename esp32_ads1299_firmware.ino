#include <Arduino.h>
#include <SPI.h>

// ============================================================
// ESP32 <-> ADS1299
// ============================================================

#define ADS_CS      5
#define ADS_DRDY    4
#define ADS_START   27
#define ADS_RESET   33
#define ADS_PWDN    32

#define SPI_SCLK    18
#define SPI_MISO    19
#define SPI_MOSI    23

SPIClass adsSPI(VSPI);

// ADS1299 SPI speed
// MODE1 is required by ADS1299
#define ADS_SPI_SPEED 2000000

// ============================================================
// ADS1299 COMMANDS
// ============================================================

#define CMD_WAKEUP  0x02
#define CMD_STANDBY 0x04
#define CMD_RESET   0x06
#define CMD_START   0x08
#define CMD_STOP    0x0A

#define CMD_RDATAC  0x10
#define CMD_SDATAC  0x11
#define CMD_RDATA   0x12

#define CMD_RREG    0x20
#define CMD_WREG    0x40

// ============================================================
// ADS1299 REGISTERS
// ============================================================

#define REG_ID          0x00
#define REG_CONFIG1     0x01
#define REG_CONFIG2     0x02
#define REG_CONFIG3     0x03
#define REG_LOFF        0x04

#define REG_CH1SET      0x05
#define REG_CH2SET      0x06
#define REG_CH3SET      0x07
#define REG_CH4SET      0x08
#define REG_CH5SET      0x09
#define REG_CH6SET      0x0A

#define REG_BIAS_SENSP  0x0D
#define REG_BIAS_SENSN  0x0E

#define REG_LOFF_SENSP  0x0F
#define REG_LOFF_SENSN  0x10

#define REG_GPIO        0x14
#define REG_MISC1       0x15
#define REG_MISC2       0x16
#define REG_CONFIG4     0x17

// ============================================================
// ADS1299 SETTINGS
// ============================================================

// ADS1299 internal reference is approximately 4.5 V
const float VREF = 4.5;

// Gain = 24
const float GAIN = 24.0;

// 6 channels
#define NUM_CHANNELS 6

// 3 status bytes + 6 channels x 3 bytes
#define DATA_BYTES (3 + NUM_CHANNELS * 3)

volatile bool dataReady = false;

// ============================================================
// DRDY INTERRUPT
// ============================================================

void IRAM_ATTR adsDRDY_ISR()
{
    dataReady = true;
}

// ============================================================
// SPI COMMAND
// ============================================================

void adsCommand(uint8_t command)
{
    adsSPI.beginTransaction(
        SPISettings(ADS_SPI_SPEED, MSBFIRST, SPI_MODE1)
    );

    digitalWrite(ADS_CS, LOW);

    adsSPI.transfer(command);

    // ADS1299 needs command decode time
    delayMicroseconds(4);

    digitalWrite(ADS_CS, HIGH);

    adsSPI.endTransaction();
}

// ============================================================
// WRITE REGISTER
// ============================================================

void adsWriteRegister(uint8_t reg, uint8_t value)
{
    adsSPI.beginTransaction(
        SPISettings(ADS_SPI_SPEED, MSBFIRST, SPI_MODE1)
    );

    digitalWrite(ADS_CS, LOW);

    // WREG command
    adsSPI.transfer(CMD_WREG | reg);

    // Number of registers - 1
    // We are writing only one register
    adsSPI.transfer(0x00);

    // Register value
    adsSPI.transfer(value);

    delayMicroseconds(4);

    digitalWrite(ADS_CS, HIGH);

    adsSPI.endTransaction();

    delayMicroseconds(2);
}

// ============================================================
// READ REGISTER
// ============================================================

uint8_t adsReadRegister(uint8_t reg)
{
    uint8_t value;

    adsSPI.beginTransaction(
        SPISettings(ADS_SPI_SPEED, MSBFIRST, SPI_MODE1)
    );

    digitalWrite(ADS_CS, LOW);

    // RREG
    adsSPI.transfer(CMD_RREG | reg);

    // Number of registers - 1
    adsSPI.transfer(0x00);

    delayMicroseconds(4);

    value = adsSPI.transfer(0x00);

    delayMicroseconds(2);

    digitalWrite(ADS_CS, HIGH);

    adsSPI.endTransaction();

    return value;
}

// ============================================================
// RESET ADS1299
// ============================================================

void adsReset()
{
    digitalWrite(ADS_RESET, LOW);
    delayMicroseconds(10);

    digitalWrite(ADS_RESET, HIGH);

    // Wait for ADS1299 reset
    delay(10);
}

// ============================================================
// CONFIGURE ADS1299
// ============================================================

void configureADS1299()
{
    Serial.println("Configuring ADS1299...");

    // Stop continuous data mode
    adsCommand(CMD_SDATAC);

    delayMicroseconds(10);

    // --------------------------------------------------------
    // CONFIG1
    //
    // 0x96:
    // 250 SPS
    // Daisy chain disabled
    // --------------------------------------------------------

    adsWriteRegister(REG_CONFIG1, 0x96);

    // --------------------------------------------------------
    // CONFIG2
    //
    // Normal operation
    // Test signal disabled
    // --------------------------------------------------------

    adsWriteRegister(REG_CONFIG2, 0xC0);

    // --------------------------------------------------------
    // CONFIG3
    //
    // Internal reference buffer enabled
    //
    // IMPORTANT:
    // Bias amplifier is NOT enabled in this first test.
    // We'll add bias after basic ADC communication works.
    // --------------------------------------------------------

    adsWriteRegister(REG_CONFIG3, 0xE0);

    // --------------------------------------------------------
    // LOFF
    //
    // Lead-off detection disabled
    // --------------------------------------------------------

    adsWriteRegister(REG_LOFF, 0x00);

    // --------------------------------------------------------
    // CHANNELS
    //
    // 0x60:
    //
    // Gain = 24
    // Normal electrode input
    // Channel enabled
    // --------------------------------------------------------

    adsWriteRegister(REG_CH1SET, 0x60);
    adsWriteRegister(REG_CH2SET, 0x60);
    adsWriteRegister(REG_CH3SET, 0x60);
    adsWriteRegister(REG_CH4SET, 0x60);
    adsWriteRegister(REG_CH5SET, 0x60);
    adsWriteRegister(REG_CH6SET, 0x60);

    // --------------------------------------------------------
    // BIAS SENSE
    //
    // Disabled for first basic test.
    // --------------------------------------------------------

    adsWriteRegister(REG_BIAS_SENSP, 0x00);
    adsWriteRegister(REG_BIAS_SENSN, 0x00);

    // --------------------------------------------------------
    // Lead-off sensing disabled
    // --------------------------------------------------------

    adsWriteRegister(REG_LOFF_SENSP, 0x00);
    adsWriteRegister(REG_LOFF_SENSN, 0x00);

    // --------------------------------------------------------
    // MISC1
    //
    // SRB1 enabled.
    //
    // This allows one common reference electrode
    // to be routed to the negative inputs.
    // --------------------------------------------------------

    adsWriteRegister(REG_MISC1, 0x20);

    // GPIO unused
    adsWriteRegister(REG_GPIO, 0x00);

    delay(10);

    Serial.println("ADS1299 configured.");
}

// ============================================================
// READ 24-BIT SIGNED ADC VALUE
// ============================================================

int32_t read24bit()
{
    uint32_t value = 0;

    value |= ((uint32_t)adsSPI.transfer(0x00)) << 16;
    value |= ((uint32_t)adsSPI.transfer(0x00)) << 8;
    value |= ((uint32_t)adsSPI.transfer(0x00));

    // Sign extend 24-bit two's complement
    if (value & 0x800000)
    {
        value |= 0xFF000000;
    }

    return (int32_t)value;
}

// ============================================================
// READ EEG DATA
// ============================================================

void readEEG()
{
    uint8_t buffer[DATA_BYTES];

    adsSPI.beginTransaction(
        SPISettings(ADS_SPI_SPEED, MSBFIRST, SPI_MODE1)
    );

    digitalWrite(ADS_CS, LOW);

    // Read:
    //
    // 3 status bytes
    // +
    // 6 channels x 3 bytes
    //
    // = 21 bytes

    for (int i = 0; i < DATA_BYTES; i++)
    {
        buffer[i] = adsSPI.transfer(0x00);
    }

    digitalWrite(ADS_CS, HIGH);

    adsSPI.endTransaction();

    // --------------------------------------------------------
    // Decode channels
    // --------------------------------------------------------

    for (int ch = 0; ch < NUM_CHANNELS; ch++)
    {
        int index = 3 + ch * 3;

        uint32_t raw =
            ((uint32_t)buffer[index] << 16) |
            ((uint32_t)buffer[index + 1] << 8) |
            ((uint32_t)buffer[index + 2]);

        // Sign extend
        if (raw & 0x800000)
        {
            raw |= 0xFF000000;
        }

        int32_t signedValue = (int32_t)raw;

        // Convert ADC code to volts
        float voltage =
            ((float)signedValue * VREF) /
            (GAIN * 8388608.0);

        // Print channel number and voltage
        Serial.print(voltage * 1000000.0, 3);
        Serial.print(" uV");

        if (ch < NUM_CHANNELS - 1)
            Serial.print(",");
    }

    Serial.println();
}

// ============================================================
// SETUP
// ============================================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println(" ADS1299-6 + ESP32 EEG");
    Serial.println("==============================");

    // GPIO
    pinMode(ADS_CS, OUTPUT);
    pinMode(ADS_DRDY, INPUT);

    pinMode(ADS_START, OUTPUT);
    pinMode(ADS_RESET, OUTPUT);
    pinMode(ADS_PWDN, OUTPUT);

    digitalWrite(ADS_CS, HIGH);

    digitalWrite(ADS_START, LOW);

    // Power up ADS1299
    digitalWrite(ADS_PWDN, HIGH);
    digitalWrite(ADS_RESET, HIGH);

    delay(100);

    // Start SPI
    adsSPI.begin(
        SPI_SCLK,
        SPI_MISO,
        SPI_MOSI,
        ADS_CS
    );

    // Hardware reset
    adsReset();

    delay(100);

    // Read chip ID
    uint8_t id = adsReadRegister(REG_ID);

    Serial.print("ADS1299 ID = 0x");
    Serial.println(id, HEX);

    // Configure
    configureADS1299();

    // DRDY interrupt
    attachInterrupt(
        digitalPinToInterrupt(ADS_DRDY),
        adsDRDY_ISR,
        FALLING
    );

    // Start continuous data mode
    adsCommand(CMD_RDATAC);

    delayMicroseconds(10);

    // Start conversions
    digitalWrite(ADS_START, HIGH);

    adsCommand(CMD_START);

    Serial.println("ADS1299 running.");
    Serial.println("EEG data:");
}

// ============================================================
// LOOP
// ============================================================

void loop()
{
    if (dataReady)
    {
        dataReady = false;

        readEEG();
    }
}
