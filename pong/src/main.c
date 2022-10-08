#include <ti/getcsc.h>
#include <graphx.h>
#include <keypadc.h>
#include <ti/screen.h>


int ballpx = GFX_LCD_WIDTH / 2;
int ballpy = GFX_LCD_HEIGHT / 2;
int ballxmod = 2;
int ballymod = 1;
int paddley = GFX_LCD_HEIGHT / 2;
bool k_up, prevk_up, k_dw, prevk_dw = false;
bool ccd = false;
bool lost = false;

void draw()
{

    gfx_SetColor(0);
    gfx_FillRectangle_NoClip(0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT); // overdocumenting
    gfx_SetColor(255);// wow really wonder what this does almost as if it sets the color to a number of 255 which seems to be white
    gfx_FillRectangle(ballpx, ballpy, 10, 10); // ball
    gfx_FillRectangle(40, paddley, 5, 30); // human paddle
    gfx_FillRectangle(GFX_LCD_WIDTH - 40, ballpy - 15, 5, 30); // ai paddle

};
void gametick() {
    if (ballpx + 10 < GFX_LCD_WIDTH - 40 && ballpx > 0) {
        if (ballpx > 45)
        {
           ballpx = ballxmod + ballpx;

        }else{
           if (ballpx < 35) {
               ccd = true;
               lost = true;
           }
           if (ballpy > paddley + 30 || ballpy + 10 < paddley){

                ballpx = ballxmod + ballpx;

           }else{

                ballxmod = ballxmod * -1;
                ballpx = ballxmod + ballpx;

           }
        }
    }
    else {
        ballxmod = ballxmod * -1;
        ballpx = ballxmod + ballpx;
    }

    if (ballpy + 10 < GFX_LCD_HEIGHT && ballpy > 0) {
        ballpy = ballymod + ballpy;
    }
    else {
        ballymod = ballymod * -1;
        ballpy = ballymod + ballpy;
    }

    if (paddley + 30 < GFX_LCD_HEIGHT){
        if (k_dw){
            paddley = paddley + 2;
        }
    }

    if (paddley > 0){
        if (k_up){
            paddley = paddley - 2;
        }
    }
};

void kctick(){ 
//because am dumbass and dont understando bit arrays n shit i'm using the getcsc method which slows the normally 60fps program to a grueling 10fps. 
//using this method for keydetect because it'll get called once every 5 frames instead of every frame, hopefully doable now?

// DUNCE CAP WORTHY ^^^^^^^
// future me here, just steal from docs it still lagged like hell

// DUNCE CAP WORTHY ^^^^^^^
// future me here, just steal from docs but smarterly it took like 2 minutes to register one button press

// DUNCE CAP WORTHY ^^^^^^^
// future me here, just read the fucking docs.

    kb_Scan();
    
    if (kb_Data[7] & kb_Up) {
    k_up = true;
    }else{
    k_up = false;
    }

    if (kb_Data[7] & kb_Down) {
    k_dw = true;
    }else{
    k_dw = false;
    }

    if (kb_Data[6] & kb_Clear) {
    ccd = true;
    }
}

int main(void)
{
    gfx_Begin();
    gfx_SetDrawBuffer();
    //int c = 5;

    while (!ccd) {
        draw();
        gfx_SwapDraw();
        gametick();
        //if (c == 5){
        kctick();
        //    c = 0;
        //}else{
        //    c = c+1;
        //}
    }

    gfx_End();

    os_ClrHome();
    if (lost) {
        os_PutStrFull("You lost...");
        os_NewLine();
        os_PutStrFull("Press any key to exit");
    }
    
    while (!os_GetCSC());

    return 0;
}