#include "king_pawn.h"

void ascii_ctrl_bit_set(uint32_t x) {
        switch(x) {
                case 0: GPIO_E.ODR_LOW |= 1; break;
                case 1: GPIO_E.ODR_LOW |= 2; break;
                case 2: GPIO_E.ODR_LOW |= 4; break;
                case 3: GPIO_E.ODR_LOW |= 8; break;
                case 4: GPIO_E.ODR_LOW |= 16; break;
                case 5: GPIO_E.ODR_LOW |= 32; break;
                case 6: GPIO_E.ODR_LOW |= 64; break;
                case 7: GPIO_E.ODR_LOW |= 128; break;
        }
}

void ascii_ctrl_bit_clear(uint32_t x) {
        switch(x) {
                case(0): GPIO_E.ODR_LOW &= 0xFE; break;
                case(1): GPIO_E.ODR_LOW &= 0xFD; break;
                case(2): GPIO_E.ODR_LOW &= 0xFB; break;
                case(3): GPIO_E.ODR_LOW &= 0xF7; break;
                case(4): GPIO_E.ODR_LOW &= 0xEF; break;
                case(5): GPIO_E.ODR_LOW &= 0xDF; break;
                case(6): GPIO_E.ODR_LOW &= 0xBF; break;
                case(7): GPIO_E.ODR_LOW &= 0x7F; break;
        }
}

void ascii_write_controller(uint8_t c) {
        ascii_ctrl_bit_set(6);
        GPIO_E.ODR_HIGH = c;
        delay_250ns();
        ascii_ctrl_bit_clear(6);
}


uint8_t ascii_read_controller(void) {
        uint8_t rv;
        ascii_ctrl_bit_set(6);
        delay_250ns();
        delay_250ns();
        rv = GPIO_E.IDR_HIGH;
        ascii_ctrl_bit_clear(6);
        return rv;
}

void ascii_write_cmd(uint8_t command) {
        ascii_ctrl_bit_clear(0);
        ascii_ctrl_bit_clear(1);
        ascii_write_controller(command);
}

void ascii_write_data(uint8_t data) {
        ascii_ctrl_bit_set(0);
        ascii_ctrl_bit_clear(1);
        ascii_write_controller(data);
}

uint8_t ascii_read_status(void) {
        GPIO_E.MODER &= 0x0000FFFF;     
        ascii_ctrl_bit_clear(0);
        ascii_ctrl_bit_set(1);
        uint8_t rv = ascii_read_controller();
        //GPIO_E.MODER &= 0x0000FFFF;
        GPIO_E.MODER |= 0x55550000;     
        return rv;
}

uint8_t ascii_read_data(void) {
        GPIO_E.MODER &= 0x0000FFFF;     
        ascii_ctrl_bit_set(0);
        ascii_ctrl_bit_set(1);
        uint8_t rv = ascii_read_controller();
        //GPIO_E.MODER &= 0x0000FFFF;
        GPIO_E.MODER |= 0x55550000;     
        return rv;
}

void ascii_command(char command, uint32_t tid) {
        while((ascii_read_status() & 0x80) == 0x80) {
                // Do nothing, wait for status flag
        }
        delay_micro(8);
        ascii_write_cmd(command);
        delay_micro(tid);
}

void ascii_init(void) {
           ascii_ctrl_bit_set(2);
           ascii_command(0x38, 40); // Set display size and font size
           ascii_command(0xE, 40); // Set display, cursor on
           ascii_command(0x01, 1530); // Clear display
           ascii_command(0x06, 40); // Inc, no shift
}

void ascii_write_char(uint8_t charToWrite) {
        while((ascii_read_status() & 0x80) == 0x80) {
        // Do nothing, wait for status flag
    }
        delay_micro(8);
    ascii_write_data(charToWrite);
    delay_micro(45);
}

void goToXY(uint8_t row, uint8_t column) {
        uint8_t address = row - 1;
        if(column == 2) {
                address = address + 0x40;
        }
        ascii_write_cmd(0x80 | address);
}

void activateRow(uint32_t row) {
        switch(row) {
                case 0: GPIO_D.ODR_HIGH = 0x00; break;
                case 1: GPIO_D.ODR_HIGH = 0x10; break;
                case 2: GPIO_D.ODR_HIGH = 0x20; break;
                case 3: GPIO_D.ODR_HIGH = 0x40; break;
                case 4: GPIO_D.ODR_HIGH = 0x80; break;
        }
}

