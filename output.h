#ifndef OUTPUT_H
#define OUTPUT_H

#define int_ntoa(x)	inet_ntoa(*((struct in_addr *)&x))

void output(struct stream *s);

#endif
