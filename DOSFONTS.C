#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include "PALESDL.H"
#include "PALESDL_VID.H"
#include "PALE_KEYS.H"
#include "Z80DASM.H"
#include "DOSDIR.h"

#define MY_SCREEN_WIDTH 31
#define MY_SCREEN_HEIGHT 31


#define CHAR_WIDE   8
#define CHAR_HIGH   8
#define MAX_CHAR  256

#define CHAR_SIZE ((CHAR_HIGH * CHAR_WIDE) / 8)

#define CHAR_FILL 0xDB

// Global Variables
unsigned char charmap[MAX_CHAR][CHAR_SIZE];


int mycursor_x = 0;
int mycursor_y = 0;
unsigned char mycolour_foreground = 7;
unsigned char mycolour_background = 0;

// Function Prototypes
void init_fonts(void);
int  load_font(char *fname);
void showchar(int curr_char, int xpos, int ypos);

int horiz_vidbytes=64;

unsigned char byte_reverse(unsigned char value)
{
    value = (value & 0x0f) << 4 | (value & 0xf0) >> 4;
    value = (value & 0x33) << 2 | (value & 0xcc) >> 2;
    value = (value & 0x55) << 1 | (value & 0xaa) >> 1;
    return value;
}


int load_font(char *fname)
{
    FILE *fp;
    int i,j;

    fp = fopen(fname, "rb");

    for (i = 0; i < MAX_CHAR; i++) {
        fread(&charmap[i][0], CHAR_SIZE, 1, fp);
        for(j=0;j<CHAR_SIZE;j++)
            charmap[i][j] = byte_reverse(charmap[i][j]);
    }
    fclose(fp);
    return 1;
}

void init_fonts(void)
{
    memset(charmap, 0, (MAX_CHAR * CHAR_SIZE));
    load_font("vga8X8.FNT");
}

void showchar(int curr_char, int xpos, int ypos)
{
    unsigned char *bytes;
    
    int i, j, temp,memx,bank,retbyte,scanline;
    unsigned char paper,ink;
    
    unsigned char *vmem;

    vmem =  ((Uint8 *)screen->pixels ) +(ypos * CHAR_HIGH * horiz_vidbytes * 8) + xpos*8;
 	
    for(scanline = 0;scanline<CHAR_HIGH;scanline++)
    {
        bytes=vmem+(scanline*horiz_vidbytes*8);   //80 bytes
        //get pointer to character
        retbyte =  charmap[curr_char][scanline];
	for(i = 0; i< 8;i++)
	{
		if(retbyte & (0x80 >> i) )
	  	    *((unsigned char *)(bytes+i)) = mycolour_foreground;
		else
	  	    0;
	}
    }
// for scandoubliing
//    for(memx=0;memx<512;memx++)
  //      *((Uint8 *)(bytes++)) = *((Uint8 *)(bytes-512));
// enabling this is slowwww 
//   SDL_UpdateRect(screen, xpos*8,ypos * CHAR_HIGH,(xpos+1)*8,(ypos+1) * CHAR_HIGH);
}

void my_paint(unsigned char x1,unsigned char x2,unsigned char y1,unsigned char y2)
{
    unsigned char *bytes;
    unsigned char write_byte;
    int i, j, temp,memx,bank,retbyte,scanline,vert_h,n;
    unsigned char *vmem;
    unsigned int pix_height,bytes_width,bytes_high;
    char lbl[200];
  
    vmem = ((Uint8 *)screen->pixels) +(y1 * CHAR_HIGH * horiz_vidbytes * 8);
    bytes_high = y2 - y1;
    pix_height = bytes_high * CHAR_HIGH;
    bytes_width = x2 - x1;

        for(scanline = 0;scanline<pix_height;scanline++)
        {
            bytes=vmem+(scanline*horiz_vidbytes*8)+(x1*8); 
            for(n = 0;n < bytes_width * 8;n++)
            {
                *((unsigned char *)(bytes+n)) = mycolour_foreground;
            }
        }    
 	SDL_UpdateRect(screen, x1*8,y1*8,x2*8,y2*8);
  

}

void my_move(unsigned char x,unsigned char y)
{
    mycursor_x = x;
    mycursor_y = y;
}

void my_print(char text[])
{
    char c;
    int c_pos = 0;

   c = text[c_pos++];
    while(c!='\0')
    {
        switch(c)
        {
            case '\b'://backspace
                if(mycursor_x > 0)mycursor_x--;
                break;
            case '\t'://tab
                if(mycursor_x < (MY_SCREEN_WIDTH-7))mycursor_x +=5;
                break;
            case '\n'://newline
                mycursor_x = 0;
                mycursor_y ++;
                break;
            case '\f'://form feed -  here used as set foreground colour
                c = text[c_pos++];
                mycolour_foreground = c - '0';
                break;
            case '\a'://beep -  here used as set background colour
                c = text[c_pos++];
                mycolour_background = c - '0';
                break;

            default:
		showchar(c,mycursor_x,mycursor_y);
                mycursor_x ++;
                break;
        }
        
        if(mycursor_x > MY_SCREEN_WIDTH)
        {
            mycursor_x = 0;
            mycursor_y ++;
        }
        if(mycursor_y > MY_SCREEN_HEIGHT)
            mycursor_y = 0;

        c = text[c_pos++];
    }

    SDL_UpdateRect(screen, 0,0,512,512);

}

