/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "sdhc_card_driver.h"
#include "stm32f072xb.h"

#define SECTOR_COUNT 0x700000
#define SECTOR_SIZE 512


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	BYTE* buffPointer = buff;
	for(UINT i = 0; i < count; i++)
	{
		SdhcCardReadBlock(buffPointer, sector + i);
		buffPointer += 512;
	}
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	BYTE* buffPointer = (BYTE*)buff;
	for(UINT i = 0; i < count; i++)
	{
		SdhcCardWriteBlock(buffPointer, sector + i);
		buffPointer += 512;
	}
	return RES_OK;
}

#endif







/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch(cmd)
	{
		case CTRL_SYNC:
			return RES_OK;
		case GET_SECTOR_COUNT:
		{
			*(LBA_t*)buff = SECTOR_COUNT;
			return RES_OK;
		}
		case GET_SECTOR_SIZE:
		{
			*(WORD*)buff = SECTOR_SIZE;
			return RES_OK;
		}
		case GET_BLOCK_SIZE:
		{
			*(DWORD*)buff = SECTOR_SIZE;
			return RES_OK;
		}
		default:
		{
			return RES_PARERR;
		}
	}
}

/*
 *
 *
bit31:25
    Year origin from the 1980 (0..127, e.g. 37 for 2017)
bit24:21
    Month (1..12)
bit20:16
    Day of the month (1..31)
bit15:11
    Hour (0..23)
bit10:5
    Minute (0..59)
bit4:0
    Second / 2 (0..29, e.g. 25 for 50)
 */
DWORD get_fattime(void)
{
	return (((((RTC->DR & RTC_DR_YT) >> 20 ) * 10 ) + 
	                    20 + ((RTC->DR & RTC_DR_YU) >> 16)) << 25)  |
	                    
		   (((((RTC->DR & RTC_DR_MT) >> 12 ) * 10 ) + 
		                      ((RTC->DR & RTC_DR_MU) >> 8) << 21))  |
		                      
		   (((((RTC->DR & RTC_DR_DT) >> 4) * 10 ) + 
		                             (RTC->DR & RTC_DR_DU)) << 16)  |
		                             
		   (((((RTC->TR & RTC_TR_HT) >> 20) * 10 ) + 
		                     ((RTC->TR & RTC_TR_HU) >> 16)) << 11)  |
		   
		   (((((RTC->TR & RTC_TR_MNT) >> 12) * 10 ) + 
		                       ((RTC->TR & RTC_TR_MNU) >> 8)) << 5) |
		   
		   (((((RTC->TR & RTC_TR_ST) >> 4) * 10 ) + 
		                       (RTC->TR & RTC_TR_SU)) >> 1);// Seconds/2;
}
