#if !defined(SAMO_A1_H)
#define SAMO_A1_H 1

/* as long as we use the EVBs display ... */
//#define DISPLAY_INVERTED 	1
#define EEPROM_PM25LV512	1

#if !defined(CONSOLE_BPS)
//#define CONSOLE_BPS 57600
#define CONSOLE_BPS 19200
#endif

#if !defined(CTP_BPS)
#define CTP_BPS     9600
#endif

//     PA3210  Rev      RAM
// A1 -> 1000  0000     32M Prototype
// A2 -> 1001  0001     32M Prototype
// A3 -> 1010  0010     32M Prototype
// A4 -> 1011  0011     32M Prototype
// A5 -> 1100  0100     32M Prototype
// V1 -> 1101  0101     32M Production
// V2 -> 1110  0110     16M Production
// V3 -> 1111  0111     32M Production
// V4 -> 0000  1000     16M Production
// V5 -> 0001  1001
// V6 -> 0010  1010
// V7 -> 0011  1011
// V8 -> 0100  1100
// V9 -> 0101  1101

static inline int board_revision(void)
{
	return 0x08 ^ (REG_PA_DATA & 0x0F);
}


// The ports are:
//   P32 = SD_CARD_VCCEN  active low
//   P33 = SD_CARD_PWR	  active high
// Note:
//   P33 is N/C on Caiac version and VCCEN derived from P32
//     with a special driver chip

#define P32_BIT (1 << 2)
#define P33_BIT (1 << 3)

// P32 = 0, P33 = 1
#define P3_23_MASK (P32_BIT | P33_BIT)

static inline int check_card_power(void)
{
	return (REG_P3_P3D & P3_23_MASK) == P33_BIT;
}

static inline void disable_card_power(void)
{
	REG_P3_P3D = (REG_P3_P3D & ~P3_23_MASK) | P32_BIT;
}

static inline void disable_card_buffer(void)
{
	REG_P3_P3D = (REG_P3_P3D & ~P3_23_MASK);
}

static inline void enable_card_buffer(void)
{
	REG_P3_P3D = (REG_P3_P3D & ~P3_23_MASK) | P33_BIT;
}


#if !SAMO_RESTRICTIONS
// this need delay - but jackknife cannot include it or it will be too big
// so jacknife excludes this
#include <delay.h>

static inline void enable_card_power(void)
{
	disable_card_power();  // Vcc = off, Buffer = off
	delay_us(10);
	disable_card_buffer(); // Vcc = on,  Buffer = off
	delay_us(1000);
	enable_card_buffer();  // Vcc = on,  Buffer = on
}

#endif


#define SDCARD_CS_LO()	do { REG_P5_P5D &= ~(1 << 0); } while (0)
#define SDCARD_CS_HI()	do { REG_P5_P5D |=  (1 << 0); } while (0)
#define EEPROM_WP_HI()	do {} while (0)


#define POWER_OFF_CYCLES 500000
// toggle watchdog output to force power off
static inline void power_off(void)
{
	register int i;

	REG_P6_03_CFP &= ~0xc0; // select P63 as GPIO
	REG_P6_IOC6 |= (1 << 3);
	for (;;) {
		REG_P6_P6D |= (1 << 3); // P63 = High
		for (i = 0; i < POWER_OFF_CYCLES; ++i) {
			asm volatile ("nop");
		}

		REG_P6_P6D &= ~(1 << 3); // P63 = low
		for (i = 0; i < POWER_OFF_CYCLES; ++i) {
			asm volatile ("nop");
		}
	}

	// P04 high to switch off
	//REG_P0_IOC0 = 0x08;
	//REG_P0_P0D = 0x08;
}

// true if power switch is pressed
// possible detect this an do orderly shut down
// really need to be an interrupt
// this is just a reminder
//static inline int power_switch_pressed(void)
//{
//	return 0 != (REG_P0_P0D & 0x08);
//}


#if BOARD_SAMO_Vx
#define AVDD_MILLIVOLTS	       3300
#elif BOARD_SAMO_A3 || BOARD_SAMO_A5
#define AVDD_MILLIVOLTS	       3150
#else
#define AVDD_MILLIVOLTS	       3000
#endif
#define ADC_SERIES_RESISTOR_K  150
#define ADC_SHUNT_RESISTOR_K   1000

#define BATTERY_FULL  3000
#define BATTERY_LOW   2250
#define BATTERY_EMPTY 2100


