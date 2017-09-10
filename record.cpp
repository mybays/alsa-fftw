#include <iostream>

#include <alsa/asoundlib.h>

 

int main()

{

       long                             loops;

       int                                       rc;

       int                                       size;

       snd_pcm_t*                         handle;

       snd_pcm_hw_params_t*      params;

       unsigned int                  val;

       int                                       dir;

       snd_pcm_uframes_t             frames;

       char*                                  buffer;

 

       if ( (rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_CAPTURE, 0)) < 0)

       {

              std::cerr << "unable to open pcm device: " << snd_strerror(rc) << std::endl;

              exit(1);

       }

 

       snd_pcm_hw_params_alloca(&params);

 

       snd_pcm_hw_params_any(handle, params);

 

       snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

 

       snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

 

       snd_pcm_hw_params_set_channels(handle, params, 2);

 

       val = 44100;

       snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

 

       if ( (rc = snd_pcm_hw_params(handle, params)) < 0)

       {

              std::cerr << "unable to set hw parameters: " << snd_strerror(rc) << std::endl;

              exit(1);

       }

 

       snd_pcm_hw_params_get_period_size(params, &frames, &dir);

 

       size = frames * 4;

       buffer = new char[size];

 

       snd_pcm_hw_params_get_period_time(params, &val, &dir);

 

       loops = 5000000 / val;

 

       while (loops > 0)

       {

              loops --;

              rc = snd_pcm_readi(handle, buffer, frames);

              if (rc == -EPIPE)

              {

                     std::cerr << "overrun occurred" << std::endl;

                     snd_pcm_prepare(handle);

              }

              else if (rc < 0)

                     std::cerr << "error from read: " << snd_strerror(rc) << std::endl;

              else if ( rc != (int)frames)

                     std::cerr << "short read, read " << rc << " frames" << std::endl;

              rc = write(1, buffer, size);

              if (rc != size)

                     std::cerr << "short write: wrote " << rc << " bytes" << std::endl;

       }

 

       snd_pcm_drain(handle);

       snd_pcm_close(handle);

       free(buffer);

      

       return 0;

}


