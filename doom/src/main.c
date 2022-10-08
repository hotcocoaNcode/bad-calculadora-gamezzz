#include <ti/getcsc.h>
#include <graphx.h>
#include <keypadc.h>
#include <ti/screen.h>
#include <math.h>

bool k_up, k_dw, k_lf, k_rg = false;
bool ccd = false;
bool lost = false;
float pi = 3.14159;

typedef struct
{
    float cos[360];
    float sin[360];
}math; math M;

typedef struct {
    int x, y, z;
    int a;
}player; player P;

void draw()
{
    
    
    int wx[4], wy[4], wz[4];
    float CS = M.cos[P.a];
    float SN = M.sin[P.a];

    //offset bottom points
    int x1 = 40 - P.x;
    int y1 = 10 - P.y;

    int x2 = 40 - P.x;
    int y2 = 290 - P.y;

    //world x
    wx[0] = x1 * CS - y1 * SN;
    wx[1] = x2 * CS - y2 * SN;
    wx[2] = x1 * CS - y1 * SN;
    wx[3] = x2 * CS - y1 * SN;

    //world y
    wy[0] = y1 * CS + x1 * SN;
    wy[1] = y2 * CS + x2 * SN;
    wy[2] = y1 * CS + x1 * SN;
    wy[3] = y2 * CS + x2 * SN;

    //world z
    wz[0] = 0 - P.z;
    wz[1] = 0 - P.z;
    wz[2] = 0 - P.z + 40;
    wz[3] = 0 - P.z + 40;

    //screenpos calculate
    wx[0] = wx[0] * 200 / wy[0] + GFX_LCD_WIDTH / 2;
    wy[0] = wz[0] * 200 / wy[0] + GFX_LCD_HEIGHT / 2;

    wx[1] = wx[1] * 200 / wy[1] + GFX_LCD_WIDTH / 2;
    wy[1] = wz[1] * 200 / wy[1] + GFX_LCD_HEIGHT / 2;
    
    wx[2] = wx[2] * 200 / wy[2] + GFX_LCD_WIDTH / 2;
    wy[2] = wz[2] * 200 / wy[2] + GFX_LCD_HEIGHT / 2;

    //wx[3] = wx[3] * 200 / wy[3] + GFX_LCD_WIDTH / 2;
    //wy[3] = wz[3] * 200 / wy[3] + GFX_LCD_HEIGHT / 2;

    //draw bg
    gfx_SetColor(0);
    gfx_FillRectangle_NoClip(0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT);

    
    gfx_SetColor(255);

    if (wx[0] > 0 && wx[0] < GFX_LCD_WIDTH && abs(wy[0] - GFX_LCD_HEIGHT)>0 && abs(wy[0] - GFX_LCD_HEIGHT) < GFX_LCD_HEIGHT) {
        gfx_FillRectangle_NoClip(wx[0], abs(wy[0] - GFX_LCD_HEIGHT), 1, 1);
    }

    if (wx[1] > 0 && wx[1] < GFX_LCD_WIDTH && abs(wy[1] - GFX_LCD_HEIGHT)>0 && abs(wy[1] - GFX_LCD_HEIGHT) < GFX_LCD_HEIGHT) {
        gfx_FillRectangle_NoClip(wx[1], abs(wy[1] - GFX_LCD_HEIGHT), 1, 1);
    }

    if (wx[2] > 0 && wx[2] < GFX_LCD_WIDTH && abs(wy[2] - GFX_LCD_HEIGHT)>0 && abs(wy[2] - GFX_LCD_HEIGHT) < GFX_LCD_HEIGHT) {
        gfx_FillRectangle_NoClip(wx[2], abs(wy[2] - GFX_LCD_HEIGHT), 1, 1);
    }

    //if (wx[3] > 0 && wx[3] < GFX_LCD_WIDTH && abs(wy[3] - GFX_LCD_HEIGHT)>0 && abs(wy[3] - GFX_LCD_HEIGHT) < GFX_LCD_HEIGHT) {
    //    gfx_FillRectangle_NoClip(wx[3], abs(wy[3] - GFX_LCD_HEIGHT), 1, 1);
    //}

    


    

};
void gametick() {

    //movement
    if (k_lf) {
        P.a -= 4;
        if (P.a < 0) {
            P.a += 360;
        }
    }

    if (k_rg) {
        P.a += 4;
        if (P.a > 360) {
            P.a -= 360;
        }
    }

    int dx = M.sin[P.a] * 10;
    int dy = M.cos[P.a] * 10;

    if (k_up) {
        P.x += dx;
        P.y += dy;
    }

    if (k_dw) {
        P.x -= dx;
        P.y -= dy;
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
    for (int x = 0; x < 360; ++x) {
        M.cos[x] = cos(x / 180.0 * pi);
        M.sin[x] = sin(x / 180.0 * pi);
    }

    P.x = 70; P.y = -110; P.z = 20; P.a = 0;
}

int main()
{
    gfx_Begin();
    gfx_SetDrawBuffer();
    //int c = 5;

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