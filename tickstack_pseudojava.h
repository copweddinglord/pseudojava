/**
 * tickstack_pseudojava.h - Simplified Pseudo-Java Environment
 *
 * Provides a C-based environment with:
 * - Emoji and tone mapping
 * - Variant type system
 * - Multidimensional operations
 * - Robust error handling
 */

#ifndef TICKSTACK_PSEUDOJAVA_H
#define TICKSTACK_PSEUDOJAVA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "tickstack_11d.h"

/* Error Codes */
#define PJ_ERR_NULL_POINTER      201
#define PJ_ERR_MEMORY_ALLOCATION 202
#define PJ_ERR_INVALID_TYPE      203
#define PJ_ERR_INVALID_INDEX     204
#define PJ_ERR_CAPACITY_EXCEEDED 205
#define PJ_ERR_INVALID_EMOJI     206
#define PJ_ERR_INVALID_TONE      207
#define PJ_ERR_INVALID_OPERATION 208

/* Constants */
#define PJ_DIM_2D         2
#define PJ_DIM_4D         4
#define PJ_MAX_EMOJI      128
#define PJ_MAX_TONE       8
#define PJ_MAX_CLASSES    50
#define PJ_MAX_METHODS    20
#define PJ_MAX_VARIANTS   64
#define PJ_MAX_PROPS      16
#define PJ_EPSILON        1e-10

/* Data Structures */
typedef enum {
    PJ_VARIANT_NULL, PJ_VARIANT_INT, PJ_VARIANT_DOUBLE, PJ_VARIANT_STRING,
    PJ_VARIANT_BOOL, PJ_VARIANT_ARRAY, PJ_VARIANT_OBJECT, PJ_VARIANT_FUNCTION,
    PJ_VARIANT_EMOJI, PJ_VARIANT_PSEUDODIM
} VariantType;

typedef struct { char* name; struct Variant* value; } VariantProperty;

typedef struct Variant {
    VariantType type;
    union {
        int int_val;
        double double_val;
        char* string_val;
        int bool_val;
        struct { struct Variant** items; size_t len, cap; } array_val;
        struct { VariantProperty* props; size_t prop_count; } object_val;
        struct { void* (*func)(struct Variant**, int); int arg_count; } function_val;
        struct { char emoji[8]; double vector[PJ_DIM_4D]; } emoji_val;
        struct { double values[PJ_DIM_4D]; int dims; double pseudomass, pseudocharge, pseudospin, pseudotime; } pseudodim_val;
    } value;
} Variant;

typedef struct { char* name; double factor, shift; char emoji_repr[8]; } ToneMapping;

typedef struct { char emoji[8]; double vector[PJ_DIM_4D]; int dims; ToneMapping tones[PJ_MAX_TONE]; size_t tone_count; } EmojiMapping;

typedef struct PseudoJavaObject { char* class_name, *object_name; void* data; int type; struct PseudoJavaObject* next; } PseudoJavaObject;

typedef struct { char* name; PseudoJavaObject* objects; size_t obj_count; void* (*methods[PJ_MAX_METHODS])(void*); char* method_names[PJ_MAX_METHODS]; size_t method_count; } PseudoJavaClass;

/* Global State */
static EmojiMapping pj_emojis[PJ_MAX_EMOJI];
static size_t pj_emoji_count = 0;
static PseudoJavaClass pj_classes[PJ_MAX_CLASSES];
static size_t pj_class_count = 0;
static Variant* pj_variants[PJ_MAX_VARIANTS];
static size_t pj_variant_count = 0;
static int pj_initialized = 0, pj_use_4d = 0;

/* Function Prototypes */
int pj_living_number(double base, double* result);
int pj_init(void);
int pj_enable_4d(void);
int pj_disable_4d(void);
int pj_is_4d(int* result);
PseudoJavaClass* pj_create_class(const char* name);
PseudoJavaObject* pj_create_object(PseudoJavaClass* class, const char* name, int type);
int pj_add_method(PseudoJavaClass* class, const char* name, void* (*method)(void*));
void* pj_call_method(PseudoJavaObject* obj, const char* method_name, void* args);
int pj_add_emoji(const char* emoji, double* coords, int dims);
int pj_add_emoji_tone(const char* emoji, const char* tone, double factor, double shift, const char* repr);
int pj_add_emoji_with_tones(const char* emoji, double* coords, int dims, const char** tones, double* factors, double* shifts, const char** reprs, size_t tone_count);
int pj_get_emoji_vector(const char* emoji, double** result);
int pj_apply_tone(const double* vector, const char* tone, double* result);
int pj_get_toned_emoji(const char* emoji, const char* tone, const char** result);
int pj_x_inline(double* a, const double* b, int op, double* result);
int pj_x_inline_scalar(double a, double b, int op, double* result);
int pj_x_delta(double* a, const double* b, double* result);
int pj_x_delta_scalar(double a, double b, double* result);
Variant* pj_variant_null(void);
Variant* pj_variant_int(int value);
Variant* pj_variant_double(double value);
Variant* pj_variant_string(const char* value);
Variant* pj_variant_bool(int value);
Variant* pj_variant_array(size_t cap);
Variant* pj_variant_object(void);
Variant* pj_variant_function(void* (*func)(Variant**, int), int arg_count);
Variant* pj_variant_emoji(const char* emoji);
Variant* pj_variant_pseudodim(double* values, int dims);
int pj_variant_set_pseudoprop(Variant* v, double mass, double charge, double spin, double time);
void pj_variant_free(Variant* v);
int pj_variant_array_push(Variant* array, Variant* item);
int pj_variant_object_set(Variant* obj, const char* name, Variant* value);
Variant* pj_variant_object_get(Variant* obj, const char* name);
Variant* pj_variant_array_get(Variant* array, size_t idx);
int pj_variant_array_set(Variant* array, size_t idx, Variant* value);
Variant* pj_variant_call(Variant* func, Variant** args, int arg_count);
char* pj_variant_to_string(Variant* v);
Variant* pj_variant_add(Variant* a, Variant* b);
Variant* pj_variant_sub(Variant* a, Variant* b);
Variant* pj_variant_mul(Variant* a, Variant* b);
Variant* pj_variant_div(Variant* a, Variant* b);
Variant* pj_variant_delta(Variant* a, Variant* b);
int pj_variant_eq(Variant* a, Variant* b);

