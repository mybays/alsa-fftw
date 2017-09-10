/*

Example 4 - Simple sound recording

This example reads from the default PCM device
and writes to standard output for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

int main(int argc,char *argv[])
{

  snd_pcm_t *handle;
  /* Open PCM device for recording (capture). */
  int rc = snd_pcm_open(&handle, argv[1],SND_PCM_STREAM_CAPTURE, 0);
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

  /* Non Interleaved mode */
  snd_pcm_hw_params_set_access(handle, params,SND_PCM_ACCESS_RW_INTERLEAVED);

  /* Signed 16-bit little-endian format */
  snd_pcm_hw_params_set_format(handle, params,SND_PCM_FORMAT_S16_LE);

  /* Two channels (stereo) */
  //snd_pcm_hw_params_set_channels(handle, params, 2);
  /* One channels (mono)  */
  snd_pcm_hw_params_set_channels(handle, params, 1);

  int dir;
  /* 44100 bits/second sampling rate (CD quality) */
  unsigned int val = 48000;
  snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

  /* Set period size to 32 frames. */
  snd_pcm_uframes_t frames = 48;
  snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0)
  {
    fprintf(stderr,"unable to set hw parameters: %s\n",snd_strerror(rc));
    exit(1);
  }

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(params,&frames, &dir);
  int size = frames * 2; /* 2 bytes/sample, 2 channels */
  char *buffer = (char *) malloc(size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(params,&val, &dir);
  long loops = 5000000 / val;

  while (loops > 0)
  {
    loops--;
    rc = snd_pcm_readi(handle, buffer, frames);
    if (rc == -EPIPE)
    {
      /* EPIPE means overrun */
      fprintf(stderr, "overrun occurred\n");
      snd_pcm_prepare(handle);
    }
    else if (rc < 0)
    {
      fprintf(stderr,"error from read: %s\n", snd_strerror(rc));
    }
    else if (rc != (int)frames)
    {
      fprintf(stderr, "short read, read %d frames\n", rc);
    }
    ///*
    rc = write(1, buffer, size);
    if (rc != size)
      fprintf(stderr, "short write: wrote %d bytes\n", rc);
    //*/
  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  free(buffer);

  return 0;
}
