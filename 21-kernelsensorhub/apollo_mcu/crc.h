#ifndef __CRC_H__
#define __CRC_H__


/*******************************************/
/*!
 *  \fn     calc_crc8
 *
 *  \brief  Calculate the CRC-8 of the given buffer or by algorithm.
 *
 *  \model  CRC-8/MAXIM
 *          Polyomial x8+x5+x4+1 0x131(0x31)
 *          crcTnit   0x00
 *          REFIN     True
 *          REFOUT    True
 *
 *  \param  pBuf     Buffer to compute the CRC.
 *  \param  len      Length of the buffer.
 *
 *  \return crc.
 */
/*******************************************/
u8 calc_crc8(u8 *p_buf, u32 len);

uint16_t crc16_ccitt(uint8_t *data, uint16_t len);


#endif /* __CRC_H__ */
