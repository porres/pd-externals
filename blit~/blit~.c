#include "m_pd.h"
#include "math.h"

#define TWOPI (M_PI * 2)

static t_class *blit_tilde_class;

typedef struct _blit_tilde {
  t_object  x_obj;
  int x_sr;
  double x_speriod;
  double x_phase;
  int x_max_h;
  int x_h;
  t_float x_f;
} t_blit_tilde;

static void *blit_tilde_new(void) {
    t_blit_tilde *x = (t_blit_tilde *)pd_new(blit_tilde_class);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    outlet_new(&x->x_obj, &s_signal);
    x->x_f = 0;
    x->x_phase = 0;
    x->x_max_h = 0;
    x->x_h = 0;
    return (x);
}

static t_int *blit_tilde_perform(t_int *w) {
    t_blit_tilde *x = (t_blit_tilde *)(w[1]);
    t_sample *in1 = (t_sample *)(w[2]);
    t_sample *in2 = (t_sample *)(w[3]);
    t_sample *in3 = (t_sample *)(w[4]);
    t_sample *out = (t_sample *)(w[5]);
    int n = (int)(w[6]);
    int sr = x->x_sr;
    double speriod = x->x_speriod;
    double phase = x->x_phase;
    int h = x->x_h;
    int max_h = x->x_max_h;
    double incr;

    while (n--) {
        incr = *in1 * M_PI * speriod;
        if (phase == 0 || phase == M_PI)
            *out++ = 1.0;
        else if (h == 0)
            *out++ = 0.0;
        else if (*in3 >= 0 && (h % 2) == 0 && h > 1)
            *out++ = (sin(phase * (h - 1)) / sin(phase)) / (h - 1);
        else if (*in3 < 0 && (h % 2) == 1 && h > 1)
            *out++ = (sin(phase * (h - 1)) / sin(phase)) / (h - 1);
        else
            *out++ = (sin(phase * h) / sin(phase)) / h;
        phase += incr;
        if (phase >= TWOPI) {
            h = *in2 > 0 ? (2 * (*in2)) : (-2 * (*in2));
            max_h = *in1 != 0 ? (sr / (*in1)) : sr;
            if (max_h < 0)
                max_h = -max_h;
            if (h > max_h)
                h = max_h;
                phase -= TWOPI;
        }
        if (phase < 0.0) {
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

    x->x_phase = phase;
    x->x_max_h = max_h;
    x->x_h = h;

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