static inline void init_pins(void)
{
	// not enough space for adding this
	// A low on pin P63 shuts down the power supply - so try
	// to keep it high, without any glitched or we will power down
	// immediately.
	// This _must_ be the setting performed.
	// p60-63: wdt - ensure that P63(#WDTNMI) pin is set high
	//REG_P6_P6D |= (1 << 3);		 // P63 = 1 (for safety)

	/* P85: LCD_CS, P83: TFT_CTL1 */
	REG_P8_IOC8 = 0x28;
	REG_P8_03_CFP = 0x3f;
	REG_P8_45_CFP = 0x03;

	/* P07 = reset CTP */
	REG_P0_IOC0 = 0x80;
	// REG_P0_P0D  = 0x80;

	/* P65-67: SPI */
	REG_P6_47_CFP = 0x54;

	/* Serial interface */
	REG_P0_03_CFP = 0x05; // Tx/Rx
	REG_P0_47_CFP = 0x01; // Rx

	/* SDCARD CS# */
	REG_P5_03_CFP = 0x01;

	/* LCD controller */
	REG_P8_03_CFP = 0x55;
	REG_P9_47_CFP = 0x55;

	/* SDCARD power */
	REG_P3_P3D = 0x0f;
	REG_P3_IOC3 = 0x0f;
	disable_card_power();

	/* pull ups */
	REG_MISC_PUP0 = (1 << 0) | (1 << 4);
	REG_MISC_PUP6 = (1 << 5) | (1 << 4) | (1 << 3);

	/* P50 & P52: CS lines */
	REG_P5_P5D = 0x07;  // all cs lines high
	REG_P5_IOC5 = 0x07;

	REG_INT_FK01_FP03 = 0x3f; // clear outstanding interrupts

#if _not_used_
	/* set FPT1 to another gpio, make it falling edge triggered */
	REG_PINTSEL_SPT03 |= 0xC;
	REG_PINTEL_SEPT07 |= 0x2;
	REG_PINTPOL_SPP07 &= ~0x2;
#endif
}


// number of refresh cycles for initialisation
#define REFRESH_COUNT 12

// mode register settings

#define BURST_LENGTH_1 0x00
#define BURST_LENGTH_2 0x01
#define BURST_LENGTH_4 0x02
#define BURST_LENGTH_8 0x03

#define BURST_TYPE_NORMAL      (0x0 << 3)
#define BURST_TYPE_INTERLEAVE  (0x1 << 3)

#define CAS_LATENCY_2	       (0x2 << 4)
#define CAS_LATENCY_3	       (0x3 << 4)

#define OPERATION_NORMAL       (0x00 << 7)
#define OPERATION_SINGLE       (0x04 << 7)

#define MODE_WORD (0				\
		   | BURST_LENGTH_2		\
		   | BURST_TYPE_NORMAL		\
		   | CAS_LATENCY_2		\
		   | OPERATION_SINGLE		\
		)

// as SDRAM_A0 connects to CPU_A1 => shift of 1 bit
#define ADDRESS_SHIFT	1

// define the start if SDRAM ought to move to .lds file
// since that should be the definitive reference for the memory map
#define SDRAM_START ((volatile unsigned char *) 0x10000000)

#define SDRAM_FIRST_BYTE ((SDRAM_START)[0])

// put the above together to get the place to write the mode to the SDRAM
#define SDRAM_MODE_STORE (SDRAM_START[(MODE_WORD << ADDRESS_SHIFT)])

// 48MHz => 20ns clock cycle

// e.g.
// SDRAM = 4M * 4 Banks * 16 bits
// refresh = 8k
// Trp = Trcd = 15ns	     1 clock   (1..4)
// Tras = 42ns		     3 clocks  (1..8)
// Trc = 67ns		     4 clocks  (1..16)
#define CLKS_TRP  4
#define CLKS_TRAS 8
#define CLKS_TRC  15

#define SDRAM_CONFIGURATION_32M (0                                      \
				 | ((CLKS_TRP - 1) << T24NS_SHIFT)	\
				 | ((CLKS_TRAS - 1) << T60NS_SHIFT)	\
				 | ((CLKS_TRC - 1) << T80NS_SHIFT)	\
				 | ADDRC_16M_x_16_bits_x_1)

#define SDRAM_CONFIGURATION_16M (0                                      \
				 | ((CLKS_TRP - 1) << T24NS_SHIFT)	\
				 | ((CLKS_TRAS - 1) << T60NS_SHIFT)	\
				 | ((CLKS_TRC - 1) << T80NS_SHIFT)	\
				 | ADDRC__8M_x_16_bits_x_1)