/* Function Implementations */
int pj_living_number(double base, double* result) {
    if (!result) return PJ_ERR_NULL_POINTER;
    static unsigned long seed = 0;
    if (!seed) seed = (unsigned long)time(NULL);
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    *result = base + ((double)seed / 0x7fffffff - 0.5) * 0.01;
    return 0;
}

int pj_init(void) {
    if (pj_initialized) return 0;
    pj_emoji_count = pj_class_count = pj_variant_count = pj_use_4d = 0;
    pj_initialized = 1;
    return 0;
}

int pj_enable_4d(void) { pj_use_4d = 1; return 0; }
int pj_disable_4d(void) { pj_use_4d = 0; return 0; }
int pj_is_4d(int* result) { if (!result) return PJ_ERR_NULL_POINTER; *result = pj_use_4d; return 0; }

PseudoJavaClass* pj_create_class(const char* name) {
    if (!name || pj_class_count >= PJ_MAX_CLASSES) return NULL;
    PseudoJavaClass* c = &pj_classes[pj_class_count++];
    c->name = strdup(name);
    if (!c->name) return NULL;
    c->objects = NULL; c->obj_count = c->method_count = 0;
    return c;
}

PseudoJavaObject* pj_create_object(PseudoJavaClass* c, const char* name, int type) {
    if (!c || !name || c->obj_count >= PJ_MAX_CLASSES) return NULL;
    PseudoJavaObject* obj = (PseudoJavaObject*)calloc(1, sizeof(PseudoJavaObject));
    if (!obj) return NULL;
    obj->class_name = strdup(c->name);
    obj->object_name = strdup(name);
    if (!obj->class_name || !obj->object_name) {
        free(obj->class_name); free(obj->object_name); free(obj); return NULL;
    }
    obj->type = type; obj->next = c->objects; c->objects = obj; c->obj_count++;
    return obj;
}

int pj_add_method(PseudoJavaClass* c, const char* name, void* (*method)(void*)) {
    if (!c || !name || !method || c->method_count >= PJ_MAX_METHODS) return PJ_ERR_NULL_POINTER;
    c->method_names[c->method_count] = strdup(name);
    if (!c->method_names[c->method_count]) return PJ_ERR_MEMORY_ALLOCATION;
    c->methods[c->method_count++] = method;
    return 0;
}

void* pj_call_method(PseudoJavaObject* obj, const char* method_name, void* args) {
    if (!obj || !method_name) return NULL;
    PseudoJavaClass* c = NULL;
    for (size_t i = 0; i < pj_class_count; i++)
        if (!strcmp(pj_classes[i].name, obj->class_name)) { c = &pj_classes[i]; break; }
    if (!c) return NULL;
    for (size_t i = 0; i < c->method_count; i++)
        if (!strcmp(c->method_names[i], method_name)) return c->methods[i](args);
    return NULL;
}

int pj_add_emoji(const char* emoji, double* coords, int dims) {
    if (!emoji || !coords || pj_emoji_count >= PJ_MAX_EMOJI || (dims != 2 && dims != 4)) return PJ_ERR_INVALID_EMOJI;
    EmojiMapping* m = &pj_emojis[pj_emoji_count++];
    strncpy(m->emoji, emoji, 7); m->emoji[7] = '\0';
    memcpy(m->vector, coords, dims * sizeof(double));
    for (int i = dims; i < PJ_DIM_4D; i++) m->vector[i] = 0.0;
    m->dims = dims; m->tone_count = 0;
    return 0;
}

int pj_add_emoji_tone(const char* emoji, const char* tone, double factor, double shift, const char* repr) {
    if (!emoji || !tone || !repr) return PJ_ERR_NULL_POINTER;
    for (size_t i = 0; i < pj_emoji_count; i++) {
        if (!strcmp(pj_emojis[i].emoji, emoji)) {
            EmojiMapping* m = &pj_emojis[i];
            if (m->tone_count >= PJ_MAX_TONE) return PJ_ERR_CAPACITY_EXCEEDED;
            ToneMapping* t = &m->tones[m->tone_count++];
            t->name = strdup(tone);
            if (!t->name) return PJ_ERR_MEMORY_ALLOCATION;
            t->factor = factor; t->shift = shift;
            strncpy(t->emoji_repr, repr, 7); t->emoji_repr[7] = '\0';
            return 0;
        }
    }
    return PJ_ERR_INVALID_EMOJI;
}

