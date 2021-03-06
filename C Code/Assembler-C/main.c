#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
/*

IMPORTANT HINT: The C Code may differ partially from the assembler implementation.
Those changes affect only specific parts of the code, e.g. the conerting from ASCII numbers into
floating point format.
*/

// C specific declaration of functions
void start_time();
int stop_time();
void seed(int n);
uint32_t rand_1();
float frand();
float generate_list_item(int min_value, int max_value);
void generate_list(int n, int min, int max, float a[]);
void merge(float a[],int lo, int mid, int hi, float aux[]);
void recursive_merge(float a[], int lo, int hi, float aux[]);
void print_array(float data[], int n, FILE* fileDescriptor);
void fsort(float data[], unsigned int n);
void writeFile(float a[], int n, FILE* fileDescriptor );
float* readInputFromFile(FILE* fileDescriptor);

time_t start;
time_t end;
int itemnumber; // number of items to be sorted
void start_time()
{
    start = time(NULL);
}
// return time since beginning of program execution register $9 in MIPS
int stop_time()
{
    end = time(NULL) - start;
    return end;
}

// gloabal variables - constants
uint32_t const_max_value = 2147483647; // 2^31 -1
int const_a = 1103515245; // init a value for 32 bit CPU
uint32_t const_m = 2147483648; // 2^31,

uint32_t x;

// random generator functions
void seed(int n) { // generate random x
    int execution_time = stop_time() * 3600; //seconds are to small
    int random_addr[1]; // some random address allocation
    uint32_t random_number = (uintptr_t) random_addr * execution_time;
    x = random_number / n;
}


// conflicting with rand of stdlib
uint32_t rand_1(){

    uint32_t y = 181218000;
    uint32_t z = 260644315;
    uint32_t c = 38271601;

    // Calculate linear congruential generator
    x = 69069 * x + 12345;

    // Xor Shift
    y ^= y << 13;
    y ^= y >> 17;
    y ^= y << 5;

    // multiply with carry - custom

    /* The Assembler implementation might differ in that place
       to simplify things e.g. no 64 bit calculation
    */

    uint64_t t;
    t = 698769069 * z + c;
    c = t >> 32;
    z = t;
    x = x + y +z;

    x = x % const_m;
    return x;
}

// return float value 0 > x < 1
float frand(){
    unsigned int current_rand = rand_1();
    float random_number = (float) current_rand / (float) const_max_value;
    return random_number;
}

float generate_list_item(int min_value, int max_value){
    float random_value = frand();
    float t = min_value + (random_value * ( max_value - min_value));
    //printf("List item = %f\n",t);
    return t;
}

void generate_list(int n, int min, int max, float a[]){
    // This can be implemented with heap pointers incrementation in assembler and by decrementing n while n != 0
    int t = 0;
    while(n >= 0){
        a[t] = generate_list_item( min, max );
        t++;
        n--;
    }
}

// function that merges two sub lists
void merge(float a[],int lo, int mid, int hi, float aux[])
{
    int i = lo;
    int j = mid + 1;
    int k;
    // copy the string to the aux at the exactly same positions
    for(k = lo; k <= hi; k++)
    {
        aux[k] = a[k];
    }

    // merge everything back to the original array a
    for(k = lo; k <= hi; k++)
    {
        if(i > mid)  // the first half is already merged in aux, but the second not yet
        {
            a[k] = aux[j];
            j++;
        }
        else  // the first half isn't empty
        {
            if(j > hi)  // the second half has been already merged
            {
                a[k]=aux[i]; // add the rest of the first half to the array
                i++;
            }
            else
            {
                if(aux[j] < aux[i])  // compare the values currently under consideration
                {
                    a[k]= aux[j];
                    j++;
                }
                else
                {
                    a[k]=aux[i];
                    i++;
                }
            }
        }
    }
}

void recursive_merge(float a[], int lo, int hi, float aux[])
{
    int mid;
    if (hi > lo)
    {
        mid = (lo + hi)/2; // divide the list into two halfs
        recursive_merge(a,lo,mid,aux); // sort the left half recursively
        recursive_merge(a,mid+1,hi,aux); // sort the right half recursively
        merge(a,lo,mid,hi,aux); // merge both halves to one sorted half
    }
}

void print_array(float data[], int n, FILE* fileOutputDescriptor){
    /*
    * The function writes the output  into a file
    * the assembler output does not format in the right way
    * since it uses big endian.
    *
    */
    int temp;
    for(temp = 0; temp < n; temp++ ){
        printf("%f", data[temp] );
        printf("\n");
        fprintf(fileOutputDescriptor, "%f\n", data[temp]);
        fprintf(fileOutputDescriptor, "\n", data[temp]);
    }
}

void fsort(float data[], unsigned int n){
    //Sorts n floating point numbers stored in memory starting from *data
    float *aux =  malloc(n * sizeof(float)); // allocate memory on the heap
    recursive_merge(data,0,n-1,aux);
    free(aux);
}


/*
 * This functions reads the floating point numbers in HEX from file.
 */
