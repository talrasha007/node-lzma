#include <7zTypes.h>

class BufferInStream : public IByteIn {
public:
    BufferInStream(const char *in, size_t len) : _in(in), _cur(in), _end(in + len) {
        Read = read;
    }

public:
    bool eof() { return _cur >= _end; }
    bool overflow() { return _cur > _end; }
    size_t read() { return _cur - _in; }

private:
    static Byte read(void *p) {
        BufferInStream *pthis = (BufferInStream*)p;
        return pthis->_cur < pthis->_end ? Byte(*pthis->_cur++) : (++pthis->_cur, 0);
    }

private:
    const char *_in, *_cur, *_end;
};

class BufferOutStream : public IByteOut {
public:
    BufferOutStream(char *out, size_t len) : _out(out), _cur(out), _end(out + len) {
        Write= write;
    }

public:
    size_t written() { return _cur - _out; }

private:
    static void write(void *p, Byte b) {
        BufferOutStream *pthis = (BufferOutStream*)p;
        if (pthis->_cur < pthis->_end) {
            *pthis->_cur++ = char(b);
        }
    }

private:
    char *_out, *_cur, *_end;
};