int pj_add_emoji_with_tones(const char* emoji, double* coords, int dims, const char** tones, double* factors, double* shifts, const char** reprs, size_t tone_count) {
    if (!emoji || !coords || !tones || !factors || !shifts || !reprs || (dims != 2 && dims != 4) || tone_count > PJ_MAX_TONE) return PJ_ERR_NULL_POINTER;
    if (pj_emoji_count >= PJ_MAX_EMOJI) return PJ_ERR_CAPACITY_EXCEEDED;
    int ret = pj_add_emoji(emoji, coords, dims);
    if (ret) return ret;
    for (size_t i = 0; i < tone_count; i++) {
        if (!tones[i] || !reprs[i]) { pj_emoji_count--; memset(&pj_emojis[pj_emoji_count], 0, sizeof(EmojiMapping)); return PJ_ERR_NULL_POINTER; }
        ret = pj_add_emoji_tone(emoji, tones[i], factors[i], shifts[i], reprs[i]);
        if (ret) { pj_emoji_count--; memset(&pj_emojis[pj_emoji_count], 0, sizeof(EmojiMapping)); return ret; }
    }
    return 0;
}

int pj_get_emoji_vector(const char* emoji, double** result) {
    if (!emoji || !result) return PJ_ERR_NULL_POINTER;
    for (size_t i = 0; i < pj_emoji_count; i++)
        if (!strcmp(pj_emojis[i].emoji, emoji)) { *result = pj_emojis[i].vector; return 0; }
    return PJ_ERR_INVALID_EMOJI;
}

int pj_apply_tone(const double* vector, const char* tone, double* result) {
    if (!vector || !tone || !result) return PJ_ERR_NULL_POINTER;
    int dims = pj_use_4d ? 4 : 2;
    memcpy(result, vector, dims * sizeof(double));
    for (size_t i = 0; i < pj_emoji_count; i++)
        for (size_t j = 0; j < pj_emojis[i].tone_count; j++)
            if (!strcmp(pj_emojis[i].tones[j].name, tone)) {
                double f = pj_emojis[i].tones[j].factor, s = pj_emojis[i].tones[j].shift;
                for (int k = 0; k < dims; k++) result[k] = vector[k] * f + s;
                return 0;
            }
    return PJ_ERR_INVALID_TONE;
}

int pj_get_toned_emoji(const char* emoji, const char* tone, const char** result) {
    if (!emoji || !tone || !result) return PJ_ERR_NULL_POINTER;
    for (size_t i = 0; i < pj_emoji_count; i++)
        if (!strcmp(pj_emojis[i].emoji, emoji)) {
            for (size_t j = 0; j < pj_emojis[i].tone_count; j++)
                if (!strcmp(pj_emojis[i].tones[j].name, tone)) { *result = pj_emojis[i].tones[j].emoji_repr; return 0; }
            *result = pj_emojis[i].emoji;
            return 0;
        }
    return PJ_ERR_INVALID_EMOJI;
}

int pj_x_inline(double* a, const double* b, int op, double* result) {
    if (!a || !b || !result) return PJ_ERR_NULL_POINTER;
    switch (op) {
        case 0: for (size_t i = 0; i < TICKSTACK_DIM; i++) result[i] = a[i] + b[i]; break;
        case 1: for (size_t i = 0; i < TICKSTACK_DIM; i++) result[i] = a[i] - b[i]; break;
        case 2: for (size_t i = 0; i < TICKSTACK_DIM; i++) result[i] = a[i] * b[i]; break;
        case 3: for (size_t i = 0; i < TICKSTACK_DIM; i++) {
            if (fabs(b[i]) < PJ_EPSILON) return PJ_ERR_INVALID_OPERATION;
            result[i] = a[i] / b[i]; } break;
        case 4: for (size_t i = 0; i < TICKSTACK_DIM; i++) { double d = a[i] - b[i]; result[i] = a[i] + d * TICKSTACK_PHI; } break;
        case 5: {
            double d[TICKSTACK_DIM], mag = 0.0;
            for (size_t i = 0; i < TICKSTACK_DIM; i++) { d[i] = a[i] - b[i]; mag += d[i] * d[i]; }
            mag = sqrt(mag); double f = 1.0 / (1.0 + mag * TICKSTACK_THIRD);
            for (size_t i = 0; i < TICKSTACK_DIM; i++) result[i] = a[i] * f; break; }
        default: memcpy(result, a, TICKSTACK_DIM * sizeof(double)); return PJ_ERR_INVALID_OPERATION;
    }
    return 0;
}

int pj_x_inline_scalar(double a, double b, int op, double* result) {
    if (!result) return PJ_ERR_NULL_POINTER;
    switch (op) {
        case 0: *result = a + b; break;
        case 1: *result = a - b; break;
        case 2: *result = a * b; break;
        case 3: if (fabs(b) < PJ_EPSILON) return PJ_ERR_INVALID_OPERATION; *result = a / b; break;
        case 4: *result = a + (a - b) * TICKSTACK_PHI; break;
        case 5: *result = a * (1.0 / (1.0 + fabs(a - b) * TICKSTACK_THIRD)); break;
        default: *result = a; return PJ_ERR_INVALID_OPERATION;
    }
    return 0;
}

int pj_x_delta(double* a, const double* b, double* result) {
    if (!a || !b || !result) return PJ_ERR_NULL_POINTER;
    for (size_t i = 0; i < TICKSTACK_DIM; i++) result[i] = a[i] - b[i];
    return 0;
}

int pj_x_delta_scalar(double a, double b, double* result) {
    if (!result) return PJ_ERR_NULL_POINTER;
    *result = a - b;
    return 0;
}

Variant* pj_variant_null(void) {
    Variant* v = (Variant*)malloc(sizeof(Variant));
    if (!v) return NULL;
    v->type = PJ_VARIANT_NULL;
    if (pj_variant_count < PJ_MAX_VARIANTS) pj_variants[pj_variant_count++] = v;
    return v;
}

