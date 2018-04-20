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
#define PI X3DAUDIO_PI

#include "x3daudiovector.cpp"

#include "helper_funcs.cpp"


enum {
    CONFIG_MONO = 0,
    CONFIG_STEREO,
    CONFIG_2POINT1,
    CONFIG_SURROUND,
    CONFIG_QUAD,
    CONFIG_4POINT1,
    CONFIG_5POINT1,
    CONFIG_7POINT1,
    CONFIG_5POINT1_SURROUND,
    CONFIG_7POINT1_SURROUND,
};


// First work with mono
const struct ChannelConfig {
    UINT32 nChannels;
    UINT32 channelMask;
    const char* ChannelConfigName;
} kChannelConfigs[] = {
    {1, SPEAKER_MONO,             "SPEAKER_MONO            "},
    {2, SPEAKER_STEREO,           "SPEAKER_STEREO          "},
    {3, SPEAKER_2POINT1,          "SPEAKER_2POINT1         "},
    {4, SPEAKER_SURROUND,         "SPEAKER_SURROUND        "},
    {4, SPEAKER_QUAD,             "SPEAKER_QUAD            "},
    {5, SPEAKER_4POINT1,          "SPEAKER_4POINT1         "},
    {6, SPEAKER_5POINT1,          "SPEAKER_5POINT1         "},
    {8, SPEAKER_7POINT1,          "SPEAKER_7POINT1         "},
    {6, SPEAKER_5POINT1_SURROUND, "SPEAKER_5POINT1_SURROUND"},
    {8, SPEAKER_7POINT1_SURROUND, "SPEAKER_7POINT1_SURROUND"},
};


#include "dump.cpp"


const float kDefaultSpeedOfSound = 343.5f;



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
     // printf("%s, %d, ", kChannelConfigs[i].ChannelConfigName, (kChannelConfigs[i].channelMask & SPEAKER_LOW_FREQUENCY) ? 2 : 0);
     printf("%s,  ", kChannelConfigs[i].ChannelConfigName);
     dump_mask_fields(&inst);
    }

    nl();

    for (size_t i = 0; i < ARRAY_COUNT(kSpeeds); ++i) {
     DWORD dwChannelMask = SPEAKER_MONO;
     X3DAudioInitialize(dwChannelMask, kSpeeds[i], inst);
     DWORD dwSpeed = *(DWORD*)&kSpeeds[i];
     // printf("speed = %X, %g\n", dwSpeed, kSpeeds[i]);
     // dump_bytes(&inst, sizeof(inst));
     dump_speed_fields(&inst);
    }
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
    F3DAUDIO_LISTENER listener = create_DEFAULT_LISTENER_F();


    uint32_t nEmitterChannels = 1, nOutputChannels = 2;

    F3DAUDIO_EMITTER emitter = create_DEFAULT_EMITTER_F();
    emitter.Position = VecF(0.0f, 0.0f, 10.0f);

    // ugh
    X3DAUDIO_DSP_SETTINGS xsettings = create_DSP_SETTINGS(nEmitterChannels, nOutputChannels);
    F3DAUDIO_DSP_SETTINGS fsettings = create_DSP_SETTINGS_F(nEmitterChannels, nOutputChannels);

    uint32_t Flags = F3DAUDIO_CALCULATE_MATRIX;

    // non-NULL ptrs
    CHECK_FAIL(F3DAudioCheckCalculateParams(NULL, &listener, &emitter, Flags, &fsettings));
    CHECK_FAIL(F3DAudioCheckCalculateParams(finstance, NULL, &emitter, Flags, &fsettings));
    CHECK_FAIL(F3DAudioCheckCalculateParams(finstance, &listener, NULL, Flags, &fsettings));
    CHECK_FAIL(F3DAudioCheckCalculateParams(finstance, &listener, &emitter, Flags, NULL));
    float *old_matrix = fsettings.pMatrixCoefficients;
    fsettings.pMatrixCoefficients = NULL;
    CHECK_FAIL(F3DAudioCheckCalculateParams(finstance, &listener, &emitter, Flags, &fsettings));
    fsettings.pMatrixCoefficients = old_matrix;


    // TODO: F3DAUDIO_CALCULATE_ZEROCENTER

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
            float diff = fabs(e1 - e2);
            if (diff > tol) {
                return 0;
            }
        }
    }

    return 1;
}


