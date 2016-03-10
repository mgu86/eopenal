/*┌────────────────────────────────────────────────────────────────────────┐
  │        _______                    _____________                        │
  │   _______  __ \______________________    |__  /                        │
  │   _  _ \  / / /__  __ \  _ \_  __ \_  /| |_  /                         │
  │   /  __/ /_/ /__  /_/ /  __/  / / /  ___ |  /___                       │
  │   \___/\____/ _  .___/\___//_/ /_//_/  |_/_____/                       │
  │               /_/                                                      │
  │                                                                        │
  │ eOpenAL - OpenAL Soft NIF for Erlang                                   │
  │ Copyright (C) 2016  Eric des Courtis                                   │
  │                                                                        │
  │ This library is free software; you can redistribute it and/or          │
  │ modify it under the terms of the GNU Lesser General Public             │
  │ License as published by the Free Software Foundation; either           │
  │ version 2.1 of the License, or (at your option) any later version.     │
  │                                                                        │
  │ This library is distributed in the hope that it will be useful,        │
  │ but WITHOUT ANY WARRANTY; without even the implied warranty of         │
  │ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      │
  │ Lesser General Public License for more details.                        │
  │                                                                        │
  │ You should have received a copy of the GNU Lesser General Public       │
  │ License along with this library; if not, write to the Free Software    │
  │ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,             │
  │ MA  02110-1301  USA                                                    │
  └────────────────────────────────────────────────────────────────────────┘
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include "erl_nif.h"

typedef struct {
    ALenum v;
    int error;
} alenum_or_error_t;

typedef struct {
    ALCenum v;
    int error;
} alcenum_or_error_t;

static ErlNifResourceType *eopenal_buffer_type = NULL;
static ErlNifResourceType *eopenal_source_type = NULL;
static ErlNifResourceType *eopenal_device_type = NULL;
static ErlNifResourceType *eopenal_context_type = NULL;

static ERL_NIF_TERM mk_error(ErlNifEnv *env, ERL_NIF_TERM t);
static ERL_NIF_TERM eopenal_error_to_atom(ErlNifEnv *env, ALenum error);
static ERL_NIF_TERM eopenal_alc_error_to_atom(ErlNifEnv *env, ALCenum error);
static alenum_or_error_t eopenal_atom_to_alenum(ErlNifEnv *env, ERL_NIF_TERM atom);
static alcenum_or_error_t eopenal_atom_to_alcenum(ErlNifEnv* env, ALCdevice *dev, ERL_NIF_TERM atom);

static ERL_NIF_TERM eopenal_alcGetIntegerv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alcSuspendContext(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alcProcessContext(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alcMakeContextCurrent(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alcCreateContext(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alcOpenDevice(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alcIsExtensionPresent(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alcGetString(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGenBuffers(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alIsBuffer(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alBufferData(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetBufferf(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetBufferi(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGenSources(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alIsSource(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourcef(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourcefv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSource3f(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourcei(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetSourcef(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetSourcefv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetSourcei(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourcePlay(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourcePlayv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourcePause(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourcePausev(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourceStop(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourceStopv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourceRewind(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourceRewindv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourceQueueBuffers(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alSourceUnqueueBuffers(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alListenerf(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alListener3f(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alListenerfv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alListeneri(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetListenerf(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetListener3f(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetListenerfv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetListeneri(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alEnable(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alDisable(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alIsEnabled(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetBoolean(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetDouble(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetFloat(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetInteger(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetBooleanv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetDoublev(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetFloatv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetIntegerv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alGetString(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alDistanceModel(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alDopplerFactor(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alDopplerVelocity(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
static ERL_NIF_TERM eopenal_alIsExtensionPresent(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);

static void garbage_collect_eopenal_buffer(ErlNifEnv *env, void *resource);
static void garbage_collect_eopenal_source(ErlNifEnv *env, void *resource);
static void garbage_collect_eopenal_device(ErlNifEnv *env, void *resource);
static void garbage_collect_eopenal_context(ErlNifEnv *env, void *resource);

static int handle_load(ErlNifEnv *env, void **priv, ERL_NIF_TERM load_info);
static int handle_upgrade(ErlNifEnv* env, void** priv_data, void** old_priv_data, ERL_NIF_TERM load_info);

/* ERL_NIF >= 2.8 */
#if ERL_NIF_MAJOR_VERSION > 2 || \
            (ERL_NIF_MAJOR_VERSION == 2 && \
                 (ERL_NIF_MINOR_VERSION > 8 || (ERL_NIF_MINOR_VERSION == 8)))