Variant* pj_variant_int(int value) {
    Variant* v = (Variant*)malloc(sizeof(Variant));
    if (!v) return NULL;
    v->type = PJ_VARIANT_INT; v->value.int_val = value;
    if (pj_variant_count < PJ_MAX_VARIANTS) pj_variants[pj_variant_count++] = v;
    return v;
}

Variant* pj_variant_double(double value) {
    Variant* v = (Variant*)malloc(sizeof(Variant));
    if (!v) return NULL;
    v->type = PJ_VARIANT_DOUBLE; v->value.double_val = value;
    if (pj_variant_count < PJ_MAX_VARIANTS) pj_variants[pj_variant_count++] = v;
    return v;
}

Variant* pj_variant_string(const char* value) {
    if (!value) return pj_variant_null();
    Variant* v = (Variant*)malloc(sizeof(Variant));
    if (!v) return NULL;
    v->type = PJ_VARIANT_STRING; v->value.string_val = strdup(value);
    if (!v->value.string_val) { free(v); return NULL; }
    if (pj_variant_count < PJ_MAX_VARIANTS) pj_variants[pj_variant_count++] = v;
    return v;
}

Variant* pj_variant_bool(int value) {
    Variant* v = (Variant*)malloc(sizeof(Variant));
    if (!v) return NULL;
    v->type = PJ_VARIANT_BOOL; v->value.bool_val = value ? 1 : 0;
    if (pj_variant_count < PJ_MAX_VARIANTS) pj_variants[pj_variant_count++] = v;
    return v;
}

Variant* pj_variant_array(size_t cap) {
    if (!cap) cap = 4;
    Variant* v = (Variant*)malloc(sizeof(Variant));
    if (!v) return NULL;
    v->type = PJ_VARIANT_ARRAY;
    v->value.array_val.items = (Variant**)malloc(cap * sizeof(Variant*));
    if (!v->value.array_val.items) { free(v); return NULL; }
    v->value.array_val.len = 0; v->value.array_val.cap = cap;
    if (pj_variant_count < PJ_MAX_VARIANTS) pj_variants[pj_variant_count++] = v;
    return v;
}

Variant* pj_variant_object(void) {
    Variant* v = (Variant*)malloc(sizeof(Variant));
    if (!v) return NULL;
    v->type = PJ_VARIANT_OBJECT;
    v->value.object_val.props = (VariantProperty*)malloc(PJ_MAX_PROPS * sizeof(VariantProperty));
    if (!v->value.object_val.props) { free(v); return NULL; }
    v->value.object_val.prop_count = 0;
    if (pj_variant_count < PJ_MAX_VARIANTS) pj_variants[pj_variant_count++] = v;
    return v;
}

Variant* pj_variant_function(void* (*func)(Variant**, int), int arg_count) {
    if (!func) return pj_variant_null();
    Variant* v = (Variant*)malloc(sizeof(Variant));
    if (!v) return NULL;
    v->type = PJ_VARIANT_FUNCTION; v->value.function_val.func = func; v->value.function_val.arg_count = arg_count;
    if (pj_variant_count < PJ_MAX_VARIANTS) pj_variants[pj_variant_count++] = v;
    return v;
}

Variant* pj_variant_emoji(const char* emoji) {
    if (!emoji) return pj_variant_null();
    Variant* v = (Variant*)malloc(sizeof(Variant));
    if (!v) return NULL;
    v->type = PJ_VARIANT_EMOJI;
    strncpy(v->value.emoji_val.emoji, emoji, 7); v->value.emoji_val.emoji[7] = '\0';
    double* vec;
    if (pj_get_emoji_vector(emoji, &vec)) { free(v); return NULL; }
    memcpy(v->value.emoji_val.vector, vec, PJ_DIM_4D * sizeof(double));
    if (pj_variant_count < PJ_MAX_VARIANTS) pj_variants[pj_variant_count++] = v;
    return v;
}

Variant* pj_variant_pseudodim(double* values, int dims) {
    if (!values || (dims != 2 && dims != 4)) return NULL;
    Variant* v = (Variant*)malloc(sizeof(Variant));
    if (!v) return NULL;
    v->type = PJ_VARIANT_PSEUDODIM; v->value.pseudodim_val.dims = dims;
    memcpy(v->value.pseudodim_val.values, values, dims * sizeof(double));
    for (int i = dims; i < PJ_DIM_4D; i++) v->value.pseudodim_val.values[i] = 0.0;
    v->value.pseudodim_val.pseudomass = v->value.pseudodim_val.pseudocharge = v->value.pseudodim_val.pseudospin = v->value.pseudodim_val.pseudotime = 0.0;
    if (pj_variant_count < PJ_MAX_VARIANTS) pj_variants[pj_variant_count++] = v;
    return v;
}

int pj_variant_set_pseudoprop(Variant* v, double mass, double charge, double spin, double time) {
    if (!v || v->type != PJ_VARIANT_PSEUDODIM) return PJ_ERR_INVALID_TYPE;
    v->value.pseudodim_val.pseudomass = mass;
    v->value.pseudodim_val.pseudocharge = charge;
    v->value.pseudodim_val.pseudospin = spin;
    v->value.pseudodim_val.pseudotime = time;
    return 0;
}

