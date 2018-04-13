static X3DAUDIO_VECTOR Vec(float x, float y, float z) {
    return X3DAUDIO_VECTOR{x, y, z};
}

static X3DAUDIO_VECTOR VectorAdd(X3DAUDIO_VECTOR a, X3DAUDIO_VECTOR b) {
    return Vec(a.x + b.x, a.y + b.y, a.z + b.z);
}

static X3DAUDIO_VECTOR VectorSub(X3DAUDIO_VECTOR a, X3DAUDIO_VECTOR b) {
    return Vec(a.x - b.x, a.y - b.y, a.z - b.z);
}

static float VectorSquareLength(X3DAUDIO_VECTOR v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

static float VectorLength(X3DAUDIO_VECTOR v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

static float Distance(X3DAUDIO_VECTOR a, X3DAUDIO_VECTOR b) {
    return VectorLength(VectorSub(a, b));
}



static F3DAUDIO_VECTOR VecF(float x, float y, float z) {
    return F3DAUDIO_VECTOR{x, y, z};
}

static F3DAUDIO_VECTOR VectorAddF(F3DAUDIO_VECTOR a, F3DAUDIO_VECTOR b) {
    return VecF(a.x + b.x, a.y + b.y, a.z + b.z);
}

static F3DAUDIO_VECTOR VectorSubF(F3DAUDIO_VECTOR a, F3DAUDIO_VECTOR b) {
    return VecF(a.x - b.x, a.y - b.y, a.z - b.z);
}

static float VectorSquareLengthF(F3DAUDIO_VECTOR v) {
    return v.x*v.x + v.y*v.y + v.z*v.z;
}

static float VectorLengthF(F3DAUDIO_VECTOR v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

static float FDistanceF(F3DAUDIO_VECTOR a, F3DAUDIO_VECTOR b) {
    return VectorLengthF(VectorSubF(a, b));
}
