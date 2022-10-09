#include <ti/getcsc.h>
#include <graphx.h>
#include <keypadc.h>
#include <ti/screen.h>
#include <math.h>

bool k_up, k_dw, k_lf, k_rg = false;
bool ccd = false;
bool lost = false;
const float pi = 3.14159;
const float pi2 = pi / 2;
const float pi3 = 3 * pi / 2;

float px, py, pdx, pdy, pa;

// 11:44 in video 1

// 320x240 -> 106x48
// Much MUCH more managable for graphics...

int SCWIDTH = 106;
int SCHEIGHT = 48;
int SCALEFACTOR = 5;

void drawPixelScale5(int x, int y, int c) {
    gfx_SetColor(c);
    gfx_FillRectangle(x*5, y*5, 5, 5);
}

int mapX = 8, mapY = 8, mapS = 32;
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

float dist(float ax, float ay, float bx, float by, float ang) {
    //PYTAGORAS TEROEM
    //WEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
    return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

void drawRays3D() {
    int r, mx, my, mp, dof;
    float rx, ry, ra, xo, yo;


    //quick scaling fix
    float usePX = px * 2, usePY = py * 2;
    

    ra = pa;
    for (r = 0; r < 1; r++) {
        
        
        //HORIZONTAL CHECK
        dof = 0;
        float aTan = -1 / tan(ra);

        if (ra>pi) { //up
            ry = (((int)usePY >> 6) << 6) - 0.0001;
            rx = (usePY - ry) * aTan + usePX;
            yo = -64;
            xo = -yo * aTan;
        }
        if (ra < pi) { //down
            ry = (((int)usePY >> 6) << 6) + 64;
            rx = (usePY - ry) * aTan + usePX;
            yo = 64;
            xo = -yo * aTan;
        }
        if (ra == 0 || ra == pi) { // left or right exactly
            rx = usePX;
            ry = usePY;
            dof = 8;
        }
        while (dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                dof = 8;
            }
            else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }
        gfx_SetColor(103);
        gfx_Line(usePX/2, usePY / 2, rx/2, ry/2);
        

        //VERTICAL CHECK
        dof = 0;
        float nTan = -tan(ra);

        if (ra > pi2 && ra<pi3) { //right
            rx = (((int)usePX >> 6) << 6) - 0.0001;
            ry = (usePX - rx) * nTan + usePY;
            xo = -64;
            yo = -xo * nTan;
        }
        if (ra < pi2 || ra > pi3) { //left
            rx = (((int)usePX >> 6) << 6) + 64;
            ry = (usePX - rx) * nTan + usePY;
            xo = 64;
            yo = -xo * nTan;
        }
        if (ra == 0 || ra == pi) { //up or down exactly
            ry = usePY;
            rx = usePX;
            dof = 8;
        }
        while (dof < 8) {
            my = (int)(ry) >> 6;
            mx = (int)(rx) >> 6;
            mp = mx * mapY + my;
            if (mp > 0 && mp < mapY * mapX && map[mp] == 1) {
                dof = 8;
            }
            else {
                ry += yo;
                rx += xo;
                dof += 1;
            }
        }
        gfx_SetColor(224);
        gfx_Line(usePX / 2, usePY / 2, rx / 2, ry / 2);
    }
}

void drawMap2D()
{
    int x, y, xo, yo;
    for (y = 0; y < mapY; y++) {
        for (x = 0; x < mapX; x++) {
            if (map[y*mapX+x]==1)
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
}

void draw()
{
    gfx_FillScreen(74);
    drawMap2D();
    drawPlayer();
    drawRays3D();
};

void gametick() {

    //movement
    if (k_lf) {
        pa -= 0.1;
        if (pa < 0) {
            pa += 2 * pi;
        }
        pdx = cos(pa)*5;
        pdy = sin(pa)*5;
    }

    if (k_rg) {
        pa += 0.1;
        if (pa > 2*pi) {
            pa -= 2 * pi;
        }
        pdx = cos(pa) * 5;
        pdy = sin(pa) * 5;
    }

    if (k_up) {
        px += pdx;
        py += pdy;
    }

    if (k_dw) {
        px -= pdx;
        py -= pdy;
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
    px = 30;
    py = 30;
    pa = 0;
    pdx = cos(pa) * 5;
    pdy = sin(pa) * 5;
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