void pj_variant_free(Variant* v) {
    if (!v) return;
    switch (v->type) {
        case PJ_VARIANT_STRING: free(v->value.string_val); break;
        case PJ_VARIANT_ARRAY:
            for (size_t i = 0; i < v->value.array_val.len; i++) pj_variant_free(v->value.array_val.items[i]);
            free(v->value.array_val.items); break;
        case PJ_VARIANT_OBJECT:
            for (size_t i = 0; i < v->value.object_val.prop_count; i++) {
                free(v->value.object_val.props[i].name); pj_variant_free(v->value.object_val.props[i].value);
            }
            free(v->value.object_val.props); break;
        default: break;
    }
    for (size_t i = 0; i < pj_variant_count; i++)
        if (pj_variants[i] == v) { memmove(&pj_variants[i], &pj_variants[i + 1], (pj_variant_count - i - 1) * sizeof(Variant*)); pj_variant_count--; break; }
    free(v);
}

int pj_variant_array_push(Variant* array, Variant* item) {
    if (!array || !item || array->type != PJ_VARIANT_ARRAY) return PJ_ERR_INVALID_TYPE;
    if (array->value.array_val.len >= array->value.array_val.cap) {
        size_t new_cap = array->value.array_val.cap * 2;
        Variant** new_items = (Variant**)realloc(array->value.array_val.items, new_cap * sizeof(Variant*));
        if (!new_items) return PJ_ERR_MEMORY_ALLOCATION;
        array->value.array_val.items = new_items; array->value.array_val.cap = new_cap;
    }
    array->value.array_val.items[array->value.array_val.len++] = item;
    return 0;
}

int pj_variant_object_set(Variant* obj, const char* name, Variant* value) {
    if (!obj || !name || !value || obj->type != PJ_VARIANT_OBJECT) return PJ_ERR_INVALID_TYPE;
    if (obj->value.object_val.prop_count >= PJ_MAX_PROPS) return PJ_ERR_CAPACITY_EXCEEDED;
    VariantProperty* p = &obj->value.object_val.props[obj->value.object_val.prop_count++];
    p->name = strdup(name); p->value = value;
    return p->name ? 0 : PJ_ERR_MEMORY_ALLOCATION;
}

Variant* pj_variant_object_get(Variant* obj, const char* name) {
    if (!obj || !name || obj->type != PJ_VARIANT_OBJECT) return NULL;
    for (size_t i = 0; i < obj->value.object_val.prop_count; i++)
        if (!strcmp(obj->value.object_val.props[i].name, name)) return obj->value.object_val.props[i].value;
    return NULL;
}

Variant* pj_variant_array_get(Variant* array, size_t idx) {
    if (!array || array->type != PJ_VARIANT_ARRAY || idx >= array->value.array_val.len) return NULL;
    return array->value.array_val.items[idx];
}

int pj_variant_array_set(Variant* array, size_t idx, Variant* value) {
    if (!array || !value || array->type != PJ_VARIANT_ARRAY || idx >= array->value.array_val.len) return PJ_ERR_INVALID_TYPE;
    pj_variant_free(array->value.array_val.items[idx]);
    array->value.array_val.items[idx] = value;
    return 0;
}

Variant* pj_variant_call(Variant* func, Variant** args, int arg_count) {
    if (!func || func->type != PJ_VARIANT_FUNCTION || !args || func->value.function_val.arg_count != arg_count) return NULL;
    return (Variant*)func->value.function_val.func(args, arg_count);
}

char* pj_variant_to_string(Variant* v) {
    if (!v) return strdup("null");
    static char buf[256];
    switch (v->type) {
        case PJ_VARIANT_NULL: return strdup("null");
        case PJ_VARIANT_INT: snprintf(buf, 256, "%d", v->value.int_val); break;
        case PJ_VARIANT_DOUBLE: snprintf(buf, 256, "%.10g", v->value.double_val); break;
        case PJ_VARIANT_STRING: return strdup(v->value.string_val);
        case PJ_VARIANT_BOOL: return strdup(v->value.bool_val ? "true" : "false");
        case PJ_VARIANT_ARRAY: {
            char* r = strdup("[");
            for (size_t i = 0; i < v->value.array_val.len; i++) {
                char* s = pj_variant_to_string(v->value.array_val.items[i]);
                size_t len = strlen(r) + strlen(s) + 4;
                char* new_r = (char*)realloc(r, len);
                if (!new_r) { free(r); free(s); return NULL; }
                r = new_r; strcat(r, s); if (i < v->value.array_val.len - 1) strcat(r, ", "); free(s);
            }
            strcat(r, "]"); return r;
        }
        case PJ_VARIANT_OBJECT: {
            char* r = strdup("{");
            for (size_t i = 0; i < v->value.object_val.prop_count; i++) {
                char* s = pj_variant_to_string(v->value.object_val.props[i].value);
                size_t len = strlen(r) + strlen(v->value.object_val.props[i].name) + strlen(s) + 4;
                char* new_r = (char*)realloc(r, len);
                if (!new_r) { free(r); free(s); return NULL; }
                r = new_r; strcat(r, v->value.object_val.props[i].name); strcat(r, ": "); strcat(r, s);
                if (i < v->value.object_val.prop_count - 1) strcat(r, ", "); free(s);
            }
            strcat(r, "}"); return r;
        }
        case PJ_VARIANT_FUNCTION: return strdup("<function>");
        case PJ_VARIANT_EMOJI: return strdup(v->value.emoji_val.emoji);
        case PJ_VARIANT_PSEUDODIM:
            snprintf(buf, 256, "(%.*g, %.*g%s)", v->value.pseudodim_val.dims > 2 ? 10 : 2,
                     v->value.pseudodim_val.values[0], v->value.pseudodim_val.dims > 2 ? 10 : 2,
                     v->value.pseudodim_val.values[1], v->value.pseudodim_val.dims > 2 ? ", ..." : "");
            break;
        default: return strdup("unknown");
    }
    return strdup(buf);
}

