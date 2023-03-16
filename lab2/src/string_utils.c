/*
 * This is the same as `strcmp` in standard library
 */
int strcmp(const char *s1, const char *s2)
{
        while (*s1 == *s2++) {
                if (*s1++ == 0) return 0;
        }
        return (*(unsigned char *)s1 - *(unsigned char *)--s2);
}

int string_hex_to_int(char *str, int len)
{
        int out = 0;
        for (int i = 0 ; i < len; i++) {
                out <<= 4;
                if (str[i] >= '0' && str[i] <= '9') {
                        out += str[i] - '0';
                } else {
                        switch (str[i]) {
                        case 'A':
                                out += 10;
                                break;
                        case 'B':
                                out += 11;
                                break;
                        case 'C':
                                out += 12;
                                break;
                        case 'D':
                                out += 13;
                                break;
                        case 'E':
                                out += 14;
                                break;
                        default:
                                out += 15;
                                break;
                        }
                }
        }
        return out;
}

unsigned int strlen(const char *str)
{
	const char *s;
	for (s = str; *s; ++s);
	return (s - str);
}

unsigned int big_bytes_to_uint(char* ptr, int len)
{
        unsigned int ans = 0;
        for (int i = 0; i < len; i++) {
                ans <<= 8;
                ans += (int)ptr[i];
        }
        return ans;
}