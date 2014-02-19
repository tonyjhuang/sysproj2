typedef struct fatent_s {
  unsigned int used:1; 
  unsigned int eof:1; 
  unsigned int next:30;
} fatent;
