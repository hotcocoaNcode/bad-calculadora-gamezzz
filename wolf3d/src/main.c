#include <ti/getcsc.h>
#include <graphx.h>
#include <keypadc.h>
#include <ti/screen.h>
#include <math.h>

bool k_up, k_dw, k_lf, k_rg, k_2n, k_al = false;
bool ccd = false;
bool lost = false;
#define PI 3.14159
#define PI2 PI / 2
#define PI3 3 * PI / 2
#define DRADS 0.017453 // single degree in radians

float px, py, pdx, pdy, pa;

// 19:23 in video 3

// 320x240 -> 106x48
// Much MUCH more managable for graphics...


typedef struct {
    int type; //static, item, enemy
    int state; // enable/disable
    int map;
    int x, y, z;
}sprite; sprite Saul;

const int SCWIDTH = 106;
const int SCHEIGHT = 48;
int SCALEFACTOR = 5;

void drawPixelScale5(int x, int y, int c) {
    gfx_SetColor(c);
    gfx_FillRectangle(x*5, y*5, 5, 5);
}

int mapX = 8, mapY = 8, mapS = 32;
int map[] =
{
 148,148,148,148,148,148,148,148,
 148,0,0,0,0,0,0,148,
 148,0,0,0,0,0,0,148,
 148,0,0,9,9,9,130,148,
 148,0,0,9,9,9,0,148,
 148,0,0,9,9,0,0,148,
 148,0,0,130,0,0,0,148,
 148,148,148,148,148,148,148,148,
};

int SaulTex[] = {
    74, 42, 65, 97, 97, 97, 97, 97, 97, 97, 97,97,97,196, 197, 229, 229,
74, 42, 65, 97, 97, 97, 237, 237,237, 237, 237,97,97,196, 197, 229, 229,
74, 42, 97, 237, 97, 237, 237, 237,237, 237, 237,97,97,97, 197, 229, 229,
74, 42, 97, 237, 238, 237, 237, 237,237, 237, 237,97,97,97, 197, 229, 229,
74, 42, 97, 237, 238, 238, 237, 237,237, 237, 237,237,237,97, 197, 229, 229,
74, 42, 97, 237, 238, 237, 237, 237,237, 237, 237,237,237,97, 197, 229, 229,
74, 42, 97, 237, 33, 0, 33, 237,237, 237, 33,0,33,97, 197, 229,229,
74, 42, 97, 237, 238, 237, 237, 237,237, 237, 237,237,237,97, 197, 229,229,
74, 42, 97, 237, 237, 237, 237, 237,237, 237, 237,237,237,97, 197, 229,229,
74, 42, 97, 237, 237, 237, 237, 237,237, 237, 237,237,237,97, 197, 229,229,
74, 42, 97, 237, 237, 237, 237, 237,237, 237, 237,237,237,97, 197, 229,229,
74, 42, 97, 237, 237, 237, 236, 236,236, 236, 236,237,237,97, 197, 229,229,
74, 42, 97, 237, 237, 237, 236, 236,236, 236, 236,237,237,97, 197, 229,229,
74, 42, 65, 237, 237, 237, 237, 237,237, 237, 237,237,237,196, 197, 229,229,
74, 42, 65, 65, 237, 237, 237, 237,237, 237, 237,237,196,196, 197, 229,229,
74, 42, 65, 65, 65, 237, 237, 237,237, 237, 237,196,196,196, 197, 229,229,
74, 42, 65, 65, 65, 237, 237, 237,237, 237, 237,196,196,196, 197, 229,229,
};

int depth[13];

#define dist(ax, ay, bx, by) sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay))