uint8_t readColumn(void) {
        uint8_t idr = GPIO_D.IDR_HIGH;
        if(idr & 0x01) return 1;
        if(idr & 0x02) return 2;
        if(idr & 0x04) return 3;
        if(idr & 0x08) return 4;
        return 0;
}
uint8_t keys[] = {0x1,0x2,0x3,0xA,0x4,0x5,0x6,0xB,0x7,0x8,0x9,0xC,0xE,0x0,0xF,0xD};
uint8_t * keyb(void) {
        //uint8_t keys[] = {0x1,0x2,0x3,0xA,0x4,0x5,0x6,0xB,0x7,0x8,0x9,0xC,0xE,0x0,0xF,0xD};
        static uint8_t returnKeys[4] = {0xFF,0xFF,0xFF,0xFF};
        for(char row = 1; row <= 4; row++) {
                activateRow(row);
                delay_250ns();
                char column = readColumn();
                if(column != 0) {
                        returnKeys[row-1] = keys[4 * (row - 1) + (column - 1)];
                } else {
                        returnKeys[row-1] = 0xFF;
                }
        }
        activateRow(0);
        return returnKeys;
}

void draw_object(POBJECT this) {
        for(int i = 0; i < MAX_POINTS; i++) {
                pixel(this->geo->px[i].x + this->posX, this->geo->px[i].y + this->posY); //, 1);
        }
}

void clear_object(POBJECT this) {
        for(int i = 0; i < MAX_POINTS; i++) {
                pixel(this->geo->px[i].x + this->posX, this->geo->px[i].y + this->posY); //, 0);
        }
}

void set_object_speed(POBJECT this, int x, int y) {
        this->dirX = x;
        this->dirY = y;
}

void delay_250ns(void) {
        STK.CTRL = 0;
        STK.LOAD = ((168/4) -1); // Have to add one as said in manual
        STK.VAL = 0;
        STK.CTRL = 5;
        while((STK.CTRL & 0x10000) == 0) {
                // Do nothing :S
        }
        STK.CTRL = 0;
}

void delay_micro(uint32_t us) {
        while(us--) {
                delay_250ns();
                delay_250ns();
                delay_250ns();
                delay_250ns();
        }
}

void delay_milli(uint32_t ms){
        #ifdef SIMULATOR
        while (ms--){
                delay_micro(1);
        }
        #else
        while(ms--){
                delay_micro(1000);
        }
#endif
}

void delay_500ns( void ){
#ifndef SIMULATOR
        delay_250ns();
        delay_250ns();
#endif  
}

void graphic_ctrl_bit_set(uint8_t x){
        uint8_t c;
        c = GPIO_E.ODR_LOW;
        c &= ~B_SELECT;
        c |= (~B_SELECT & x);
        GPIO_E.ODR_LOW = c;
}
void graphic_ctrl_bit_clear(uint8_t x){
        uint8_t c;
        c = GPIO_E.ODR_LOW;
        c &= ~B_SELECT;
        c &= ~x;
        GPIO_E.ODR_LOW = c;
}

void select_controller(uint8_t controller){
        switch(controller){
                case 0:
                        graphic_ctrl_bit_clear(B_CS1|B_CS2);
                        break;
                case B_CS1:
                        graphic_ctrl_bit_set(B_CS1);
                        graphic_ctrl_bit_clear(B_CS2);
                        break;
                case B_CS2:
                        graphic_ctrl_bit_set(B_CS2);
                        graphic_ctrl_bit_clear(B_CS1);                  
                        break;
                case B_CS1|B_CS2:
                        graphic_ctrl_bit_set(B_CS1|B_CS2);
                        break;
        }
}

void graphic_wait_ready(void){
    graphic_ctrl_bit_clear(B_E);
    GPIO_E.MODER = 0x00005555;
    graphic_ctrl_bit_clear(B_RS);
    graphic_ctrl_bit_set(B_RW);
    delay_500ns();
    uint8_t busy;
    while(1) { // Wait for display not to be busy
        graphic_ctrl_bit_set(B_E);
        delay_500ns();
                busy = GPIO_E.IDR_HIGH;
        graphic_ctrl_bit_clear(B_E);
        delay_500ns();
        //uint8_t i = *GPIO_IDR_HIGH;
        if((busy & LCD_BUSY) == 0) {
            break;
        }
    }
    graphic_ctrl_bit_set(B_E);
    GPIO_E.MODER = 0x55555555;
}

