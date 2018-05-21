#ifndef KING_PAWN.H
#define KING_PAWN.H

//#define SIMULATOR
typedef unsigned int uint32_t;
typedef unsigned char uint8_t, byte;
uint8_t backbuffer[1024]; // 128 * 64 /8

enum {B_RS = 0x01, B_RW = 0x02, B_SELECT = 0x04, B_CS1 = 0x08, B_CS2 = 0x10, B_RST = 0x20, B_E = 0x40}; 
enum {LCD_OFF = 0x3E, LCD_ON = 0x3F, LCD_SET_ADD = 0x40, LCD_BUSY = 0x80, LCD_SET_PAGE = 0xB8, LCD_DISP_START = 0xC0};
#define PAGE 8
#define ADD 63
#define SCREEN_WIDTH  127
#define SCREEN_HEIGHT 63
#define MAX_POINTS 100

typedef struct t_STK{
        uint32_t        CTRL;
        uint32_t        LOAD;
        uint32_t        VAL;
} tSTK;

#define STK (*((volatile tSTK*) 0xE000E010))

typedef struct t_GPIO{
        uint32_t        MODER;
        uint32_t        OTYPER;
        uint32_t        OSPEEDR;
        uint32_t        PUPDR;
        union{
                uint32_t        IDR;
                struct{
                        byte IDR_LOW;
                        byte IDR_HIGH;
                };
        };
        union{
                uint32_t      ODR;
                struct{
                        byte ODR_LOW;
                        byte ODR_HIGH;
                };
        };
} tGPIO;

#define GPIO_D (*((volatile tGPIO*) 0x40020C00))
#define GPIO_E (*((volatile tGPIO*) 0x40021000))

typedef struct tPoint {
        uint8_t x;
        uint8_t y;
} POINT;

typedef struct tGeometry {
        int numPoints;
        int sizeX;
        int sizeY;
        POINT px[MAX_POINTS];
} GEOMETRY, *PGEOMETRY;

typedef struct tObj {
        PGEOMETRY geo;
        int dirX, dirY;
        int posX, posY;
        void(*draw) (struct tObj *);
        void(*clear) (struct tObj *);
        void(*move) (struct tObj *);
        void(*set_speed) (struct tObj *, int, int);
} OBJECT, *POBJECT;

void delay_micro(uint32_t us);
void delay_milli(uint32_t ms);
void delay_250ns(void);
void delay_500ns(void);

uint8_t graphic_read_data(uint8_t controller);
uint8_t graphic_read(uint8_t controller);
void graphic_ctrl_bit_set(uint8_t x);
void graphic_ctrl_bit_clear(uint8_t x);
void graphic_wait_ready(void);
void graphic_write(uint8_t value, uint8_t controller);
void graphic_write_command(uint8_t command, uint8_t controller);
void graphic_write_data(uint8_t data, uint8_t controller);
void graphic_initialize(void);
void graphic_clear_screen(void);
void graphic_draw_screen(void); // Backbuffer
void select_cotroller(uint8_t controller);
void clear_backbuffer(void); // Backbuffer
void pixel(int x, int y); // Backbuffer


extern OBJECT ball;
extern OBJECT leftPaddle;
extern OBJECT rightPaddle;
void move_ball(POBJECT);
void move_paddle(POBJECT);
void draw_object(POBJECT);
void clear_object(POBJECT);
void set_object_speed(POBJECT this, int x, int y);
void activateRow(uint32_t row);
uint8_t readColumn(void);
uint8_t * keyb(void);

void ascii_ctrl_bit_set(uint32_t x);
void ascii_ctrl_bit_clear(uint32_t x);
void ascii_write_controller(uint8_t c);
uint8_t ascii_read_controller();
void ascii_write_cmd(uint8_t command);
void ascii_write_data(uint8_t data);
uint8_t ascii_read_status(void);
uint8_t ascii_read_data(void);
void ascii_command(char command, uint32_t tid);
void ascii_init(void);
void ascii_write_char(uint8_t charToWrite);
void goToXY(uint8_t row, uint8_t column);
void init_app(void);

#endif // KING_PAWN.H
