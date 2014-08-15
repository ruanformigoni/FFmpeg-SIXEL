/*
 * Copyright (c) 2014 Hayaki Saito
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <sixel.h>
#include <stdio.h>
#include <unistd.h>
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "avdevice.h"
#include "libavutil/time.h"


static const unsigned char fixed_palette[] = {
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x80, 0x00,
    0x00, 0x00, 0x80, 0x80, 0x00, 0x80, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xc0,
    0x80, 0x80, 0x80, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0x00,
    0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x5f, 0x00, 0x00, 0x87, 0x00, 0x00, 0xaf,
    0x00, 0x00, 0xd7, 0x00, 0x00, 0xff, 0x00, 0x5f, 0x00, 0x00, 0x5f, 0x5f,
    0x00, 0x5f, 0x87, 0x00, 0x5f, 0xaf, 0x00, 0x5f, 0xd7, 0x00, 0x5f, 0xff,
    0x00, 0x87, 0x00, 0x00, 0x87, 0x5f, 0x00, 0x87, 0x87, 0x00, 0x87, 0xaf,
    0x00, 0x87, 0xd7, 0x00, 0x87, 0xff, 0x00, 0xaf, 0x00, 0x00, 0xaf, 0x5f,
    0x00, 0xaf, 0x87, 0x00, 0xaf, 0xaf, 0x00, 0xaf, 0xd7, 0x00, 0xaf, 0xff,
    0x00, 0xd7, 0x00, 0x00, 0xd7, 0x5f, 0x00, 0xd7, 0x87, 0x00, 0xd7, 0xaf,
    0x00, 0xd7, 0xd7, 0x00, 0xd7, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0x5f,
    0x00, 0xff, 0x87, 0x00, 0xff, 0xaf, 0x00, 0xff, 0xd7, 0x00, 0xff, 0xff,
    0x5f, 0x00, 0x00, 0x5f, 0x00, 0x5f, 0x5f, 0x00, 0x87, 0x5f, 0x00, 0xaf,
    0x5f, 0x00, 0xd7, 0x5f, 0x00, 0xff, 0x5f, 0x5f, 0x00, 0x5f, 0x5f, 0x5f,
    0x5f, 0x5f, 0x87, 0x5f, 0x5f, 0xaf, 0x5f, 0x5f, 0xd7, 0x5f, 0x5f, 0xff,
    0x5f, 0x87, 0x00, 0x5f, 0x87, 0x5f, 0x5f, 0x87, 0x87, 0x5f, 0x87, 0xaf,
    0x5f, 0x87, 0xd7, 0x5f, 0x87, 0xff, 0x5f, 0xaf, 0x00, 0x5f, 0xaf, 0x5f,
    0x5f, 0xaf, 0x87, 0x5f, 0xaf, 0xaf, 0x5f, 0xaf, 0xd7, 0x5f, 0xaf, 0xff,
    0x5f, 0xd7, 0x00, 0x5f, 0xd7, 0x5f, 0x5f, 0xd7, 0x87, 0x5f, 0xd7, 0xaf,
    0x5f, 0xd7, 0xd7, 0x5f, 0xd7, 0xff, 0x5f, 0xff, 0x00, 0x5f, 0xff, 0x5f,
    0x5f, 0xff, 0x87, 0x5f, 0xff, 0xaf, 0x5f, 0xff, 0xd7, 0x5f, 0xff, 0xff,
    0x87, 0x00, 0x00, 0x87, 0x00, 0x5f, 0x87, 0x00, 0x87, 0x87, 0x00, 0xaf,
    0x87, 0x00, 0xd7, 0x87, 0x00, 0xff, 0x87, 0x5f, 0x00, 0x87, 0x5f, 0x5f,
    0x87, 0x5f, 0x87, 0x87, 0x5f, 0xaf, 0x87, 0x5f, 0xd7, 0x87, 0x5f, 0xff,
    0x87, 0x87, 0x00, 0x87, 0x87, 0x5f, 0x87, 0x87, 0x87, 0x87, 0x87, 0xaf,
    0x87, 0x87, 0xd7, 0x87, 0x87, 0xff, 0x87, 0xaf, 0x00, 0x87, 0xaf, 0x5f,
    0x87, 0xaf, 0x87, 0x87, 0xaf, 0xaf, 0x87, 0xaf, 0xd7, 0x87, 0xaf, 0xff,
    0x87, 0xd7, 0x00, 0x87, 0xd7, 0x5f, 0x87, 0xd7, 0x87, 0x87, 0xd7, 0xaf,
    0x87, 0xd7, 0xd7, 0x87, 0xd7, 0xff, 0x87, 0xff, 0x00, 0x87, 0xff, 0x5f,
    0x87, 0xff, 0x87, 0x87, 0xff, 0xaf, 0x87, 0xff, 0xd7, 0x87, 0xff, 0xff,
    0xaf, 0x00, 0x00, 0xaf, 0x00, 0x5f, 0xaf, 0x00, 0x87, 0xaf, 0x00, 0xaf,
    0xaf, 0x00, 0xd7, 0xaf, 0x00, 0xff, 0xaf, 0x5f, 0x00, 0xaf, 0x5f, 0x5f,
    0xaf, 0x5f, 0x87, 0xaf, 0x5f, 0xaf, 0xaf, 0x5f, 0xd7, 0xaf, 0x5f, 0xff,
    0xaf, 0x87, 0x00, 0xaf, 0x87, 0x5f, 0xaf, 0x87, 0x87, 0xaf, 0x87, 0xaf,
    0xaf, 0x87, 0xd7, 0xaf, 0x87, 0xff, 0xaf, 0xaf, 0x00, 0xaf, 0xaf, 0x5f,
    0xaf, 0xaf, 0x87, 0xaf, 0xaf, 0xaf, 0xaf, 0xaf, 0xd7, 0xaf, 0xaf, 0xff,
    0xaf, 0xd7, 0x00, 0xaf, 0xd7, 0x5f, 0xaf, 0xd7, 0x87, 0xaf, 0xd7, 0xaf,
    0xaf, 0xd7, 0xd7, 0xaf, 0xd7, 0xff, 0xaf, 0xff, 0x00, 0xaf, 0xff, 0x5f,
    0xaf, 0xff, 0x87, 0xaf, 0xff, 0xaf, 0xaf, 0xff, 0xd7, 0xaf, 0xff, 0xff,
    0xd7, 0x00, 0x00, 0xd7, 0x00, 0x5f, 0xd7, 0x00, 0x87, 0xd7, 0x00, 0xaf,
    0xd7, 0x00, 0xd7, 0xd7, 0x00, 0xff, 0xd7, 0x5f, 0x00, 0xd7, 0x5f, 0x5f,
    0xd7, 0x5f, 0x87, 0xd7, 0x5f, 0xaf, 0xd7, 0x5f, 0xd7, 0xd7, 0x5f, 0xff,
    0xd7, 0x87, 0x00, 0xd7, 0x87, 0x5f, 0xd7, 0x87, 0x87, 0xd7, 0x87, 0xaf,
    0xd7, 0x87, 0xd7, 0xd7, 0x87, 0xff, 0xd7, 0xaf, 0x00, 0xd7, 0xaf, 0x5f,
    0xd7, 0xaf, 0x87, 0xd7, 0xaf, 0xaf, 0xd7, 0xaf, 0xd7, 0xd7, 0xaf, 0xff,
    0xd7, 0xd7, 0x00, 0xd7, 0xd7, 0x5f, 0xd7, 0xd7, 0x87, 0xd7, 0xd7, 0xaf,
    0xd7, 0xd7, 0xd7, 0xd7, 0xd7, 0xff, 0xd7, 0xff, 0x00, 0xd7, 0xff, 0x5f,
    0xd7, 0xff, 0x87, 0xd7, 0xff, 0xaf, 0xd7, 0xff, 0xd7, 0xd7, 0xff, 0xff,
    0xff, 0x00, 0x00, 0xff, 0x00, 0x5f, 0xff, 0x00, 0x87, 0xff, 0x00, 0xaf,
    0xff, 0x00, 0xd7, 0xff, 0x00, 0xff, 0xff, 0x5f, 0x00, 0xff, 0x5f, 0x5f,
    0xff, 0x5f, 0x87, 0xff, 0x5f, 0xaf, 0xff, 0x5f, 0xd7, 0xff, 0x5f, 0xff,
    0xff, 0x87, 0x00, 0xff, 0x87, 0x5f, 0xff, 0x87, 0x87, 0xff, 0x87, 0xaf,
    0xff, 0x87, 0xd7, 0xff, 0x87, 0xff, 0xff, 0xaf, 0x00, 0xff, 0xaf, 0x5f,
    0xff, 0xaf, 0x87, 0xff, 0xaf, 0xaf, 0xff, 0xaf, 0xd7, 0xff, 0xaf, 0xff,
    0xff, 0xd7, 0x00, 0xff, 0xd7, 0x5f, 0xff, 0xd7, 0x87, 0xff, 0xd7, 0xaf,
    0xff, 0xd7, 0xd7, 0xff, 0xd7, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x5f,
    0xff, 0xff, 0x87, 0xff, 0xff, 0xaf, 0xff, 0xff, 0xd7, 0xff, 0xff, 0xff,
    0x08, 0x08, 0x08, 0x12, 0x12, 0x12, 0x1c, 0x1c, 0x1c, 0x26, 0x26, 0x26,
    0x30, 0x30, 0x30, 0x3a, 0x3a, 0x3a, 0x44, 0x44, 0x44, 0x4e, 0x4e, 0x4e,
    0x58, 0x58, 0x58, 0x62, 0x62, 0x62, 0x6c, 0x6c, 0x6c, 0x76, 0x76, 0x76,
    0x80, 0x80, 0x80, 0x8a, 0x8a, 0x8a, 0x94, 0x94, 0x94, 0x9e, 0x9e, 0x9e,
    0xa8, 0xa8, 0xa8, 0xb2, 0xb2, 0xb2, 0xbc, 0xbc, 0xbc, 0xc6, 0xc6, 0xc6,
    0xd0, 0xd0, 0xd0, 0xda, 0xda, 0xda, 0xe4, 0xe4, 0xe4, 0xee, 0xee, 0xee,
};


typedef struct SIXELContext {
    AVClass *class;
    AVRational time_base;   /**< Time base */
    int64_t    time_frame;  /**< Current time */
    AVRational framerate;
    char *reset_position;
    int row;
    int col;
    int reqcolors;
    LSOutputContextPtr output;
    sixel_palette_t *palette;
    int fixedpal;
    enum methodForDiffuse diffuse;
    int threshold;
    int dropframe;
    int ignoredelay;
} SIXELContext;

