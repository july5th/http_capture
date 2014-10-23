#ifndef OUTPUT_H
#define OUTPUT_H

#define int_ntoa(x)	inet_ntoa(*((struct in_addr *)&x))

int output(struct stream *s);
void output_init();

#endif
