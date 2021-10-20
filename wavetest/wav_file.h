/*
@name: wav_file.h
@brief: about audio wave file access.

create @ 20161020, by SongYanChao, sycep@163.com
	the first edition.

Copyright (C) 2016 SongYanChao <sycep@163.com>
*/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public Licens
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-
 */


#ifndef __WAV_FILE_H__

#define __WAV_FILE_H__







/* the file path error, fail to open it */
#define ERR_PATH					-1

/* error when read or write */
#define ERR_ACCESS					-2

/* malloc fail, memory insufficient */
#define ERR_MEM						-3

/* wav file head error */
#define ERR_WAV_HEAD				-4

/* chunk format error */
#define ERR_CHUNK					-5



typedef unsigned short				uint16;
typedef unsigned int				uint32;


/* wave file specification, detail in 
"multimedia programming interface and data specifications 1.0", 
IBM & Microsoft */
typedef struct {
	/* format tag
	WAVE_FORMAT_PCM			0X0001	(microsoft pulse code modulation format)
	IBM_FORMAT_MULAW		0X0101
	IBM_FORMAT_ALAW			0x0102
	IBM_FORMAT_ADPCM		0x0103 */
	uint16 tag;		
	uint16 channels;	/* number of channels, 1 mono, 2 stereo */
	uint32 sample;		/* sample rate */
	uint32 bandwidth;	/* bytes rate, in order to estimate buffer size */
	uint16 align;		/* block alignment in bytes for wave data */
} wave_format_t;


typedef struct {
  wave_format_t wf;
  uint16 bits;		/* sample size, bits length */
} pcm_wave_format_t;


typedef struct {
	const char *name;
	size_t size;
	unsigned char *data;

} file_t;


typedef struct {
	file_t file;		/* file size */
	unsigned char *wav_data;	/* wave data */
	size_t wav_len;		/* wave data length */
	pcm_wave_format_t format;	/* pcm parameter */
} wav_file_t;



/*
parse the .wav file architecture.
@path: file path
@wav: return wav data structure.
@return: 0 success, <0 fail.
*/



int wav_file_parse(const char *path, wav_file_t *wav);




#endif
