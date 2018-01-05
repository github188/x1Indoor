#define OVERRIDE_NORMALIZE16
int normalize16(const ace_sig_t *x, ace_word16_t *y, ace_sig_t max_scale, int len)
{
   ace_sig_t max_val=1;
   int sig_shift;
   int dead1, dead2, dead3, dead4, dead5, dead6;

   __asm__ __volatile__ (
         "\tmov %1, #1 \n"
         "\tmov %3, #0 \n"

         ".normalize16loop1%=: \n"

         "\tldr %4, [%0], #4 \n"
         "\tcmps %4, %1 \n"
         "\tmovgt %1, %4 \n"
         "\tcmps %4, %3 \n"
         "\tmovlt %3, %4 \n"

         "\tsubs %2, %2, #1 \n"
         "\tbne .normalize16loop1%=\n"

         "\trsb %3, %3, #0 \n"
         "\tcmp %1, %3 \n"
         "\tmovlt %1, %3 \n"
   : "=r" (dead1), "=r" (max_val), "=r" (dead3), "=r" (dead4),
   "=r" (dead5), "=r" (dead6)
   : "0" (x), "2" (len)
   : "cc");

   sig_shift=0;
   while (max_val>max_scale)
   {
      sig_shift++;
      max_val >>= 1;
   }
   
   __asm__ __volatile__ (
         ".normalize16loop%=: \n"

         "\tldr %4, [%0], #4 \n"
         "\tldr %5, [%0], #4 \n"
         "\tmov %4, %4, asr %3 \n"
         "\tstrh %4, [%1], #2 \n"
         "\tldr %4, [%0], #4 \n"
         "\tmov %5, %5, asr %3 \n"
         "\tstrh %5, [%1], #2 \n"
         "\tldr %5, [%0], #4 \n"
         "\tmov %4, %4, asr %3 \n"
         "\tstrh %4, [%1], #2 \n"
         "\tsubs %2, %2, #1 \n"
         "\tmov %5, %5, asr %3 \n"
         "\tstrh %5, [%1], #2 \n"

         "\tbge .normalize16loop%=\n"
   : "=r" (dead1), "=r" (dead2), "=r" (dead3), "=r" (dead4),
   "=r" (dead5), "=r" (dead6)
   : "0" (x), "1" (y), "2" (len>>2), "3" (sig_shift)
   : "cc", "memory");
   return sig_shift;
}

