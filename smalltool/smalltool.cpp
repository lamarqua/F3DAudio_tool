#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h> 
#include <XAudio2.h>
#include <X3Daudio.h>

#define _SPEAKER_COMBINATIONS_ 
#include "F3DAudio.h"

#include <stdio.h>
#include <stdint.h>

#include <assert.h>
#include <math.h>

#define CAST(T, x) ((T)(x))
#define ARRAY_COUNT(x) (sizeof(x)/sizeof((x)[0]))

#include "x3daudiovector.cpp"
#include "helper_funcs.cpp"
#include "dump.cpp"


X3DAUDIO_VECTOR bX = Vec(1.0f, 0.0f, 0.0f);
X3DAUDIO_VECTOR bY = Vec(0.0f, 1.0f, 0.0f);
X3DAUDIO_VECTOR bZ = Vec(0.0f, 0.0f, 1.0f);
X3DAUDIO_VECTOR zeroVec = Vec(0.0f, 0.0f, 0.0f);

F3DAUDIO_VECTOR bXF = VecF(1.0f, 0.0f, 0.0f);
F3DAUDIO_VECTOR bYF = VecF(0.0f, 1.0f, 0.0f);
F3DAUDIO_VECTOR bZF = VecF(0.0f, 0.0f, 1.0f);
F3DAUDIO_VECTOR zeroVecF = VecF(0.0f, 0.0f, 0.0f);

const float kDefaultSpeedOfSound = 343.5f;

// First work with mono 
const struct ChannelConfig {
    UINT32 nChannels;
    UINT32 channelMask;
    const char* ChannelConfigName;
} kChannelConfigs[] = {
    /* CONFIG #0 */ {1, SPEAKER_MONO,            
                        "SPEAKER_MONO            "},
    /* CONFIG #1 */ {2, SPEAKER_STEREO,          
                        "SPEAKER_STEREO          "},
    /* CONFIG #2 */ {3, SPEAKER_2POINT1,         
                        "SPEAKER_2POINT1         "},
    /* CONFIG #3 */ {4, SPEAKER_QUAD,            
                        "SPEAKER_QUAD            "},
    /* CONFIG #4 */ {4, SPEAKER_SURROUND,        
                        "SPEAKER_SURROUND        "},
    /* CONFIG #5 */ {5, SPEAKER_4POINT1,         
                        "SPEAKER_4POINT1         "},
    /* CONFIG #6 */ {6, SPEAKER_5POINT1,         
                        "SPEAKER_5POINT1         "},
    /* CONFIG #7 */ {6, SPEAKER_5POINT1_SURROUND,
                        "SPEAKER_5POINT1_SURROUND"},
    /* CONFIG #8 */ {8, SPEAKER_7POINT1,
                        "SPEAKER_7POINT1         "},
    /* CONFIG #9 */ {8, SPEAKER_7POINT1_SURROUND,
                        "SPEAKER_7POINT1_SURROUND"},
};


void re_instance_data()
{
    X3DAUDIO_HANDLE inst;

    const float kSpeeds[] = {FLT_MIN, X3DAUDIO_SPEED_OF_SOUND, 300.0f, 1.0f, 1e10f};

    // printf("%X\n\n", SPEAKER_5POINT1 & SPEAKER_5POINT1_SURROUND & SPEAKER_7POINT1_SURROUND & SPEAKER_7POINT1 & ~SPEAKER_2POINT1 & ~SPEAKER_4POINT1);

    for (size_t i = 0; i < ARRAY_COUNT(kChannelConfigs); ++i) {
     DWORD dwChannelMask = kChannelConfigs[i].channelMask;
     X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, inst);
     // printf("%08X\n", dwChannelMask);
     // dump_bytes(&inst, sizeof(inst));
     printf("%s, %d, ", kChannelConfigs[i].ChannelConfigName,
        (kChannelConfigs[i].channelMask & SPEAKER_LOW_FREQUENCY) ? 2 : 0);
     dump_mask_fields(&inst);
    }

    nl();

    // for (size_t i = 0; i < ARRAY_COUNT(kSpeeds); ++i) {
    //  DWORD dwChannelMask = SPEAKER_MONO;
    //  X3DAudioInitialize(dwChannelMask, kSpeeds[i], inst);
    //  DWORD dwSpeed = *(DWORD*)&kSpeeds[i];
    //  // printf("speed = %X, %g\n", dwSpeed, kSpeeds[i]);
    //  // dump_bytes(&inst, sizeof(inst));
    //  dump_speed_fields(&inst);
    // }
}

