#include "king_pawn.h"

void startup(void) __attribute__((naked)) __attribute__((section (".start_section")) );

void startup ( void ){ // Clear
__asm volatile(
        " LDR R0,=0x2001C000\n"         /* set stack */
        " MOV SP,R0\n"
        " BL main\n"                            /* call main */
        ".L1: B .L1\n"                          /* never return */
        ) ;
}

OBJECT ball;
OBJECT leftPaddle;
OBJECT rightPaddle;

void main(int argc, char **argv) {
                      int i = 0;
        // Load all game objects
        POBJECT p = &ball;
        POBJECT pl = &leftPaddle;
        POBJECT pr = &rightPaddle;
        
        init_app();
        graphic_initialize();
#ifndef SIMULATOR
        graphic_clear_screen();
#endif
        ascii_init();
        graphic_ctrl_bit_clear(B_SELECT);
        // Set speed for ball
        p->set_speed(p, 2, 2);
        // Game loop
        while(1) {
                pl->set_speed(pl,0,0);
                pr->set_speed(pr,0,0);
                ascii_ctrl_bit_set(2);
                goToXY(1,2);
                char *s;
                char test1[] = "TIELL-1 Lab 5 / Pong";
                s = test1;
                while(*s) {
                        ascii_write_char(*s++);
                }
                graphic_ctrl_bit_clear(B_SELECT);
                // </Write to screen>
                                           uint8_t * pressedKeys = keyb();
                //delay_milli(30);
                for(uint8_t i = 0; i < 4; i++) {
                        switch(pressedKeys[i]) {
                                case 0x01: pl->set_speed(pl,0,-5); break;
                                case 0x04: pl->set_speed(pl,0,5); break;
                                case 0x0C: pr->set_speed(pr,0,-5); break;
                                case 0x0D: pr->set_speed(pr,0,5); break;
                        }
                }
                                           clear_backbuffer();
                p->move(p);
                pl->move(pl);
                pr->move(pr);
                                    graphic_write_command(LCD_ON, B_CS1|B_CS2);
                                           graphic_draw_screen(); 
                delay_milli(30);
                ascii_init();
        }
}
void move_ball(POBJECT this) {
        clear_object(this);
        // Move ball
        this->posX += this->dirX;
        this->posY += this->dirY;
        // Wall collisions
        if(this->posY < 0) {
                this->posY = 0;
                this->dirY = -this->dirY;
        } else if(this->posY > 63) {
                this->posY = 63;
                this->dirY = -this->dirY;
        }
        // Define left and right paddle
        POBJECT lp = &leftPaddle;
        POBJECT rp = &rightPaddle;
        // Collision left paddle
        if(this->posX <= lp->posX + lp->geo->sizeX) {
                if((this->posY >= lp->posY) && (this->posY <= lp->posY + lp->geo->sizeY)) {
                        this->posX = lp->posX + lp->geo->sizeX + 1;
                        this->dirX = -this->dirX;
                } else {
                        ascii_ctrl_bit_set(2);
                        goToXY(1,1);
                        char *s;
                        char test1[] = "   Player 2 wins!";
                        s = test1;
                        while(*s) {
                                ascii_write_char(*s++);
                        }
                        graphic_ctrl_bit_clear(B_SELECT);
                        // </Write to screen>
                        delay_milli(500);
                        this->posX = 63;
                        this->dirX = -this->dirX;
                        this->dirY = -this->dirY;
                }
        }
        // Collision right paddle
        if(this->posX >= rp->posX) {
                if((this->posY >= rp->posY) && (this->posY <= rp->posY + rp->geo->sizeY)) {
                        this->posX = rp->posX - 1;
                        this->dirX = -this->dirX;
                } else {
                        // Right player lost
                        // <Write to screen>
                        ascii_ctrl_bit_set(2);
                        goToXY(1,1);
                        char *s;
                        char test1[] = "   Player 1 wins!";
                        s = test1;
                        while(*s) {
                                ascii_write_char(*s++);
                        }
                        graphic_ctrl_bit_clear(B_SELECT);
                        // </Write to screen>
                        delay_milli(500);
                        this->posX = 63;
                        this->dirX = -this->dirX;
                        this->dirY = -this->dirY;
                }
        }
        this->draw(this);
}
void move_paddle(POBJECT this) {
        clear_object(this);
        
        // Move paddle in y direction only.
        this->posY += this->dirY;
        
        // Wall collisions todo add width check with paddle.
        if(this->posY < 0) {
                this->posY = 0;
        } else if((this->posY + this->geo->sizeY) > 63) {
                this->posY = (63 - this->geo->sizeY);
        }
        
        this->draw(this);
}
GEOMETRY ball_geometry = { // Clear
        20,
        4, 4,
        {
                                                                 {0, 1}, {0, 2}, 
                {1, 0}, {1, 1}, {1, 2}, {1, 3}, 
                {2, 0}, {2, 1}, {2, 2}, {2, 3},
                                                                 {3, 1}, {3, 2}
           }
};
OBJECT ball = {      // Clear
        &ball_geometry,
        0, 0,
        63, 32,
        draw_object,
        clear_object,
        move_ball,
        set_object_speed
};

GEOMETRY paddle_geometry = { // Clear
           100,
           4, 25,
           {
                                {0, 0},  {1, 0}, 
                                {0, 1},  {1, 1}, 
                                {0, 2},  {1, 2}, 
                                {0, 3},  {1, 3},  
                                {0, 4},  {1, 4},  
                                {0, 5},  {1, 5},  
                                {0, 6},  {1, 6},  
                                {0, 7},  {1, 7}, 
                                {0, 8},  {1, 8},  
                                {0, 9},  {1, 9},  
                                {0, 10}, {1, 10},
                                {0, 11}, {1, 11}, 
                                {0, 12}, {1, 12},
                                {0, 13}, {1, 13}, 
                                {0, 14}, {1, 14}, 
                                {0, 15}, {1, 15},
                                {0, 16}, {1, 16}, 
                                {0, 17}, {1, 17}, 
                                {0, 18}, {1, 18}, 
                                {0, 19}, {1, 19}, 
                                {0, 20}, {1, 20},
           }
};

OBJECT leftPaddle = { // Clear
        &paddle_geometry,
        0, 0,
        1, 32,
        draw_object,
        clear_object,
        move_paddle,
        set_object_speed
};
OBJECT rightPaddle = { // Clear
        &paddle_geometry,
        0, 0,
        126, 32,
        draw_object,
        clear_object,
        move_paddle,
        set_object_speed
};
void init_app(void) {
        #ifdef USBDM
                  *((unsigned long *)0x40023830)=0x18; /* starta klockor port D och E */
                  __asm volatile(" LDR R0,=0x08000209\n BLX R0\n"); /* initierar PLL */
        #endif
        GPIO_E.MODER = 0x55555555;
        GPIO_D.MODER = 0x55000000;
        GPIO_D.OTYPER &= 0x0000FFFF;   
        GPIO_D.OTYPER |= 0x00000000;     
        GPIO_D.PUPDR = 0x00AA0000;
        GPIO_D.OTYPER = 0x00000000;
}
