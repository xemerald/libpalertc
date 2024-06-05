/**
 * @file mode16.c
 * @author Benjamin Ming Yang @ Department of Geology, National Taiwan University
 * @brief Program for Palert mode 16 data packet.
 * @version 0.1
 * @date 2024-06-02
 *
 * @copyright Copyright (c) 2024
 *
 */

/* Standard C header include */
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
/* Local header include */
#include <libpalertc.h>
#include <mode16.h>

/**
 * @brief Parse the palert mode 16 timestamp to calendar time(UTC)
 *
 * @param pam16h
 * @return double
 */
double pac_m16_sptime_get( const PALERT_M16_HEADER *pam16h )
{
	return PALERT_M16_TIMESTAMP_GET( pam16h ) + PALERT_M16_WORD_GET( pam16h->msec ) / 10000.0;
}

/**
 * @brief
 *
 * @param pam16h
 * @return double
 */
double pac_m16_scale_get( const PALERT_M16_HEADER *pam16h )
{
	return *((float *)pam16h->scale);
}

/**
 * @brief
 *
 * @param pam16h
 * @return double
 */
double pac_m16_ntp_offset_get( const PALERT_M16_HEADER *pam16h )
{
	return *((float *)pam16h->ntp_offset);
}

/**
 * @brief
 *
 * @param packet
 * @param nbuf
 * @param ... should be float * type
 */
void pac_m16_data_extract( const PALERT_M16_PACKET *packet, int nbuf, ... )
{
/* Shortcut for the input data */
	const float *_data = (float *)&packet->bytes[PALERT_M16_HEADER_LENGTH];
	const float * const _data_end = (float *)((uint8_t *)_data + PALERT_M16_WORD_GET( packet->header.data_len ));
/* */
	float  *_buffer[packet->header.nchannel];
	float   dumping[PALERT_MAX_SAMPRATE] = { 0 };
	va_list ap;

/* */
	for ( int i = 0; i < packet->header.nchannel; i++ )
		_buffer[i] = dumping;
/* */
	va_start(ap, nbuf);
	for ( int i = 0; nbuf > 0 && i < packet->header.nchannel; nbuf--, i++ )
		if ( !(_buffer[i] = va_arg(ap, float *)) )
			_buffer[i] = dumping;
	va_end(ap);
/* Go thru all the input data */
	for ( int i = 0; _data < _data_end; i++, _data += packet->header.nchannel )
		for ( int j = 0; j < packet->header.nchannel; j++ )
			_buffer[j][i] = _data[j];

	return;
}
