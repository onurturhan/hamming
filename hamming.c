#include "stdio.h"

typedef unsigned int uint32;
typedef unsigned char uint8;

#define NO_ERROR     0
#define SINGLE_ERROR 1
#define DOUBLE_ERROR 2

static const char *ERROR_TYPE[] =  {
    "NO ERROR", "SINGLE ERROR", "DOUBLE ERROR",
};

uint8 calc_hamming(uint32 value);
uint8 insert_error(uint32 value, uint32 error);

uint8 calc_hamming(uint32 value)
{
    uint32 ii;

    uint8 D[32]={0x00};
    uint8 CB[8]={0x00};

    uint8 checkbyte=0;

    for(ii=0; ii<32; ii++)
        D[ii] = (value >> ii) & 0x1;
            
    /* BCH EDAC: https://www.gaisler.com/doc/gr712rc-usermanual.pdf Section: 5.10.1  - Page:64 */
    
    CB[0] =(D[0]+D[4]+D[ 6]+D[ 7]+D[ 8]+D[ 9]+D[11]+D[14]+D[17]+D[18]+D[19]+D[21]+D[26]+D[28]+D[29]+D[31])%2;
    CB[1] =(D[0]+D[1]+D[ 2]+D[ 4]+D[ 6]+D[ 8]+D[10]+D[12]+D[16]+D[17]+D[18]+D[20]+D[22]+D[24]+D[26]+D[28])%2;
    CB[2] =(D[0]+D[3]+D[ 4]+D[ 7]+D[ 9]+D[10]+D[13]+D[15]+D[16]+D[19]+D[20]+D[23]+D[25]+D[26]+D[29]+D[31] +1)%2;
    CB[3] =(D[0]+D[1]+D[ 5]+D[ 6]+D[ 7]+D[11]+D[12]+D[13]+D[16]+D[17]+D[21]+D[22]+D[23]+D[27]+D[28]+D[29] +1)%2;
    CB[4] =(D[2]+D[3]+D[ 4]+D[ 5]+D[ 6]+D[ 7]+D[14]+D[15]+D[18]+D[19]+D[20]+D[21]+D[22]+D[23]+D[30]+D[31])%2;
    CB[5] =(D[8]+D[9]+D[10]+D[11]+D[12]+D[13]+D[14]+D[15]+D[24]+D[25]+D[26]+D[27]+D[28]+D[29]+D[30]+D[31])%2;
    CB[6] =(D[0]+D[1]+D[ 2]+D[ 3]+D[ 4]+D[ 5]+D[ 6]+D[ 7]+D[24]+D[25]+D[26]+D[27]+D[28]+D[29]+D[30]+D[31])%2;     
        
    for(ii=0; ii<8; ii++)
        checkbyte |= (CB[ii] << ii) & (0x1 << ii);

    return checkbyte;       
}

uint8 insert_error(uint32 value, uint32 error)
{
    static uint32 se = 0; /* single error */
    static uint32 de = 0; /* double error */
    
    uint8 cb = calc_hamming(value);
    
    switch(error)
    {
        case SINGLE_ERROR:
            cb ^= (1<<(se%7)); se++;
        break;
        
        case DOUBLE_ERROR:
            cb ^= (1<<(de%7)); de++;
            cb ^= (1<<(de%7)); de++;
        break;
        
        default:
            
        break;
    }
    
    return cb;
}

void main(void)
{
    uint32 ii,jj;
    uint32 value = 0x00000028; 
    uint8 checkbyte[3] = {0x00};
    
    for(jj=0; jj<7; jj++)
    {
        printf("##################### %d ####################### \n", jj);
        
        for(ii=0; ii<3; ii++)
        {
            checkbyte[ii] = insert_error(value, ii);
            printf("Value: 0x%08X CheckByte: 0x%02X with \"%s\" \n", 
                        value, checkbyte[ii], ERROR_TYPE[ii]);
        }
    }
    
}