static int detect_scene_change(SIXELContext *const c,
                               sixel_palette_t *palette)
{
    int score;
    int i;
    unsigned int r = 0;
    unsigned int g = 0;
    unsigned int b = 0;
    static unsigned int average_r = 0;
    static unsigned int average_g = 0;
    static unsigned int average_b = 0;

    if (c->palette == NULL) {
        goto detected;
    }

    if (c->palette->origcolors * 4 < palette->origcolors * 3) {
        goto detected;
    }

    if (c->palette->origcolors * 3 > palette->origcolors * 4) {
        goto detected;
    }

    for (i = 0; i < palette->ncolors; i++) {
        r += palette->data[i * 3 + 0];
        g += palette->data[i * 3 + 1];
        b += palette->data[i * 3 + 2];
    }
    score = (r - average_r) * (r - average_r)
          + (g - average_g) * (g - average_g)
          + (b - average_b) * (b - average_b);

    if (score > c->threshold * palette->ncolors * palette->ncolors) {
        goto detected;
    }

    return 0;

detected:
    average_r = r;
    average_g = g;
    average_b = b;
    return 1;
}

static int prepare_static_palette(SIXELContext *const c,
                                  AVCodecContext *const codec)
{
    c->palette = sixel_palette_create(c->reqcolors);
    memcpy(c->palette->data, fixed_palette, c->reqcolors * 3);

    return 0;
}