static ErlNifFunc nif_funcs[] = {
    {"alcGetIntegerv",          3, eopenal_alcGetIntegerv,          0},
    {"alcSuspendContext",       1, eopenal_alcSuspendContext,       0},
    {"alcProcessContext",       1, eopenal_alcProcessContext,       0},
    {"alcMakeContextCurrent",   1, eopenal_alcMakeContextCurrent,   0},
    {"alcCreateContext",        1, eopenal_alcCreateContext,        0},
    {"alcCreateContext",        2, eopenal_alcCreateContext,        0},
    {"alcGetString",            1, eopenal_alcGetString,            0},
    {"alcGetString",            2, eopenal_alcGetString,            0},
    {"alcOpenDevice",           0, eopenal_alcOpenDevice,           0},
    {"alcOpenDevice",           1, eopenal_alcOpenDevice,           0},
    {"alcIsExtensionPresent",   1, eopenal_alcIsExtensionPresent,   0},
    {"alcIsExtensionPresent",   2, eopenal_alcIsExtensionPresent,   0},
    {"alGenBuffers",            1, eopenal_alGenBuffers,            0},
    {"alIsBuffer",              1, eopenal_alIsBuffer,              0},
    {"alBufferData",            4, eopenal_alBufferData,            0},
    {"alGetBufferf",            2, eopenal_alGetBufferf,            0},
    {"alGetBufferi",            2, eopenal_alGetBufferi,            0},
    {"alGenSources",            1, eopenal_alGenSources,            0},
    {"alIsSource",              1, eopenal_alIsSource,              0},
    {"alSourcef",               3, eopenal_alSourcef,               0},
    {"alSourcefv",              3, eopenal_alSourcefv,              0},
    {"alSource3f",              5, eopenal_alSource3f,              0},
    {"alSourcei",               3, eopenal_alSourcei,               0},
    {"alGetSourcef",            2, eopenal_alGetSourcef,            0},
    {"alGetSourcefv",           2, eopenal_alGetSourcefv,           0},
    {"alGetSourcei",            2, eopenal_alGetSourcei,            0},
    {"alSourcePlay",            1, eopenal_alSourcePlay,            0},
    {"alSourcePlayv",           1, eopenal_alSourcePlayv,           0},
    {"alSourcePause",           1, eopenal_alSourcePause,           0},
    {"alSourcePausev",          1, eopenal_alSourcePausev,          0},
    {"alSourceStop",            1, eopenal_alSourceStop,            0},
    {"alSourceStopv",           1, eopenal_alSourceStopv,           0},
    {"alSourceRewind",          1, eopenal_alSourceRewind,          0},
    {"alSourceRewindv",         1, eopenal_alSourceRewindv,         0},
    {"alSourceQueueBuffers",    2, eopenal_alSourceQueueBuffers,    0},
    {"alSourceUnqueueBuffers",  2, eopenal_alSourceUnqueueBuffers,  0},
    {"alListenerf",             2, eopenal_alListenerf,             0},
    {"alListener3f",            4, eopenal_alListener3f,            0},
    {"alListenerfv",            2, eopenal_alListenerfv,            0},
    {"alListeneri",             2, eopenal_alListeneri,             0},
    {"alGetListenerf",          1, eopenal_alGetListenerf,          0},
    {"alGetListener3f",         1, eopenal_alGetListener3f,         0},
    {"alGetListenerfv",         1, eopenal_alGetListenerfv,         0},
    {"alGetListeneri",          1, eopenal_alGetListeneri,          0},
    {"alEnable",                1, eopenal_alEnable,                0},
    {"alDisable",               1, eopenal_alDisable,               0},
    {"alIsEnabled",             1, eopenal_alIsEnabled,             0},
    {"alGetBoolean",            1, eopenal_alGetBoolean,            0},
    {"alGetDouble",             1, eopenal_alGetDouble,             0},
    {"alGetFloat",              1, eopenal_alGetFloat,              0},
    {"alGetInteger",            1, eopenal_alGetInteger,            0},
    {"alGetBooleanv",           1, eopenal_alGetBooleanv,           0},
    {"alGetDoublev",            1, eopenal_alGetDoublev,            0},
    {"alGetFloatv",             1, eopenal_alGetFloatv,             0},
    {"alGetIntegerv",           1, eopenal_alGetIntegerv,           0},
    {"alGetString",             1, eopenal_alGetString,             0},
    {"alDistanceModel",         1, eopenal_alDistanceModel,         0},
    {"alDopplerFactor",         1, eopenal_alDopplerFactor,         0},
    {"alDopplerVelocity",       1, eopenal_alDopplerVelocity,       0},
    {"alIsExtensionPresent",    1, eopenal_alIsExtensionPresent,    0}
};
#else
static ErlNifFunc nif_funcs[] = {
    {"alcGetIntegerv",          3, eopenal_alcGetIntegerv           },
    {"alcSuspendContext",       1, eopenal_alcSuspendContext        },
    {"alcProcessContext",       1, eopenal_alcProcessContext        },
    {"alcMakeContextCurrent",   1, eopenal_alcMakeContextCurrent    },
    {"alcCreateContext",        1, eopenal_alcCreateContext         },
    {"alcCreateContext",        2, eopenal_alcCreateContext         },
    {"alcGetString",            1, eopenal_alcGetString             },
    {"alcGetString",            2, eopenal_alcGetString             },
    {"alcOpenDevice",           0, eopenal_alcOpenDevice            },
    {"alcOpenDevice",           1, eopenal_alcOpenDevice            },
    {"alcIsExtensionPresent",   1, eopenal_alcIsExtensionPresent    },
    {"alcIsExtensionPresent",   2, eopenal_alcIsExtensionPresent    },
    {"alGenBuffers",            1, eopenal_alGenBuffers             },
    {"alIsBuffer",              1, eopenal_alIsBuffer               },
    {"alBufferData",            4, eopenal_alBufferData             },
    {"alGetBufferf",            2, eopenal_alGetBufferf             },
    {"alGetBufferi",            2, eopenal_alGetBufferi             },
    {"alGenSources",            1, eopenal_alGenSources             },
    {"alIsSource",              1, eopenal_alIsSource               },
    {"alSourcef",               3, eopenal_alSourcef                },
    {"alSourcefv",              3, eopenal_alSourcefv               },
    {"alSource3f",              5, eopenal_alSource3f               },
    {"alSourcei",               3, eopenal_alSourcei                },
    {"alGetSourcef",            2, eopenal_alGetSourcef             },
    {"alGetSourcefv",           2, eopenal_alGetSourcefv            },
    {"alGetSourcei",            2, eopenal_alGetSourcei             },
    {"alSourcePlay",            1, eopenal_alSourcePlay             },
    {"alSourcePlayv",           1, eopenal_alSourcePlayv            },
    {"alSourcePause",           1, eopenal_alSourcePause            },
    {"alSourcePausev",          1, eopenal_alSourcePausev           },
    {"alSourceStop",            1, eopenal_alSourceStop             },
    {"alSourceStopv",           1, eopenal_alSourceStopv            },
    {"alSourceRewind",          1, eopenal_alSourceRewind           },
    {"alSourceRewindv",         1, eopenal_alSourceRewindv          },
    {"alSourceQueueBuffers",    2, eopenal_alSourceQueueBuffers     },
    {"alSourceUnqueueBuffers",  2, eopenal_alSourceUnqueueBuffers   },
    {"alListenerf",             2, eopenal_alListenerf              },
    {"alListener3f",            4, eopenal_alListener3f             },
    {"alListenerfv",            2, eopenal_alListenerfv             },
    {"alListeneri",             2, eopenal_alListeneri              },
    {"alGetListenerf",          1, eopenal_alGetListenerf           },
    {"alGetListener3f",         1, eopenal_alGetListener3f          },
    {"alGetListenerfv",         1, eopenal_alGetListenerfv          },
    {"alGetListeneri",          1, eopenal_alGetListeneri           },
    {"alEnable",                1, eopenal_alEnable                 },
    {"alDisable",               1, eopenal_alDisable                },
    {"alIsEnabled",             1, eopenal_alIsEnabled              },
    {"alGetBoolean",            1, eopenal_alGetBoolean             },
    {"alGetDouble",             1, eopenal_alGetDouble              },
    {"alGetFloat",              1, eopenal_alGetFloat               },
    {"alGetInteger",            1, eopenal_alGetInteger             },
    {"alGetBooleanv",           1, eopenal_alGetBooleanv            },
    {"alGetDoublev",            1, eopenal_alGetDoublev             },
    {"alGetFloatv",             1, eopenal_alGetFloatv              },
    {"alGetIntegerv",           1, eopenal_alGetIntegerv            },
    {"alGetString",             1, eopenal_alGetString              },
    {"alDistanceModel",         1, eopenal_alDistanceModel          },
    {"alDopplerFactor",         1, eopenal_alDopplerFactor          },
    {"alDopplerVelocity",       1, eopenal_alDopplerVelocity        },
    {"alIsExtensionPresent",    1, eopenal_alIsExtensionPresent     }
};
#endif

