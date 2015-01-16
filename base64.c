#include <openssl/evp.h>  
#include <openssl/bio.h>  
#include <openssl/buffer.h>  

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char * Base64Encode(const char * input, int length)  
{  
    BIO * bmem = NULL;  
    BIO * b64 = NULL;  
    BUF_MEM * bptr = NULL;  
  
    b64 = BIO_new(BIO_f_base64());  
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);  
    bmem = BIO_new(BIO_s_mem());  
    b64 = BIO_push(b64, bmem);  
    BIO_write(b64, input, length);  
    BIO_flush(b64); 
    BIO_get_mem_ptr(b64, &bptr);  
  
    char * buff = (char *)malloc(bptr->length + 1); 
    memcpy(buff, bptr->data, bptr->length); 
    buff[bptr->length] = 0;  
  
    BIO_free_all(b64);  
  
    return buff;  
}  
