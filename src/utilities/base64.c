#include "base64.h"

#include "log.h"

// Based on :
// https://gist.github.com/barrysteyn/7308212

int base64_encode(const char* buffer, size_t length, char** b64text) {
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	*b64text=(*bufferPtr).data;

	return 1;
}

int base64_decode(char* b64message, char** buffer, int* length) {
	BIO *bio, *b64;

	int decodeLen = calc_base64_length(b64message);
	*buffer = (unsigned char*)malloc(decodeLen + 1);
	(*buffer)[decodeLen] = '\0';

	bio = BIO_new_mem_buf(b64message, -1);
	b64 = BIO_new(BIO_f_base64());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	size_t b64_len = strlen(b64message);
	*length = BIO_read(bio, *buffer, b64_len);
	if(*length != decodeLen) {
		free(*buffer);
		log_error("Base64 decoding error");
		return -1;
	}
	BIO_free_all(bio);

	return 1;
}

/**
 * Get the base64 plaintext len with a base64 string
 *
 * @return plaintext size
 */
int calc_base64_length(const char* b64input) {
	size_t len = strlen(b64input);
	int padding = 0;

	if (b64input[len-1] == '=' && b64input[len-2] == '=') 
		padding = 2;
	else if (b64input[len-1] == '=')
		padding = 1;

	int int_len = (int) len;
	return (int_len*3)/4 - padding;
}
