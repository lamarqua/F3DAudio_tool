void nl() {
    puts("");
}

void dump_bytes(void *bytes, size_t n_bytes)
{
    for (size_t i = 0; i < n_bytes; ++i) {
        printf("%02X ", ((uint8_t*) bytes)[i]);
    }
    nl();
}

void dump_bytes_reverse(void *bytes, size_t n_bytes)
{
    for (size_t i = n_bytes; i > 0; --i) {
        printf("%02X ", ((uint8_t*) bytes)[i-1]);
    }
    nl();
}

void dump_mask_fields(X3DAUDIO_HANDLE* instance) {
    struct handle_internal {
        DWORD ChannelMask;
        DWORD CM1;
        DWORD CM2;
        float SpeedOfSound;
        float SOS1;
    } internal;
    assert(sizeof(internal) == sizeof(*instance));

    memcpy(&internal, instance, sizeof(internal));

    printf("mask = %08X, CM1 = %08X, CM2 = %08X\n",
        internal.ChannelMask, internal.CM1, internal.CM2);
}

void dump_speed_fields(X3DAUDIO_HANDLE* instance) {
    struct handle_internal {
        DWORD ChannelMask;
        DWORD CM1;
        DWORD CM2;
        float SpeedOfSound;
        float SOS1;
    } internal;
    assert(sizeof(internal) == sizeof(*instance));

    memcpy(&internal, instance, sizeof(internal));

    printf("SpeedOfSound = %f, SOS1 = %f\n",
        internal.SpeedOfSound, internal.SOS1);
    dump_bytes_reverse(&internal.SpeedOfSound, 4);
    dump_bytes_reverse(&internal.SOS1, 4);
}

void dump_VECTOR(X3DAUDIO_VECTOR vector) {
    printf("{ %f\t%f\t%f }", vector.x, vector.y, vector.z);
}

void dump_CONE(X3DAUDIO_CONE* cone) {
    printf("Dumping cone:\n");
    printf("InnerAngle: %f PI\n", cone->InnerAngle / X3DAUDIO_PI);
    printf("OuterAngle: %f PI\n", cone->OuterAngle / X3DAUDIO_PI);

    printf("InnerVolume: %f\n", cone->InnerVolume);
    printf("OuterVolume: %f\n", cone->OuterVolume);
    printf("InnerLPF: %f\n", cone->InnerLPF);
    printf("OuterLPF: %f\n", cone->OuterLPF);
    printf("InnerReverb: %f\n", cone->InnerReverb);
    printf("OuterReverb: %f\n", cone->OuterReverb);

//    FLOAT32 InnerAngle; // inner cone angle in radians, must be within [0.0f, X3DAUDIO_2PI]
//    FLOAT32 OuterAngle; // outer cone angle in radians, must be within [InnerAngle, X3DAUDIO_2PI]
//
//    FLOAT32 InnerVolume; // volume level scaler on/within inner cone, used only for matrix calculations, must be within [0.0f, 2.0f] when used
//    FLOAT32 OuterVolume; // volume level scaler on/beyond outer cone, used only for matrix calculations, must be within [0.0f, 2.0f] when used
//    FLOAT32 InnerLPF;    // LPF (both direct and reverb paths) coefficient subtrahend on/within inner cone, used only for LPF (both direct and reverb paths) calculations, must be within [0.0f, 1.0f] when used
//    FLOAT32 OuterLPF;    // LPF (both direct and reverb paths) coefficient subtrahend on/beyond outer cone, used only for LPF (both direct and reverb paths) calculations, must be within [0.0f, 1.0f] when used
//    FLOAT32 InnerReverb; // reverb send level scaler on/within inner cone, used only for reverb calculations, must be within [0.0f, 2.0f] when used
//    FLOAT32 OuterReverb; // reverb send level scaler on/beyond outer cone,
}