float* readInputFromFile(FILE* fileDescriptor){
    float a[12500]; // fixed value for reading input
    // ASCII Codes
    int ascii_a = 65; // ASCII A
    int ascii_f = 70; // ASCII F
    int zero =  48; // ASCII 0
    int nine = 57; // ASCII 9

    // define union for byte to float conversion
    union float_bytes {
        float val;
        int integer;
    } input;

    // read loop
    unsigned char ascii_buffer;
    input.integer = 0;
    int j = 0;
    while(ascii_buffer != 46) // ascii value .
    {
        fread(&ascii_buffer, sizeof(unsigned char), 1, fileDescriptor);
        if(ascii_buffer != 44) // ASCII ,
            {
            if( (ascii_buffer >= zero) && (ascii_buffer <= nine)){
                input.integer = input.integer << 4;
                input.integer  = input.integer + ascii_buffer - zero;
            }
            if( (ascii_buffer >= ascii_a) && (ascii_buffer <= ascii_f) ) {
                input.integer = input.integer << 4;
                input.integer = input.integer + ascii_buffer - (ascii_a - 10);
            }
        }
        if( ascii_buffer == 44) // ascii value ,
            // read next number
        {
            a[j] = input.val;
            input.integer = 0;
            j++;
        }
    };
    // read last number
    a[j] = input.val;
    j++;
    // close file
    fclose(fileDescriptor);
    int k;
    float* result = malloc(j * sizeof(float));
    for(k = 0; k < j; k++) {
       result[k] = a[k]; // copy from large array into fitting array
    }
    itemnumber = j; // set global counter for items / elements
    return result;

}


int main(){
    start_time();
    int n;
    int min_value;
    int max_value;
    int fileRead = 3;
    int input_type;
    float *data; // array that contains the input

    /*
     * There is no goto here like in assembler in case an invalid number was presented
     * goto statements are considered as bad practice and strongly discouraged in C!!!
     * we decided not use a while loop instead since it does not represent the assembler code properly
     */
        // Ask if the user want to read numbers from file input
        printf("\nDo you want to read numbers from a file?, yes (1) or no (0)\n");
        scanf("%i", &input_type);
        FILE* fileOutputDescriptor = fopen( "C:\\assembler\\merge_output.txt", "w+" ); // open file with syscall
    if (input_type == 0) {
        //Ask for n
        printf("\nPlease enter here the amount of numbers that should be generated:\n");
        scanf("%i", &n);
        if (n < 0) {
            printf("\nThe wanted amount of numbers is negative\n");
            fprintf(fileOutputDescriptor, "\nThe wanted amount of numbers is negative\n");
            return 0;
        }
        //Ask for datarange

        printf("\nPlease enter the min value of the wished data range:\n");
        scanf("%i", &min_value);

        printf("\nPlease enter the max value of the wished data range:\n");
        scanf("%i", &max_value);

        //error checking
        if (min_value >= max_value) {
            printf("\nError: Your min and max value are either in wrong order or they are the same.\n");
            fprintf(fileOutputDescriptor, "\nError: Your min and max value are either in wrong order or they are the same.\n");
            return 0;
        }

        if ((min_value > const_max_value) || (max_value > const_max_value)) {
            printf("\nWe don't support such high numbers\n");
            fprintf(fileOutputDescriptor, "\nWe don't support such high numbers\n");
            return 0;
        }
        data = malloc(n * sizeof(float)); //Remark: data = a in assemblercode!!!!
        seed(n); // initialize seed
        generate_list(n, min_value, max_value, data); // generate with random items
    }
    else if(input_type == 1){
        FILE* input_file = fopen("C:\\assembler\\mergesort_recursive_input.txt", "r");
        if(input_file == NULL){
            printf("\nThe file 'mergesort_recursive_input.txt' was not found in c:\\assembler\\ . Please check if the file is available.\n\n");
            fprintf(fileOutputDescriptor, "\nThe file 'mergesort_recursive_input.txt' was not found in c:\\assembler\\ . Please check if the file is available.\n\n");
            return 0;
        }
        data = readInputFromFile(input_file);
        n = itemnumber;
        fclose(input_file);
    }
    else{
        printf("\nError: This is nit a valid parameter. Please try it again.\n");
        fprintf(fileOutputDescriptor, "\nError: This is nit a valid parameter. Please try it again.\n");
        return 0;
    }

    printf("\nThe unsorted array is:\n");
    fprintf(fileOutputDescriptor, "\nThe unsorted array is:\n");

    print_array(data, n, fileOutputDescriptor); //print unsorted array

    fsort(data,n);             // sort the given items

    printf("\nThe sorted array is:\n");
    fprintf(fileOutputDescriptor, "\nThe sorted array is:\n");

    print_array(data,n, fileOutputDescriptor); // print to console and file

    printf("\nSeems that everything is OK... But never trust a running system. There MUST be a bug! :D");
    fprintf(fileOutputDescriptor,"\nSeems that everything is OK... But never trust a running system. There MUST be a bug! :D");

    fclose(fileOutputDescriptor);
    return 0;
}
