

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef USE_DOS
  #include <io.h>
#endif


#include <stdlib.h>
//#include "../libdsk.h"

#include "Z80.h"
#include "KOGELMEM.h"
#include "KOGELIO.h"

#include "../PALESDL_IO.H"


unsigned char Z80_In(unsigned int fred)
{
        return(lynx_inp(fred));
}

void Z80_Out(unsigned int fred,unsigned char mabel)
{
//      if((fred & 0x207f)==0x207f)
//                      {
                        
        fred = fred & 0xFF;  //128 only  could req 207f
        lynx_outp(fred,mabel);
}

void Z80_Patch (Z80_Regs *Regs)   /* Called when ED FE occurs. Can be used */
                                   /* to emulate disk access etc.           */
{

}
           /* This is called after IPeriod T-States */
                                   /* have been executed. It should return  */
                                   /* Z80_IGNORE_INT, Z80_NMI_INT or a byte */
                                   /* identifying the device (most often    */
                                   /* 0xFF)  */
int Z80_Interrupt(void)
{

//      static int ii=0,jj=0;
/*      
        if ((start_ss_nmi==1))// || (break_nmi==1))     //not corrrect EMU
        {
                start_ss_nmi=0;
                return(Z80_NMI_INT);
        }       
        else if ( start_im1_irq==1 || (break_nmi==1))//who knows, is the key to here or what !!?
        {
                        start_im1_irq=0;
                        return(0xff);//IM1 interrupt device  FF - rst 38
        }
        else
*/
                return(Z80_IGNORE_INT );

}
