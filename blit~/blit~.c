/* PD external implementation of a band-limited impulse train based on
 * [Stilson and Smith 1996]: https://ccrma.stanford.edu/~stilti/papers/blit.pdf.
 *
 * Implementation by Dario Sanfilippo (2017-2018): sanfilippo.dario@gmail.com.
 *
 * This external uses Katja Vetter's PD lib builder:
 * https://github.com/pure-data/pd-lib-builder.
 *
 * Features: impulse frequency; number of harmonics (less or equal to the
 * non-aliasing maximum allowed at a given frequency); unipolar or bipolar
 * output.
 *
 * BLIT generators can be used to create band-limited oscillators (sawtooth,
 * square, triangle, rectangle) or simply for pulse modulation. See the paper
 * for details.
 */

#include "m_pd.h"
#include "math.h"

#define TWOPI (M_PI * 2.0)

static t_class *blit_tilde_class;

typedef struct _blit_tilde {
  t_object  x_obj;
  int x_sr;         /* samplerate */
  double x_speriod; /* sampling period */
  double x_phase;   /* phase in rad */
  int x_max_h;      /* max number of non-aliasing harmonics at a given freq */
  int x_h;          /* number of harmonics */
  t_float x_f;
} t_blit_tilde;

static void *blit_tilde_new(void) {
    t_blit_tilde *x = (t_blit_tilde *)pd_new(blit_tilde_class);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, &s_signal);
    x->x_f = 0.0;
    x->x_phase = 0.0;
    x->x_max_h = 0;
    x->x_h = 0;
    return (x);
}

static t_int *blit_tilde_perform(t_int *w) {
    t_blit_tilde *x = (t_blit_tilde *)(w[1]);
    t_sample *in1 = (t_sample *)(w[2]); /* impulse frequency */
    t_sample *in2 = (t_sample *)(w[3]); /* number of harmonics */
    t_sample *in3 = (t_sample *)(w[4]); /* unipolar or bipolar impulse train */
    t_sample *out = (t_sample *)(w[5]); /* band-limited impulse train */
    int n = (int)(w[6]);
    int sr = x->x_sr;
    double speriod = x->x_speriod;
    double phase = x->x_phase;  /* */
    int h = x->x_h;             /* recall these from the previous block */
    int max_h = x->x_max_h;     /* */
    double incr;

    while (n--) {
        incr = *in1 * M_PI * speriod; /* phase increment */
        if (phase == 0.0 || phase == M_PI) /* avoid divide by 0 */
            *out++ = 1.0;
        else if (h == 0)
            *out++ = 0.0;
        else if (*in3 >= 0.0 && (h % 2) == 0 && h > 1) /* odd harmonics */
            *out++ = (sin(phase * (h - 1)) / sin(phase)) / (h - 1);
        else if (*in3 < 0.0 && (h % 2) == 1 && h > 1) /* even harmonics */
            *out++ = (sin(phase * (h - 1)) / sin(phase)) / (h - 1);
        else
            *out++ = (sin(phase * h) / sin(phase)) / h;
        phase += incr;
        if (phase >= TWOPI) {
            /* ph wrap-around and ph-locked harmonic variation */
            h = *in2 > 0 ? (2 * (*in2)) : (-2 * (*in2));
                /* number of harmonics */
            max_h = *in1 != 0 ? (sr / (*in1)) : sr; /* maximum allowed */
            if (max_h < 0)
                max_h = -max_h;
            if (h > max_h) /* clip harmonics if maximum allowed is exceeded */
                h = max_h;
            phase -= TWOPI;
        }
        if (phase < 0.0) { /* same as above */
            h = *in2 > 0 ? (2 * (*in2)) : (-2 * (*in2));
            max_h = *in1 != 0 ? (sr / (*in1)) : sr;
            if (max_h < 0)
                max_h = -max_h;
            if (h > max_h)
                h = max_h;
            phase += TWOPI;
        }
        in1++;
        in2++;
        in3++;
    }

    x->x_phase = phase; /* */
    x->x_max_h = max_h; /* store these values for the next block */
    x->x_h = h;         /* */

    return (w + 7);
}

void blit_tilde_dsp(t_blit_tilde *x, t_signal **sp) {
    x->x_speriod = 1.0 / sp[0]->s_sr;
    x->x_sr = sp[0]->s_sr;
    dsp_add(
        blit_tilde_perform,
        6,
        x,
        sp[0]->s_vec,
        sp[1]->s_vec,
        sp[2]->s_vec,
        sp[3]->s_vec,
        sp[0]->s_n);
}

void blit_tilde_setup(void) {
    blit_tilde_class = class_new(
        gensym("blit~"),
        (t_newmethod)blit_tilde_new,
        0,
        sizeof(t_blit_tilde),
        CLASS_DEFAULT,
        A_DEFFLOAT,
        0);

    class_addmethod(
        blit_tilde_class,
        (t_method)blit_tilde_dsp,
        gensym("dsp"),
        0);

    CLASS_MAINSIGNALIN(
        blit_tilde_class,
        t_blit_tilde,
        x_f);
}