#define MATRIX_TEST_TOLERANCE 1e-10
void test_mono_setup()
{
    X3DAUDIO_HANDLE xinstance;
    F3DAUDIO_HANDLE finstance;

    X3DAudioInitialize(SPEAKER_MONO, X3DAUDIO_SPEED_OF_SOUND, xinstance);
    F3DAudioInitialize(SPEAKER_MONO, X3DAUDIO_SPEED_OF_SOUND, finstance);


    UINT32 flags =  X3DAUDIO_CALCULATE_MATRIX;

    X3DAUDIO_LISTENER listener = create_DEFAULT_LISTENER();
    // dump_LISTENER(&listener);

    UINT32 nEmitterChannels = 1;
    X3DAUDIO_EMITTER emitter = create_DEFAULT_EMITTER();
    // dump_EMITTER(&emitter);


    X3DAUDIO_DSP_SETTINGS xsettings = create_DSP_SETTINGS(nEmitterChannels, 1);
    F3DAUDIO_DSP_SETTINGS fsettings = create_DSP_SETTINGS_F(nEmitterChannels, 1);

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

            if (!matrices_equal(xsettings.pMatrixCoefficients, fsettings.pMatrixCoefficients, xsettings.SrcChannelCount, xsettings.DstChannelCount, MATRIX_TEST_TOLERANCE)) {
                printf("Error in matrix comparison\n");
                __debugbreak();
                X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);
                F3DAudioCalculate(finstance, CAST(const F3DAUDIO_LISTENER*, &listener), CAST(const F3DAUDIO_EMITTER*, &emitter), flags, &fsettings);
            }
        }
    }

    // const float k
    // TODO: test cones. What tolerance to use :( ?


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

    F3DAudioInternalChecks();
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

    // const X3DAUDIO_VECTOR kVectorTranslations[] = {
    //     Vec(0.0, 0.0, 0.0),
    //     Vec(50.0, -3.0, 99.6),
    //     Vec(0.0, 10.0, 0.0),
    //     Vec(0.0, 0.0, 100.0),
    // };

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
    emitter.OrientFront = Vec(0.0f, sinf(3*PI/4), cosf(3*PI/4));
    emitter.OrientTop = zeroVec;
    emitter.ChannelCount = nEmitterChannels;
    float angle = 0.0f;
    emitter.Position = Vec(0.0f, 2.0f * sinf(angle), 2.0f * cosf(angle));
    // emitter.Position = VectorAdd(emitter.Position, kVectorTranslations[1]);
    emitter.Velocity = zeroVec;
    emitter.pCone = nullptr;
    emitter.CurveDistanceScaler = 1.0f;
    // dump_EMITTER(&emitter);

    X3DAUDIO_DSP_SETTINGS xsettings = create_DSP_SETTINGS(nEmitterChannels, nOutputChannels);
    F3DAUDIO_DSP_SETTINGS fsettings  = create_DSP_SETTINGS_F(nEmitterChannels, nOutputChannels);
    // memcpy(&fsettings, &xsettings, sizeof(fsettings));

    // TODO add that to test suite
    // X3DAudioCalculate(xinstance, &listener, &emitter, F3DAUDIO_CALCULATE_EMITTER_ANGLE, &xsettings);
    // dump_DSP_settings(&xsettings);

    // F3DAudioCalculate(finstance, CAST(const F3DAUDIO_LISTENER*, &listener), CAST(const F3DAUDIO_EMITTER*, &emitter), F3DAUDIO_CALCULATE_EMITTER_ANGLE, CAST(F3DAUDIO_DSP_SETTINGS *, &xsettings));
    // dump_DSP_settings(&xsettings);

    // float ar[] = {1.0f, 2.0f, 0.1f, 0.2f};
    // dump_values("values.txt", ar, 2, 2);

    float inner_half_angle = PI / 3.0f;
    float outer_half_angle = PI * 1.0 / 2.0f;

    X3DAUDIO_CONE cone = create_CONE(2 * inner_half_angle, 2 * outer_half_angle, 2.0f, 1.0f);
    listener.pCone = &cone;

    emitter.Position = Vec(0.0f, 0.0f, -1e-7f);
    X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);

    // dump_DSP_settings(&xsettings);

    int n_rows = 3;
    int n_samples = 101;
    float start_angle = 0.0f;
    float end_angle = PI / 1.0f;
    float* vals = new float[n_samples * n_rows];

    // dump_CONE(&cone);

    for (int i = 0; i < n_samples; ++i) {
        float alpha = (1.0f * i) / (n_samples - 1);
        angle = lerp(alpha, start_angle, end_angle);
        emitter.Position = Vec(0.0f, 2.0f * sinf(angle), 2.0f * cosf(angle));
        vals[i] = angle / (2.0f * PI);

        X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);
        vals[n_samples + i] = xsettings.pMatrixCoefficients[0];

        F3DAudioCalculate(finstance, CAST(const F3DAUDIO_LISTENER*, &listener), CAST(const F3DAUDIO_EMITTER*, &emitter), flags, &fsettings);
        vals[n_samples*2 + i] = fsettings.pMatrixCoefficients[0];

        // vals[n_samples*3 + i] = lerp(alpha, 1.0f, 0.0f);
        // fflush(stderr);
        // fflush(stdout);
        // if ((i % 10 == 0)) {
            // dump_MATRIX(xsettings.pMatrixCoefficients, xsettings.SrcChannelCount, xsettings.DstChannelCount);
            // dump_DSP_settings(&xsettings);
        // }
    }
    dump_values("../values.txt", vals, n_rows, n_samples);
    delete[](vals);

    listener = create_DEFAULT_LISTENER();
    emitter = create_DEFAULT_EMITTER();
    // emitter.Position = Vec(0.0f, 0.0f, 10.0f);
    float CDS = 10.0f;
    emitter.CurveDistanceScaler = CDS;
    // X3DAUDIO_DISTANCE_CURVE_POINT points[] = { {0.0f, 1.0f}, {1.0f, 0.1f} };
    X3DAUDIO_DISTANCE_CURVE_POINT points[] = { {0.0f, 1.0f}, {0.5, 0.9f}, {1.0f, 0.1f} };

    X3DAUDIO_DISTANCE_CURVE curve = {points, ARRAY_COUNT(points)};
    // emitter.pVolumeCurve = &curve;
    // emitter.CurveDistanceScaler = 2.0f;
    // X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);
    // dump_DSP_settings(&xsettings);

    const float kZDistances[] = { 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f, 4.0f, 4.5f, 5.0f}; //, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 100.0f}; //, 250.0f, 500.0f, 1000.0f };

    n_rows = 4;
    n_samples = ARRAY_COUNT(kZDistances);
    vals = new float[n_samples * n_rows];

    for (int i = 0; i < ARRAY_COUNT(kZDistances); ++i) {
        float cur_dist = kZDistances[i] * CDS;
        vals[i] = cur_dist;
        emitter.Position = Vec(0.0f, 0.0f, cur_dist);
        X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);
        vals[n_samples + i] = xsettings.pMatrixCoefficients[0];

        emitter.pVolumeCurve = &curve;
        X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);
        vals[n_samples*2 + i] = xsettings.pMatrixCoefficients[0];

        F3DAudioCalculate(finstance, CAST(const F3DAUDIO_LISTENER*, &listener), CAST(const F3DAUDIO_EMITTER*, &emitter), flags, &fsettings);
        vals[n_samples*3 + i] = fsettings.pMatrixCoefficients[0];

        // printf("%g ", fsettings.pMatrixCoefficients[0] - xsettings.pMatrixCoefficients[0]);

        emitter.pVolumeCurve = nullptr;
    }
    dump_values("../curve_vals.txt", vals, n_rows, n_samples);
    delete[](vals);



    cur_config = CONFIG_5POINT1;
    nOutputChannels = kChannelConfigs[cur_config].nChannels;
    channelMask = kChannelConfigs[cur_config].channelMask;

    X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, xinstance);
    // __debugbreak();
    F3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, finstance);


    // zero pos...
    listener = create_DEFAULT_LISTENER();
    emitter = create_DEFAULT_EMITTER();
    float smol_radius;
    smol_radius = 2e-7f;
    angle = PI / 8.0f;
    emitter.Position = Vec(smol_radius * sin(angle), 0.0f, smol_radius * cos(angle));
    // emitter.Position = Vec(0.0f, 0.0f, 1.0f);
    xsettings = create_DSP_SETTINGS(1, nOutputChannels);
    X3DAudioCalculate(xinstance, &listener, &emitter, F3DAUDIO_CALCULATE_EMITTER_ANGLE | F3DAUDIO_CALCULATE_MATRIX | F3DAUDIO_CALCULATE_DOPPLER, &xsettings);
    // dump_DSP_settings(&xsettings, cur_config);
    // TODO: check redirect to lfe, zero center...


    // ----

    listener = create_DEFAULT_LISTENER();
    emitter = create_DEFAULT_EMITTER();

    // listener.OrientTop = bX;

    // float kChannelAzimuths[] = { 0.0f, PI / 2.0f };
    // nEmitterChannels = ARRAY_COUNT(kChannelAzimuths);
    emitter.ChannelCount = nEmitterChannels;
    emitter.ChannelRadius = 1.0f;
    // emitter.pChannelAzimuths = kChannelAzimuths;
    emitter.OrientFront = Vec(0.0f, 0.0f, -1.0f);
    emitter.OrientTop = bY;

    xsettings = create_DSP_SETTINGS(nEmitterChannels, nOutputChannels);
    fsettings = create_DSP_SETTINGS_F(nEmitterChannels, nOutputChannels);


    float kStartAngles[] = { 0.0f, PI/4.0f };
    float kEndAngles[] = { PI/4.0f, 3.0f*PI/4.0f };
    int kIndexOfInterest[] = { 1, 5 };
    n_samples = 101;
    n_rows = ARRAY_COUNT(kStartAngles) + 1;
    vals = new float[n_samples * n_rows];

    for (int cur_row = 1; cur_row - 1 < ARRAY_COUNT(kStartAngles); ++cur_row) {
        start_angle = kStartAngles[cur_row - 1];
        end_angle = kEndAngles[cur_row - 1];
        for (int i = 0; i < n_samples; ++i) {
            float alpha = (1.0f * i) / (n_samples - 1);
            angle = lerp(alpha, start_angle, end_angle);
            vals[i] = alpha; //(angle - start_angle) / (end_angle - start_angle);
            emitter.Position = Vec(sinf(angle), 0.0f, cosf(angle));
            X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);
            float v0 = xsettings.pMatrixCoefficients[kIndexOfInterest[cur_row - 1]];
            vals[n_samples*cur_row + i] = v0;
            // X3DAUDIO_VECTOR x = VectorSub(emitter.Position, listener.Position);
            // X3DAUDIO_VECTOR p0 = Vec(sinf(start_angle), 0.0f, cosf(start_angle));
            // X3DAUDIO_VECTOR p1 = Vec(sinf(end_angle), 0.0f, cosf(end_angle));
            // float dot = VectorDot(x, p1);
            // vals[n_samples*2 + i] = acosf(dot) / (end_angle - start_angle);

            // vals[n_samples* 2 + i] = xsettings.pMatrixCoefficients[1];
        }
    }
    dump_values("../diffuse_vals2.txt", vals, n_rows, n_samples);
    delete[](vals);


    start_angle = 0.0f;
    end_angle = 3*PI / 4.0f;

    n_samples = 8;
    // flags = X3DAUDIO_CALCULATE_ZEROCENTER;
    for (int i = 0; i <= n_samples; ++i) {
        float alpha = 1.0f * i / n_samples;
        angle = lerp(alpha, start_angle, end_angle);
        emitter.Position = Vec(sinf(angle), 0.0f, cosf(angle));
        X3DAudioCalculate(xinstance, &listener, &emitter, flags, &xsettings);
        // dump_DSP_settings(&xsettings, cur_config);
        // printf("Angle: %d / %d PI\n", i, n_samples);
        // dump_MATRIX(xsettings.pMatrixCoefficients, xsettings.SrcChannelCount, xsettings.DstChannelCount, cur_config);
    }


    // TODO:
    // test matrix for null and non null distance

    // TODO: test that elevation doesn't change the diffusion by using a large CDS

    cur_config = CONFIG_5POINT1;
    nOutputChannels = kChannelConfigs[cur_config].nChannels;
    channelMask = kChannelConfigs[cur_config].channelMask;

    X3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, xinstance);
    F3DAudioInitialize(channelMask, X3DAUDIO_SPEED_OF_SOUND, finstance);

    // zero pos...
    listener = create_DEFAULT_LISTENER();
    emitter = create_DEFAULT_EMITTER();
    emitter.CurveDistanceScaler = 100.0f;
    float radius;
    radius = 1.0f;
    // angle = PI * 6.0f / 8.0f;
    for (int i = 0; i <= 16; ++i) {
        angle = PI * i / 8.0f;
        emitter.Position = Vec(radius * sin(angle), 0.0f, radius * cos(angle));
        // emitter.Position = Vec(0.0f, 0.0f, 1.0f);
        xsettings = create_DSP_SETTINGS(1, nOutputChannels);
        flags = F3DAUDIO_CALCULATE_EMITTER_ANGLE | F3DAUDIO_CALCULATE_MATRIX | F3DAUDIO_CALCULATE_DOPPLER;
        flags |= F3DAUDIO_CALCULATE_ZEROCENTER;
        F3DAudioCalculate(xinstance, CAST(const F3DAUDIO_LISTENER*, &listener), CAST(const F3DAUDIO_EMITTER*, &emitter), flags, &fsettings);
        dump_DSP_settings(CAST(X3DAUDIO_DSP_SETTINGS*, &fsettings), cur_config);
    }
}

