#ifndef _AVUTIL_STUBS_H_               
#define _AVUTIL_STUBS_H_

#include <stdio.h>

#include <caml/mlvalues.h>

#include <libavutil/samplefmt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>

#include "polymorphic_variant_values_stubs.h"

#define Val_none Val_int(0)
#define Some_val(v) Field(v,0)

#define ERROR_MSG_SIZE 256
#define EXN_ERROR "ffmpeg_exn_error"

#define Fail(...) {                                               \
    snprintf(ocaml_av_exn_msg, ERROR_MSG_SIZE, __VA_ARGS__);            \
    caml_callback(*caml_named_value("ffmpeg_exn_failure"), caml_copy_string(ocaml_av_exn_msg)); \
  }

/* ffmpeg logs are routed through an OCaml callback, which requires
 * unlocking/locking the runtime.
 *
 * Calling avfilter_graph_free and pretty much any function causes 
 * logs to be written so, in turn, they also need to unlock/lock the
 * runtime around them.
 *
 * Final brick of this cluster fuck, finalizers registered on the C
 * side cannot release the runtime or register global roots so we have
 * to send the finalizer to the OCaml side and register it here.
 *
 * The Finalize macro is used to register OCaml finalizers for custom
 * values that are instantiated on the C side without ever being
 * surfaced on the OCaml side. */
#define Finalize(f,v) \
  caml_callback2(*caml_named_value("ffmpeg_gc_finalise"),*caml_named_value(f),v)

void ocaml_avutil_raise_error(int err);

extern char ocaml_av_exn_msg[];

#define List_init(list) (list) = Val_emptylist

#define List_add(list, cons, val) {             \
    (cons) = caml_alloc(2, 0);                  \
    Store_field((cons), 0, (val));              \
    Store_field((cons), 1, (list));             \
    (list) = (cons);                            \
  }


/***** Global initialisation *****/

void ocaml_ffmpeg_register_thread();

/**** AVRational ****/
#define rational_of_value(v) ((AVRational){Int_val(Field((v), 0)), Int_val(Field((v), 1))})

void value_of_rational(const AVRational * r, value * pv);


/**** Time format ****/

int64_t second_fractions_of_time_format(value time_format);


/**** Channel layout ****/

uint64_t ChannelLayout_val(value v);

value Val_ChannelLayout(uint64_t cl);


/**** Sample format ****/

#define Sample_format_val(v) (Int_val(v))

enum AVSampleFormat SampleFormat_val(value v);

enum AVSampleFormat AVSampleFormat_of_Sample_format(int i);

value Val_SampleFormat(enum AVSampleFormat sf);
enum caml_ba_kind bigarray_kind_of_AVSampleFormat(enum AVSampleFormat sf);


/**** Pixel format ****/

int PixelFormat_val(value);

value Val_PixelFormat(enum AVPixelFormat pf);


/***** AVFrame *****/

#define Frame_val(v) (*(struct AVFrame**)Data_custom_val(v))

value value_of_frame(AVFrame *frame);

/***** AVSubtitle *****/
#define Subtitle_val(v) (*(struct AVSubtitle**)Data_custom_val(v))

value value_of_subtitle(AVSubtitle *subtitle);

int subtitle_header_default(AVCodecContext *avctx);

#endif // _AVUTIL_STUBS_H_ 