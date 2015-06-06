#ifndef	_I2C_H_
#define _I2C_H_

/*
 *	SDA (data) and SCL (clock) bits
 *	
 *	Special note!!!
 *	
 *	If the clock and data lines are in the same port, you will need
 *	to beware of the Read/Modify/Write issue in the PIC - since
 *	a bit set or clear on any one bit in a port will read and write
 *	back all other bits. 
 */

#ifndef _XTAL_FREQ
 // Unless specified elsewhere, 4MHz system frequency is assumed
 #define _XTAL_FREQ 4000000
#endif

/* Uncomment the next line to use the PIC's SSP Module*/
#define I2C_MODULE 1

#ifdef I2C_MODULE
/* I2C module uses PORT C */
#define SCL             RC3             /* clock on port C bit 2 */
#define SCL_DIR         TRISC3
#define SDA     RC4                     /* data on port C bit 1 */
#define SDA_DIR         TRISC4
#define I2CTRIS TRISC
#define MASTER_MODE     0B1011          /* I2C firmware controlled Master Mode (slave idle) */
#define SSPMode(val)   SSPCON &=0xF0; SSPCON|=(val & 0xf)

#else
/* Change port as required - defaults to port b */
#define	SCL		RB2		/* clock on port B bit 2 */
#define	SCL_DIR		TRISB2

#define SDA             RB1             /* data on port B bit 1 */
#define	SDA_DIR		TRISB1
#define I2CTRIS TRISB

#endif

#define M_SDA_INP	0x02
#define M_SDA_OUT   0xFD
#define M_SCL_INP   0x04
#define M_SCL_OUT	0xFB

#define I2C_INPUT	1		/* data direction input */
#define I2C_OUTPUT	0		/* data direction output */

#define I2C_READ	0x01		/* read bit used with address */
#define I2C_WRITE	0x00		/* write bit used with address */

#define FALSE		0
#define TRUE		!FALSE

#define I2C_ERROR	(-1)
#define I2C_LAST	FALSE		/* SendAck: no more bytes to send */
#define I2C_MORE	TRUE		/* SendAck: more bytes to send */

#define i2cStart()		i2cRestart()
#define i2cWriteTo(address)	i2cOpen((address), I2C_WRITE)
#define i2cReadFrom(address)	i2cOpen((address), I2C_READ)

#ifdef I2C_MODULE
#define SCL_HIGH() SCL_DIR = I2C_INPUT
#define SCL_LOW()  SCL_DIR = I2C_OUTPUT
#define SDA_HIGH() SDA_DIR = I2C_INPUT
#define SDA_LOW()  SDA_DIR = I2C_OUTPUT
#else
#define SCL_HIGH() SCL = 1; SCL_DIR = I2C_OUTPUT
#define SCL_LOW()  SCL = 0; SCL_DIR = I2C_OUTPUT
#define SDA_HIGH() SDA = 1; SDA_DIR = I2C_OUTPUT
#define SDA_LOW()  SDA = 0; SDA_DIR = I2C_OUTPUT
#endif

/*
 * Timings for the i2c bus. Times are rounded up to the nearest
 * micro second.
 */

#define I2C_TM_BUS_FREE		5
#define	I2C_TM_START_SU		5
#define I2C_TM_START_HD		4
#define I2C_TM_SCL_LOW		5
#define	I2C_TM_SCL_HIGH		4
#define I2C_TM_DATA_SU		1
#define I2C_TM_DATA_HD          0
#define I2C_TM_SCL_TO_DATA	4	/* SCL low to data valid */
#define	I2C_TM_STOP_SU		4
#define I2C_TM_SCL_TMO		10	/* clock time out */

extern signed char	i2cReadAcknowledge(void);
extern unsigned char	i2cSendAddress(unsigned char, unsigned char);
extern unsigned char	i2cSendByte(unsigned char);
extern int		i2cReadByte(void);
extern void		i2cRestart(void);
extern void		i2cStop(void);
extern void		i2cSendAcknowledge(unsigned char);
extern signed char	i2cPutByte(unsigned char);
extern int		i2cGetByte(unsigned char);
extern unsigned char	i2cOpen(unsigned char, unsigned char);
extern unsigned char	i2cGetString(unsigned char *, unsigned char);
extern int		i2cPutString(const unsigned char *, unsigned char);
extern unsigned char	i2cWaitForSCL(void);
extern void 		i2cFree(void);
extern unsigned char	i2cread(unsigned char);
#endif			/* _I2C_H_ */