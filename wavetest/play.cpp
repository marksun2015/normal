/*
@name: play.c
@brief: about audio wave file playback.

create @ 20161110, by SongYanChao, sycep@163.com
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


#include <stdio.h>
#include <alsa/asoundlib.h>
#include "wav_file.h"

/* one writing time = 1 >> PERIOD_FACTOR second */
#define PERIOD_FACTOR			3

static int audio_init(pcm_wave_format_t *format, snd_pcm_t **handle)
{
	int dir, err;
	snd_pcm_t *hdl;
	unsigned int val;
	snd_pcm_uframes_t frames;
	snd_pcm_hw_params_t *params;
	snd_pcm_format_t fmt = SND_PCM_FORMAT_S16_LE;

	/* Open PCM device for playback. */
	err = snd_pcm_open(&hdl, "default", SND_PCM_STREAM_PLAYBACK, 0);

	if (err < 0) {
		return err;
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(hdl, params);

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(hdl, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	switch (format->bits) {
		case 8: {
			fmt = SND_PCM_FORMAT_S8;
		}
		case 24: {
			fmt = SND_PCM_FORMAT_S24_LE;
		}
		case 32: {
			fmt = SND_PCM_FORMAT_S32_LE;
		}
		default: {
			/* s16 */
		}
	}

	snd_pcm_hw_params_set_format(hdl, params, fmt);

	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(hdl, params, format->wf.channels);

	/* 44100 bits/second sampling rate (CD quality) */
	dir = 0;
	val = format->wf.sample;
	snd_pcm_hw_params_set_rate_near(hdl, params, &val, &dir);

	/* Set period size to 32 frames. */
	dir = 0;
	/* test about this value, min value */
	frames = format->wf.sample >> PERIOD_FACTOR;
	snd_pcm_hw_params_set_period_size_near(hdl, params, &frames, &dir);

	/* Write the parameters to the driver */
	err = snd_pcm_hw_params(hdl, params);

	if (err < 0) {
		return err;
	}

	*handle = hdl;

	return 0;
}


/* return <0 fail, >=0 success */
static int play_wav(snd_pcm_t *handle, wav_file_t *wav)
{
	int len = wav->wav_len;
	snd_pcm_uframes_t frames;
	int err, size, offset = 0;
	int bytes_per_frames = (wav->format.bits >> 3) * wav->format.wf.channels;

	/* frames * bytes/sample * channels */
	frames = wav->format.wf.sample >> PERIOD_FACTOR;
	size = frames * bytes_per_frames;

    //printf("frames:%d ",frames);
    //printf("len:%d ",len);
    //printf("size:%d ",size);
	//frames = 32;
	//size = 32*2;

	while (len > size) {
		err = snd_pcm_writei(handle, wav->wav_data+offset, frames);

		if (err == -EPIPE) {
			fprintf(stderr, "xrun !\n");
			snd_pcm_prepare(handle);
			continue;
		}

		if (err < 0) {
			fprintf(stderr, "writei error: %d, %s\n", err, snd_strerror(err));
			break;
		}

		if (err != (int)frames) {
			fprintf(stderr, "part write %d\n", err);
			offset += err * bytes_per_frames;
			len -= err * bytes_per_frames;
			continue;
		}

		offset += size;
		len -= size;
	}

	snd_pcm_drain(handle);
	snd_pcm_close(handle);

	return err;
}



int main(int argc, char *argv[])
{
	int err;
	wav_file_t wav;
	snd_pcm_t *handle;

	memset(&wav, 0, sizeof(wav));

	err = wav_file_parse("PLC_no_response.wav", &wav);

	if (err < 0) {
		printf("wav file parse fail, err %d \n", err);
	}

	err = audio_init(&wav.format, &handle);

	if (err < 0) {
		fprintf(stderr, "error from set param: %s/n", snd_strerror(err));
	}

	play_wav(handle, &wav);

	return 0;
}