void dump_CURVE(X3DAUDIO_DISTANCE_CURVE* curve) {
    printf("[ ");
    for (UINT32 i = 0; i < curve->PointCount; ++i) {
        printf("%f\t", curve->pPoints[i].Distance);
    }
    for (UINT32 i = 0; i < curve->PointCount; ++i) {
        printf("%f\t", curve->pPoints[i].DSPSetting);
    }
    printf("]\n");
// X3DAUDIO_DISTANCE_CURVE_POINT* pPoints;    // distance curve point array, must have at least PointCount elements with no duplicates and be sorted in ascending order with respect to Distance
// UINT32                         PointCount; // number of distance curve points, must be >= 2 as all distance curves must have at least two endpoints, defining DSP settings at 0.0f and 1.0f normalized distance
}

const char* kChannelsMono[] = { "C " };
const char* kChannelsStereo[] = { "L ", "R " };
const char* kChannels2Point1[] = { "L ", "R ", "LF" };
const char* kChannelsSurround[] = { "L ", "R ", "C ", "BC" };
const char* kChannelsQuad[] = { "L ", "R ", "BL", "BR" };
const char* kChannels4Point1[] = { "L ", "R ", "LF", "BL", "BR" };
const char* kChannels5Point1[] = { "L ", "R ", "C ", "LF", "BL", "BR" };
const char* kChannels7Point1[] = { "L ", "R ", "C ", "LF", "BL", "BR", "LC", "RC" };
const char* kChannels5Point1Surround[] = { "L ", "R ", "C ", "LF", "SL" , "SR" };
const char* kChannels7Point1Surround[] = { "L ", "R ", "C ", "LF", "BL", "BR", "SL" , "SR" };

const char** kChannelSpeakerDescs[] = {
    kChannelsMono,
    kChannelsStereo,
    kChannels2Point1,
    kChannelsSurround,
    kChannelsQuad,
    kChannels4Point1,
    kChannels5Point1,
    kChannels7Point1,
    kChannels5Point1Surround,
    kChannels7Point1Surround,
};

void dump_MATRIX(FLOAT32* matrix, UINT32 SrcChannelCount, UINT32 DstChannelCount, uint32_t ChannelConfig = -1) {

    // for (UINT32 i = 0; i < SrcChannelCount; ++i) {
    //     printf("[ ");
    //     for (UINT32 j = 0; j < DstChannelCount; ++j) {
    //         printf("%f ", matrix[SrcChannelCount * j + i]);
    //     }
    //     printf("]\n");
    // }

    for (UINT32 row = 0; row < DstChannelCount; ++row) {
        if (ChannelConfig != -1) {
            printf("%s ", kChannelSpeakerDescs[ChannelConfig][row]);
        }
        printf("[ ");
        for (UINT32 col = 0; col < SrcChannelCount; ++col) {
            printf("%f\t", matrix[SrcChannelCount * row + col]);
        }
        printf("]\n");
    }
}