static int prepare_dynamic_palette(SIXELContext *const c,
                                   AVCodecContext *const codec,
                                   AVPacket *const pkt)
{
    int ret;
    static sixel_palette_t *palette = NULL;

    if (!palette) {
        palette = sixel_palette_create(c->reqcolors);
    }
    ret = sixel_prepare_palette(pkt->data, codec->width, codec->height, 3,
                                LARGE_NORM, REP_CENTER_BOX, QUALITY_HIGH,
                                palette);
    if (ret != 0) {
        sixel_palette_unref(palette);
        palette = NULL;
        return (-1);
    }
    if (detect_scene_change(c, palette)) {
        if (c->palette) {
            sixel_palette_unref(c->palette);
        }
        c->palette = palette;
        palette = NULL;
    }
    return 0;
}

static FILE *sixel_output_file = NULL;

static int sixel_putchar(int c)
{
    return fputc(c, sixel_output_file);
}

static int sixel_printf(char const *fmt, ...)
{
    int ret;
    va_list ap;

    va_start(ap, fmt);
    ret = vfprintf(sixel_output_file, fmt, ap);
    va_end(ap);

    return ret;
}

static int sixel_write_header(AVFormatContext *s)
{
    SIXELContext *c = s->priv_data;
    AVCodecContext *codec = s->streams[0]->codec;
    int ret = 0;

    if (s->nb_streams > 1
        || codec->codec_type != AVMEDIA_TYPE_VIDEO
        || codec->codec_id   != CODEC_ID_RAWVIDEO) {
        av_log(s, AV_LOG_ERROR, "Only supports one rawvideo stream\n");
        return AVERROR(EINVAL);
    }

    if (codec->pix_fmt != PIX_FMT_RGB24) {
        av_log(s, AV_LOG_ERROR,
               "Unsupported pixel format '%s', choose rgb24\n",
               av_get_pix_fmt_name(codec->pix_fmt));
        return AVERROR(EINVAL);
    }
    if (!s->filename || strcmp(s->filename, "pipe:") == 0) {
        sixel_output_file = stdout;
        c->output = LSOutputContext_create(putchar, printf);
    } else {
        sixel_output_file = fopen(s->filename, "w");
        c->output = LSOutputContext_create(sixel_putchar, sixel_printf);
    }
    if (!isatty(fileno(sixel_output_file))) {
        c->ignoredelay = 1;
    }
    c->palette = NULL;
    c->reset_position = malloc(64);
    if (c->row <= 1 && c->col <= 1) {
        strcpy(c->reset_position, "\033[H");
    } else {
        sprintf(c->reset_position, "\033[%d;%dH", c->row, c->col);
    }
    fprintf(sixel_output_file, "\033[?25l\0337");
    c->time_base = s->streams[0]->codec->time_base;
    c->time_frame = av_gettime() / av_q2d(c->time_base);

    if (c->fixedpal) {
        ret = prepare_static_palette(c, codec);
        if (ret != 0) {
            return ret;
        }
    }
    return ret;
}

