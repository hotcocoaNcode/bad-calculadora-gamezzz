#include <ti/getcsc.h>
#include <graphx.h>
#include <keypadc.h>
#include <ti/screen.h>
#include <math.h>

bool k_up, k_dw, k_lf, k_rg = false;
bool ccd = false;
bool lost = false;
float pi = 3.14159;

// 320x240 -> 106x48
// Much MUCH more managable for graphics...

int SCWIDTH = 106;
int SCHEIGHT = 48;
int SCALEFACTOR = 5;


//tl:dr
//fuck it i give up


/*
ALRIGHT, RANT COMMENT HERE. 
FOR SOME REASON WHEN I ADDED CLIPPING THE ENTIRE THING JUST FUCKING DIED.

I HAVE NO CLUE WHY. IT JUST DID. IT ONL DREW THE TOP LINE AND I AM BASICALLY CLUELESS AS TO WHY.
HELL, EVEN WHEN I TRIED TO FILL THE WALLS WITH NO CLIPPING IT DREW IT ONLY BEHIND THE PLAYER.

I AM GOING TO GO LEARN TRIG AND COME BACK IN 2 YEARS. I DO NOT UNDERSTAND ANY OF WHY IT WORKED EVEN 3 PERCENT.

FOR NOW I WILL MAKE A RAYCASTER.

FUCK THIS SHIT I'M OUT
*/

typedef struct
{
    float cos[360];
    float sin[360];
}math; math M;

typedef struct {
    int x, y, z;
    int a;
}player; player P;

void drawPixel(int x, int y, int c) {
    gfx_SetColor(c);
    gfx_FillRectangle(x*5 + 2, y*5 + 2, 5, 5);
}

void drawWallLine(int x, int y, int h, int c) {
    gfx_SetColor(c);
    gfx_FillRectangle(x * 5 + 2, y * 5 + 2 + (h*5), 5, 5 * h);
}

void clipBehindPlayer(int* x1, int* y1, int* z1, int x2, int y2, int z2) {
    float da = *y1;
    float db = y2;
    float d = da - db;
    if (d == 0) {
        d = 1;
    }
    float s = da / (da - db);
    *x1 = *x1 + s * (x2 - (*x1));
    *y1 = *y1 + s * (y2 - (*y1)); 
    if (*y1 == 0) {
        *y1 = 1;
    }
    *z1 = *z1 + s * (z2 - (*z1));
}

void drawWall(int x1, int x2, int b1, int b2, int t1, int t2) {
    int dyb = b2 - b1;
    int dyt = t2 - t1;
    int dx = x2 - x1;

    if (x1 < 0) {
        x1 = 1;
    }
    if (x2 < 0) {
        x2 = 0;
    }
    if (x1 > SCWIDTH - 0) {
        x1 = SCWIDTH - 0;
    }
    if (x2 > SCWIDTH - 0) {
        x2 = SCWIDTH - 0;
    }

    if (dx == 0) {
        dx = 1;
    }
    int xs = x1;

    for (int x = x1; x < x2; ++x) {

        int y1 = dyb * (x - xs + 0.5) / dx + b1;
        int y2 = dyt * (x - xs + 0.5) / dx + t1;

        if (y1 < 0) {
            y1 = 0;
        }
        if (y2 < 0) {
            y2 = 0;
        }
        if (y1 > SCHEIGHT - 0) {
            y1 = SCHEIGHT - 0;
        }
        if (y2 > SCHEIGHT - 0) {
            y2 = SCHEIGHT - 0;
        }

        drawPixel(x, y1, 255);
        drawPixel(x, y2, 255);
    }
}

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
    wx[2] = wx[0];
    wx[3] = wx[1];

    //world y
    wy[0] = y1 * CS + x1 * SN;
    wy[1] = y2 * CS + x2 * SN;
    wy[2] = wy[0];
    wy[3] = wy[1];

    //world z
    wz[0] = 0 - P.z;
    wz[1] = 0 - P.z;
    wy[2] = wy[0] + 40;
    wy[3] = wy[1] + 40;

    if (wy[0] < 1 && wy[1] < 1) {
        return; //wall behind player donut draw
    }

    //p1
    if (wy[0] < 1) {
        clipBehindPlayer(&wx[0], &wy[0], &wz[0], wx[1], wy[1], wz[1]); // bottom line
        //clipBehindPlayer(&wx[2], &wy[2], &wz[2], wx[3], wy[3], wz[3]); // top line
    }


    //p2
    if (wy[1] < 1) {
        clipBehindPlayer(&wx[1], &wy[1], &wz[1], wx[0], wy[0], wz[0]); // bottom line
        //clipBehindPlayer(&wx[3], &wy[3], &wz[3], wx[2], wy[2], wz[2]); // top line
    }

    //screenpos calculate
    wx[0] = wx[0] * 200 / wy[0] + SCWIDTH / 2;
    wy[0] = wz[0] * 200 / wy[0] + SCHEIGHT / 2;


    wx[1] = wx[1] * 200 / wy[1] + SCWIDTH / 2;
    wy[1] = wz[1] * 200 / wy[1] + SCHEIGHT / 2;

    wx[2] = wx[2] * 200 / wy[2] + SCWIDTH / 2;
    wy[2] = wz[2] * 200 / wy[2] + SCHEIGHT / 2;


    wx[3] = wx[3] * 200 / wy[3] + SCWIDTH / 2;
    wy[3] = wz[3] * 200 / wy[3] + SCHEIGHT / 2;

    //normalize draw Y to top-left from 3Dsage's bottom-left
    wy[0] = abs(wy[0] - SCHEIGHT);
    wy[1] = abs(wy[1] - SCHEIGHT);
    wy[2] = abs(wy[2] - SCHEIGHT);
    wy[3] = abs(wy[3] - SCHEIGHT);

    //draw bg
    gfx_ZeroScreen();

    //I would be using polygon but since it creates utterly fucked render fails when the points are offscreen, I'll just have to deal w/it.
    //Also, for some weird ass reason it appears skewed. Not my problem anymore

   // int drawpoints[8] = {
   //     wx[0], wy[0],
   //     wx[1], wy[1],
   //     wx[3], wy[3],
   //     wx[2], wy[2],
    //};

    //gfx_Polygon(drawpoints, 4);

    //gfx_FillTriangle(wx[0], wy[0], wx[1], wy[1], wx[3], wy[3]);
    //gfx_FillTriangle(wx[0], wy[0], wx[2], wy[2], wx[3], wy[3]);

    drawWall(wx[0], wx[1], wy[0], wy[1], wy[2], wy[3]);

    

};
void gametick() {

    //movement
    if (k_lf) {
        P.a -= 2;
        if (P.a < 0) {
            P.a += 360;
        }
    }

    if (k_rg) {
        P.a += 2;
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