#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef USE_DOS
  #include <io.h>
#endif


#include <stdlib.h>
//#include "../libdsk.h"

#include "../PALEDISK.H"
#include "../PALESDL.H"
#include "../PALESDL_IO.H"
#include "../PALESDL_VID.H"


unsigned Z80_RDMEM(dword A)
{
        char lbl[100];
        if(hw_type==LYNX_HARDWARE_HACKSPECCY)
        {
                        return(bank0[A]);
        }
        if(hw_type==LYNX_HARDWARE_128 || hw_type==LYNX_HARDWARE_192 || hw_type==LYNX_HARDWARE_256)
        {
                if((bank_latch & 0x08)==0)
                {
                        if (A<0x6000)   
                                return(bank0[A]);
                        if(bank0[0xe000]!=0xff) //Disk ROM can be turned off
                        {
 //FIXME - DISKS 
//                               if((A>=0xe000)   && ((disk_options & 0x10)==0))
                                        return(bank0[A]);
                        }
                }
                //THIS IS THE CORRECT PRIORITY FOR CPM TO WORK
                if ((bank_latch & 0x02)==0x02)  // WORKS VIDEO RAM
                {
                                return(bank2[A]);  //other video banks read
                }else

                if ((bank_latch & 0x04)==0)     //USER RAM
                {
                                return(bank1[A]);
                }

                //      if(hw_type==LYNX_HARDWARE_256)
                //      {
                //              if((bank_latch & 0x22)==0x02)   //VIDEO RAM
                //                              return(bank2[A]); 
                //              else if((bank_latch & 0x22)==0x22)      //VIDEO RAM
                //              {
                //                              return(bank3[A]);  //the seconds set of video memory
                //              }
                //      }
            //  if(hw_type==LYNX_HARDWARE_192 || hw_type==LYNX_HARDWARE_256)
                //      {
                //              if ((bank_latch & 0x01)==0x01)  //expanded user ram
                //                              return(bank4[A]);
                //      }
        }
        else
        {
                //see if ROM is paged in or not
                if((bank_latch & 0x10)==0 )             //ROMS BANK 0
                {
                        //      sprintf(lbl," BANK0 READ PC=%4X  VidLatch=%X BankLatch=%X Addr=%X:( ",get_pc(),video_latch,bank_latch,A);
                        if (A<0x6000)
                                return(bank0[A]);
                        //                      if((A>=0x6000) && (A<0x8000))
                        //                              return(bank0[0xe000+(A-0x6000)]);
                        //                      if(A>=0x8000 && A<0xc000)
                        //                              return(bank0[A-0x8000]);
                        //                      if(A>=0xc000 && A<0xe000)
                        //                              return(bank0[0x4000+(A-0xc000)]);
                        if(bank0[0xe000]!=0xff)// Disk rom is special - can be turned off
                        {

//FIXME - DISKS 
                                if((A>=0xe000)  ) // && ((disk_options&0x10)==0))
                                return(bank0[A]);
                        }
                }       
                
                if ((bank_latch & 0x20)==0)     //USER RAM
                {
                        if(hw_type==LYNX_HARDWARE_96)
                        {
                                return(bank1[A]);
                        }
                        else
                        {
                                if(A>=0x6000 && A<0x8000)
                                        return(bank1[A]);
                                if(A>=0xe000 && A<=0x10000)             //Mirror
                                        return(bank1[A-0x8000]);        
                                if(A>=0x8000 && A<0xa000)
                                        return(bank1[A]);
                                if(A>=0x0000 && A<0x2000)
                                        return(bank1[A+0x8000]);        //mirror
                                if(A>=0x2000 && A<0x4000)
                                        return(bank1[A+0x8000]);        //mirror
                                if(A>=0x4000 && A<0x6000)
                                        return(bank1[A+0x2000]);        //mirror
                                if(A>=0xa000 && A<0xc000)
                                        return(bank1[A-0x2000]);        
                                if(A>=0xc000 && A<0xe000)
                                        return(bank1[A-0x6000]);        //mirror
                        }
                }
                if ((bank_latch & 0x40)==0x40)  //vidram
                {
                        if ((video_latch & 0x04)==0) 
                        {
                                if(A>=0xa000 && A<0xe000)
                                        return(bank2[A]);               //BLUE  RED
                                if(A>=0x8000 && A<0xa000)
                                        return(bank2[A+0x2000]);        //mirror
                                if(A>=0x6000 && A<0x8000)
                                        return(bank2[A+0x6000]);        //mirror
                                if(A>=0xe000 && A<0x10000)
                                        return(bank2[A-0x2000]);        //mirror
                                if(A>=0x0000 && A<0x2000)
                                        return(bank2[A+0xa000]);        //mirror
                                if(A>=0x2000 && A<0x4000)
                                        return(bank2[A+0x8000]);        //mirror
                                if(A>=0x4000 && A<0x6000)
                                        return(bank2[A+0x8000]);        //mirror
                        }
                        if ((video_latch & 0x08)==0)
                        {
                                if(A>=0xa000 && A<0xe000)
                                        return(bank3[A]);//             if((video_latch & 0x20)==0x20 && (bank_latch & 0x02)==0x02)
                                                                                        //AGREEN  GREEN
                                if(A>=0x8000 && A<0xa000)
                                        return(bank3[A+0x2000]);        //mirror
                                if(A>=0x6000 && A<0x8000)
                                        return(bank3[A+0x6000]);        //mirror
                                if(A>=0xe000 && A<0x10000)
                                        return(bank3[A-0x2000]);        //mirror

                                if(A>=0x0000 && A<0x2000)
                                        return(bank3[A+0xa000]);        //mirror
                                if(A>=0x2000 && A<0x4000)
                                        return(bank3[A+0x8000]);        //mirror
                                if(A>=0x4000 && A<0x6000)
                                        return(bank3[A+0x8000]);        //mirror
                        }
                }

        }
        //      sprintf(lbl," ERROR - couldnt find that memory address PC=%4X   VidLatch=%X BankLatch=%X Addr=%X:( ",get_pc(),video_latch,bank_latch,A);
        //      MessageBox(NULL,lbl,"PALE ",MB_YESNOCANCEL | MB_DEFBUTTON1);
        return(0xff);  //  ;>
}

