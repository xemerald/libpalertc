/**
 * @file mode1.c
 * @author Benjamin Ming Yang @ Department of Geology, National Taiwan University
 * @brief
 * @version 0.1
 * @date 2024-06-02
 *
 * @copyright Copyright (c) 2024
 *
 */

/* Standard C header include */
#include <stdio.h>
#include <stdint.h>
/* Local header include */
#include <libpalertc.h>
#include <mode1.h>
#include <misc.h>

/**
 * @brief Parse the Palert Mode 1 system time to calendar time(UTC)
 *
 * @param pam1h
 * @param tzoffset_sec
 * @return double
 */
double pac_m1_systime_get( const PALERT_M1_HEADER *pam1h, long tzoffset_sec )
{
#ifdef _SPARC
	return misc_mktime( ((pam1h->sys_year[1] << 8) + pam1h->sys_year[0]), ((pam1h->sys_month[1] << 8) + pam1h->sys_month[0]),
		((pam1h->sys_day[1] << 8) + pam1h->sys_day[0]), ((pam1h->sys_hour[1] << 8) + pam1h->sys_hour[0]),
		((pam1h->sys_minute[1] << 8) + pam1h->sys_minute[0]), pam1h->sys_second ) +
		tzoffset_sec + ((pam1h->sys_tenmsec << 3) + (pam1h->sys_tenmsec << 1)) / 1000.0;
#else
	return misc_mktime( pam1h->sys_year, pam1h->sys_month, pam1h->sys_day, pam1h->sys_hour, pam1h->sys_minute, pam1h->sys_second ) +
		tzoffset_sec + ((pam1h->sys_tenmsec << 3) + (pam1h->sys_tenmsec << 1)) / 1000.0;
#endif
}

/**
 * @brief Parse the Palert Mode 1 event time to calendar time(UTC)
 *
 * @param pam1h
 * @param tzoffset_sec
 * @return double
 */
double pac_m1_evtime_get( const PALERT_M1_HEADER *pam1h, long tzoffset_sec )
{
#ifdef _SPARC
	return misc_mktime( ((pam1h->ev_year[1] << 8) + pam1h->ev_year[0]), ((pam1h->ev_month[1] << 8) + pam1h->ev_month[0]),
		((pam1h->ev_day[1] << 8) + pam1h->ev_day[0]), ((pam1h->ev_hour[1] << 8) + pam1h->ev_hour[0]),
		((pam1h->ev_minute[1] << 8) + pam1h->ev_minute[0]), pam1h->ev_second ) +
		tzoffset_sec + ((pam1h->ev_tenmsec << 3) + (pam1h->ev_tenmsec << 1))/1000.0;
#else
	return misc_mktime( pam1h->ev_year, pam1h->ev_month, pam1h->ev_day, pam1h->ev_hour, pam1h->ev_minute, pam1h->ev_second ) +
		tzoffset_sec + ((pam1h->ev_tenmsec << 3) + (pam1h->ev_tenmsec << 1)) / 1000.0;
#endif
}

/**
 * @brief
 *
 * @param chan_seq
 * @return char*
 */
char *pac_m1_chan_code_get( const int chan_seq )
{
#define X(a, b, c) b,
	static char *chan_code[] = {
		PALERT_M1_CHAN_TABLE
	};
#undef X

	return chan_code[chan_seq];
}

/**
 * @brief
 *
 * @param chan_seq
 * @return double
 */
double pac_m1_chan_unit_get( const int chan_seq )
{
#define X(a, b, c) c,
	double chan_unit[] = {
		PALERT_M1_CHAN_TABLE
	};
#undef X

	return chan_unit[chan_seq];
}

/**
 * @brief
 *
 * @param pam1h
 * @return char*
 */
