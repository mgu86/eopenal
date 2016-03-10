% ┌────────────────────────────────────────────────────────────────────────┐
% │        _______                    _____________                        │
% │   _______  __ \______________________    |__  /                        │
% │   _  _ \  / / /__  __ \  _ \_  __ \_  /| |_  /                         │
% │   /  __/ /_/ /__  /_/ /  __/  / / /  ___ |  /___                       │
% │   \___/\____/ _  .___/\___//_/ /_//_/  |_/_____/                       │
% │               /_/                                                      │
% │                                                                        │
% │ eOpenAL - OpenAL Soft NIF for Erlang                                   │
% │ Copyright (C) 2016  Eric des Courtis                                   │
% │                                                                        │
% │ This library is free software; you can redistribute it and/or          │
% │ modify it under the terms of the GNU Lesser General Public             │
% │ License as published by the Free Software Foundation; either           │
% │ version 2.1 of the License, or (at your option) any later version.     │
% │                                                                        │
% │ This library is distributed in the hope that it will be useful,        │
% │ but WITHOUT ANY WARRANTY; without even the implied warranty of         │
% │ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      │
% │ Lesser General Public License for more details.                        │
% │                                                                        │
% │ You should have received a copy of the GNU Lesser General Public       │
% │ License along with this library; if not, write to the Free Software    │
% │ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,             │
% │ MA  02110-1301  USA                                                    │
% └────────────────────────────────────────────────────────────────────────┘

-module(eopenal).

-on_load(load_nif/0).

-export([
    alcCreateContext/1,
    alcCreateContext/2,
    alcGetIntegerv/3,
    alcGetString/1,
    alcGetString/2,
    alcIsExtensionPresent/1,
    alcIsExtensionPresent/2,
    alcMakeContextCurrent/1,
    alcOpenDevice/0,
    alcOpenDevice/1,
    alcProcessContext/1,
    alcSuspendContext/1,
    alBufferData/4,
    alDisable/1,
    alDistanceModel/1,
    alDopplerFactor/1,
    alDopplerVelocity/1,
    alEnable/1,
    alGenBuffers/1,
    alGenSources/1,
    alGetBoolean/1,
    alGetBooleanv/1,
    alGetBufferf/2,
    alGetBufferi/2,
    alGetDouble/1,
    alGetDoublev/1,
    alGetFloat/1,
    alGetFloatv/1,
    alGetInteger/1,
    alGetIntegerv/1,
    alGetListener3f/1,
    alGetListenerf/1,
    alGetListenerfv/1,
    alGetListeneri/1,
    alGetSourcef/2,
    alGetSourcefv/2,
    alGetSourcei/2,
    alGetString/1,
    alIsBuffer/1,
    alIsEnabled/1,
    alIsExtensionPresent/1,
    alIsSource/1,
    alListener3f/4,
    alListenerf/2,
    alListenerfv/2,
    alListeneri/2,
    alSource3f/5,
    alSourcef/3,
    alSourcefv/3,
    alSourcei/3,
    alSourcePause/1,
    alSourcePausev/1,
    alSourcePlay/1,
    alSourcePlayv/1,
    alSourceQueueBuffers/2,
    alSourceRewind/1,
    alSourceRewindv/1,
    alSourceStop/1,
    alSourceStopv/1,
    alSourceUnqueueBuffers/2
]).

-opaque eopenal_device()  :: binary().
-opaque eopenal_buffer()  :: binary().
-opaque eopenal_source()  :: binary().
-opaque eopenal_context() :: binary().
-export_type([
    eopenal_device/0,
    eopenal_buffer/0,
    eopenal_source/0,
    eopenal_context/0
]).

%% -------------------------------------------------------------------------
%% API
%% -------------------------------------------------------------------------
alGenBuffers(N) when is_integer(N), N >= 0 ->
    erlang:nif_error(not_loaded).

alcOpenDevice() ->
    erlang:nif_error(not_loaded).

alcOpenDevice(DevStr) when is_list(DevStr) ->
    erlang:nif_error(not_loaded).

alcGetString(_Param) ->
    erlang:nif_error(not_loaded).

alcGetString(_Device, _Param) ->
    erlang:nif_error(not_loaded).

alcIsExtensionPresent(Param) when is_list(Param) ->
    erlang:nif_error(not_loaded).

alcIsExtensionPresent(_Device, Param) when is_list(Param) ->
    erlang:nif_error(not_loaded).

