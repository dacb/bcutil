#ifndef _ERROR_H
#define _ERROR_H

void warning(const char *fmt, ...);
void fatal_error(const char *fmt, ...);
void die(int how);

#endif /* _ERROR_H */
