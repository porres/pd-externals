/* Simple cosine function.
 *
 * Implementation by Dario Sanfilippo (2017-2018): sanfilippo.dario@gmail.com.
 *
 * This external uses Katja Vetter's PD lib builder:
 * https://github.com/pure-data/pd-lib-builder.
 */

#include <math.h>
#include "m_pd.h"

static t_class *cosine_tilde_class;

typedef struct _cosine_tilde {
    t_object x_obj;
    float x_f;
} t_cosine_tilde;

static void *cosine_tilde_new(void) {
    t_cosine_tilde *x = (t_cosine_tilde *)pd_new(cosine_tilde_class);
    outlet_new(&x->x_obj, &s_signal);
    x->x_f = 0.0;
    return (x);
}

static t_int *cosine_tilde_perform(t_int *w) {
    t_sample *in = (t_sample *)(w[1]);
    t_sample *out = (t_sample *)(w[2]);
    int n = (int)(w[3]);

    while (n--) {
        *out++ = cos(*in++);
    }

    return (w+4);
}

void cosine_tilde_dsp(t_cosine_tilde *x, t_signal **sp) {
    dsp_add(
        cosine_tilde_perform,
        3,
        sp[0]->s_vec,
        sp[1]->s_vec,
        sp[0]->s_n);
}

void cosine_tilde_setup(void) {
    cosine_tilde_class = class_new(
        gensym("cosine~"),
        (t_newmethod)cosine_tilde_new,
        0,
        sizeof(t_cosine_tilde),
        0,
        0);

    class_addmethod(
        cosine_tilde_class,
        (t_method)cosine_tilde_dsp,
        gensym("dsp"),
        0);

    CLASS_MAINSIGNALIN(
        cosine_tilde_class,
        t_cosine_tilde,
        x_f);
}



