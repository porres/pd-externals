/* Simple sine function.
 *
 * Implementation by Dario Sanfilippo (2017-2018): sanfilippo.dario@gmail.com.
 *
 * This external uses Katja Vetter's PD lib builder:
 * https://github.com/pure-data/pd-lib-builder.
 */

#include <math.h>
#include "m_pd.h"

static t_class *sine_tilde_class;

typedef struct _sine_tilde {
    t_object x_obj;
    float x_f;
} t_sine_tilde;

static void *sine_tilde_new(void) {
    t_sine_tilde *x = (t_sine_tilde *)pd_new(sine_tilde_class);
    outlet_new(&x->x_obj, &s_signal);
    x->x_f = 0;
    return (x);
}

static t_int *sine_tilde_perform(t_int *w) {
    t_sample *in = (t_sample *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]);

    while (n--) {
        *out++ = sin(*in++);
    }

    return (w+4);
}

void sine_tilde_dsp(t_sine_tilde *x, t_signal **sp) {
    dsp_add(
        sine_tilde_perform,
        3,
        sp[0]->s_vec,
        sp[1]->s_vec,
        sp[0]->s_n);
}

void sine_tilde_setup(void) {
    sine_tilde_class = class_new(
        gensym("sine~"),
        (t_newmethod)sine_tilde_new,
        0,
        sizeof(t_sine_tilde),
        0,
        0);

    class_addmethod(
        sine_tilde_class,
        (t_method)sine_tilde_dsp,
        gensym("dsp"),
        0);

    CLASS_MAINSIGNALIN(
        sine_tilde_class,
        t_sine_tilde,
        x_f);
}



