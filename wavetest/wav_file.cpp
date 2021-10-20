/*
 * parsing file
 */

#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include "wav_file.h"

#define MIN_WAV_HEAD_SIZE			36

typedef struct {
        unsigned char *id;
        size_t len;
        unsigned char *data;
} chunk_t;

static int file_load(file_t *file)
{
        FILE *fp;
        size_t size;

        fp = fopen(file->name, "rb");
        if (fp == NULL) {
                return ERR_PATH;
        }

        fseek(fp, 0, SEEK_END);
        file->size = ftell(fp);
        file->data = (unsigned char *)malloc(file->size);
        if (file->data == NULL) {
                fclose(fp);
                return ERR_MEM;
        }

        fseek(fp, 0, SEEK_SET);
        size = fread(file->data, 1, file->size, fp);
        fclose(fp);
        if (size != file->size) {
                free(file->data);
                return ERR_ACCESS;
        }

        return 0;
}

static uint32 u8_u32_le(unsigned char *buf)
{
        uint32 nu;

        nu = buf[3];
        nu <<= 8;
        nu |= buf[2];
        nu <<= 8;
        nu |= buf[1];
        nu <<= 8;
        nu |= buf[0];

        return nu;
}



static uint16 u8_u16_le(unsigned char *buf)
{
        uint16 nu;

        nu = buf[1];
        nu <<= 8;
        nu |= buf[0];

        return nu;
}



static int get_chunk(unsigned char *start, size_t size, chunk_t *chunk)
{
        chunk->id = start;
        chunk->len = u8_u32_le(start + 4);

        if (chunk->len > (size-8)) {
                return ERR_CHUNK;
        }

        chunk->data = start + 8;

        return 0;
}

/* return entire chunk size, include chunk id and chunk length domain */
static __inline size_t chunk_entire_size(chunk_t *chunk)
{
        /* chunk id 4 byte, length domain 4 byte */
        return chunk->len + 8;
}

static __inline unsigned char *next_chunk(chunk_t * current)
{
        /* current->id point to chunk start */
        return current->id + chunk_entire_size(current);
}

static int wav_head_check(wav_file_t *wav, void (*chunk_cb)(wav_file_t *, chunk_t *))
{
        uint32 len;
        size_t size = wav->file.size;
        unsigned char *ptr = wav->file.data;

        if (wav->file.size < MIN_WAV_HEAD_SIZE) {
                return ERR_WAV_HEAD;
        }

        /* first 4 bytes must RIFF */
        if (memcmp(ptr, "RIFF", 4) != 0) {
                return ERR_WAV_HEAD;
        }

        ptr += 4;
        size -= 4;

        len = u8_u32_le(ptr);

        /* 4 byte "RIFF", 4 byte length, length byte data */
        if (len != (wav->file.size-8)) {
                return ERR_WAV_HEAD;
        }

        ptr += 4;
        size -= 4;

        /* form type, 4 bytes must "WAVE" */
        if (memcmp(ptr, "WAVE", 4) != 0) {
                return ERR_WAV_HEAD;
        }

        ptr += 4;
        size -= 4;

        while (1) {
                int err;
                size_t offset;
                chunk_t chunk;
                memset(&chunk, 0, sizeof(chunk));

                err = get_chunk(ptr, size, &chunk);

                if (err < 0) {
                        return err;
                }

                if (chunk_cb != NULL) {
                        chunk_cb(wav, &chunk);
                }

                offset = chunk_entire_size(&chunk);

                if (size > offset) {
                        /* more chunk */
                        size -= offset;
                } else if (size == offset) {
                        /* this is the last chunk */
                        return 0;
                } else {
                        return ERR_CHUNK;
                }

                ptr = next_chunk(&chunk);
        }
}

static void chunk_process(wav_file_t *wav, chunk_t *chunk)
{
        unsigned char *ptr = chunk->data;

        if (memcmp(chunk->id, "fmt ", 4) == 0) {
                /* format parameter chunk, length must >= 16 */
                wav->format.wf.tag = u8_u16_le(ptr);
                ptr += 2;
                wav->format.wf.channels = u8_u16_le(ptr);
                ptr += 2;
                wav->format.wf.sample = u8_u32_le(ptr);
                ptr += 4;
                wav->format.wf.bandwidth = u8_u32_le(ptr);
                ptr += 4;
                wav->format.wf.align = u8_u16_le(ptr);
                ptr += 2;
                wav->format.bits = u8_u16_le(ptr);
        } else if (memcmp(chunk->id, "data", 4) == 0) {
                /* data chunk */
                wav->wav_data = chunk->data;
                wav->wav_len = chunk->len;
        } else {
                /* other chunk */
        }
}

int wav_file_parse(const char *path, wav_file_t *wav)
{
        int err;

        wav->file.name = path;
        err = file_load(&wav->file);
        if (err < 0) {
                return err;
        }

        return wav_head_check(wav, chunk_process);
}

#if 0
void mainxxx(void)
{
        wav_file_t wav;
        memset(&wav, 0, sizeof(wav));
        wav_file_parse("f:/syc/vmshare/xry-djx.wav", &wav);
}
#endif