static int sixel_write_packet(AVFormatContext *s, AVPacket *pkt)
{
    SIXELContext * const c = s->priv_data;
    AVCodecContext * const codec = s->streams[0]->codec;
    int ret = 0;
    int64_t curtime, delay;
    struct timespec ts;
    LSImagePtr im;

    if (!c->ignoredelay) {
        /* Calculate the time of the next frame */
        c->time_frame += INT64_C(1000000);

        /* wait based on the frame rate */
        curtime = av_gettime();
        delay = c->time_frame * av_q2d(c->time_base) - curtime;
        if (delay <= 0) {
            if (c->dropframe) {
                if (delay < INT64_C(-1000000) * av_q2d(c->time_base) * 2) {
                    return 0;
                }
            }
        } else {
            ts.tv_sec = delay / 1000000;
            ts.tv_nsec = (delay % 1000000) * 1000;
            nanosleep(&ts, NULL);
        }
    }

    fprintf(sixel_output_file, "%s", c->reset_position);

    if (!c->fixedpal) {
        ret = prepare_dynamic_palette(c, codec, pkt);
        if (ret != 0) {
            return ret;
        }
    }

    /* create intermidiate bitmap image */
    unsigned char *p = malloc(codec->width * codec->height * 3);
    memcpy(p, pkt->data, codec->width * codec->height * 3);

    im = sixel_create_image(p, codec->width, codec->height, 3, c->palette);
    if (!im) {
        return AVERROR(ENOMEM);
    }
    ret = sixel_apply_palette(im, c->diffuse, 1, c->palette->cachetable);
    if (ret != 0) {
        return AVERROR(ret);
    }
    LibSixel_LSImageToSixel(im, c->output);
    LSImage_destroy(im);
    fflush(stdout);
    return 0;
}