Variant* pj_variant_add(Variant* a, Variant* b) {
    if (!a || !b) return pj_variant_null();
    if (a->type == PJ_VARIANT_INT && b->type == PJ_VARIANT_INT) return pj_variant_int(a->value.int_val + b->value.int_val);
    if ((a->type == PJ_VARIANT_INT || a->type == PJ_VARIANT_DOUBLE) && (b->type == PJ_VARIANT_INT || b->type == PJ_VARIANT_DOUBLE)) {
        double av = a->type == PJ_VARIANT_INT ? a->value.int_val : a->value.double_val;
        double bv = b->type == PJ_VARIANT_INT ? b->value.int_val : b->value.double_val;
        return pj_variant_double(av + bv);
    }
    if (a->type == PJ_VARIANT_STRING && b->type == PJ_VARIANT_STRING) {
        char* r = (char*)malloc(strlen(a->value.string_val) + strlen(b->value.string_val) + 1);
        if (!r) return NULL;
        strcpy(r, a->value.string_val); strcat(r, b->value.string_val);
        Variant* v = pj_variant_string(r); free(r); return v;
    }
    if (a->type == PJ_VARIANT_PSEUDODIM && b->type == PJ_VARIANT_PSEUDODIM && a->value.pseudodim_val.dims == b->value.pseudodim_val.dims) {
        int d = a->value.pseudodim_val.dims;
        double v[PJ_DIM_4D];
        for (int i = 0; i < d; i++) v[i] = a->value.pseudodim_val.values[i] + b->value.pseudodim_val.values[i];
        Variant* r = pj_variant_pseudodim(v, d);
        if (r) pj_variant_set_pseudoprop(r, a->value.pseudodim_val.pseudomass + b->value.pseudodim_val.pseudomass,
                                         a->value.pseudodim_val.pseudocharge + b->value.pseudodim_val.pseudocharge,
                                         a->value.pseudodim_val.pseudospin + b->value.pseudodim_val.pseudospin,
                                         a->value.pseudodim_val.pseudotime + b->value.pseudodim_val.pseudotime);
        return r;
    }
    if (a->type == PJ_VARIANT_EMOJI && b->type == PJ_VARIANT_EMOJI) {
        int d = pj_use_4d ? 4 : 2;
        double v[PJ_DIM_4D];
        for (int i = 0; i < d; i++) v[i] = a->value.emoji_val.vector[i] + b->value.emoji_val.vector[i];
        return pj_variant_pseudodim(v, d);
    }
    return pj_variant_null();
}

Variant* pj_variant_sub(Variant* a, Variant* b) {
    if (!a || !b) return pj_variant_null();
    if (a->type == PJ_VARIANT_INT && b->type == PJ_VARIANT_INT) return pj_variant_int(a->value.int_val - b->value.int_val);
    if ((a->type == PJ_VARIANT_INT || a->type == PJ_VARIANT_DOUBLE) && (b->type == PJ_VARIANT_INT || b->type == PJ_VARIANT_DOUBLE)) {
        double av = a->type == PJ_VARIANT_INT ? a->value.int_val : a->value.double_val;
        double bv = b->type == PJ_VARIANT_INT ? b->value.int_val : b->value.double_val;
        return pj_variant_double(av - bv);
    }
    if (a->type == PJ_VARIANT_PSEUDODIM && b->type == PJ_VARIANT_PSEUDODIM && a->value.pseudodim_val.dims == b->value.pseudodim_val.dims) {
        int d = a->value.pseudodim_val.dims;
        double v[PJ_DIM_4D];
        for (int i = 0; i < d; i++) v[i] = a->value.pseudodim_val.values[i] - b->value.pseudodim_val.values[i];
        Variant* r = pj_variant_pseudodim(v, d);
        if (r) pj_variant_set_pseudoprop(r, fabs(a->value.pseudodim_val.pseudomass - b->value.pseudodim_val.pseudomass),
                                         a->value.pseudodim_val.pseudocharge - b->value.pseudodim_val.pseudocharge,
                                         a->value.pseudodim_val.pseudospin, a->value.pseudodim_val.pseudotime);
        return r;
    }
    if (a->type == PJ_VARIANT_EMOJI && b->type == PJ_VARIANT_EMOJI) {
        int d = pj_use_4d ? 4 : 2;
        double v[PJ_DIM_4D];
        for (int i = 0; i < d; i++) v[i] = a->value.emoji_val.vector[i] - b->value.emoji_val.vector[i];
        return pj_variant_pseudodim(v, d);
    }
    return pj_variant_null();
}

