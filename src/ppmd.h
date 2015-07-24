#pragma once

class PpmdTr {
public:
    static void setup(v8::Handle<v8::Object>& exports);

    static size_t getUnpackSize(const char *in, size_t len);
    static int decompress(char *out, size_t *outLen, const char *in, size_t *inLen);
    static int compress(char *out, size_t *outLen, const char *in, size_t inLen, int order, int dicmemMB);
};