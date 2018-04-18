
X3DAUDIO_DSP_SETTINGS create_DSP_SETTINGS(UINT32 nEmitterSrcChannels, UINT32 nDstChannels) {
    X3DAUDIO_DSP_SETTINGS result = {};
    result.pMatrixCoefficients = new FLOAT32[nEmitterSrcChannels * nDstChannels];
    memset(result.pMatrixCoefficients, 0, nEmitterSrcChannels * nDstChannels * sizeof(FLOAT32));
    result.SrcChannelCount = nEmitterSrcChannels;
    result.DstChannelCount = nDstChannels;

    // for pDelayTimes, must DstChannelCount == 2??

    return result;
}

F3DAUDIO_DSP_SETTINGS create_DSP_SETTINGS_F(uint32_t nEmitterSrcChannels, uint32_t nDstChannels) {
    F3DAUDIO_DSP_SETTINGS result = {};
    result.pMatrixCoefficients = new float[nEmitterSrcChannels * nDstChannels];
    memset(result.pMatrixCoefficients, 0, nEmitterSrcChannels * nDstChannels * sizeof(float));
    result.SrcChannelCount = nEmitterSrcChannels;
    result.DstChannelCount = nDstChannels;

    // for pDelayTimes, must DstChannelCount == 2??

    return result;
}

X3DAUDIO_CONE create_CONE(FLOAT32 InnerAngle, FLOAT32 OuterAngle, FLOAT32 InnerVolume, FLOAT32 OuterVolume) {
    X3DAUDIO_CONE result = {};
    result.InnerAngle = InnerAngle;
    result.OuterAngle = OuterAngle;
    result.InnerVolume = InnerVolume;
    result.OuterVolume = OuterVolume;
    // TODO LPF and Reverb stuff

    return result;
}

X3DAUDIO_LISTENER create_DEFAULT_LISTENER() {
    X3DAUDIO_LISTENER listener = {};
    listener.OrientFront = bZ;
    listener.OrientTop = bY;
    listener.Position = zeroVec;
    listener.Velocity = zeroVec;
    listener.pCone = nullptr;

    return listener;
}

X3DAUDIO_EMITTER create_DEFAULT_EMITTER() {
    X3DAUDIO_EMITTER emitter = {};
    emitter.OrientFront = bZ;
    emitter.OrientTop = zeroVec;
    emitter.ChannelCount = 1;
    emitter.Position = zeroVec;
    emitter.Velocity = zeroVec;
    emitter.CurveDistanceScaler = 1.0f;

    return emitter;
}

F3DAUDIO_LISTENER create_DEFAULT_LISTENER_F() {
    F3DAUDIO_LISTENER listener = {};
    listener.OrientFront = bZF;
    listener.OrientTop = bYF;
    listener.Position = zeroVecF;
    listener.Velocity = zeroVecF;
    listener.pCone = nullptr;

    return listener;
}

F3DAUDIO_EMITTER create_DEFAULT_EMITTER_F() {
    F3DAUDIO_EMITTER emitter = {};
    emitter.OrientFront = bZF;
    emitter.OrientTop = zeroVecF;
    emitter.ChannelCount = 1;
    emitter.Position = zeroVecF;
    emitter.Velocity = zeroVecF;
    emitter.CurveDistanceScaler = 1.0f;

    return emitter;
}