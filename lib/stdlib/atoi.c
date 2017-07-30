// A simple atoi() function
int atoi(char *str)
{
    int res = 0; // Initialize result
    int i;
    // Iterate through all characters of input string and
    // update result
    for (i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
  
    // return result.
    return res;
}
