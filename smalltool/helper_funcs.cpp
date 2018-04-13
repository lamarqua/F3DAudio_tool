
X3DAUDIO_DSP_SETTINGS create_DSP_SETTINGS(UINT32 nEmitterSrcChannels, UINT32 nDstChannels) {
    X3DAUDIO_DSP_SETTINGS result = {};
    result.pMatrixCoefficients = new FLOAT32[nEmitterSrcChannels * nDstChannels];
    memset(result.pMatrixCoefficients, 0, nEmitterSrcChannels * nDstChannels * sizeof(FLOAT32));
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