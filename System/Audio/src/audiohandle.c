#include <audiohandler.h>

void AudioHandler_create(AudioHandler* me, audio_length l, audio_data* d)
{
	me->data = d;
	me->length = l;
	me->index = 0;
}

audio_data AudioHandler_getNext(AudioHandler* me)
{
	audio_data ret = me->data[me->index];
	me->index = (me->index+1) % me->length;
	return ret;
}
