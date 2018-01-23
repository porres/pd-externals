/* Sinusoidal oscillator based on C's sine function.
 *
 * Implementation by Dario Sanfilippo (2017-2018): sanfilippo.dario@gmail.com.
 *
 * This external uses Katja Vetter's PD lib builder:
 * https://github.com/pure-data/pd-lib-builder.
 */

#include <math.h>
#include "m_pd.h"

#define TWOPI (M_PI * 2.0)

static t_class *sinusoid_tilde_class;

typedef struct _sinusoid_tilde {
    t_object x_obj;
    double x_phase;
    double x_speriod;
    float x_f;
} t_sinusoid_tilde;

static void *sinusoid_tilde_new(void) {
    t_sinusoid_tilde *x = (t_sinusoid_tilde *)pd_new(sinusoid_tilde_class);
    outlet_new(&x->x_obj, gensym("signal"));
    x->x_f = 0.0;
    x->x_phase = 0.0;
    return (x);
}

static t_int *sinusoid_tilde_perform(t_int *w) {
    t_sinusoid_tilde *x = (t_sinusoid_tilde *)(w[1]);
    t_sample *in = (t_sample *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    double speriod = x->x_speriod;
    double phase = x->x_phase;

    while (n--) {
        double incr = *in++ * TWOPI * speriod;
        *out++ = sin(phase);
        phase += incr;
        if (phase >= TWOPI)
            phase -= TWOPI;
        if (phase < 0.0)
            phase += TWOPI;
    }

    x->x_phase = phase;

    return (w+5);
}

void sinusoid_tilde_dsp(t_sinusoid_tilde *x, t_signal **sp) {
    x->x_speriod = 1.0/sp[0]->s_sr;
    dsp_add(
        sinusoid_tilde_perform,
        4,
        x,
        sp[0]->s_vec,
        sp[1]->s_vec,
        sp[0]->s_n);
}

void sinusoid_tilde_setup(void) {
    sinusoid_tilde_class = class_new(
        gensym("sinusoid~"),
        (t_newmethod)sinusoid_tilde_new,
        0,
        sizeof(t_sinusoid_tilde),
        0,
        0);

    class_addmethod(
        sinusoid_tilde_class,
        (t_method)sinusoid_tilde_dsp,
        gensym("dsp"),
        0);

    CLASS_MAINSIGNALIN(
        sinusoid_tilde_class,
        t_sinusoid_tilde,
        x_f);
}