void dump_DSP_settings(X3DAUDIO_DSP_SETTINGS* settings, uint32_t ChannelConfig = -1) {
//    FLOAT32* pMatrixCoefficients; // [inout] matrix coefficient table, receives an array representing the volume level used to send from source channel S to destination channel D, stored as pMatrixCoefficients[SrcChannelCount * D + S], must have at least SrcChannelCount*DstChannelCount elements
//    FLOAT32* pDelayTimes;         // [inout] delay time array, receives delays for each destination channel in milliseconds, must have at least DstChannelCount elements (stereo final mix only)
//    UINT32 SrcChannelCount;       // [in] number of source channels, must equal number of channels in respective emitter
//    UINT32 DstChannelCount;       // [in] number of destination channels, must equal number of channels of the final mix
//
//    FLOAT32 LPFDirectCoefficient; // [out] LPF direct-path coefficient
//    FLOAT32 LPFReverbCoefficient; // [out] LPF reverb-path coefficient
//    FLOAT32 ReverbLevel; // [out] reverb send level
//    FLOAT32 DopplerFactor; // [out] doppler shift factor, scales resampler ratio for doppler shift effect, where the effective frequency = DopplerFactor * original frequency
//    FLOAT32 EmitterToListenerAngle; // [out] emitter-to-listener interior angle, expressed in radians with respect to the emitter's front orientation
//
//    FLOAT32 EmitterToListenerDistance; // [out] distance in user-defined world units from the emitter base to listener position, always calculated
//    FLOAT32 EmitterVelocityComponent; // [out] component of emitter velocity vector projected onto emitter->listener vector in user-defined world units/second, calculated only for doppler
//    FLOAT32 ListenerVelocityComponent; // [out] component of listener velocity vector projected onto emitter->listener vector in user-defined world units/second, calculated only for doppler
    printf("\tDumping X3DAUDIO_DSP_SETTINGS\n");
    UINT32 SrcChannelCount = settings->SrcChannelCount;
    UINT32 DstChannelCount = settings->DstChannelCount;

    printf("SrcChannelCount: %d\nDstChannelCount: %d\n", SrcChannelCount, DstChannelCount);

    if (settings->pMatrixCoefficients) {
        printf("Matrix coefficients:\n");

        if (ChannelConfig != -1) {
            printf("%s\n", kChannelConfigs[ChannelConfig].ChannelConfigName);
        }
        dump_MATRIX(settings->pMatrixCoefficients, SrcChannelCount, DstChannelCount, ChannelConfig);
    } else {
        printf("Matrix coefficients: (NULL)\n");
    }

    if (settings->pDelayTimes) {
        printf("Delay times: [ ");
        for (UINT32 i = 0; i < DstChannelCount; ++i) {
            printf("%f ", settings->pDelayTimes[i]);
        }
    } else {
        printf("Delay times: (NULL)\n");
    }

    printf("LPFDirectCoefficient: %f\n", settings->LPFDirectCoefficient);
    printf("LPFReverbCoefficient: %f\n", settings->LPFReverbCoefficient);
    printf("ReverbLevel: %f\n", settings->ReverbLevel);
    printf("DopplerFactor: %f\n", settings->DopplerFactor);
    printf("EmitterToListenerAngle: %f PI\n", settings->EmitterToListenerAngle / X3DAUDIO_PI);
    printf("EmitterToListenerDistance: %f\n", settings->EmitterToListenerDistance);
    printf("EmitterVelocityComponent: %f\n", settings->EmitterVelocityComponent);
    printf("ListenerVelocityComponent: %f\n", settings->ListenerVelocityComponent);
    nl();
}

void dump_LISTENER(X3DAUDIO_LISTENER* listener) {
    printf("\tDumping listener\n");
    printf("Front: "); dump_VECTOR(listener->OrientFront); nl();
    printf("Top:   "); dump_VECTOR(listener->OrientTop); nl();
    printf("Pos:   "); dump_VECTOR(listener->Position); nl();
    printf("Vel:   "); dump_VECTOR(listener->Velocity); nl();

    dump_CONE(listener->pCone);
//    X3DAUDIO_VECTOR OrientFront; // orientation of front direction, used only for matrix and delay calculations or listeners with cones for matrix, LPF (both direct and reverb paths), and reverb calculations, must be normalized when used
//    X3DAUDIO_VECTOR OrientTop;   // orientation of top direction, used only for matrix and delay calculations, must be orthonormal with OrientFront when used
//
//    X3DAUDIO_VECTOR Position; // position in user-defined world units, does not affect Velocity
//    X3DAUDIO_VECTOR Velocity; // velocity vector in user-defined world units/second, used only for doppler calculations, does not affect Position
//
//    X3DAUDIO_CONE* pCone; // sound cone, used only for matrix, LPF (both direct and reverb paths), and reverb calculations, NULL specifies omnidirectionality
}