char *pac_m1_trigmode_get( const PALERT_M1_HEADER *pam1h )
{
/* */
#define X(a, b, c) b,
	static char *trigmode_str[] = {
		PALERT_TRIGMODE_TABLE
	};
#undef X
#define X(a, b, c) c,
	uint8_t trigmode_bit[] = {
		PALERT_TRIGMODE_TABLE
	};
#undef X
/* */
	for ( int i = 0; i < PALERT_TRIGMODE_COUNT; i++ ) {
		if ( pam1h->event_flag[0] & trigmode_bit[i] )
			return trigmode_str[i];
	}

	return NULL;
}

/**
 * @brief Parse the four kind of Palert IP address
 *
 * @param pam1h
 * @param iptype
 * @param dest
 * @return char*
 */
char *pac_m1_ip_get( const PALERT_M1_HEADER *pam1h, const int iptype, char *dest )
{
	switch ( iptype ) {
	case PALERT_SET_IP:
		misc_ipv4str_gen(dest, pam1h->palert_ip[0], pam1h->palert_ip[1], pam1h->palert_ip[2], pam1h->palert_ip[3]);
		break;
	case PALERT_NTP_IP:
		misc_ipv4str_gen(dest, pam1h->ntp_server[0], pam1h->ntp_server[1], pam1h->ntp_server[2], pam1h->ntp_server[3]);
		break;
	case PALERT_TCP0_IP:
		misc_ipv4str_gen(dest, pam1h->tcp0_server[1], pam1h->tcp0_server[0], pam1h->tcp0_server[3], pam1h->tcp0_server[2]);
		break;
	case PALERT_TCP1_IP:
		misc_ipv4str_gen(dest, pam1h->tcp1_server[1], pam1h->tcp1_server[0], pam1h->tcp1_server[3], pam1h->tcp1_server[2]);
		break;
	default:
		fprintf(stderr, "pac_m1_ip_get: Unknown IP type for Palert mode 1 or 2 packet.\n");
		break;
	}

	return dest;
}

/**
 * @brief
 *
 * @param packet
 * @param buffer_0
 * @param buffer_1
 * @param buffer_2
 * @param buffer_3
 * @param buffer_4
 */
void pac_m1_data_extract(
	const PALERT_M1_PACKET *packet, int32_t buffer_0[PALERT_M1_SAMPLE_NUMBER], int32_t buffer_1[PALERT_M1_SAMPLE_NUMBER],
	int32_t buffer_2[PALERT_M1_SAMPLE_NUMBER], int32_t buffer_3[PALERT_M1_SAMPLE_NUMBER], int32_t buffer_4[PALERT_M1_SAMPLE_NUMBER]
) {
/* Shortcut for the input data */
	const PALERT_M1_DATA *_data = packet->data;
/* */
	int32_t *_buffer[PALERT_M1_CHAN_COUNT] = { buffer_0, buffer_1, buffer_2, buffer_3, buffer_4 };
	int32_t  dumping[PALERT_M1_SAMPLE_NUMBER] = { 0 };

/* */
	for ( int i = 0; i < PALERT_M1_CHAN_COUNT; i++ )
		if ( !_buffer[i] )
			_buffer[i] = dumping;

#ifdef _SPARC
	uint32_t hbyte;
/* Go thru all the input data */
	for ( int i = 0; i < PALERT_M1_SAMPLE_NUMBER; i++, _data++ ) {
	/* Channel seq: HLZ(0), HLN(1), HLE(2), PD(3), Disp(4) */
		for ( int j = 0; j < PALERT_M1_CHAN_COUNT; j++ ) {
			hbyte         = _data->cmp[j][1];
			_buffer[j][i] = (hbyte << 8) + _data->cmp[j][0];
			if ( hbyte & 0x80 )
				_buffer[j][i] |= 0xffff0000;
		}
	}
#else
/* Go thru all the input data */
	for ( int i = 0; i < PALERT_M1_SAMPLE_NUMBER; i++, _data++ )
	/* Channel seq: HLZ(0), HLN(1), HLE(2), PD(3), Disp(4) */
		for ( int j = 0; j < PALERT_M1_CHAN_COUNT; j++ )
			_buffer[j][i] = _data->cmp[j];
#endif

	return;
}