Variant* pj_variant_mul(Variant* a, Variant* b) {
    if (!a || !b) return pj_variant_null();
    if (a->type == PJ_VARIANT_INT && b->type == PJ_VARIANT_INT) return pj_variant_int(a->value.int_val * b->value.int_val);
    if ((a->type == PJ_VARIANT_INT || a->type == PJ_VARIANT_DOUBLE) && (b->type == PJ_VARIANT_INT || b->type == PJ_VARIANT_DOUBLE)) {
        double av = a->type == PJ_VARIANT_INT ? a->value.int_val : a->value.double_val;
        double bv = b->type == PJ_VARIANT_INT ? b->value.int_val : b->value.double_val;
        return pj_variant_double(av * bv);
    }
    if (a->type == PJ_VARIANT_STRING && b->type == PJ_VARIANT_INT && b->value.int_val >= 0) {
        int n = b->value.int_val;
        size_t len = strlen(a->value.string_val);
        if (!n || !len) return pj_variant_string("");
        char* r = (char*)malloc(len * n + 1);
        if (!r) return NULL;
        r[0] = '\0'; for (int i = 0; i < n; i++) strcat(r, a->value.string_val);
        Variant* v = pj_variant_string(r); free(r); return v;
    }
    if (a->type == PJ_VARIANT_PSEUDODIM && b->type == PJ_VARIANT_PSEUDODIM && a->value.pseudodim_val.dims == b->value.pseudodim_val.dims) {
        double dot = 0.0;
        for (int i = 0; i < a->value.pseudodim_val.dims; i++) dot += a->value.pseudodim_val.values[i] * b->value.pseudodim_val.values[i];
        return pj_variant_double(dot);
    }
    if (a->type == PJ_VARIANT_PSEUDODIM && (b->type == PJ_VARIANT_INT || b->type == PJ_VARIANT_DOUBLE)) {
        double s = b->type == PJ_VARIANT_INT ? b->value.int_val : b->value.double_val;
        double v[PJ_DIM_4D];
        for (int i = 0; i < a->value.pseudodim_val.dims; i++) v[i] = a->value.pseudodim_val.values[i] * s;
        Variant* r = pj_variant_pseudodim(v, a->value.pseudodim_val.dims);
        if (r) pj_variant_set_pseudoprop(r, a->value.pseudodim_val.pseudomass * fabs(s),
                                         a->value.pseudodim_val.pseudocharge * s, a->value.pseudodim_val.pseudospin,
                                         a->value.pseudodim_val.pseudotime);
        return r;
    }
    if ((a->type == PJ_VARIANT_INT || a->type == PJ_VARIANT_DOUBLE) && b->type == PJ_VARIANT_PSEUDODIM) return pj_variant_mul(b, a);
    return pj_variant_null();
}

Variant* pj_variant_div(Variant* a, Variant* b) {
    if (!a || !b) return pj_variant_null();
    if ((a->type == PJ_VARIANT_INT || a->type == PJ_VARIANT_DOUBLE) && (b->type == PJ_VARIANT_INT || b->type == PJ_VARIANT_DOUBLE)) {
        double av = a->type == PJ_VARIANT_INT ? a->value.int_val : a->value.double_val;
        double bv = b->type == PJ_VARIANT_INT ? b->value.int_val : b->value.double_val;
        return fabs(bv) < PJ_EPSILON ? NULL : pj_variant_double(av / bv);
    }
    if (a->type == PJ_VARIANT_PSEUDODIM && (b->type == PJ_VARIANT_INT || b->type == PJ_VARIANT_DOUBLE)) {
        double s = b->type == PJ_VARIANT_INT ? b->value.int_val : b->value.double_val;
        if (fabs(s) < PJ_EPSILON) return NULL;
        double v[PJ_DIM_4D];
        for (int i = 0; i < a->value.pseudodim_val.dims; i++) v[i] = a->value.pseudodim_val.values[i] / s;
        Variant* r = pj_variant_pseudodim(v, a->value.pseudodim_val.dims);
        if (r) pj_variant_set_pseudoprop(r, a->value.pseudodim_val.pseudomass / fabs(s),
                                         a->value.pseudodim_val.pseudocharge / s, a->value.pseudodim_val.pseudospin,
                                         a->value.pseudodim_val.pseudotime);
        return r;
    }
    return pj_variant_null();
}

Variant* pj_variant_delta(Variant* a, Variant* b) {
    if (!a || !b) return pj_variant_null();
    if ((a->type == PJ_VARIANT_INT || a->type == PJ_VARIANT_DOUBLE) && (b->type == PJ_VARIANT_INT || b->type == PJ_VARIANT_DOUBLE)) {
        double av = a->type == PJ_VARIANT_INT ? a->value.int_val : a->value.double_val;
        double bv = b->type == PJ_VARIANT_INT ? b->value.int_val : b->value.double_val;
        double d = av - bv;
        return pj_variant_double(av + d * TICKSTACK_PHI);
    }
    if (a->type == PJ_VARIANT_PSEUDODIM && b->type == PJ_VARIANT_PSEUDODIM && a->value.pseudodim_val.dims == b->value.pseudodim_val.dims) {
        int d = a->value.pseudodim_val.dims;
        double v[PJ_DIM_4D], delta[PJ_DIM_4D];
        for (int i = 0; i < d; i++) { delta[i] = a->value.pseudodim_val.values[i] - b->value.pseudodim_val.values[i]; v[i] = a->value.pseudodim_val.values[i] + delta[i] * TICKSTACK_PHI; }
        Variant* r = pj_variant_pseudodim(v, d);
        if (r) {
            double md = a->value.pseudodim_val.pseudomass - b->value.pseudodim_val.pseudomass;
            double cd = a->value.pseudodim_val.pseudocharge - b->value.pseudodim_val.pseudocharge;
            double sd = a->value.pseudodim_val.pseudospin - b->value.pseudodim_val.pseudospin;
            double td = a->value.pseudodim_val.pseudotime - b->value.pseudodim_val.pseudotime;
            pj_variant_set_pseudoprop(r, a->value.pseudodim_val.pseudomass + md * TICKSTACK_PHI,
                                     a->value.pseudodim_val.pseudocharge + cd * TICKSTACK_PHI,
                                     a->value.pseudodim_val.pseudospin + sd * TICKSTACK_PHI,
                                     a->value.pseudodim_val.pseudotime + td * TICKSTACK_PHI);
        }
        return r;
    }
    if (a->type == PJ_VARIANT_EMOJI && b->type == PJ_VARIANT_EMOJI) {
        int d = pj_use_4d ? 4 : 2;
        double v[PJ_DIM_4D], delta[PJ_DIM_4D];
        for (int i = 0; i < d; i++) { delta[i] = a->value.emoji_val.vector[i] - b->value.emoji_val.vector[i]; v[i] = a->value.emoji_val.vector[i] + delta[i] * TICKSTACK_PHI; }
        return pj_variant_pseudodim(v, d);
    }
    return pj_variant_null();
}

