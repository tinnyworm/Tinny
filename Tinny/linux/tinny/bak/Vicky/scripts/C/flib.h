#ifndef FLIB_H
#define FLIB_H

extern int      xatoi(const char *);
extern int      xatoip(const char *, const char *);
extern int      xatoin(const char *, int);

extern long     xfatoln(const char *, int);
extern long     xfatolp(const char *, const char *);
extern long     xatoln(const char *, int);
extern long     xatolp(const char *, const char *);
extern long     xatolzip5p(const char *, const char *);
extern long     xatolzip5n(const char *);

extern double   xatof(const char *);
extern double   xatofp(const char *, const char *);
extern double   xatofn(const char *, int);

extern float    checkSum(const float* buffer, int numEntries);

#endif