static ERL_NIF_TERM mk_error(ErlNifEnv *env, ERL_NIF_TERM t)
{
    return enif_make_tuple2(env, enif_make_atom(env, "error"), t);
}

static ERL_NIF_TERM eopenal_error_to_atom(ErlNifEnv *env, ALenum error)
{
    switch(error){
    case AL_NO_ERROR:
        return enif_make_atom(env, "al_no_error");
    case AL_INVALID_NAME:
        return enif_make_atom(env, "al_invalid_name");
    case AL_INVALID_ENUM:
        return enif_make_atom(env, "al_invalid_enum");
    case AL_INVALID_VALUE:
        return enif_make_atom(env, "al_invalid_value");
    case AL_INVALID_OPERATION:
        return enif_make_atom(env, "al_invalid_operation");
    case AL_OUT_OF_MEMORY:
        return enif_make_atom(env, "al_out_of_memory");
    default:
        break;
    }
    return enif_make_atom(env, "al_unknown");
}

static ERL_NIF_TERM eopenal_alc_error_to_atom(ErlNifEnv *env, ALCenum error)
{
    switch(error){
    case ALC_NO_ERROR:
        return enif_make_atom(env, "alc_no_error");
    case ALC_INVALID_DEVICE:
        return enif_make_atom(env, "alc_invalid_device");
    case ALC_INVALID_CONTEXT:
        return enif_make_atom(env, "alc_invalid_context");
    case ALC_INVALID_ENUM:
        return enif_make_atom(env, "alc_invalid_enum");
    case ALC_INVALID_VALUE:
        return enif_make_atom(env, "alc_invalid_value");
    case ALC_OUT_OF_MEMORY:
        return enif_make_atom(env, "alc_out_of_memory");
    default:
        break;
    }
    return enif_make_atom(env, "alc_unknown");
}

static alenum_or_error_t eopenal_atom_to_alenum(ErlNifEnv* env, ERL_NIF_TERM atom)
{
    char buf[512] = "";
    char *c;
    alenum_or_error_t e;
    ALenum error;

    if(!enif_get_atom(env, atom, buf, (unsigned)sizeof(buf), ERL_NIF_LATIN1)){
        e.error = 1;
        return e;
    }

    for(c = buf; *c != '\0'; c++) *c = (char)toupper((int)*c);
    e.v = alGetEnumValue(buf);
    error = alGetError();
    if(error != AL_NO_ERROR){
        e.error = 1;
        return e;
    }

    return e;
}

static alcenum_or_error_t eopenal_atom_to_alcenum(ErlNifEnv* env, ALCdevice *dev, ERL_NIF_TERM atom)
{
    char buf[512] = "";
    char *c;
    alcenum_or_error_t e;
    ALCenum error;

    if(!enif_get_atom(env, atom, buf, (unsigned)sizeof(buf), ERL_NIF_LATIN1)){
        e.error = 1;
        return e;
    }

    for(c = buf; *c != '\0'; c++) *c = (char)toupper((int)*c);
    e.v = alcGetEnumValue(dev, buf);
    error = alcGetError(dev);
    if(error != ALC_NO_ERROR){
        e.error = 1;
        return e;
    }

    return e;
}

static ERL_NIF_TERM eopenal_alcGetIntegerv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALCdevice **dev_ptr = NULL;
    alcenum_or_error_t tmp;
    ALCenum pname;
    ALCsizei size;
    ErlNifUInt64 size64;
    ErlNifBinary bin;
    ALCenum error;

    if(argc != 3) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_device_type, (void **)&dev_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alcenum(env, *dev_ptr, argv[1]);
    if(tmp.error){
        return enif_make_badarg(env);
    }
    pname = tmp.v;
    if(!enif_get_uint64(env, argv[2], &size64)){
        return enif_make_badarg(env);
    }
    size = (ALCsizei)size64;
    size *= sizeof(ALCint);

    if(!enif_alloc_binary((size_t)size, &bin)){
        return enif_make_badarg(env);
    }

    alcGetIntegerv(*dev_ptr, pname, size, (ALCint *)bin.data);
    error = alcGetError(*dev_ptr);
    if(error != ALC_NO_ERROR){
        return mk_error(env, eopenal_alc_error_to_atom(env, error));
    }

    return enif_make_binary(env, &bin);
}