void my_input(char *prompt,char *text)
{
    char c;
    char lbl[200];
    char charout[4];
    int text_ptr = 0;
    //Get input string from the user

    char cursor[5] = {'<',' ','\b','\b','\0'};

    sprintf(lbl,"%s",prompt);
    my_print(lbl);
    my_print(cursor);


// for the console terminal
//    c = getchar();
//    scanf("%s",text);
//    return;

  
    c = my_getkey();
    while(c != 13 && c != 27)
    {
	if(c==0)
	{
	}
	
	else if(c == 8)//backspace
        {
            if(text_ptr > 0)
            {
                text_ptr--;
                my_print("\b");
                my_print(cursor);
            }
        }
        else
        {
            text[text_ptr++] = c;
            sprintf(charout,"%c",c);        
            my_print(charout);
            my_print(cursor);
            //my_print(" -");
            //sprintf(lbl," %d ",(unsigned int)c);        
            //my_print(lbl);
    
            if(text_ptr>50) break;
        }
        c = my_getkey();
    }
    text[text_ptr] = '\0';
}

void show_help()
{
    char name[200];
    char lbl[200];

    my_print("\f1");
    my_paint(0,32,6,27);
    my_move(0,7);
        my_print("\f6P\f4a\f2l\f3e\f5 SDL - Linux\n\f6Version:");
        sprintf(lbl,"%.2f",PALE_REV);
        my_print(lbl);
        my_print("\n\n\f6Peter Todd 2003 - 2012\n\n");
    my_print("\f7\a2");
    my_print("F2  - Show Status\n");
    my_print("F3  - Pause Emu\n");
    my_print("F4  - Toggle Speed\n");
    my_print("F5  - Change Machine\n");
    my_print("F6  - Load TAP File\n");
    my_print("F7  - FullScreen\n");
    my_print("F8  - unused (vnc problem)\n");
    my_print("F9  - Load LSF File\n");
    my_print("F10 - Load LDF Disk File\n");
    my_print("F11 - Hard Reset Machine\n");
    my_print("F12 - Quit Pale\n\n");
    my_print("\f7\a0\n");
    my_input("Press Return ",name);
}



void my_show_directory(char *pathext,unsigned int highlight,char *name)
{
    char cmdresult[4000];
    my_print("\f0");
    my_paint(0,31,0,20);
    my_print("\f2\a0");
    my_move(0,0);
    get_directory(pathext,cmdresult,highlight,name);
    
    my_print(cmdresult);
    my_print("\f7\a0");
}



void my_filechooser(char *pathext,char *name)
{
      unsigned int highlight = 0;
      char list_dir[256];

      Uint8 *key;
      while(1)
        {       
            my_show_directory(pathext,highlight,name);
            while(1)
            {
  						SDL_PumpEvents();
						key = SDL_GetKeyState(NULL);
                        //no need to poll anything, interrupts work magic in the background :)
                        if (key[SDLK_F12])
                        {
                                finish_emu=1;
                        return;
                        }
                        if (key[SDLK_ESCAPE])
                                return;
                        if (key[SDLK_UP])
                        {
                                waitkeyup(SDLK_UP);
                                if(highlight>0)highlight--;
                        break;
                        }
                        if (key[SDLK_DOWN])
                        {
                                waitkeyup(SDLK_DOWN);
                                highlight++;
                                break;          
                        }
                        if (key[SDLK_RETURN])
                                return;
             }       
        }
}



void my_show_speed()
{
        char lbl[20];
        int s = emuspeeds[emu_speed];
    my_print("\f0");
    my_paint(0,10,0,1);
    my_print("\f4\a0");
    my_move(0,0);
    sprintf(lbl,"%d\%",s);
    my_print(lbl);
    my_print("\f7\a0");
}

/*

void draw_overlay()
{
	my_show_speed();
}*/


void get_statusregs(char* lbl)
{
        sprintf(lbl,
      //          "PC:%04X IR: %02X AF:%04X HL:%04X\nDE:%04X BC:%04X SP:%04X IX:%04X\nIY:%04X",
                "PC:%04X AF:%04X HL:%04X SP:%04X\nDE:%04X BC:%04X\nIX:%04X IY:%04X  IR: %02X\n\f6Bank:%02X Video:%02X\f7",
            z80_get_reg(Z80_REG_PC),
                z80_get_reg(Z80_REG_IR),
                z80_get_reg(Z80_REG_AF),
                z80_get_reg(Z80_REG_HL),
                z80_get_reg(Z80_REG_DE),
                z80_get_reg(Z80_REG_BC),
                z80_get_reg(Z80_REG_SP),
                z80_get_reg(Z80_REG_IX),
                z80_get_reg(Z80_REG_IY),0,0);//,
//                                bank_latch,
  //                              video_latch);
}