void dump_EMITTER(X3DAUDIO_EMITTER* emitter) {
    printf("\tDumping emitter\n");

    printf("ChannelCount: %d\n", emitter->ChannelCount);
    printf("ChannelRadius: %f\n", emitter->ChannelRadius);
    if (emitter->pChannelAzimuths) {
        printf("ChannelAzimuths: [");
        for (UINT32 i = 0; i < emitter->ChannelCount; ++i) {
            printf("%f ", emitter->pChannelAzimuths[i]);
        }
        printf("]\n");
    }

    dump_CONE(emitter->pCone);

    printf("Front: "); dump_VECTOR(emitter->OrientFront); nl();
    printf("Top:   "); dump_VECTOR(emitter->OrientTop); nl();
    printf("Pos:   "); dump_VECTOR(emitter->Position); nl();
    printf("Vel:   "); dump_VECTOR(emitter->Velocity); nl();

    printf("InnerRadius: %f\n", emitter->InnerRadius);
    printf("InnerRadiusAngle: %f\n", emitter->InnerRadiusAngle);

    printf("VolumeCurve: "); dump_CURVE(emitter->pVolumeCurve); nl();
    printf("LFECurve: "); dump_CURVE(emitter->pLFECurve); nl();
    printf("LPFDirectCurve: "); dump_CURVE(emitter->pLPFDirectCurve); nl();
    printf("LPFReverbCurve: "); dump_CURVE(emitter->pLPFReverbCurve); nl();
    printf("ReverbCurve: "); dump_CURVE(emitter->pReverbCurve); nl();

    printf("CurveDistanceScaler: %f\n", emitter->CurveDistanceScaler);
    printf("DopplerScaler: %f\n", emitter->DopplerScaler);

//    X3DAUDIO_CONE* pCone; // sound cone, used only with single-channel emitters for matrix, LPF (both direct and reverb paths), and reverb calculations, NULL specifies omnidirectionality
//    X3DAUDIO_VECTOR OrientFront; // orientation of front direction, used only for emitter angle calculations or with multi-channel emitters for matrix calculations or single-channel emitters with cones for matrix, LPF (both direct and reverb paths), and reverb calculations, must be normalized when used
//    X3DAUDIO_VECTOR OrientTop;   // orientation of top direction, used only with multi-channel emitters for matrix calculations, must be orthonormal with OrientFront when used
//
//    X3DAUDIO_VECTOR Position; // position in user-defined world units, does not affect Velocity
//    X3DAUDIO_VECTOR Velocity; // velocity vector in user-defined world units/second, used only for doppler calculations, does not affect Position
//
//    FLOAT32 InnerRadius;      // inner radius, must be within [0.0f, FLT_MAX]
//    FLOAT32 InnerRadiusAngle; // inner radius angle, must be within [0.0f, X3DAUDIO_PI/4.0)
//
//    UINT32 ChannelCount;       // number of sound channels, must be > 0
//    FLOAT32 ChannelRadius;     // channel radius, used only with multi-channel emitters for matrix calculations, must be >= 0.0f when used
//    FLOAT32* pChannelAzimuths; // channel azimuth array, used only with multi-channel emitters for matrix calculations, contains positions of each channel expressed in radians along the channel radius with respect to the front orientation vector in the plane orthogonal to the top orientation vector, or X3DAUDIO_2PI to specify an LFE channel, must have at least ChannelCount elements, all within [0.0f, X3DAUDIO_2PI] when used
//
//    X3DAUDIO_DISTANCE_CURVE* pVolumeCurve;    // volume level distance curve, used only for matrix calculations, NULL specifies a default curve that conforms to the inverse square law, calculated in user-defined world units with distances <= CurveDistanceScaler clamped to no attenuation
//    X3DAUDIO_DISTANCE_CURVE* pLFECurve;       // LFE level distance curve, used only for matrix calculations, NULL specifies a default curve that conforms to the inverse square law, calculated in user-defined world units with distances <= CurveDistanceScaler clamped to no attenuation
//    X3DAUDIO_DISTANCE_CURVE* pLPFDirectCurve; // LPF direct-path coefficient distance curve, used only for LPF direct-path calculations, NULL specifies the default curve: [0.0f,1.0f], [1.0f,0.75f]
//    X3DAUDIO_DISTANCE_CURVE* pLPFReverbCurve; // LPF reverb-path coefficient distance curve, used only for LPF reverb-path calculations, NULL specifies the default curve: [0.0f,0.75f], [1.0f,0.75f]
//    X3DAUDIO_DISTANCE_CURVE* pReverbCurve;    // reverb send level distance curve, used only for reverb calculations, NULL specifies the default curve: [0.0f,1.0f], [1.0f,0.0f]
//
//    FLOAT32 CurveDistanceScaler; // curve distance scaler, used to scale normalized distance curves to user-defined world units and/or exaggerate their effect, used only for matrix, LPF (both direct and reverb paths), and reverb calculations, must be within [FLT_MIN, FLT_MAX] when used
//    FLOAT32 DopplerScaler;       // doppler shift scaler, used to exaggerate doppler shift effect, used only for doppler calculations, must be within [0.0f, FLT_MAX] when used
}