#define EXPECT_SE(expr) do {\
        int found_se = 0; \
        __try { \
            expr; \
        } \
        __except(GetExceptionCode() == STATUS_BREAKPOINT ? \
            EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { \
            found_se = 1;\
        } \
        if (!found_se) { \
            OutputDebugString("Expected structured exception\n"); \
            __debugbreak(); \
        } \
    } while(0)

#define QUICK_ASSERT(x) do { \
    if (!(x)) { __debugbreak(); } \
    } while (0)

#define CHECK_OK(x) QUICK_ASSERT((x) == PARAM_CHECK_OK)
#define CHECK_FAIL(x) QUICK_ASSERT((x) == PARAM_CHECK_FAIL)

void test_checks()
{
    F3DAUDIO_HANDLE finstance;
    
    // non-NULL instance
    CHECK_FAIL(F3DAudioCheckInitParams(SPEAKER_MONO, kDefaultSpeedOfSound, NULL));

    // Test the SpeedOfSound >= FLT_MIN check
    CHECK_FAIL(F3DAudioCheckInitParams(SPEAKER_MONO, 0.0, finstance));

    // Check unauthorized speaker configuration
    CHECK_FAIL(F3DAudioCheckInitParams(SPEAKER_ALL, kDefaultSpeedOfSound, finstance));

    F3DAudioInitialize(SPEAKER_STEREO, kDefaultSpeedOfSound, finstance);

    // boilerplate initialization
    F3DAUDIO_LISTENER listener = {};
    listener.OrientFront = bZF;
    listener.OrientTop = bYF;
    listener.Position = zeroVecF;
    listener.Velocity = zeroVecF;
    listener.pCone = nullptr;

    F3DAUDIO_EMITTER emitter = {};
    emitter.OrientFront = bZF;
    emitter.OrientTop = zeroVecF;
    emitter.ChannelCount = 1;
    emitter.Position = VecF(0.0f, 0.0f, 10.0f);
    emitter.Velocity = zeroVecF;
    emitter.CurveDistanceScaler = 1.0f;

    uint32_t nEmitterChannels = 1, nOutputChannels = 2;

    // ugh
    X3DAUDIO_DSP_SETTINGS xsettings = create_DSP_SETTINGS(nEmitterChannels, nOutputChannels);
    F3DAUDIO_DSP_SETTINGS fsettings;
    memcpy(&fsettings, &xsettings, sizeof(fsettings));

    uint32_t Flags = F3DAUDIO_CALCULATE_MATRIX;

    // non-NULL ptrs
    CHECK_FAIL(F3DAudioCheckCalculateParams(NULL, &listener, &emitter, Flags, &fsettings));
    CHECK_FAIL(F3DAudioCheckCalculateParams(finstance, NULL, &emitter, Flags, &fsettings));
    CHECK_FAIL(F3DAudioCheckCalculateParams(finstance, &listener, NULL, Flags, &fsettings));
    CHECK_FAIL(F3DAudioCheckCalculateParams(finstance, &listener, &emitter, Flags, NULL));


#define CHECK_FAIL_USUAL_PARAMS() CHECK_FAIL(F3DAudioCheckCalculateParams(finstance, &listener, &emitter, Flags, &fsettings))
#define CHECK_OK_USUAL_PARAMS() CHECK_OK(F3DAudioCheckCalculateParams(finstance, &listener, &emitter, Flags, &fsettings))

    // listener fields
    listener.OrientFront = zeroVecF;
    CHECK_FAIL_USUAL_PARAMS();
    listener.OrientFront = bZF;
    listener.OrientTop = zeroVecF;
    CHECK_FAIL_USUAL_PARAMS();
    listener.OrientTop = bYF;
    // TODO: check listener cone

    // emitter fields
    emitter.OrientFront = zeroVecF;
    CHECK_OK_USUAL_PARAMS();
    Flags |= F3DAUDIO_CALCULATE_EMITTER_ANGLE;
    CHECK_FAIL_USUAL_PARAMS();
    emitter.OrientFront = bZF;
    // TODO: OrientTop for multi-channel emitter case
    // TODO: emitter cone


    // CHECK_FAIL_USUAL_PARAMS();
    // emitter.OrientFront = bZF;
    // emitter.OrientTop = zeroVecF;
    // CHECK_FAIL_USUAL_PARAMS();
    // emitter.OrientTop = bYF;

    CHECK_OK(F3DAudioCheckCalculateParams(finstance, &listener, &emitter, Flags, &fsettings));
}