int pj_variant_eq(Variant* a, Variant* b) {
    if (!a && !b) return 1;
    if (!a || !b) return 0;
    if (a->type != b->type) {
        if ((a->type == PJ_VARIANT_INT || a->type == PJ_VARIANT_DOUBLE) && (b->type == PJ_VARIANT_INT || b->type == PJ_VARIANT_DOUBLE)) {
            double av = a->type == PJ_VARIANT_INT ? a->value.int_val : a->value.double_val;
            double bv = b->type == PJ_VARIANT_INT ? b->value.int_val : b->value.double_val;
            return fabs(av - bv) < PJ_EPSILON;
        }
        return 0;
    }
    switch (a->type) {
        case PJ_VARIANT_NULL: return 1;
        case PJ_VARIANT_INT: return a->value.int_val == b->value.int_val;
        case PJ_VARIANT_DOUBLE: return fabs(a->value.double_val - b->value.double_val) < PJ_EPSILON;
        case PJ_VARIANT_STRING: return !strcmp(a->value.string_val, b->value.string_val);
        case PJ_VARIANT_BOOL: return a->value.bool_val == b->value.bool_val;
        case PJ_VARIANT_ARRAY:
            if (a->value.array_val.len != b->value.array_val.len) return 0;
            for (size_t i = 0; i < a->value.array_val.len; i++)
                if (!pj_variant_eq(a->value.array_val.items[i], b->value.array_val.items[i])) return 0;
            return 1;
        case PJ_VARIANT_OBJECT:
            if (a->value.object_val.prop_count != b->value.object_val.prop_count) return 0;
            for (size_t i = 0; i < a->value.object_val.prop_count; i++) {
                Variant* av = a->value.object_val.props[i].value;
                Variant* bv = pj_variant_object_get(b, a->value.object_val.props[i].name);
                if (!pj_variant_eq(av, bv)) return 0;
            }
            return 1;
        case PJ_VARIANT_FUNCTION: return a->value.function_val.func == b->value.function_val.func && a->value.function_val.arg_count == b->value.function_val.arg_count;
        case PJ_VARIANT_EMOJI: return !strcmp(a->value.emoji_val.emoji, b->value.emoji_val.emoji);
        case PJ_VARIANT_PSEUDODIM:
            if (a->value.pseudodim_val.dims != b->value.pseudodim_val.dims) return 0;
            for (int i = 0; i < a->value.pseudodim_val.dims; i++)
                if (fabs(a->value.pseudodim_val.values[i] - b->value.pseudodim_val.values[i]) >= PJ_EPSILON) return 0;
            return fabs(a->value.pseudodim_val.pseudomass - b->value.pseudodim_val.pseudomass) < PJ_EPSILON &&
                   fabs(a->value.pseudodim_val.pseudocharge - b->value.pseudodim_val.pseudocharge) < PJ_EPSILON &&
                   fabs(a->value.pseudodim_val.pseudospin - b->value.pseudodim_val.pseudospin) < PJ_EPSILON &&
                   fabs(a->value.pseudodim_val.pseudotime - b->value.pseudodim_val.pseudotime) < PJ_EPSILON;
        default: return 0;
    }
}

static void __attribute__((constructor)) pj_auto_init(void) {
    pj_init();
    double c1[] = {1.0, 0.5}, c2[] = {-1.0, -0.5}, c3[] = {2.0, 0.8}, c4[] = {-2.0, -0.8};
    double c5[] = {1.0, 0.5, 0.25, 0.125}, c6[] = {-1.0, -0.5, -0.25, -0.125};
    double c7[] = {0.5, 1.0, 1.5, 2.0}, c8[] = {-0.5, -1.0, -1.5, -2.0};
    pj_add_emoji("😀", c1, 2); pj_add_emoji("😢", c2, 2); pj_add_emoji("😥", c3, 2); pj_add_emoji("😎", c4, 2);
    pj_add_emoji("🌌", c5, 4); pj_add_emoji("🔮", c6, 4); pj_add_emoji("🧠", c7, 4); pj_add_emoji("🌐", c8, 4);
    pj_add_emoji_tone("😀", "happy", 1.2, 0.2, "😊"); pj_add_emoji_tone("😀", "sad", 0.8, -0.2, "😞");
    pj_add_emoji_tone("😢", "very", 1.5, -0.3, "😭"); pj_add_emoji_tone("😢", "little", 0.5, -0.1, "😿");
    pj_add_emoji_tone("🌌", "cosmic", 2.0, 0.5, "✨"); pj_add_emoji_tone("🔮", "mystic", 1.8, -0.3, "🪄");
    pj_add_emoji_tone("🧠", "genius", 1.5, 0.8, "💡"); pj_add_emoji_tone("🌐", "vortex", 2.2, -0.5, "🌪️");
}

#endif /* TICKSTACK_PSEUDOJAVA_H */