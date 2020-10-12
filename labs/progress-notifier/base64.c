#include <stdio.h>
#include "logger.h"
#include <inttypes.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

size_t curr;
size_t src = 0;
int codeMode;

static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

// Base64 encoding C code retrieved from https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64#C
int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize)
{
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;

   /* increment over the length of the string, three characters at a time */
   for (curr= 0; curr < dataLength; curr += 3) 
   {
      /* these three 8-bit (ASCII) characters become one 24-bit number */
      n = ((uint32_t)data[curr]) << 16; //parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((curr+1) < dataLength)
         n += ((uint32_t)data[curr+1]) << 8;//parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((curr+2) < dataLength)
         n += data[curr+2];

      /* this 24-bit number gets separated into four 6-bit numbers */
      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;
            
      /*
       * if we have one byte available, then its encoding is spread
       * out over two characters
       */
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n1];

      /*
       * if we have only two bytes available, then their encoding is
       * spread out over three chars
       */
      if((curr+1) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n2];
      }

      /*
       * if we have all three bytes available, then their encoding is spread
       * out over four characters
       */
      if((curr+2) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n3];
      }
   }

   /*
    * create and add padding that is required if we did not have a multiple of 3
    * number of characters available
    */
   if (padCount > 0) 
   { 
      for (; padCount < 3; padCount++) 
      { 
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = '=';
      } 
   }
   if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
   result[resultIndex] = 0;
   return 0;   /* indicate success */
}

// Base64 decoding C code retrieved from https://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64#C
int base64decode (char *in, size_t inLen, unsigned char *out, size_t *outLen) { 
    char *end = in + inLen;
    char iter = 0;
    uint32_t buf = 0;
    size_t len = 0;
    
    curr = 0;
    while (in < end) {
        curr++;
        unsigned char c = d[*in++];
        
        switch (c) {
        case WHITESPACE: continue;   /* skip whitespace */
        case INVALID:    return 1;   /* invalid input, return error */
        case EQUALS:                 /* pad character, end of data */
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++; // increment the number of iteration
            /* If the buffer is full, split it into bytes */
            if (iter == 4) {
                if ((len += 3) > *outLen) return 1; /* buffer overflow */
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0; iter = 0;

            }   
        }
    }
   
    if (iter == 3) {
        if ((len += 2) > *outLen) return 1; /* buffer overflow */
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    }
    else if (iter == 2) {
        if (++len > *outLen) return 1; /* buffer overflow */
        *(out++) = (buf >> 4) & 255;
    }

    *outLen = len; /* modify to reflect the actual output size */
    return 0;
}

void showProgress(){
    int progress = (float)curr / src * 100;
    infof("%s is %d%% complete\n", codeMode ? "Encoding of the file" : "Decoding of the file", progress);
}

int main(int argc, char **argv){
    char *strMode, *fileName, *orgBuf, *dstBuf;
    FILE *orgFile, *dstFile;

    if(argc != 3){
        errorf("Error: incorrect number of parameters.\n");
        errorf("Usage: ./base64 [--encode|--decode] <filename.txt>\n");
        return 0;
    }

    strMode = argv[1];
    if(strcmp(strMode, "--encode") != 0 && strcmp(strMode, "--decode") != 0){
        errorf("Error: incorrect mode.\n");
        errorf("Usage: ./base64 [--encode|--decode] <filename.txt>\n");
        return 0;
    }

    fileName = argv[2];
    if ((orgFile = fopen(fileName, "r")) == NULL)
    {
        errorf("Error: File %s could not be read\n", fileName);
        return 0;
    }

    codeMode = strcmp(strMode, "--decode");
    signal(SIGINT, showProgress);
    signal(SIGUSR1, showProgress);

    struct stat st;
    stat(fileName, &st);
    orgBuf = malloc(sizeof(char) * st.st_size);
    size_t dstLength = 4 * ((st.st_size + 2) / 3.0) + 3;
    dstBuf = malloc(sizeof(char) * dstLength);

    if(orgBuf == NULL || dstBuf == NULL){
        errorf("Error: memory could not be allocated.\n");
        fclose(orgFile);
        return 0;
    }

    int line;
    char buf[BUFSIZ];
    while((line = fread(buf, sizeof(char), BUFSIZ, orgFile)) > 0){
        size_t toCopy;
        if(BUFSIZ > st.st_size - src){
            toCopy = st.st_size - src;
        }else{
            toCopy = BUFSIZ;
        }
        strncpy(orgBuf + src, buf, toCopy);
        src += line;
    }

    fclose(orgFile);
    if (codeMode && base64encode(orgBuf, src, dstBuf, dstLength) != 0)
    {
        errorf("Error: could not encode file.\n");
        free(orgBuf);
        free(dstBuf);
        return 0;
    }
    if(!codeMode && base64decode(orgBuf, src, (unsigned char *)dstBuf, &dstLength) != 0){
        errorf("Error: could not decode file.\n");
        free(orgBuf);
        free(dstBuf);
        return 0;
    }

    char * dstFileName = strchr(fileName, '.');
    if(!dstFileName){
        errorf("Error: could not create name of destination file.\n");
        free(orgBuf);
        free(dstBuf);
        return 0;
    }else{
        *dstFileName = 0;
    }

    sprintf(dstFileName, "%s-%s.txt", fileName, codeMode ? "encoded" : "decoded");
    if((dstFile = fopen(dstFileName, "w")) == NULL){
        errorf("Error: could not open destination file.\n");
        free(orgBuf);
        free(dstBuf);
        return 0;
    }

    size_t writes, maxRead;
    size_t totalWrites = 0;
    size_t dstLen = strlen(dstBuf);
    while(totalWrites < dstLen){
        if(BUFSIZ > dstLen - totalWrites){
            maxRead = dstLen - totalWrites;
        }else{
            maxRead = BUFSIZ;
        }

        if((writes = fwrite(dstBuf + totalWrites, sizeof(char), maxRead, dstFile)) != maxRead){
            errorf("Error: could not write in destination file.\n");
            free(orgBuf);
            free(dstBuf);
            fclose(dstFile);
            return 0;
        }

       totalWrites += writes; 
    }

    infof("%s completed sucesfully. Output file is %s\n", codeMode ? "Encoding" : "Decoding", dstFileName);

    fclose(dstFile);
    free(orgBuf);
    free(dstBuf);

    return 0;
}
