// A simple atoi() function
int atoi(char *str)
{
    int res = 0; // Initialize result
    int i;
    char* p = str;
    // Iterate through all characters of input string and
    // update result
    while(*p){
        char c = *p;
        if(c >= '0' && c <= '9'){
            res = res*10 + c - '0';
        }else{
            return 0;
        }
        p++;
    }
  
    // return result.
    return res;
}