uint8_t graphic_read(uint8_t controller){
    graphic_ctrl_bit_clear(B_E);
    GPIO_E.MODER = 0x00005555;
    graphic_ctrl_bit_set(B_RS | B_RW);
    select_controller(controller);
    delay_500ns();
    graphic_ctrl_bit_set(B_E);
    delay_500ns();
    uint8_t RV = GPIO_E.IDR_HIGH;
    graphic_ctrl_bit_clear(B_E);
    GPIO_E.MODER = 0x55555555;
    if(controller == B_CS1){
        select_controller(B_CS1);
        graphic_wait_ready();
    }
    if(controller == B_CS2){
        select_controller(B_CS2);
        graphic_wait_ready();
    }    
    return RV;
}

void graphic_write(uint8_t value, uint8_t controller) {
        GPIO_E.ODR_HIGH = value;
        select_controller(controller);
        delay_500ns();
        graphic_ctrl_bit_set(B_E);
        delay_500ns();
        graphic_ctrl_bit_clear( B_E );
        if(controller & B_CS1) {
                select_controller( B_CS1);
                graphic_wait_ready();
        }
        if(controller & B_CS2) {
                select_controller( B_CS2);
                graphic_wait_ready();
        }
}

void graphic_write_command(uint8_t command, uint8_t controller){
    graphic_ctrl_bit_clear(B_E);
    select_controller(controller);
    graphic_ctrl_bit_clear(B_RS | B_RW);
    graphic_write(command, controller);
}


void graphic_write_data(uint8_t data, uint8_t controller){
    graphic_ctrl_bit_clear(B_E);
    select_controller(controller);
    graphic_ctrl_bit_set(B_RS);
    graphic_ctrl_bit_clear(B_RW);
    graphic_write(data, controller);
}

uint8_t graphics_read_data(uint8_t controller){
    (void) graphic_read(controller);
    return graphic_read(controller);
}

void graphic_initialize(void){
    graphic_ctrl_bit_set(B_E);
    graphic_ctrl_bit_set(B_SELECT);
    delay_micro(10);
    graphic_ctrl_bit_clear(B_CS1 | B_CS2 | B_RST | B_E);
    delay_milli(30);
    graphic_ctrl_bit_set(B_RST);
    graphic_write_command(LCD_OFF, B_CS1|B_CS2);
    graphic_write_command(LCD_ON, B_CS1|B_CS2);
    graphic_write_command(LCD_DISP_START, B_CS1|B_CS2);
    graphic_write_command(LCD_SET_ADD, B_CS1|B_CS2);
    graphic_write_command(LCD_SET_PAGE, B_CS1|B_CS2);
    select_controller(0);
}

void graphic_clear_screen(void) {
        uint8_t i, j;
        for(j = 0; j < 8; j++) {
                graphic_write_command(LCD_SET_PAGE | j, B_CS1|B_CS2);
                graphic_write_command(LCD_SET_ADD | 0, B_CS1|B_CS2);
                for(i = 0; i <= 63; i++){
                        graphic_write_data(0, B_CS1|B_CS2);
                }
        }
}
void graphic_draw_screen(void)
{
           uint8_t i, j, controller, c;
           uint32_t k = 0;
           
           for(c = 0; c < 2; c++) 
           {
                      controller = (c == 0) ? B_CS1 : B_CS2;
                      for(j = 0; j < 8; j++) 
                      {
                                graphic_write_command(LCD_SET_PAGE | j, controller);
                                graphic_write_command(LCD_SET_ADD  | 0, controller);
                                for(i = 0; i <= 63; i++, k++) 
                                {
                                           graphic_write_data(backbuffer[k], controller);
                                }
                      }
           }
}
void clear_backbuffer(void)
{
           int i;
           for( i = 0; i < 1024; i++)
           {
                      backbuffer[i] = 0;
           }
}
void pixel(int x, int y)
{
           uint8_t mask;
           int index = 0;
           if((x>128)||(x<1)||(y>64)||(y<1)) return;
           
           mask = 1 << ((y-1)%8);
           
           if(x>64)
           {
                      x-=65;
                      index = 512;
           }
           index += x + ((y-1)/8)*64;
           
           backbuffer[index] |= mask;
}