void drawSprite() {
    float sx = Saul.x - px;
    float sy = Saul.y - py;
    float sz = Saul.z;

    float CS = cos(-pa), SN = sin(-pa);
    float a = sy * CS + sx * SN;
    float b = sx * CS - sy * SN;
    sx = a; 
    sy = b;

    sx = (sx * 108 / sy) + GFX_LCD_WIDTH / 2;
    sy = (sz * 108 / sy) + GFX_LCD_HEIGHT / 2;

    int x, y;
    int scale = 16*80/b;

    if (scale < 0) {
        scale = 0;
    }

    if (scale > 32) {
        scale = 32; //keep fps managable
    }

    for (int i = -scale / 2; i < scale / 2; i++) {
        y = sy + i * 5;
        for (int i2 = -scale/2; i2 < scale/2; i2++) {
            x = sx + i2 * 5;
            if (x > 0 && x < GFX_LCD_WIDTH && b > depth[(int)x] /*i'm not sure how or why b > depth prevents the dot from drawing reverse from where it should but i am very thankful for it. as a wise man once said "it's a feature not a bug"*/ && b < depth[(int)x / 20]) {
                gfx_SetColor(224);
                gfx_FillRectangle(x - 2, y - 2, 5, 5);
            }
        }
    }
    
    
}


void drawRays3D() {
    int r, mx, my, mp, dof;
    float rx, ry, ra, xo, yo;


    //quick scaling fix
    float usePX = px * 2, usePY = py * 2;
    

    

    ra = pa - DRADS*8;
    if (ra < 0) {
        ra += PI * 2;
    }
    if (ra > 2 * PI) {
        ra -= PI * 2;
    }
    for (r = 0; r < 13; r++) {
        
        
        //HORIZONTAL CHECK
        dof = 0;
        float disH = 999999, hx = usePX, hy = usePY;
        float aTan = -1 / tan(ra);

        int walltypeh, walltypev = 0;

        if (ra>PI) { //up
            ry = (((int)usePY >> 6) << 6) - 0.0001;
            rx = (usePY - ry) * aTan + usePX;
            yo = -64;
            xo = -yo * aTan;
        }
        if (ra < PI) { //down
            ry = (((int)usePY >> 6) << 6) + 64;
            rx = (usePY - ry) * aTan + usePX;
            yo = 64;
            xo = -yo * aTan;
        }
        if (ra == 0 || ra == PI) { // left or right exactly
            rx = 999999;
            ry = 999999;
            dof = 8;
        }
        while (dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && map[mp]>0) {
                dof = 8;
                hx = rx;
                hy = ry;
                disH = dist(usePX, usePY, rx, ry);
                walltypeh = map[mp];
            }
            else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }
        

        //VERTICAL CHECK
        dof = 0;
        float disV = 999999, vx = usePX, vy = usePY;
        float nTan = -tan(ra);

        if (ra > PI2 && ra<PI3) { //right
            rx = (((int)usePX >> 6) << 6) - 0.0001;
            ry = (usePX - rx) * nTan + usePY;
            xo = -64;
            yo = -xo * nTan;
        }
        if (ra < PI2 || ra > PI3) { //left
            rx = (((int)usePX >> 6) << 6) + 64;
            ry = (usePX - rx) * nTan + usePY;
            xo = 64;
            yo = -xo * nTan;
        }
        if (ra == 0 || ra == PI) { //up or down exactly
            ry = 999999;
            rx = 999999;
            dof = 8;
        }
        while (dof < 8) {
            my = (int)(ry) >> 6;
            mx = (int)(rx) >> 6;
            mp = mx * mapY + my;
            if (mp > 0 && mp < mapY * mapX && map[mp]>0) {
                dof = 8;
                vx = rx;
                vy = ry;
                disV = dist(usePX, usePY, vx, vy);
                walltypev = map[mp];
            }
            else {
                ry += yo;
                rx += xo;
                dof += 1;
            }
        }

        int disT = 99999;
        if (disV < disH) {
            gfx_SetColor(walltypev-1);
            rx = vx;
            ry = vy;
            disT = disV;
        }
        if (disH < disV) {
            gfx_SetColor(walltypeh);
            rx = hx;
            ry = hy;
            disT = disH;
        }

        float ca = pa - ra;
        
        if (ca < 0) {
            ca -= 2 * PI;
        }

        if (ca > 2 * PI) {
            ca -= 2 * PI;
        }
        
        disT = disT * cos(ca); //anti fisheye

        float lineH = (64 * SCHEIGHT) / disT;
        if (lineH > SCHEIGHT) {
            lineH = SCHEIGHT;
        }

        depth[r] = disT;

        gfx_FillRectangle(r * 20, fabsf(lineH - SCHEIGHT), 20, lineH*5);
        ra += DRADS * 4;
        if (ra < 0) {
            ra += PI * 2;
        }
        if (ra > 2 * PI) {
            ra -= PI * 2;
        }
        
    }
    drawSprite();
}


