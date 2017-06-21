
//note hexstr must use capital letters
int hexstr2int(char *a, int len)
{
    int i;
    int val = 0;

    for(i=0;i<len;i++){
			if(a[i] <= 57)
			 val += (a[i]-48)*(1<<(4*(len-1-i)));
			else
			 val += (a[i]-55)*(1<<(4*(len-1-i)));
		}

    return val;
}

char hexstr2char(char A){
	return (char)(A > '9')? (A &~ 0x20) - 'A' + 10: (A - '0');
}

//bytenr number of bytes in n
void int2hexstr(char *buffer,int n, int bytenr) {
	int i;
	for(i = bytenr * 4 - 4; i >= 0; i -= 4) {
		int d = (n >> i) & 0xf;
		if(d < 10) {
			*buffer++ = d + '0';
		}
		else {
			*buffer++ = d - 10 + 'A';
		}
	}
}