static int matrices_equal(float* m1, float* m2, size_t n_rows, size_t n_cols, float tol) {
    for (size_t row = 0; row < n_rows; ++row) {
        for (size_t col = 0; col < n_cols; ++col) {
            float e1 = m1[row*n_cols + col];
            float e2 = m2[row*n_cols + col];
            if (fabs(e1 - e2) > tol) {
                return 0;
            }
        }
    } 

    return 1;
}

void test_mono_setup()
{
    X3DAUDIO_HANDLE xinstance;
    F3DAUDIO_HANDLE finstance;
 
    X3DAudioInitialize(SPEAKER_MONO, X3DAUDIO_SPEED_OF_SOUND, xinstance);
    F3DAudioInitialize(SPEAKER_MONO, X3DAUDIO_SPEED_OF_SOUND, finstance);

    
    UINT32 flags =  X3DAUDIO_CALCULATE_MATRIX;

    X3DAUDIO_LISTENER listener = {};
    listener.OrientFront = bZ;
    listener.OrientTop = bY;
    listener.Position = zeroVec;
    listener.Velocity = zeroVec;
    listener.pCone = nullptr;
    // dump_LISTENER(&listener);

    UINT32 nEmitterChannels = 1;
    X3DAUDIO_EMITTER emitter = {};
    emitter.OrientFront = bZ;
    emitter.OrientTop = zeroVec;
    emitter.ChannelCount = nEmitterChannels;
    emitter.Position = Vec(0.0f, 0.0f, 10.0f);
    emitter.Velocity = zeroVec;
    emitter.CurveDistanceScaler = 1.0f;
    // dump_EMITTER(&emitter);


    X3DAUDIO_DSP_SETTINGS xsettings = create_DSP_SETTINGS(nEmitterChannels, 1);
    F3DAUDIO_DSP_SETTINGS fsettings;
    memcpy(&fsettings, &xsettings, sizeof(fsettings));

    const float kZDistances[] = { 0.0f, 0.1f, 0.5f, 1.0f, 2.0f, 3.0f, 10.0f, 100.0f, 1000.0f };
    const float kDistanceCurveScales[] = { 1.0f, 10.0f, 0.1f };

    for (int icds = 0; icds < ARRAY_COUNT(kDistanceCurveScales); ++icds) {
        emitter.CurveDistanceScaler = kDistanceCurveScales[icds];
        for (int izd = 0; izd < ARRAY_COUNT(kZDistances); ++izd) {
            // printf("dist = %f\n", kZDistances[izd]);
            emitter.Position = Vec(0.0, 0.0, kZDistances[izd]);
            // printf("n_dist = %f\n", Distance(emitter.Position, listener.Position) / emitter.CurveDistanceScaler);

            X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);
            // dump_MATRIX(fsettings.pMatrixCoefficients, fsettings.SrcChannelCount, fsettings.DstChannelCount);

            F3DAudioCalculate(finstance, CAST(const F3DAUDIO_LISTENER*, &listener), CAST(const F3DAUDIO_EMITTER*, &emitter), flags, &fsettings);
            // dump_MATRIX(fsettings.pMatrixCoefficients, fsettings.SrcChannelCount, fsettings.DstChannelCount);

            if (!matrices_equal(xsettings.pMatrixCoefficients, fsettings.pMatrixCoefficients, xsettings.SrcChannelCount, xsettings.DstChannelCount, 0.0f)) {
                printf("Error in matrix comparison\n");
                __debugbreak();
            }
        }
    }
}

