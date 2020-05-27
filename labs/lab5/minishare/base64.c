static const char base64lut[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   
void base64(unsigned char *_dst,unsigned char *_src,int len) {
	unsigned char *dst=_dst,*src=_src;
	
	while (len>=3) {
		(*dst++)=base64lut[src[0] >> 2];
		(*dst++)=base64lut[((src[0] << 4) & 0x30) | (src[1] >> 4)];
		(*dst++)=base64lut[((src[1] << 2) & 0x3c) | (src[2] >> 6)];
		(*dst++)=base64lut[src[2] & 0x3f];
		src+=3;
		len -= 3;
    }
    if (len > 0) {
		(*dst++)=base64lut[src[0]>>2];
		unsigned char f=(src[0] << 4) & 0x30;
		if (len>1) f|=src[1] >> 4;
		(*dst++)=base64lut[f];
		(*dst++)=(len<2)?'=':base64lut[(src[1]<<2)&0x3c];
		(*dst++)='=';
    }
    *dst='\0';
}
