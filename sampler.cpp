/*
 * Parallel port audio digitizer
 *
 * Toni Wilen 2010
 */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "events.h"
#include "custom.h"
#include "sampler.h"

#include "dxwrap.h"

#include <dsound.h>

#include "win32.h"

#define SAMPLESIZE 4
#define RECORDBUFFER 40000
#define SAMPLEBUFFER 4000

static LPDIRECTSOUNDCAPTURE lpDS2r = NULL;
static LPDIRECTSOUNDCAPTUREBUFFER lpDSBprimary2r = NULL;
static LPDIRECTSOUNDCAPTUREBUFFER lpDSB2r = NULL;
static int inited;
static uae_u8 *samplebuffer;
static int samplerate = 44100;
static float clockspersample;
static int vsynccnt;

static int capture_init (void)
{
	HRESULT hr;
	DSCBUFFERDESC sound_buffer_rec;
	WAVEFORMATEX wavfmt;

	wavfmt.wFormatTag = WAVE_FORMAT_PCM;
	wavfmt.nChannels = 2;
	wavfmt.nSamplesPerSec = samplerate;
	wavfmt.wBitsPerSample = 16;
	wavfmt.nBlockAlign = wavfmt.wBitsPerSample / 8 * wavfmt.nChannels;
	wavfmt.nAvgBytesPerSec = wavfmt.nBlockAlign * wavfmt.nSamplesPerSec;
	wavfmt.cbSize = 0;

	hr = DirectSoundCaptureCreate (&record_devices[currprefs.win32_samplersoundcard].guid, &lpDS2r, NULL);
	if (FAILED (hr)) {
		write_log (L"SAMPLER: DirectSoundCaptureCreate() failure: %s\n", DXError (hr));
		return 0;
	}
	memset (&sound_buffer_rec, 0, sizeof (DSCBUFFERDESC));
	sound_buffer_rec.dwSize = sizeof (DSCBUFFERDESC);
	sound_buffer_rec.dwBufferBytes = RECORDBUFFER * SAMPLESIZE;
	sound_buffer_rec.lpwfxFormat = &wavfmt;
	sound_buffer_rec.dwFlags = 0 ;

	hr = lpDS2r->CreateCaptureBuffer (&sound_buffer_rec, &lpDSB2r, NULL);
	if (FAILED (hr)) {
		write_log (L"SAMPLER: CreateCaptureSoundBuffer() failure: %s\n", DXError(hr));
		return 0;
	}

	hr = lpDSB2r->Start (DSCBSTART_LOOPING);
	if (FAILED (hr)) {
		write_log (L"SAMPLER: DirectSoundCaptureBuffer_Start failed: %s\n", DXError (hr));
		return 0;
	}
	samplebuffer = xcalloc (uae_u8, SAMPLEBUFFER * SAMPLESIZE);
	write_log (L"SAMPLER: Parallel port sampler initialized\n");
	return 1;
}

static void capture_free (void)
{
	if (lpDSB2r) {
		lpDSB2r->Stop ();
		lpDSB2r->Release ();
		write_log (L"SAMPLER: Parallel port sampler freed\n");
	}
	lpDSB2r = NULL;
	if (lpDS2r)
		lpDS2r->Release ();
	lpDS2r = NULL;
	xfree (samplebuffer);
	samplebuffer = NULL;
}

static evt oldcycles;
static int oldoffset;

uae_u8 sampler_getsample (int channel)
{
	HRESULT hr;
	static DWORD cap_pos;
	DWORD t, cur_pos;
	void *p1, *p2;
	DWORD len1, len2;
	evt cycles;
	int offset;
	int sample, samplecnt, diff;

//	if (channel)
//		return 0;
	channel = 0;

	if (!inited) {
		if (!capture_init ()) {
			capture_free ();
			return 0;
		}
		inited = 1;
		oldcycles = get_cycles ();
		oldoffset = -1;
		hr = lpDSB2r->GetCurrentPosition (&t, &cap_pos);
		if (FAILED (hr)) {
			sampler_free ();
			return 0;
		}	
	}
	uae_s16 *sbuf = (uae_s16*)samplebuffer;

	vsynccnt = 0;
	sample = 0;
	samplecnt = 0;
	cycles = get_cycles () - oldcycles;
	offset = cycles / clockspersample;
	if (oldoffset < 0 || offset >= SAMPLEBUFFER || offset < 0) {
		if (oldoffset >= 0 && offset >= SAMPLEBUFFER) {
			while (oldoffset < SAMPLEBUFFER) {
				sample += sbuf[oldoffset * SAMPLESIZE / 2 + channel];
				oldoffset++;
				samplecnt++;
			}
		}
		//write_log (L"%d\n", cur_pos - cap_pos);
		hr = lpDSB2r->Lock (cap_pos, SAMPLEBUFFER * SAMPLESIZE, &p1, &len1, &p2, &len2, 0);
		if (FAILED (hr))
			return 0;
		memcpy (samplebuffer, p1, len1);
		if (p2)
			memcpy (samplebuffer + len1, p2, len2);
		lpDSB2r->Unlock (p1, len1, p2, len2);
		cap_pos += SAMPLEBUFFER * SAMPLESIZE;

		hr = lpDSB2r->GetCurrentPosition (&t, &cur_pos);
		if (FAILED (hr))
			return 0;
		if (cur_pos >= cap_pos)
			diff = cur_pos - cap_pos;
		else
			diff = RECORDBUFFER * SAMPLESIZE - cap_pos + cur_pos;
		if (diff > RECORDBUFFER * SAMPLESIZE - 4 * SAMPLEBUFFER * SAMPLESIZE) {
			write_log (L"!");
			diff -= RECORDBUFFER * SAMPLESIZE;
		} else if (diff > RECORDBUFFER * SAMPLESIZE / 2) {
			cap_pos = cur_pos;
			write_log (L"*");
			diff = 0;
		}

		cap_pos += diff;
		if (cap_pos < 0)
			cap_pos += RECORDBUFFER * SAMPLESIZE;
		if (cap_pos >= RECORDBUFFER * SAMPLESIZE)
			cap_pos -= RECORDBUFFER * SAMPLESIZE;

		if (offset < 0)
			offset = 0;
		if (offset >= SAMPLEBUFFER)
			offset -= SAMPLEBUFFER;
		oldoffset = 0;
		oldcycles = get_cycles ();
	}
	while (oldoffset <= offset) {
		sample += sbuf[oldoffset * SAMPLESIZE / 2 + channel];
		samplecnt++;
		oldoffset++;
	}
	oldoffset = offset;
	if (samplecnt > 0)
		sample /= samplecnt;
	return (sample / 256) - 128;
}

int sampler_init (void)
{
	clockspersample = (float)maxvpos * maxhpos * vblank_hz * CYCLE_UNIT / samplerate;
	if (currprefs.win32_samplersoundcard < 0)
		return 0;
	return 1;
}

void sampler_free (void)
{
	inited = 0;
	vsynccnt = 0;
	capture_free ();
}

void sampler_vsync (void)
{
	if (!inited)
		return;
	vsynccnt++;
	if (vsynccnt > 50) {
		sampler_free ();
		return;
	}
}