alcCreateContext(_Device) ->
    erlang:nif_error(not_loaded).

alcCreateContext(_Device, Params) when is_list(Params) ->
    erlang:nif_error(not_loaded).

alcMakeContextCurrent(_Context) ->
    erlang:nif_error(not_loaded).

alListener3f(Param, X, Y, Z) when is_atom(Param), is_float(X), is_float(Y), is_float(Z) ->
    erlang:nif_error(not_loaded).

alListenerfv(Param, _Vector) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGenSources(N) when is_integer(N), N >= 0 ->
    erlang:nif_error(not_loaded).

alSourcef(_Source, Param, Val) when is_atom(Param), is_float(Val) ->
    erlang:nif_error(not_loaded).

alSource3f(_Source, Param, X, Y, Z) when is_atom(Param), is_float(X), is_float(Y), is_float(Z) ->
    erlang:nif_error(not_loaded).

alSourcei(_Source, Param, Val) when is_atom(Param), is_integer(Val) ->
    erlang:nif_error(not_loaded).

alIsBuffer(_Buffer) ->
    erlang:nif_error(not_loaded).

alBufferData(_Buffer, Format, Data, Frequency)
    when is_atom(Format), is_binary(Data), is_integer(Frequency) ->
    erlang:nif_error(not_loaded).

alGetBufferf(_Buffer, Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetBufferi(_Buffer, Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alIsSource(_Source) ->
    erlang:nif_error(not_loaded).

alSourcefv(_Source, Param, Values) when is_atom(Param), is_list(Values) ->
    erlang:nif_error(not_loaded).

alGetSourcef(_Source, Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetSourcefv(_Source, Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetSourcei(_Source, Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alSourcePlay(_Source) ->
    erlang:nif_error(not_loaded).

alSourcePlayv(Sources) when is_list(Sources) ->
    erlang:nif_error(not_loaded).

alSourcePause(_Source) ->
    erlang:nif_error(not_loaded).

alSourcePausev(Sources) when is_list(Sources) ->
    erlang:nif_error(not_loaded).

alSourceStop(_Source) ->
    erlang:nif_error(not_loaded).

alSourceStopv(Sources) when is_list(Sources) ->
    erlang:nif_error(not_loaded).

alSourceRewind(_Source) ->
    erlang:nif_error(not_loaded).

alSourceRewindv(Sources) when is_list(Sources) ->
    erlang:nif_error(not_loaded).

alSourceQueueBuffers(_Source, Buffers) when is_list(Buffers) ->
    erlang:nif_error(not_loaded).

alSourceUnqueueBuffers(_Source, Buffers) when is_list(Buffers) ->
    erlang:nif_error(not_loaded).

alListenerf(Param, Value) when is_atom(Param), is_float(Value) ->
    erlang:nif_error(not_loaded).

alListeneri(Param, Value) when is_atom(Param), is_integer(Value) ->
    erlang:nif_error(not_loaded).

alGetListenerf(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetListener3f(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetListenerfv(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetListeneri(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alEnable(Capability) when is_atom(Capability) ->
    erlang:nif_error(not_loaded).

alDisable(Capability) when is_atom(Capability) ->
    erlang:nif_error(not_loaded).

alIsEnabled(Capability) when is_atom(Capability) ->
    erlang:nif_error(not_loaded).

alGetBoolean(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetDouble(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetFloat(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetInteger(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetBooleanv(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetDoublev(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetFloatv(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetIntegerv(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alGetString(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alDistanceModel(Param) when is_atom(Param) ->
    erlang:nif_error(not_loaded).

alDopplerFactor(Value) when is_float(Value) ->
    erlang:nif_error(not_loaded).

alDopplerVelocity(Value) when is_float(Value) ->
    erlang:nif_error(not_loaded).

alIsExtensionPresent(Ext) when is_list(Ext) ->
    erlang:nif_error(not_loaded).

alcProcessContext(_Context) ->
    erlang:nif_error(not_loaded).

alcSuspendContext(_Context) ->
    erlang:nif_error(not_loaded).

alcGetIntegerv(_Device, Param, Size) when is_atom(Param), is_integer(Size), Size > 0 ->
    erlang:nif_error(not_loaded).

%% -------------------------------------------------------------------------
%% on_load callback
%% -------------------------------------------------------------------------

load_nif() ->
    erlang:load_nif(filename:join(code:priv_dir(eopenal), "eopenal"), 0).