void drawMap2D()
{
    int x, y, xo, yo;
    for (y = 0; y < mapY; y++) {
        for (x = 0; x < mapX; x++) {
            if (map[y*mapX+x]>0)
            {
                gfx_SetColor(255);
            }
            else {
                gfx_SetColor(0);
            }
            xo = x * (mapS);
            yo = y * (mapS);
            gfx_FillRectangle(xo+1, yo+1, mapS-1, mapS-1);
        }
    }
}

void drawPlayer() {
    gfx_SetColor(231);
    gfx_FillRectangle(px-2, py-2, 5, 5);
    gfx_Line(px, py, px + pdx * 5, py + pdy * 5);

    gfx_SetColor(224);
    gfx_FillRectangle(Saul.x - 2, Saul.y - 2, 5, 5);
}

void drawHud() {
    gfx_SetColor(255);
    gfx_FillRectangle(GFX_LCD_WIDTH - 60, 0, 60, GFX_LCD_HEIGHT);
}

void draw()
{
    gfx_FillScreen(74);
    gfx_SetColor(30);
    gfx_FillRectangle(0, 0, GFX_LCD_WIDTH, GFX_LCD_HEIGHT * 0.25);
    drawRays3D();
    drawHud();


    if (k_al) {
        drawMap2D();
        drawPlayer();
    }
};

void gametick() {

    //movement
    if (k_lf) {
        pa -= 0.1;
        if (pa < 0) {
            pa += 2 * PI;
        }
        pdx = cos(pa)*5;
        pdy = sin(pa)*5;
    }

    if (k_rg) {
        pa += 0.1;
        if (pa > 2*PI) {
            pa -= 2 * PI;
        }
        pdx = cos(pa) * 5;
        pdy = sin(pa) * 5;
    }


    int xo = 0;
    if (pdx < 0) {
        xo = -20;
    }
    else {
        xo = 20;
    }

    int yo = 0;
    if (pdy < 0) {
        yo = -20;
    }
    else {
        yo = 20;
    }

    int upx = px * 2;
    int upy = py * 2;

    int ipx = upx / 64.0, ipx_add_xo = (upx + xo) / 64.0, ipx_sub_xo = (upx - xo) / 64.0;
    int ipy = upy / 64.0, ipy_add_yo = (upy + yo) / 64.0, ipy_sub_yo = (upy - yo) / 64.0;

    if (k_up) {
        
        if (map[ipy * mapX + ipx_add_xo] == 0) { px += pdx; }
        if (map[ipy_add_yo * mapX + ipx] == 0) { py += pdy; }
        
    }

    if (k_dw) {
        if (map[ipy * mapX + ipx_sub_xo] == 0) { px -= pdx; }
        if (map[ipy_sub_yo * mapX + ipx] == 0) { py -= pdy; }
    }

    if (pdx < 0) {
        xo = -30;
    }
    else {
        xo = 30;
    }

    if (pdy < 0) {
        yo = -30;
    }
    else {
        yo = 30;
    }

    ipx = upx / 64.0, ipx_add_xo = (upx + xo) / 64.0;
    ipy = upy / 64.0, ipy_add_yo = (upy + yo) / 64.0;

    if (k_2n) {
        if (map[ipy_add_yo * mapX + ipx_add_xo] == 130) { map[ipy_add_yo * mapX + ipx_add_xo] = 0; }
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

    if (kb_Data[1] & kb_2nd) {
        k_2n = true;
    }
    else {
        k_2n = false;
    }

    if (kb_Data[2] & kb_Alpha) {
        k_al = true;
    }
    else {
        k_al = false;
    }

    if (kb_Data[6] & kb_Clear) {
    ccd = true;
    }
}

void init() {
    gfx_Begin();
    gfx_SetDrawBuffer();
    px = 40;
    py = 40;
    pa = 0;
    pdx = cos(pa) * 5;
    pdy = sin(pa) * 5;
    Saul.type = 1;
    Saul.x = 192;
    Saul.y = 40;
    Saul.z = 20;
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
        while (!os_GetCSC());
    }
    
    return 0;
}