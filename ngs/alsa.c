/* 
  A Minimal Capture Program

  This program opens an audio interface for capture, configures it for
  stereo, 16 bit, 44.1kHz, interleaved conventional read/write
  access. Then its reads a chunk of random data from it, and exits. It
  isn't meant to be a real program.

  From on Paul David's tutorial : http://equalarea.com/paul/alsa-audio.html

  Fixes rate and buffer problems

*/

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <string.h>
#include <unistd.h>
#include<signal.h>

#include "ent.h"


int done = 0;

void sig_handler(int signo)
{
    if (signo == SIGINT) {
	printf("received SIGINT\n");
	done = 1;
    }
}

//==============================================================================

void filter(uint8_t *buf,int len,uint8_t *buf2,int *len2)
{
int i;
    int count = 0;
    for(i=0; i < len; i+=4) {
	*buf2++ = buf[i+1];
	count++;
	*buf2++ = buf[i];
	count++;
    }
    *len2 = count;
}

/*
void filter(uint8_t *buf,int len,uint8_t *buf2,int *len2)
{
    int randomData = open("/dev/urandom", O_RDONLY);
    if (randomData < 0) {
	printf("something went wrong\n");
        return;
    }
    ssize_t result = read(randomData, buf2, len/2);
    if (result < 0)
    {
        printf("something went wrong2\n");
	return;
    }
    *len2 = len/2;
}
*/

//==============================================================================
	      
//int main (int argc, char *argv[])
void loop(void (*cb)(double))
{
  int i;
  int err;
  uint8_t *buffer;
  uint8_t *buffer2;
  int buffer_frames = 44100;
  unsigned int rate = 44100;
  snd_pcm_t *capture_handle;
  snd_pcm_hw_params_t *hw_params;

//    test();
//    exit(1);

  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

    if (signal(SIGINT, sig_handler) == SIG_ERR) {
	printf("\ncan't catch SIGINT\n");
    }

  if ((err = snd_pcm_open (&capture_handle, "hw:0,0", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    fprintf (stderr, "cannot open audio device %s (%s)\n", 
             "hw:0,0",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "audio interface opened\n");
		   
  if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
    fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params allocated\n");
				 
  if ((err = snd_pcm_hw_params_any (capture_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params initialized\n");
	
  if ((err = snd_pcm_hw_params_set_access (capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    fprintf (stderr, "cannot set access type (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params access setted\n");
	
  if ((err = snd_pcm_hw_params_set_format (capture_handle, hw_params, format)) < 0) {
    fprintf (stderr, "cannot set sample format (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params format setted\n");
	
  if ((err = snd_pcm_hw_params_set_rate_near (capture_handle, hw_params, &rate, 0)) < 0) {
    fprintf (stderr, "cannot set sample rate (%s)\n",
             snd_strerror (err));
    exit (1);
  }
	
  fprintf(stdout, "hw_params rate setted\n");

  if ((err = snd_pcm_hw_params_set_channels (capture_handle, hw_params, 2)) < 0) {
    fprintf (stderr, "cannot set channel count (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params channels setted\n");
	
  if ((err = snd_pcm_hw_params (capture_handle, hw_params)) < 0) {
    fprintf (stderr, "cannot set parameters (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "hw_params setted\n");
	
  snd_pcm_hw_params_free (hw_params);

  fprintf(stdout, "hw_params freed\n");
	
  if ((err = snd_pcm_prepare (capture_handle)) < 0) {
    fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
    exit (1);
  }

  fprintf(stdout, "audio interface prepared\n");

  int len = buffer_frames * snd_pcm_format_width(format) / 8 * 2;
  buffer = malloc(len);
  buffer2 = malloc(len);

  fprintf(stdout, "buffer allocated %d\n",len);

//    FILE * pFile;
//    pFile = fopen ("wave.bin", "wb");
    
  for (i = 0; !done; ++i) {
    if ((err = snd_pcm_readi (capture_handle, buffer, buffer_frames)) != buffer_frames) {
      if(!done) {
	fprintf (stderr, "read from audio interface failed (%s)\n",  snd_strerror (err));
      }
      break;
    }
//    fprintf(stdout, "read %d done\n", i);
//    fwrite (buffer , sizeof(char), len, pFile);

    int len2 = 0;
    filter(buffer,len,buffer2,&len2);
    double csq = 0;
    calc(buffer2,len2,0,&csq);	// 0-bytes, 1-bits
    (*cb)(csq);
  }

//  fclose (pFile);
    free(buffer);
    free(buffer2);

  fprintf(stdout, "buffer freed\n");
	
  snd_pcm_close (capture_handle);
  fprintf(stdout, "audio interface closed\n");

  exit (0);
}

