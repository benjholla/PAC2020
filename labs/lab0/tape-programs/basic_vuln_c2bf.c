// Compilable with: https://github.com/arthaud/c2bf
// Example: $ c2bf -o basic_vuln_c2bf.bf basic_vuln_c2bf.c
//
// NOTE: c2bf has been compiled and installed on lab VM already with a symbolic link "c2bf"
//       You can run it by running "c2bf --help"

// note simulator is slow to execute this, especially for large inputs
// executable with: https://www.jdoodle.com/execute-brainfuck-online/ (which uses an optimizing brainfuck compiler https://github.com/Wilfred/bfc)
// alternatively try https://www.nayuki.io/page/optimizing-brainfuck-compiler

/*
 * A representative implementation of the
 * C standard library's strcpy function
 *
 * Copies null-terminated memory blocks
 * from src to dst
 *
 * An input of 100 A's causes this program to print ?
 * AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
 */
void strcpy(int[] dst, int[] src) {
    int i = 0;
    while(src[i] != 0) {
        dst[i] = src[i];
        i = i + 1;
    }
    dst[i] = 0; // add trailing null
}

/*
 * A simple print function to print a null
 * terminated string to stdout
 */
void print(int[] s) {
    int i = 0;
    while(s[i] != 0) {
        write_char(s[i]);
        i = i + 1;
    }
}

/*
 * A simple read function to get a string from stdin
 * Reads from stdin until a newline or null terminator
 *
 * Note: Since compiler does not support returning an array,
 * the array to hold the result is passed as a parameter.
 */
void getline(int[] line) {
    int i = 0;
    int c = read_char();
    while(c != 0 && c != '\n') {
        line[i] = c;
        i = i + 1;
        c = read_char();
    }
    line[i] = 0; // add trailing null
}

int buf[64];
int input[256];
getline(input); // vulnerable to inputs longer than 255 characters
strcpy(buf, input); // vulnerable to inputs longer than 63 characters
print(buf);