void dump_values(const char* filename, float* values, size_t n_rows, size_t n_cols) {
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        __debugbreak();
        return;
    }
    for (size_t row = 0; row < n_rows; ++row) {
        for (size_t col = 0; col < n_cols; ++col) {
            fprintf(fp, "%f ", values[row*n_cols + col]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

float lerp(float alpha, float x, float y) {
    return (1 - alpha) * x + alpha * y;
}

int main(int argc, char* argv[])
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        return -1;
    }
    HMODULE XAudioDLL = LoadLibraryExW(L"XAudioD2_7.DLL", nullptr, 0x00000800 /* LOAD_LIBRARY_SEARCH_SYSTEM32 */);
    UINT32 XAudio2flags = XAUDIO2_DEBUG_ENGINE;

    IXAudio2* pXAudio2 = nullptr;
    hr = XAudio2Create(&pXAudio2, XAudio2flags);
    if (FAILED(hr)) {
        return -2;
    }

    X3DAUDIO_HANDLE xinstance;
    F3DAUDIO_HANDLE finstance;
 
    // re_instance_data();

    size_t cur_config = 0;
    UINT32 nOutputChannels = kChannelConfigs[cur_config].nChannels;
    UINT32 channelMask = kChannelConfigs[cur_config].channelMask;

    X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, xinstance);
    // __debugbreak();
    F3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, finstance);

    test_checks();
    test_mono_setup();

    UINT32 flags =  X3DAUDIO_CALCULATE_MATRIX
        // | X3DAUDIO_CALCULATE_DELAY
        // | X3DAUDIO_CALCULATE_LPF_DIRECT
        // | X3DAUDIO_CALCULATE_LPF_REVERB
        // | X3DAUDIO_CALCULATE_REVERB
        // | X3DAUDIO_CALCULATE_DOPPLER
        // | X3DAUDIO_CALCULATE_EMITTER_ANGLE
        ;

    const X3DAUDIO_VECTOR kVectorTranslations[] = {
        Vec(0.0, 0.0, 0.0),
        Vec(50.0, -3.0, 99.6),
        Vec(0.0, 10.0, 0.0),
        Vec(0.0, 0.0, 100.0),
    };

    X3DAUDIO_LISTENER listener = {};
    listener.OrientFront = bZ;
    listener.OrientTop = bY;
    listener.Position = zeroVec;
    // listener.Position = VectorAdd(listener.Position, kVectorTranslations[1]);
    listener.Velocity = zeroVec;
    listener.pCone = nullptr;
    // dump_LISTENER(&listener);

    UINT32 nEmitterChannels = 1;
    X3DAUDIO_EMITTER emitter = {};
    emitter.OrientFront = zeroVec;
    emitter.OrientTop = zeroVec;
    emitter.ChannelCount = nEmitterChannels;
    const float PI = X3DAUDIO_PI;
    float angle = 0.0f;
    emitter.Position = Vec(0.0f, sinf(angle), cosf(angle));
    // emitter.Position = VectorAdd(emitter.Position, kVectorTranslations[1]);
    emitter.Velocity = zeroVec;
    emitter.pCone = nullptr;
    emitter.CurveDistanceScaler = 1.0f;
    // dump_EMITTER(&emitter);

    X3DAUDIO_DSP_SETTINGS xsettings = create_DSP_SETTINGS(nEmitterChannels, nOutputChannels);
    // dump_DSP_settings(&xsettings);

    // X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);
    // dump_DSP_settings(&xsettings);

    // float ar[] = {1.0f, 2.0f, 0.1f, 0.2f};
    // dump_values("values.txt", ar, 2, 2);


    X3DAUDIO_CONE cone = create_CONE(PI / 1.0f, PI / 0.5f, 1.0, 0.0);
    listener.pCone = &cone;

    int n_samples = 11;
    float start_angle = PI / 2.0f;
    float end_angle = PI / 1.0f;
    float* vals = new float[n_samples * 3];

    dump_CONE(&cone);
    for (int i = 0; i < n_samples; ++i) {
        float alpha = (1.0f * i) / (n_samples - 1);
        angle = lerp(alpha, start_angle, end_angle);

        emitter.Position = Vec(0.0f, sinf(angle), cosf(angle));
        X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);

        vals[i] = angle / (2.0f * PI);
        vals[n_samples + i] = xsettings.pMatrixCoefficients[0];
        vals[n_samples*2 + i] = lerp(alpha, 1.0f, 0.0f);
        dump_values("../values.txt", vals, 3, n_samples);
        // fflush(stderr);
        // fflush(stdout);
        // if ((i % 10 == 0)) {
            // dump_MATRIX(xsettings.pMatrixCoefficients, xsettings.SrcChannelCount, xsettings.DstChannelCount);
            // dump_DSP_settings(&xsettings);    
        // }
    }
}