void Z80_WRMEM(dword A,byte V)
{
        char lbl[100];

        if(hw_type==LYNX_HARDWARE_HACKSPECCY)
        {
                if((A<0x4000))
                        return;
                else
                        bank0[A]=V;
                return;
        }

        if(hw_type==LYNX_HARDWARE_128 || hw_type==LYNX_HARDWARE_192 || hw_type==LYNX_HARDWARE_256)
        {
                if((bank_latch & 0x80)==0)
                {
                        bank1[A]=V;                             //USER MEMORY WRITE
                }
                if((bank_latch & 0x40)==0x40) //WORKS
                {
                        bank2[A]=V;             //standard video  write to red/blue/green
                }               
                
                //      if(hw_type==LYNX_HARDWARE_192 || hw_type==LYNX_HARDWARE_256)
                //      {
                //              if((bank_latch & 0x10)==0x10)
                //                                      bank4[A]=V;
                //      }
                //      if(hw_type==LYNX_HARDWARE_256)
                //      {
                //                      if((bank_latch & 0x60)==0x40) //WORKS
                //                                      bank2[A]=V;             //standard video  write to red/blue/green
                //                      else if((bank_latch & 0x60)==0x60)
                //                      {
                //                              bank3[A]=V;             //secnond vid bank
                //                      }
                //      }
        }
        else
        {
                if((bank_latch & 0x01)==0)
                {
                        if(hw_type==LYNX_HARDWARE_96)
                                                bank1[A]=V;
                        else
                        {
                                if(A>=0x6000 && A<0x8000)
                                        bank1[A]=V;
                                else if(A>=0xe000 && A<=0x10000)        //mirror
                                        bank1[A-0x8000]=V;
                                else if(A>=0x8000 && A<0xa000)
                                        bank1[A]=V;
                                else if(A>=0x0000 && A<0x2000)
                                        bank1[A+0x8000]=V;
                                else if(A>=0x2000 && A<0x4000)
                                        bank1[A+0x6000]=V;
                                else if(A>=0x4000 && A<0x6000)
                                        bank1[A+0x2000]=V;
                                else if(A>=0x0000 && A<0x2000)
                                        bank1[A+0x8000]=V;
                                else if(A>=0xc000 && A<0xe000)
                                        bank1[A-0x6000]=V;
                                else if(A>=0xa000 && A<0xc000)
                                        bank1[A-0x2000]=V;
                        }
                }
                //              if((video_latch & 0x20)==0x20 && (bank_latch & 0x02)==0x02)
                if((bank_latch & 0x02)==0x02)
                {
                        if ((video_latch & 0x04)==0) 
                        {
                                if(A>=0xa000 && A<0xe000)
                                        bank2[A]=V;                             // BLUE RED
                                else if(A>=0x8000 && A<0xa000)
                                        bank2[A+0x2000]=V;                              // mirror
                                else if(A>=0x6000 && A<0x8000)
                                        bank2[A+0x6000]=V;                              // mirror
                                else if(A>=0xe000 && A<0x10000)
                                        bank2[A-0x2000]=V;                              // mirror
                                else if(A>=0x0000 && A<0x2000)
                                        bank2[A+0xa000]=V;                              // mirror
                                else if(A>=0x2000 && A<0x4000)
                                        bank2[A+0x8000]=V;                              // mirror
                                else if(A>=0x4000 && A<0x6000)
                                        bank2[A+0x8000]=V;                              // mirror
                                //NOT TESTED YET - WERE MISSING
                                //ONE MIRROR OUT OF THE VIDEO READS AND WRITESS
                                //              else if(A>=0xa000 && A<0xc000)
                                //                      bank2[A-0x2000]=V;                              // mirror
                        }
                }
                //              if((video_latch & 0x20)==0x20 && (bank_latch & 0x04)==0x04)
                if((bank_latch & 0x04)==0x04)
                {
                        if ((video_latch & 0x08)==0) 
                        {
                                if(A>=0xa000 && A<0xe000)
                                        bank3[A]=V;                             //AGREEN  GREEN
                                else if(A>=0x8000 && A<0xa000)
                                        bank3[A+0x2000]=V;                              // mirror
                                else if(A>=0x6000 && A<0x8000)
                                        bank3[A+0x6000]=V;                              // mirror
                                else if(A>=0xe000 && A<0x10000)
                                        bank3[A-0x2000]=V;                              // mirror

                                else if(A>=0x0000 && A<0x2000)
                                        bank3[A+0xa000]=V;                              // mirror
                                else if(A>=0x2000 && A<0x4000)
                                        bank3[A+0x8000]=V;                              // mirror
                                else if(A>=0x4000 && A<0x6000)
                                        bank3[A+0x8000]=V;                              // mirror
                        }
                }
        }
}