static ERL_NIF_TERM eopenal_alcSuspendContext(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALCcontext **ctx_ptr = NULL;
    ALCdevice *dev;
    ALCenum error;

    if(argc != 1) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_context_type, (void **)&ctx_ptr)){
        return enif_make_badarg(env);
    }

    dev = alcGetContextsDevice(*ctx_ptr);
    if(!dev) return enif_make_badarg(env);
    error = alcGetError(dev);
    if(error != ALC_NO_ERROR){
        return mk_error(env, eopenal_alc_error_to_atom(env, error));
    }

    alcSuspendContext(*ctx_ptr);
    error = alcGetError(dev);
    if(error != ALC_NO_ERROR){
        return mk_error(env, eopenal_alc_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alcProcessContext(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALCcontext **ctx_ptr = NULL;
    ALCdevice *dev;
    ALCenum error;

    if(argc != 1) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_context_type, (void **)&ctx_ptr)){
        return enif_make_badarg(env);
    }

    dev = alcGetContextsDevice(*ctx_ptr);
    if(!dev) return enif_make_badarg(env);
    error = alcGetError(dev);
    if(error != ALC_NO_ERROR){
        return mk_error(env, eopenal_alc_error_to_atom(env, error));
    }

    alcProcessContext(*ctx_ptr);
    error = alcGetError(dev);
    if(error != ALC_NO_ERROR){
        return mk_error(env, eopenal_alc_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alcMakeContextCurrent(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALCboolean result;
    ALCdevice *dev;
    ALCenum error;
    ALCcontext **ctx_ptr = NULL;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_context_type, (void **)&ctx_ptr)){
        return enif_make_badarg(env);
    }

    dev = alcGetContextsDevice(*ctx_ptr);
    if(!dev) return enif_make_badarg(env);
    error = alcGetError(dev);
    if(error != ALC_NO_ERROR){
        return mk_error(env, eopenal_alc_error_to_atom(env, error));
    }

    result = alcMakeContextCurrent(*ctx_ptr);
    if(result == ALC_TRUE) return enif_make_atom(env, "true");
    error = alcGetError(dev);
    if(error != ALC_NO_ERROR){
        return mk_error(env, eopenal_alc_error_to_atom(env, error));
    }

    return enif_make_atom(env, "false");
}

static ERL_NIF_TERM eopenal_alcCreateContext(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALCdevice **dev_ptr = NULL;
    ALCcontext *ctx;
    ALCcontext **ctx_ref = NULL;
    ALCint *attrlist = NULL;
    unsigned len;
    unsigned i;
    alcenum_or_error_t tmp;
    ALCenum pname;
    ERL_NIF_TERM head;
    ERL_NIF_TERM tail;
    ERL_NIF_TERM list;
    const ERL_NIF_TERM *t;
    int arity;
    ErlNifSInt64 val64;
    ERL_NIF_TERM ctx_resource;
    ALCenum error;

    if(argc != 1 && argc != 2) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_device_type, (void **)&dev_ptr)){
        return enif_make_badarg(env);
    }

    switch(argc){
    case 1:
        break;
    case 2:
        if(!enif_get_list_length(env, argv[1], &len)) return enif_make_badarg(env);
        attrlist = (ALCint *)enif_alloc(sizeof(ALCint) * (size_t)(len + 1) * 2);
        for(list = argv[1], i = 0; enif_get_list_cell(env, list, &head, &tail); i++, list = tail){
            if(!enif_get_tuple(env, head, &arity, &t)){
                enif_free(attrlist);
                return enif_make_badarg(env);
            }

            if(arity != 2){
                enif_free(attrlist);
                return enif_make_badarg(env);
            }

            tmp = eopenal_atom_to_alcenum(env, *dev_ptr, t[0]);
            if(tmp.error){
                enif_free(attrlist);
                return enif_make_badarg(env);
            }
            pname = tmp.v;

            if(!enif_get_int64(env, t[1], &val64)){
                enif_free(attrlist);
                return enif_make_badarg(env);
            }


            attrlist[i*2] = pname;
            attrlist[i*2+1] = (ALCint)val64;
        }
        attrlist[i*2] = 0;
        attrlist[i*2+1] = 0;
        break;
    default:
        return enif_make_badarg(env);
    }

    ctx = alcCreateContext(*dev_ptr, attrlist);
    if(attrlist != NULL) enif_free(attrlist);
    if(ctx == NULL){
        error = alcGetError(*dev_ptr);
        if(error != ALC_NO_ERROR){
            return mk_error(env, eopenal_alc_error_to_atom(env, error));
        }
        return enif_make_badarg(env);
    }
    error = alcGetError(*dev_ptr);
    if(error != ALC_NO_ERROR){
        return mk_error(env, eopenal_alc_error_to_atom(env, error));
    }

    ctx_ref = (ALCcontext **)enif_alloc_resource(eopenal_context_type, sizeof(ALCcontext *));
    *ctx_ref = ctx;
    ctx_resource = enif_make_resource(env, ctx_ref);
    enif_release_resource(ctx_ref);
    return ctx_resource;
}

static ERL_NIF_TERM eopenal_alcGetString(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    const ALCchar *s;
    alcenum_or_error_t tmp;
    ALCenum pname;
    ALCdevice *dev = NULL;
    ALCdevice **dev_ptr = NULL;
    ERL_NIF_TERM param;
    ALCenum error;
    const ALCchar *device;
    const ALCchar *next;
    ERL_NIF_TERM list;
    size_t len = 0;


    if(argc != 1 && argc != 2) return enif_make_badarg(env);

    switch(argc){
    case 1:
        param = argv[0];
        break;
    case 2:
        if(!enif_get_resource(env, argv[0], eopenal_device_type, (void **)&dev_ptr)){
            return enif_make_badarg(env);
        }
        dev = *dev_ptr;
        param = argv[1];
        break;
    default:
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alcenum(env, dev, param);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;


    s = alcGetString(dev, pname);
    if(s == NULL){
        error = alcGetError(dev);
        if(error != ALC_NO_ERROR){
            return mk_error(env, eopenal_alc_error_to_atom(env, error));
        }
        return enif_make_badarg(env);
    }

    if (alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT") == ALC_TRUE) {
        if(pname == ALC_DEVICE_SPECIFIER || pname == ALC_CAPTURE_DEVICE_SPECIFIER){
            list = enif_make_list(env, 0);
            device = s;
            next = s + 1;
            while(device && *device != '\0' && next && *next != '\0'){
                list = enif_make_list_cell(
                    env,
                    enif_make_string(
                        env,
                        device,
                        ERL_NIF_LATIN1
                    ),
                    list
                );
                len = strlen(device);
                device += (len + 1);
                next += (len + 2);
            }
            return list;
        }
    }
    return enif_make_string(env, (const char *)s, ERL_NIF_LATIN1);
}

static ERL_NIF_TERM eopenal_alcIsExtensionPresent(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    char buf[512];
    ALCdevice *dev = NULL;
    ALCdevice **dev_ptr = NULL;
    ERL_NIF_TERM ext_name;
    ALCboolean result;
    ALCenum error;

    if(argc != 1 && argc != 2) return enif_make_badarg(env);
    switch(argc){
    case 1:
        ext_name = argv[0];
        break;
    case 2:
        if(!enif_get_resource(env, argv[0], eopenal_device_type, (void **)&dev_ptr)){
            return enif_make_badarg(env);
        }
        dev = *dev_ptr;
        ext_name = argv[1];
        break;
    default:
        return enif_make_badarg(env);
    }
    if(!enif_get_string(env, ext_name, buf, (unsigned)sizeof(buf), ERL_NIF_LATIN1)){
        if(!enif_get_atom(env, ext_name, buf, (unsigned)sizeof(buf), ERL_NIF_LATIN1)){
            return enif_make_badarg(env);
        }
    }


    result = alcIsExtensionPresent(dev, (const ALCchar *)buf);
    error = alcGetError(dev);
    if(error != ALC_NO_ERROR){
        return mk_error(env, eopenal_alc_error_to_atom(env, error));
    }

    if(result == ALC_TRUE) return enif_make_atom(env, "true");
    return enif_make_atom(env, "false");
}

static ERL_NIF_TERM eopenal_alcOpenDevice(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALCdevice **dev_ref;
    ALCdevice *dev;
    ERL_NIF_TERM dev_resource;
    char buf[4096];

    if(argc != 1 && argc != 0) return enif_make_badarg(env);
    switch(argc){
    case 0:
        dev = alcOpenDevice(NULL);
        break;
    case 1:
        if(!enif_get_string(env, argv[0], buf, (unsigned)sizeof(buf), ERL_NIF_LATIN1)){
            return enif_make_badarg(env);
        }
        dev = alcOpenDevice(buf);
        break;
    default:
        return enif_make_badarg(env);
    }

    if(!dev){
        return enif_make_badarg(env);
    }

    dev_ref = (ALCdevice **)enif_alloc_resource(eopenal_device_type, sizeof(ALCdevice *));
    *dev_ref = dev;
    dev_resource = enif_make_resource(env, dev_ref);
    enif_release_resource(dev_ref);
    return dev_resource;
}

static ERL_NIF_TERM eopenal_alGenBuffers(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifSInt64 i;
    ALenum error;
    ALsizei n;
    ALuint *buffers;
    ALuint *buf_ref;
    ERL_NIF_TERM list;

    if(argc != 1) return enif_make_badarg(env);

    if(!enif_get_int64(env, argv[0], &i)){
        return enif_make_badarg(env);
    }

    n = (ALsizei)i;
    buffers = (ALuint *)enif_alloc((size_t)n * sizeof(ALuint));

    alGenBuffers(n, buffers);
    error = alGetError();
    if(error != AL_NO_ERROR){
        enif_free(buffers);
        return mk_error(env, eopenal_error_to_atom(env, error));
    }


    list = enif_make_list(env, 0);

    for(ErlNifSInt64 j = 0; j < i; j++){
        buf_ref = (ALuint *)enif_alloc_resource(eopenal_buffer_type, sizeof(ALuint));
        *buf_ref = buffers[j];
        list = enif_make_list_cell(env, enif_make_resource(env, buf_ref), list);
        enif_release_resource(buf_ref);
    }

    enif_free(buffers);
    return list;
}

static ERL_NIF_TERM eopenal_alIsBuffer(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *buf_ptr = NULL;
    ALboolean ret;
    if(argc != 1) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_buffer_type, (void **)&buf_ptr)){
        return enif_make_badarg(env);
    }

    ret = alIsBuffer(*buf_ptr);
    if(ret == AL_TRUE) return enif_make_atom(env, "true");
    return enif_make_atom(env, "false");
}