void get_statusstack(char *lbl)
{
        unsigned int i,sp;
        char lbl1[300],lbl2[300],lbl3[300],lbl4[300];
        //STACK
        sp=z80_get_reg(Z80_REG_SP);
        
        sprintf(lbl2," ");
        for (i=0;i<5;++i)
        {
                sprintf(lbl3,"%02X%02X ",bank1[sp+i*2],bank1[sp+i*2]);
                strcat(lbl2,lbl3);
        }
        strcpy(lbl,lbl2);
}



int disassemble (char *destin,unsigned int jogval,int bank)
{
        int g,retr,f,start,mid,end,linecount;
        unsigned int curp,peecee;
        char lbl[200];
        char dis_buf[4000];
        char return_char[4];


        //Poss might have to increase this
        // to avoid routine below scanning backwards into other vars space
        peecee=z80_get_reg(Z80_REG_PC);
        curp=peecee-8+jogval;
        
        //skip forward to the relevant byte
        sprintf(return_char,"\n");
        sprintf(dis_buf,"\0");
        mid=0;
        for (f=0;f<12;f++)
        {
                if(curp==peecee)
                {
                        mid=strlen(dis_buf);
                        sprintf(lbl,"->");
                        strcat(dis_buf,lbl);
                }
                sprintf(lbl,"%4X - ",curp);
                strcat(dis_buf,lbl);
//              retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
                if(bank==0)
                                retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
                else if(bank==1)
                                retr=Z80_Dasm((unsigned char *)bank1+curp,lbl,curp);
                else if(bank==2)
                                retr=Z80_Dasm((unsigned char *)bank2+curp,lbl,curp);
                else if(bank==3)
                                retr=Z80_Dasm((unsigned char *)bank3+curp,lbl,curp);
                else if(bank==4)
                                retr=Z80_Dasm((unsigned char *)bank4+curp,lbl,curp);
                //              retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
//              retr=Z80_Dasm((unsigned char *)bank0+curp,lbl,curp);
                if(retr !=0)
                {
                    strcat(dis_buf,lbl);
                    strcat(dis_buf,return_char);
                    curp+=retr;//advance to the next opcode
                }
                else
                    curp++; //advance to the next opcode
        }
        end=strlen(dis_buf);

        if (mid!=0)
        {
                //okay, we've got a listing, now search thru
                //to fnd the PC line, and then take the 14 lines either side of it
                //and copy them to the output buffer
                start=0;
                f=0;    
                //Skip to PC line
//              while(dis_buf[f++]!='_')
//              {
//                      if(f>end)return(0);
//              };
                //Now go back 6 lines
                f=mid;
                for(g=0;g<4;g++)
                {
                        while(dis_buf[f--]!='\n')
                        {
                                if(f<=0)return(0);
                        }
                }
                start=f+2;
                for(f=start;f<end;f++)
                        destin[f-start]=dis_buf[f];
        }
                else return(0);

//if(end > 150)
  //  end = 150;
                for(f=0;f<end;f++)
                        destin[f]=dis_buf[f];
                destin[f]='\0';
        return(1);
}


void draw_overlay()
{
    char txt1[1000],txt2[1000];

   // my_move(0,0);
   // query_6845(&txt1,&txt2);
   // my_print(txt1);
    my_print("\f0");
    my_paint(0,32,12,24);
    my_print("\f4\a0");
    my_move(0,12);
    disassemble (txt2,0,0);
    my_print(txt2);

    my_print("\f0");
    my_paint(0,32,6,12);
    my_print("\f5");
    my_move(0,7);
    get_statusregs(txt2);
    my_print(txt2);
    my_print("\f4");
    sprintf(txt1,"\n\f2Machine Type: \f5%d\f2   Emu Speed: \f5%d\f4",mc_type,emu_speed);
    my_print(txt1);
    my_print("\f7");
}
/*
void draw_overlay2()
{
    char txt1[1000],txt2[1000];
if(usePageFlipping == 0)vidmem = vga_mem;//     vidpage_flip(1); //Dont flip vidpage - write direct comes next

   // my_move(0,0);
   // query_6845(&txt1,&txt2);
   // my_print(txt1);
    my_print("\f0");
    my_paint(0,32,5,24);
    my_print("\f5\a0");
    my_move(0,12);
        dpmi_status(&txt2);
    my_move(0,5);
    my_print(txt2);
    my_print("\f7");
}


void draw_overlay3()
{
    char txt1[1000],txt2[1000];
if(usePageFlipping == 0)vidmem = vga_mem;//     vidpage_flip(1); //Dont flip vidpage - write direct comes next

   // my_move(0,0);
   // query_6845(&txt1,&txt2);
   // my_print(txt1);
    my_print("\f0");
    my_paint(0,32,5,24);
    my_print("\f5\a0");
    my_move(0,12);
        keycodes_status(&txt2);
    my_move(0,5);
    my_print(txt2);
    my_print("\f7");
}

*/
