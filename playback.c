/*

Example 3 - Simple sound playback

This example reads standard from input and writes
to the default PCM device for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

int main(int argc,char *argv[])
{  
  snd_pcm_t *handle;
  /* Open PCM device for playback. */
  int rc = snd_pcm_open(&handle, argv[1], SND_PCM_STREAM_PLAYBACK, 0);
  if (rc < 0)
  {
    fprintf(stderr, "unable to open pcm device: %s\n",snd_strerror(rc));
    exit(1);
  }
  
  snd_pcm_hw_params_t *params;
  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,SND_PCM_FORMAT_S16_LE);

  /* Two channels (stereo) */
  snd_pcm_hw_params_set_channels(handle, params, 2);

  /* 44100 bits/second sampling rate (CD quality) */
  unsigned int val = 48000;
  int dir;
  snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);
  fprintf(stderr, "freq: %d\n",val);

  /* Set period size to 32 frames. */
  snd_pcm_uframes_t frames = 48;
  snd_pcm_hw_params_set_period_size_near(handle,params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0)
  {
    fprintf(stderr, "unable to set hw parameters: %s\n",snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params, &frames,&dir);

  int size = frames * 4; /* 2 bytes/sample, 2 channels */
  fprintf(stderr, "buffer size:%d\n",size);
  char *buffer = (char *) malloc(size);

  char *buffer2 = (char *) malloc(size/2);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params, &val, &dir);
  /* 5 seconds in microseconds divided by 
   * period time */
  long loops = 5000000 / val;

  while (loops > 0)
  {
    loops--;
    rc = read(0, buffer2, size/2);
    if (rc == 0)
    {
      fprintf(stderr, "end of file on input\n");
      break;
    }
    else if (rc != size/2)
    {
      fprintf(stderr,"short read: read %d bytes\n", rc);
    }
    int i;
    for(i=0;i<size/2;i++)
    {
      if(i%2 == 0)
      {
        buffer[2*i]=buffer2[i];
        buffer[2*i+2]=buffer2[i];
      }
      else
      {
        buffer[2*i-1]=buffer2[i];
        buffer[2*i+1]=buffer2[i];
      }
      
    }

    rc = snd_pcm_writei(handle, buffer,frames);
    if (rc == -EPIPE)
    {
      /* EPIPE means underrun */
      fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(handle);
    }
    else if (rc < 0)
    {
      fprintf(stderr,"error from writei: %s\n",snd_strerror(rc));
    }
    else if (rc != (int)frames)
    {
      fprintf(stderr, "short write, write %d frames\n", rc);
    }
  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);
  
  return 0;
}
