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


#define SECTOR_COUNT 0x700000
#define SECTOR_SIZE 512

extern void SdhcCardWriteBlock(uint8_t* buffer_in, uint32_t block_index);
extern void SdhcCardReadBlock(uint8_t* buffer_out, uint32_t block_index);


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
	for(uint32_t i = 0; i < count; i++)
	{
		SdhcCardReadBlock(buff, sector + i);
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
	for(uint32_t i = 0; i < count; i++)
	{
		SdhcCardWriteBlock((uint8_t*)buff, sector + i);
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