static ERL_NIF_TERM eopenal_alBufferData(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *buf_ptr = NULL;
    ErlNifSInt64 freq;
    ALenum format;
    alenum_or_error_t tmp;
    ErlNifBinary bin;
    ALenum error;

    if(argc != 4) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_buffer_type, (void **)&buf_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alenum(env, argv[1]);
    if(tmp.error) return enif_make_badarg(env);
    format = tmp.v;

    if(!enif_inspect_binary(env, argv[2], &bin)) return enif_make_badarg(env);

    if(!enif_get_int64(env, argv[3], &freq)) return enif_make_badarg(env);

    alBufferData(*buf_ptr, format, (ALvoid *)bin.data, (ALsizei)bin.size, (ALsizei)freq);

    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alGetBufferf(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *buf_ptr = NULL;
    alenum_or_error_t tmp;
    ALenum pname;
    ALfloat val;
    ALenum error;

    if(argc != 2) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_buffer_type, (void **)&buf_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alenum(env, argv[1]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetBufferf(*buf_ptr, pname, &val);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_double(env, (double)val);
}

static ERL_NIF_TERM eopenal_alGetBufferi(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *buf_ptr = NULL;
    alenum_or_error_t tmp;
    ALenum pname;
    ALint val;
    ALenum error;

    if(argc != 2) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_buffer_type, (void **)&buf_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alenum(env, argv[1]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetBufferi(*buf_ptr, pname, &val);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_int64(env, (ErlNifSInt64)val);
}

static ERL_NIF_TERM eopenal_alGenSources(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifSInt64 i;
    ALenum error;
    ALuint *sources;
    ALuint *src_ref;
    ERL_NIF_TERM list;

    if(argc != 1) return enif_make_badarg(env);

    if(!enif_get_int64(env, argv[0], &i)){
        return enif_make_badarg(env);
    }

    sources = (ALuint *)enif_alloc((size_t)i * sizeof(ALuint));

    alGenSources((ALsizei)i, sources);
    error = alGetError();
    if(error != AL_NO_ERROR){
        enif_free(sources);
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    list = enif_make_list(env, 0);

    for(ErlNifSInt64 j = 0; j < i; j++){
        src_ref = (ALuint *)enif_alloc_resource(eopenal_source_type, sizeof(ALuint));
        *src_ref = sources[j];
        list = enif_make_list_cell(env, enif_make_resource(env, src_ref), list);
        enif_release_resource(src_ref);
    }

    enif_free(sources);
    return list;
}

static ERL_NIF_TERM eopenal_alIsSource(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    ALboolean ret;
    if(argc != 1) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    ret = alIsBuffer(*src_ptr);
    if(ret == AL_TRUE) return enif_make_atom(env, "true");
    return enif_make_atom(env, "false");
}

static ERL_NIF_TERM eopenal_alSourcef(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    alenum_or_error_t tmp;
    ALenum pname;
    ALfloat val;
    double dval;
    ALenum error;

    if(argc != 3) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alenum(env, argv[1]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    if(!enif_get_double(env, argv[2], &dval)) return enif_make_badarg(env);
    val = (ALfloat)dval;
    alSourcef(*src_ptr, pname, val);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSourcefv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    alenum_or_error_t tmp;
    ALenum pname;
    ALfloat vector[6];
    double dvector[6];
    const ERL_NIF_TERM* tuple;
    int arity;
    ALenum error;

    if(argc != 3) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alenum(env, argv[1]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    if(!enif_get_tuple(env, argv[2], &arity, &tuple)) return enif_make_badarg(env);
    if(arity != 3 && arity != 6) return enif_make_badarg(env);

    switch(arity){
    case 3:
        if(!enif_get_double(env, tuple[0], &dvector[0])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[1], &dvector[1])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[2], &dvector[2])) return enif_make_badarg(env);
        vector[0] = (ALfloat)dvector[0];
        vector[1] = (ALfloat)dvector[1];
        vector[2] = (ALfloat)dvector[2];
        break;
    case 6:
        if(pname != AL_ORIENTATION) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[0], &dvector[0])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[1], &dvector[1])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[2], &dvector[2])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[3], &dvector[3])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[4], &dvector[4])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[5], &dvector[5])) return enif_make_badarg(env);
        vector[0] = (ALfloat)dvector[0];
        vector[1] = (ALfloat)dvector[1];
        vector[2] = (ALfloat)dvector[2];
        vector[3] = (ALfloat)dvector[3];
        vector[4] = (ALfloat)dvector[4];
        vector[5] = (ALfloat)dvector[5];
        break;
    default:
        return enif_make_badarg(env);
    }

    alSourcefv(*src_ptr, pname, vector);

    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSource3f(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    alenum_or_error_t tmp;
    ALenum pname;
    ALfloat vector[3];
    double dvector[3];
    ALenum error;

    if(argc != 5) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alenum(env, argv[1]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    if(!enif_get_double(env, argv[2], &dvector[0])) return enif_make_badarg(env);
    if(!enif_get_double(env, argv[3], &dvector[1])) return enif_make_badarg(env);
    if(!enif_get_double(env, argv[4], &dvector[2])) return enif_make_badarg(env);
    vector[0] = (ALfloat)dvector[0];
    vector[1] = (ALfloat)dvector[1];
    vector[2] = (ALfloat)dvector[2];

    alSource3f(*src_ptr, pname, vector[0], vector[1], vector[2]);

    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSourcei(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *buf_ptr = NULL;
    ALuint *src_ptr = NULL;
    alenum_or_error_t tmp;
    ALenum pname;
    ALint val;
    ErlNifSInt64 attr;
    ALenum error;

    if(argc != 3) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alenum(env, argv[1]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;


    if(!enif_get_int64(env, argv[2], &attr)){
        if(!enif_get_resource(env, argv[2], eopenal_buffer_type, (void **)&buf_ptr)){
            return enif_make_badarg(env);
        }

        alSourcei(*src_ptr, pname, *buf_ptr);

        error = alGetError();
        if(error != AL_NO_ERROR){
            return mk_error(env, eopenal_error_to_atom(env, error));
        }

        return enif_make_atom(env, "ok");
    }
    val = (ALint)attr;

    alSourcei(*src_ptr, pname, val);

    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}


static ERL_NIF_TERM eopenal_alGetSourcef(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    alenum_or_error_t tmp;
    ALenum pname;
    ALfloat val;
    ALenum error;

    if(argc != 2) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alenum(env, argv[1]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetSourcef(*src_ptr, pname, &val);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");

}

static ERL_NIF_TERM eopenal_alGetSourcefv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    alenum_or_error_t tmp;
    ALenum pname;
    ALfloat vector[6];
    ALenum error;

    if(argc != 2) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alenum(env, argv[1]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetSourcefv(*src_ptr, pname, vector);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    if(pname == AL_ORIENTATION){
        return enif_make_tuple6(env,
            enif_make_double(env, (double)vector[0]),
            enif_make_double(env, (double)vector[1]),
            enif_make_double(env, (double)vector[2]),
            enif_make_double(env, (double)vector[3]),
            enif_make_double(env, (double)vector[4]),
            enif_make_double(env, (double)vector[5])
        );
    }

    return enif_make_tuple3(env,
        enif_make_double(env, (double)vector[0]),
        enif_make_double(env, (double)vector[1]),
        enif_make_double(env, (double)vector[2])
    );
}


static ERL_NIF_TERM eopenal_alGetSourcei(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    alenum_or_error_t tmp;
    ALenum pname;
    ALint val;
    ALenum error;

    if(argc != 2) return enif_make_badarg(env);

    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    tmp = eopenal_atom_to_alenum(env, argv[1]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetSourcei(*src_ptr, pname, &val);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    switch(pname){
    case AL_SOURCE_STATE:
        switch(val){
        case AL_INITIAL:
            return enif_make_atom(env, "al_initial");
        case AL_PLAYING:
            return enif_make_atom(env, "al_playing");
        case AL_PAUSED:
            return enif_make_atom(env, "al_paused");
        case AL_STOPPED:
            return enif_make_atom(env, "al_stopped");
        default:
            break;
        }
        break;
    case AL_SOURCE_TYPE:
        switch(val){
        case AL_STATIC:
            return enif_make_atom(env, "al_static");
        case AL_STREAMING:
            return enif_make_atom(env, "al_streaming");
        case AL_UNDETERMINED:
            return enif_make_atom(env, "al_undetermined");
        default:
            break;
        }
        break;
    default:
        break;
    }

    return enif_make_int64(env, (ErlNifSInt64)val);
}


static ERL_NIF_TERM eopenal_alSourcePlay(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    alSourcePlay(*src_ptr);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSourcePlayv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *sources;
    ALuint *src_ptr;
    ERL_NIF_TERM head;
    ERL_NIF_TERM tail;
    ERL_NIF_TERM list;
    unsigned len;
    unsigned i;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_list_length(env, argv[0], &len)) return enif_make_badarg(env);
    sources = (ALuint *)enif_alloc(sizeof(ALuint) * (size_t)len);
    for(list = argv[0], i = 0; enif_get_list_cell(env, list, &head, &tail); i++, list = tail){
        if(!enif_get_resource(env, head, eopenal_source_type, (void **)&src_ptr)){
            enif_free(sources);
            return enif_make_badarg(env);
        }
        sources[i] = *src_ptr;
    }

    alSourcePlayv((ALsizei)len, sources);
    error = alGetError();
    if(error != AL_NO_ERROR){
        enif_free(sources);
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    enif_free(sources);
    return enif_make_atom(env, "ok");
}


static ERL_NIF_TERM eopenal_alSourcePause(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    alSourcePause(*src_ptr);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }
    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSourcePausev(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *sources;
    ALuint *src_ptr;
    ERL_NIF_TERM head;
    ERL_NIF_TERM tail;
    ERL_NIF_TERM list;
    unsigned len;
    unsigned i;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_list_length(env, argv[0], &len)) return enif_make_badarg(env);
    sources = (ALuint *)enif_alloc(sizeof(ALuint) * (size_t)len);
    for(list = argv[0], i = 0; enif_get_list_cell(env, list, &head, &tail); i++, list = tail){
        if(!enif_get_resource(env, head, eopenal_source_type, (void **)&src_ptr)){
            enif_free(sources);
            return enif_make_badarg(env);
        }
        sources[i] = *src_ptr;
    }

    alSourcePausev((ALsizei)len, sources);
    error = alGetError();
    if(error != AL_NO_ERROR){
        enif_free(sources);
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    enif_free(sources);
    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSourceStop(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    alSourceStop(*src_ptr);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }
    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSourceStopv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *sources;
    ALuint *src_ptr;
    ERL_NIF_TERM head;
    ERL_NIF_TERM tail;
    ERL_NIF_TERM list;
    unsigned len;
    unsigned i;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_list_length(env, argv[0], &len)) return enif_make_badarg(env);
    sources = (ALuint *)enif_alloc(sizeof(ALuint) * (size_t)len);
    for(list = argv[0], i = 0; enif_get_list_cell(env, list, &head, &tail); i++, list = tail){
        if(!enif_get_resource(env, head, eopenal_source_type, (void **)&src_ptr)){
            enif_free(sources);
            return enif_make_badarg(env);
        }
        sources[i] = *src_ptr;
    }

    alSourceStopv((ALsizei)len, sources);
    error = alGetError();
    if(error != AL_NO_ERROR){
        enif_free(sources);
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    enif_free(sources);
    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSourceRewind(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *src_ptr = NULL;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    alSourceRewind(*src_ptr);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }
    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSourceRewindv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *sources;
    ALuint *src_ptr;
    ERL_NIF_TERM head;
    ERL_NIF_TERM tail;
    ERL_NIF_TERM list;
    unsigned len;
    unsigned i;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_list_length(env, argv[0], &len)) return enif_make_badarg(env);
    sources = (ALuint *)enif_alloc(sizeof(ALuint) * (size_t)len);
    for(list = argv[0], i = 0; enif_get_list_cell(env, list, &head, &tail); i++, list = tail){
        if(!enif_get_resource(env, head, eopenal_source_type, (void **)&src_ptr)){
            enif_free(sources);
            return enif_make_badarg(env);
        }
        sources[i] = *src_ptr;
    }

    alSourceRewindv((ALsizei)len, sources);
    error = alGetError();
    if(error != AL_NO_ERROR){
        enif_free(sources);
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    enif_free(sources);
    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSourceQueueBuffers(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *buffers;
    ALuint *src_ptr = NULL;
    ALuint *buf_ptr = NULL;
    ALenum error;
    ERL_NIF_TERM head;
    ERL_NIF_TERM tail;
    ERL_NIF_TERM list;
    unsigned len;
    unsigned i;


    if(argc != 2) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    if(!enif_get_list_length(env, argv[1], &len)) return enif_make_badarg(env);
    buffers = (ALuint *)enif_alloc(sizeof(ALuint) * (size_t)len);

    for(list = argv[1], i = 0; enif_get_list_cell(env, list, &head, &tail); i++, list = tail){
        if(!enif_get_resource(env, head, eopenal_buffer_type, (void **)&buf_ptr)){
            enif_free(buffers);
        }
        buffers[i] = *buf_ptr;
    }

    alSourceQueueBuffers(*src_ptr, (ALsizei)len, buffers);
    error = alGetError();
    if(error != AL_NO_ERROR){
        enif_free(buffers);
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    enif_free(buffers);
    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alSourceUnqueueBuffers(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALuint *buffers;
    ALuint *src_ptr = NULL;
    ALuint *buf_ptr = NULL;
    ALenum error;
    ERL_NIF_TERM head;
    ERL_NIF_TERM tail;
    ERL_NIF_TERM list;
    unsigned len;
    unsigned i;


    if(argc != 2) return enif_make_badarg(env);
    if(!enif_get_resource(env, argv[0], eopenal_source_type, (void **)&src_ptr)){
        return enif_make_badarg(env);
    }

    if(!enif_get_list_length(env, argv[1], &len)) return enif_make_badarg(env);
    buffers = (ALuint *)enif_alloc(sizeof(ALuint) * (size_t)len);

    for(list = argv[1], i = 0; enif_get_list_cell(env, list, &head, &tail); i++, list = tail){
        if(!enif_get_resource(env, head, eopenal_buffer_type, (void **)&buf_ptr)){
            enif_free(buffers);
        }
        buffers[i] = *buf_ptr;
    }

    alSourceUnqueueBuffers(*src_ptr, (ALsizei)len, buffers);
    error = alGetError();
    if(error != AL_NO_ERROR){
        enif_free(buffers);
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    enif_free(buffers);
    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alListenerf(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    double dval;
    ALenum error;
    alenum_or_error_t tmp;

    if(argc != 2) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    if(!enif_get_double(env, argv[1], &dval)) return enif_make_badarg(env);

    alListenerf(pname, (ALfloat)dval);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alListener3f(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    double v[3];

    if(argc != 4) return enif_make_badarg(env);
    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    if(!enif_get_double(env, argv[1], v    )) return enif_make_badarg(env);
    if(!enif_get_double(env, argv[2], v + 1)) return enif_make_badarg(env);
    if(!enif_get_double(env, argv[3], v + 2)) return enif_make_badarg(env);

    alListener3f(pname, (ALfloat)v[0], (ALfloat)v[1], (ALfloat)v[2]);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alListenerfv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALfloat vector[6];
    double dvector[6];
    const ERL_NIF_TERM* tuple;
    int arity;
    ALenum error;

    if(argc != 2) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    if(!enif_get_tuple(env, argv[1], &arity, &tuple)) return enif_make_badarg(env);
    if(arity != 3 && arity != 6) return enif_make_badarg(env);

    switch(arity){
    case 3:
        if(!enif_get_double(env, tuple[0], &dvector[0])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[1], &dvector[1])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[2], &dvector[2])) return enif_make_badarg(env);
        vector[0] = (ALfloat)dvector[0];
        vector[1] = (ALfloat)dvector[1];
        vector[2] = (ALfloat)dvector[2];
        break;
    case 6:
        if(pname != AL_ORIENTATION) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[0], &dvector[0])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[1], &dvector[1])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[2], &dvector[2])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[3], &dvector[3])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[4], &dvector[4])) return enif_make_badarg(env);
        if(!enif_get_double(env, tuple[5], &dvector[5])) return enif_make_badarg(env);
        vector[0] = (ALfloat)dvector[0];
        vector[1] = (ALfloat)dvector[1];
        vector[2] = (ALfloat)dvector[2];
        vector[3] = (ALfloat)dvector[3];
        vector[4] = (ALfloat)dvector[4];
        vector[5] = (ALfloat)dvector[5];
        break;
    default:
        return enif_make_badarg(env);
    }

    alListenerfv(pname, vector);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alListeneri(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    ErlNifSInt64 i;

    if(argc != 2) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    if(!enif_get_int64(env, argv[1], &i)) return enif_make_badarg(env);

    alListeneri(pname, (ALint)i);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alGetListenerf(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALfloat val;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetListenerf(pname, &val);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_double(env, (double)val);
}

static ERL_NIF_TERM eopenal_alGetListener3f(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALfloat vector[3];
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetListener3f(pname, vector, vector + 1, vector + 2);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_tuple3(
        env,
        enif_make_double(env, (double)vector[0]),
        enif_make_double(env, (double)vector[1]),
        enif_make_double(env, (double)vector[2])
    );
}

static ERL_NIF_TERM eopenal_alGetListenerfv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALfloat vector[6];
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetListenerfv(pname, vector);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    if(pname == AL_ORIENTATION){
        return enif_make_tuple6(
            env,
            enif_make_double(env, (double)vector[0]),
            enif_make_double(env, (double)vector[1]),
            enif_make_double(env, (double)vector[2]),
            enif_make_double(env, (double)vector[3]),
            enif_make_double(env, (double)vector[4]),
            enif_make_double(env, (double)vector[5])
        );
    }

    return enif_make_tuple3(
        env,
        enif_make_double(env, (double)vector[0]),
        enif_make_double(env, (double)vector[1]),
        enif_make_double(env, (double)vector[2])
    );
}

static ERL_NIF_TERM eopenal_alGetListeneri(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALint val;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetListeneri(pname, &val);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_int64(env, (ErlNifSInt64)val);
}

static ERL_NIF_TERM eopenal_alEnable(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alEnable(pname);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alDisable(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alDisable(pname);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alIsEnabled(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    ALboolean result;

    if(argc != 1) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    result = alIsEnabled(pname);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    if(result == AL_TRUE) return enif_make_atom(env, "true");
    return enif_make_atom(env, "false");
}

static ERL_NIF_TERM eopenal_alGetBoolean(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    ALboolean result;

    if(argc != 1) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    result = alGetBoolean(pname);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    if(result == AL_TRUE) return enif_make_atom(env, "true");
    return enif_make_atom(env, "false");

}

static ERL_NIF_TERM eopenal_alGetDouble(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    ALdouble val;

    if(argc != 1) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    val = alGetDouble(pname);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_double(env, val);
}

static ERL_NIF_TERM eopenal_alGetFloat(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    ALfloat val;

    if(argc != 1) return enif_make_badarg(env);

    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    val = alGetFloat(pname);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_double(env, (double)val);

}

static ERL_NIF_TERM eopenal_alGetInteger(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    ALint val;

    if(argc != 1) return enif_make_badarg(env);
    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    val = alGetInteger(pname);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_int64(env, (ErlNifSInt64)val);
}

static ERL_NIF_TERM eopenal_alGetBooleanv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    ALboolean val;

    if(argc != 1) return enif_make_badarg(env);
    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetBooleanv(pname, &val);

    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_tuple1(
        env,
        ((val)? enif_make_atom(env, "true") : enif_make_atom(env, "false"))
    );
}

static ERL_NIF_TERM eopenal_alGetDoublev(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    ALdouble val;

    if(argc != 1) return enif_make_badarg(env);
    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetDoublev(pname, &val);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_tuple1(
        env,
        enif_make_double(env, (double)val)
    );
}

static ERL_NIF_TERM eopenal_alGetFloatv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    ALfloat val;

    if(argc != 1) return enif_make_badarg(env);
    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetFloatv(pname, &val);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_tuple1(
        env,
        enif_make_double(env, (double)val)
    );
}

static ERL_NIF_TERM eopenal_alGetIntegerv(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    ALint val;

    if(argc != 1) return enif_make_badarg(env);
    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alGetIntegerv(pname, &val);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_tuple1(
        env,
        enif_make_int64(env, (ErlNifSInt64)val)
    );
}

static ERL_NIF_TERM eopenal_alGetString(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;
    const ALchar *val;

    if(argc != 1) return enif_make_badarg(env);
    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    val = alGetString(pname);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_string(env, val, ERL_NIF_LATIN1);
}

static ERL_NIF_TERM eopenal_alDistanceModel(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALenum pname;
    alenum_or_error_t tmp;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    tmp = eopenal_atom_to_alenum(env, argv[0]);
    if(tmp.error) return enif_make_badarg(env);
    pname = tmp.v;

    alDistanceModel(pname);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alDopplerFactor(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    double dval;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_double(env, argv[0], &dval)){
        return enif_make_badarg(env);
    }

    alDopplerFactor((ALfloat)dval);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alDopplerVelocity(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    double dval;
    ALenum error;

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_double(env, argv[0], &dval)){
        return enif_make_badarg(env);
    }

    alDopplerVelocity((ALfloat)dval);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    return enif_make_atom(env, "ok");
}

static ERL_NIF_TERM eopenal_alIsExtensionPresent(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    ALboolean result;
    ALenum error;
    char buf[4096] = {0};

    if(argc != 1) return enif_make_badarg(env);
    if(!enif_get_string(env, argv[0], buf, (unsigned)sizeof(buf), ERL_NIF_LATIN1)){
        return enif_make_badarg(env);
    }

    result = alIsExtensionPresent((const ALchar *)buf);
    error = alGetError();
    if(error != AL_NO_ERROR){
        return mk_error(env, eopenal_error_to_atom(env, error));
    }

    if(result) return enif_make_atom(env, "true");
    return enif_make_atom(env, "false");
}

static void garbage_collect_eopenal_buffer(ErlNifEnv *env, void *resource)
{
    ALuint *buf;
    buf = (ALuint *)resource;
    if(buf == NULL) return;
    alDeleteBuffers(1, buf);
    (void)alGetError();
}


static void garbage_collect_eopenal_source(ErlNifEnv *env, void *resource)
{
    ALuint *src;
    src = (ALuint *)resource;
    if(src == NULL) return;
    alDeleteSources(1, src);
    (void)alGetError();
}

static void garbage_collect_eopenal_device(ErlNifEnv *env, void *resource)
{
    ALCdevice **dev;

    dev = (ALCdevice **)resource;
    if(dev == NULL || *dev == NULL) return;
    alcCloseDevice(*dev);
    *dev = NULL;
}

static void garbage_collect_eopenal_context(ErlNifEnv *env, void *resource)
{
    ALCcontext **ctx;

    ctx = (ALCcontext **)resource;
    if(ctx == NULL || *ctx == NULL) return;
    alcDestroyContext(*ctx);
    *ctx = NULL;
}

static int handle_load(ErlNifEnv *env, void **priv, ERL_NIF_TERM load_info)
{
    ErlNifResourceType *rt;
    ErlNifResourceFlags rf;

    rt = enif_open_resource_type(
        env,
        NULL,
        "eopenal_buffer_type",
        garbage_collect_eopenal_buffer,
        ERL_NIF_RT_CREATE,
        &rf
    );

    if(rt == NULL) return -1;
    eopenal_buffer_type = rt;

    rt = enif_open_resource_type(
        env,
        NULL,
        "eopenal_source_type",
        garbage_collect_eopenal_source,
        ERL_NIF_RT_CREATE,
        &rf
    );

    if(rt == NULL) return -1;
    eopenal_source_type = rt;

    rt = enif_open_resource_type(
        env,
        NULL,
        "eopenal_device_type",
        garbage_collect_eopenal_device,
        ERL_NIF_RT_CREATE,
        &rf
    );

    if(rt == NULL) return -1;
    eopenal_device_type = rt;

    rt = enif_open_resource_type(
        env,
        NULL,
        "eopenal_context_type",
        garbage_collect_eopenal_context,
        ERL_NIF_RT_CREATE,
        &rf
    );

    if(rt == NULL) return -1;
    eopenal_context_type = rt;

    return 0;
}

static int handle_upgrade(ErlNifEnv* env, void** priv_data, void** old_priv_data, ERL_NIF_TERM load_info)
{
    return 0;
}

static void handle_unload(ErlNifEnv* env, void* priv_data)
{
    return;
}

ERL_NIF_INIT(eopenal, nif_funcs, handle_load, NULL, handle_upgrade, handle_unload);

