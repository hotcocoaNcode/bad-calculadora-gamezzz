#include <ti/getcsc.h>
#include <graphx.h>
#include <keypadc.h>
#include <ti/screen.h>
#include <math.h>

bool k_up, k_dw, k_lf, k_rg = false;
bool ccd = false;
bool lost = false;
float pi = 3.14159;

float px, py;

// 4:51 in video 1

// 320x240 -> 106x48
// Much MUCH more managable for graphics...

int SCWIDTH = 106;
int SCHEIGHT = 48;
int SCALEFACTOR = 5;

int mapX = 8, mapY = 8, mapS = 10;
int map[] =
{
 1,1,1,1,1,1,1,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,1,0,0,0,0,1,
 1,0,0,0,0,0,0,1,
 1,0,0,0,0,1,0,1,
 1,0,0,0,0,0,0,1,
 1,1,1,1,1,1,1,1,
};

void drawPixel(int x, int y, int c) {
    gfx_SetColor(c);
    gfx_FillRectangle(x*5 + 2, y*5 + 2, 5, 5);
}

void drawMap2D()
{
    int x, y, xo, yo;
    for (int y = 0; y < mapY; y++) {
        for (int x = 0; x < mapX; x++) {
            if (map[y*mapX+x]==1)
            {
                gfx_SetColor(255);
            }
            else {
                gfx_SetColor(0);
            }
            xo = x * mapS;
            yo = y * mapS;
            gfx_FillRectangle(xo, yo, mapS, mapS);
        }
    }
}

void draw()
{
    gfx_FillScreen(9);
    drawMap2D();
};

void gametick() {

    //movement
    if (k_lf) {
        px -= 3;
    }

    if (k_rg) {
        px += 3;
    }

    if (k_up) {
        py -= 3;
    }

    if (k_dw) {
        py += 3;
    }
};

void kctick(){ 
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

    if (kb_Data[7] & kb_Left) {
        k_lf = true;
    }
    else {
        k_lf = false;
    }

    if (kb_Data[7] & kb_Right) {
        k_rg = true;
    }
    else {
        k_rg = false;
    }

    if (kb_Data[6] & kb_Clear) {
    ccd = true;
    }
}

void init() {
    gfx_Begin();
    gfx_SetDrawBuffer();
    px = 300;
    py = 300;
}

int main()
{
    init();
    

    while (!ccd) {
        draw();
        gfx_SwapDraw();
        gametick();
        kctick();
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