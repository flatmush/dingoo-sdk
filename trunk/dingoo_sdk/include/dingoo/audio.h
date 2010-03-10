#ifndef __audio_h__
#define __audio_h__

/* Audio Sample Format */
#define	AFMT_U8			8
#define AFMT_S16_LE		16

/* PCM ioctl command */
#define PCM_SET_SAMPLE_RATE 0
#define PCM_SET_CHANNEL     1
#define PCM_SET_FORMAT      2
#define PCM_SET_VOL         3
#define PCM_GET_VOL         4
#define PCM_SET_HP_VOL      6
#define PCM_GET_HP_VOL      7
#define PCM_GET_SPACE       5
#define PCM_SET_PAUSE       8
#define PCM_SET_PLAY        9
#define PCM_RESET           10
#define PCM_SET_REPLAY      11
#define PCM_SET_RECORD      12
#define PCM_SET_MUTE        13
#define PCM_SET_EQPLAY      14
#define PCM_SET_VOLFUNCTION 15
#define PCM_GET_SAMPLE_MAX  16
#define PCM_SET_RECORD_FM   17

// The following are found in the ingenic ucosii source.
extern int   pcm_ioctl(unsigned int cmd, unsigned long arg);
extern int   pcm_can_write();
extern int   pcm_can_read();
extern int   pcm_read(char* buffer, int count);
extern int   pcm_write(char* buffer, int count);

// The following come from joyrider and from disassembly.
extern void* waveout_open(void*);
extern int   waveout_write(void*, char* buffer, int count);
extern int   waveout_close(void*);
extern void  waveout_close_at_once(void*);
extern void  waveout_reset();
extern int   waveout_ioctl(void*, unsigned int cmd, unsigned long arg);
extern int   waveout_can_write();
extern int   waveout_set_volume(unsigned int vol);
extern int   waveout_get_volume(unsigned int);
extern void  codec_ticker_handler();

// The following values are educated guesses.
extern void* wavein_open(void*);
extern int   wavein_read(void*, char* buffer, int count);
extern int   wavein_close(void*);
extern int   wavein_can_read();
extern void  wavein_reset();
extern int   wavein_ioctl(void*, unsigned int cmd, unsigned long arg);

extern void         jz_audio_reset();
extern void         i2s_codec_close(unsigned int);
extern int          jz_audio_open(unsigned int);
extern void         codec_delay_us(unsigned int);
extern unsigned int codec_set_linein_volume(unsigned int);
extern void         init_jz_i2s();

extern unsigned long StartSwTimer(unsigned long handle, unsigned int count, void* handler, unsigned long option);
extern unsigned long StopSwTimer(unsigned long handle);

extern void jz_update_filler(int bits, int channels);
extern int  jz_audio_set_speed(int rate);
extern int  record_fill_1x8_u(unsigned long dst_start, int count, int id);
extern int  record_fill_2x8_u(unsigned long dst_start, int count, int id);
extern int  record_fill_1x16_s(unsigned long dst_start, int count, int id);
extern int  record_fill_2x16_s(unsigned long dst_start, int count, int id);
extern void replay_fill_1x8_u(unsigned long src_start, int count, int id);
extern void replay_fill_2x8_u(unsigned long src_start, int count, int id);
extern void replay_fill_1x16_s(unsigned long src_start, int count, int id);
extern void replay_fill_2x16_s(unsigned long src_start, int count, int id);
//attach_jz_i2s

extern void jz_i2s_replay_dma_irq(unsigned int);
extern void jz_i2s_record_dma_irq(unsigned int);

#endif