#define SDRAM_CONFIGURATION_8M  (0                                      \
				 | ((CLKS_TRP - 1) << T24NS_SHIFT)	\
				 | ((CLKS_TRAS - 1) << T60NS_SHIFT)	\
				 | ((CLKS_TRC - 1) << T80NS_SHIFT)	\
				 | ADDRC__4M_x_16_bits_x_1)

static inline void init_ram(void)
{
	// SDRAM off
	REG_SDRAMC_INI = 0;

	// P20..P27 all to SDRAM control functions
	REG_P2_03_CFP = 0x55;
	REG_P2_47_CFP = 0x55;

	// enable SDA10 on P53
	REG_P5_03_CFP = (REG_P5_03_CFP & 0x3f) | 0x80;

	// enable SDRAM clocks
	REG_CMU_PROTECT = CMU_PROTECT_OFF;
	REG_CMU_GATEDCLK0 |=
		//USBSAPB_CKE |
		//USB_CKE |
		//SDAPCPU_HCKE |
		SDAPCPU_CKE |
		SDAPLCDC_CKE |
		SDSAPB_CKE |
		DSTRAM_CKE |
		//LCDCAHBIF_CKE |
		//LCDCSAPB_CKE |
		//LCDC_CKE |
		0;
	REG_CMU_PROTECT = CMU_PROTECT_ON;

	// enable arbiter
	REG_SDRAMC_APP =
		ARBON |
		//DBF |
		//INCR |
		CAS1 |
		//CAS0 |
		APPON |
		IQB |
		0;

	// SDRAM configuration register
	if (board_revision() >= 8 || board_revision() == 6) {
		REG_SDRAMC_CTL = SDRAM_CONFIGURATION_16M;
	} else {
		REG_SDRAMC_CTL = SDRAM_CONFIGURATION_32M;
	}
	// enable RAM self-refresh
	REG_SDRAMC_REF =
		SCKON |
		SELEN |
		(0x7f << SELCO_SHIFT) |
		(0x8c << AURCO_SHIFT) |
		0;

	// begin SDRAM initialisation
	REG_SDRAMC_INI = SDRAM_CMD_FIRST;

	// delay more than 200 us
	{
		unsigned int i = 0;
		for (i = 0; i < 150000; ++i) {
				asm volatile ("nop");
		}
	}

	REG_SDRAMC_INI = SDRAM_CMD_PALL;
	SDRAM_FIRST_BYTE = 0x0;
	asm volatile ("nop");

	REG_SDRAMC_INI = SDRAM_CMD_PALL;
	SDRAM_FIRST_BYTE = 0x0;
	asm volatile ("nop");

	REG_SDRAMC_INI = SDRAM_CMD_MRS;
	SDRAM_MODE_STORE = 0x0; // the value is part of the address
	asm volatile ("nop");

	{
		unsigned int i = 0;
		for (i = 0; i < REFRESH_COUNT; ++i) {
			REG_SDRAMC_INI = SDRAM_CMD_REF;
			SDRAM_FIRST_BYTE = 0x0;
			asm volatile ("nop");
		}
	}

	//REG_SDRAMC_INI = SDRAM_CMD_MRS;
	//SDRAM_MODE_STORE = 0x0; // the value is part of the address
	//asm volatile ("nop");

	// finished the setup sequence
	REG_SDRAMC_INI = SDRAM_CMD_FINAL;

	// wait for SDRAM to come on-line
	BUSY_WAIT_FOR(REG_SDRAMC_INI & SDEN);
}


#if !defined(MEGA)
#define MEGA(x) (1024 * 1024 * (x))
#endif

static inline unsigned long ram_size(void)
{
	switch (REG_SDRAMC_CTL & ADDRC_MASK) {
	case ADDRC_32M_x_16_bits_x_1:
		return MEGA(32 * 2);
		break;
	case ADDRC_16M_x__8_bits_x_2:
		return MEGA(16 * 2);
		break;
	case ADDRC__8M_x__8_bits_x_2:
		return MEGA(8 * 2);
		break;
	case ADDRC__2M_x__8_bits_x_2:
		return MEGA(2 * 2);
		break;
	case ADDRC_16M_x_16_bits_x_1:
		return MEGA(16 * 2);
		break;
	case ADDRC__8M_x_16_bits_x_1:
		return MEGA(8 * 2);
		break;
	case ADDRC__4M_x_16_bits_x_1:
		return MEGA(4 * 2);
		break;
	case ADDRC__1M_x_16_bits_x_1:
		return MEGA(1 * 2);
		break;
	}
	return 0;
}

#endif