static int sixel_write_trailer(AVFormatContext *s)
{
    SIXELContext * const c = s->priv_data;

    if (sixel_output_file && sixel_output_file != stdout) {
        fclose(sixel_output_file);
        sixel_output_file = NULL;
    }
    if (c->output) {
        LSOutputContext_destroy(c->output);
        c->output = NULL;
    }
    if (c->palette) {
        sixel_palette_unref(c->palette);
        c->palette = NULL;
    }
    if (c->reset_position) {
        free(c->reset_position);
        c->reset_position = NULL;
    }
    fprintf(sixel_output_file, "\0338\033[?25h");
    fflush(stdout);
    return 0;
}

#define OFFSET(x) offsetof(SIXELContext, x)
#define ENC AV_OPT_FLAG_ENCODING_PARAM
static const AVOption options[] = {
    { "col",             "left position",          OFFSET(col),         AV_OPT_TYPE_INT,    {.i64 = 1},                1, 256,  ENC },
    { "row",             "top position",           OFFSET(row),         AV_OPT_TYPE_INT,    {.i64 = 1},                1, 256,  ENC },
    { "reqcolors",       "number of colors",       OFFSET(reqcolors),   AV_OPT_TYPE_INT,    {.i64 = 16},               2, 256,  ENC },
    { "fixedpal",        "use fixed palette",      OFFSET(fixedpal),    AV_OPT_TYPE_INT,    {.i64 = 0},                0, 1,    ENC, "fixedpal" },
    { "true",            NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = 1},                0, 0,    ENC, "fixedpal" },
    { "false",           NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = 0},                0, 0,    ENC, "fixedpal" },
    { "diffuse",         "dithering method",       OFFSET(diffuse),     AV_OPT_TYPE_INT,    {.i64 = DIFFUSE_ATKINSON}, 1, 6,    ENC, "diffuse" },
    { "none",            NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = DIFFUSE_NONE},     0, 0,    ENC, "diffuse" },
    { "fs",              NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = DIFFUSE_FS},       0, 0,    ENC, "diffuse" },
    { "atkinson",        NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = DIFFUSE_ATKINSON}, 0, 0,    ENC, "diffuse" },
    { "jajuni",          NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = DIFFUSE_JAJUNI},   0, 0,    ENC, "diffuse" },
    { "stucki",          NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = DIFFUSE_STUCKI},   0, 0,    ENC, "diffuse" },
    { "burkes",          NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = DIFFUSE_BURKES},   0, 0,    ENC, "diffuse" },
    { "scene-threshold", "scene change threshold", OFFSET(threshold),   AV_OPT_TYPE_INT,    {.i64 = 1600},              0, 10000,ENC },
    { "dropframe",       "drop late frames",       OFFSET(dropframe),   AV_OPT_TYPE_INT,    {.i64 = 1},                0, 1,    ENC, "dropframe" },
    { "true",            NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = 1},                0, 0,    ENC, "dropframe" },
    { "false",           NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = 0},                0, 0,    ENC, "dropframe" },
    { "ignoredelay",     "ignore frame timestamp", OFFSET(ignoredelay), AV_OPT_TYPE_INT,    {.i64 = 0},                0, 1,    ENC, "ignoredelay" },
    { "true",            NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = 1},                0, 0,    ENC, "ignoredelay" },
    { "false",           NULL,                     0,                   AV_OPT_TYPE_CONST,  {.i64 = 0},                0, 0,    ENC, "ignoredelay" },
    { NULL },
};

static const AVClass sixel_class = {
    .class_name = "sixel_outdev",
    .item_name  = av_default_item_name,
    .option     = options,
    .version    = LIBAVUTIL_VERSION_INT,
    .category   = AV_CLASS_CATEGORY_DEVICE_VIDEO_OUTPUT,
};

AVOutputFormat ff_sixel_muxer = {
    .name           = "sixel",
    .long_name      = NULL_IF_CONFIG_SMALL("SIXEL terminal device"),
    .priv_data_size = sizeof(SIXELContext),
    .audio_codec    = CODEC_ID_NONE,
    .video_codec    = CODEC_ID_RAWVIDEO,
    .write_header   = sixel_write_header,
    .write_packet   = sixel_write_packet,
    .write_trailer  = sixel_write_trailer,
    .flags          = AVFMT_NOFILE,
//    .flags          = AVFMT_NOFILE | AVFMT_VARIABLE_FPS | AVFMT_NOTIMESTAMPS,
    .priv_class     = &sixel_class,
};
