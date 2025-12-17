#include "T5LOS8051.h"

/* ---------------------------------------------------------------
 * Типы
 * ------------------------------------------------------------- */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef signed   short s16;
typedef unsigned long  u32;
typedef signed   long  s32;

/* ---------------------------------------------------------------
 * Адреса VP (регистры из ТЗ) и системные адреса DGUS
 * ------------------------------------------------------------- */

/* Дата/время */
#define VP_YEAR        0x2010
#define VP_MONTH       0x2011
#define VP_DAY         0x2012
#define VP_WEEKDAY     0x2013
#define VP_HOUR        0x2014
#define VP_MIN         0x2015
#define VP_SEC         0x2016

/* Системный RTC DGUS */
#define OS_RTC         0x0010

/* График 1 */
#define VP_G1_STATUS   0x2020
#define VP_G1_DAYS     0x2021
#define VP_G1_SPEED    0x2022
#define VP_G1_TEMP     0x2023
#define VP_G1_SHOUR    0x2024
#define VP_G1_SMIN     0x2025
#define VP_G1_EHOUR    0x2026
#define VP_G1_EMIN     0x2027

/* График 2 */
#define VP_G2_STATUS   0x2030
#define VP_G2_DAYS     0x2031
#define VP_G2_SPEED    0x2032
#define VP_G2_TEMP     0x2033
#define VP_G2_SHOUR    0x2034
#define VP_G2_SMIN     0x2035
#define VP_G2_EHOUR    0x2036
#define VP_G2_EMIN     0x2037

/* График 3 */
#define VP_G3_STATUS   0x2040
#define VP_G3_DAYS     0x2041
#define VP_G3_SPEED    0x2042
#define VP_G3_TEMP     0x2043
#define VP_G3_SHOUR    0x2044
#define VP_G3_SMIN     0x2045
#define VP_G3_EHOUR    0x2046
#define VP_G3_EMIN     0x2047

/* График 4 */
#define VP_G4_STATUS   0x2050
#define VP_G4_DAYS     0x2051
#define VP_G4_SPEED    0x2052
#define VP_G4_TEMP     0x2053
#define VP_G4_SHOUR    0x2054
#define VP_G4_SMIN     0x2055
#define VP_G4_EHOUR    0x2056
#define VP_G4_EMIN     0x2057

/* Основные регистры работы */
#define VP_ONOFF       0x2060   /* 1 – выкл, 2 – вкл */
#define VP_SPEED       0x2061   /* 1..4 */
#define VP_MODE        0x2062   /* 1 – охл, 2 – нагрев */
#define VP_T_REAL_10X  0x2063   /* *10 */
#define VP_T_REAL_INT  0x2064
#define VP_T_SET_10X   0x2065   /* *10 */
#define VP_T_SET_INT   0x2066
#define VP_GRAPH_EN    0x2067

/* Настройка Modbus */
#define VP_MB_ADDR     0x2070   /* 1..247 */
#define VP_MB_SPEED    0x2071   /* 1..7 */
#define VP_MB_PROTO    0x2072   /* 1..6 */

/* Системный АЦП */
#define OS_AD_VALUE    0x0032

/* Калибровка температуры (как в tttt) */
#define VP_T_COEF       0x2550   /* 3 слова: K1, K2, C */
#define VP_T_PRECISION  0x2554   /* 0 – 0.5°C, 1 – 1.0°C */

/* ---------------------------------------------------------------
 * Аппаратные линии
 * ------------------------------------------------------------- */

sbit RELAY_VALVE = P1^3;
sbit RELAY_LOW   = P1^1;
sbit RELAY_HIGH  = P1^2;
sbit RELAY_MID   = P1^4;
sbit RS485_TX_EN = P0^1;   /* Управление направлением RS485 */

#define RELAY_VALVE_BIT  (1u << 3)
#define RELAY_LOW_BIT    (1u << 1)
#define RELAY_HIGH_BIT   (1u << 2)
#define RELAY_MID_BIT    (1u << 4)

/* ---------------------------------------------------------------
 * Константы
 * ------------------------------------------------------------- */

#define TEMP_HYST_10X 10
#define NTC_TABLE_SIZE 100
#define FOSC     206438400UL
#define T1MS     (65536UL - FOSC/12UL/1000UL)

/* Modbus */
#define MODBUS_RX_BUF_SIZE 64
#define MODBUS_TX_BUF_SIZE 64
#define MODBUS_TIMEOUT_MS  10   /* таймаут между байтами (3.5 символа при 115200) */

/* Modbus функции */
#define MB_FUNC_READ_REGS      0x03
#define MB_FUNC_WRITE_REG      0x06
#define MB_FUNC_WRITE_REGS     0x10

/* Modbus ошибки */
#define MB_ERR_ILLEGAL_FUNC    0x01
#define MB_ERR_ILLEGAL_ADDR    0x02
#define MB_ERR_ILLEGAL_VALUE   0x03

/* ---------------------------------------------------------------
 * Таблицы NTC
 * ------------------------------------------------------------- */

static const u16 code NTC1_TABLE[NTC_TABLE_SIZE] =
{
    32040,30490,29022,27633,26317,25071,23889,22769,21707,20700,
    19788,18838,17977,17160,16383,15646,14945,14280,13647,13045,
    12472,11928,11409,10916,10447,10000, 9574, 9168, 8781, 8413,
     8062, 7727, 7407, 7103, 6812, 6534, 6270, 6017, 5775, 5545,
     5324, 5114, 4913, 4720, 4536, 4360, 4192, 4031, 3877, 3730,
     3572, 3454, 3324, 3201, 3082, 2968, 2859, 2755, 2654, 2558,
     2466, 2378, 2293, 2212, 2134, 2059, 1987, 1918, 1851, 1788,
     1726, 1668, 1611, 1557, 1504, 1454, 1406, 1359, 1314, 1271,
     1230, 1190, 1151, 1114, 1079, 1045, 1011,  980,  949,  919,
      891,  863,  837,  811,  786,  763,  740,  718,  696,  675
};

static const u16 code NTC2_TABLE[NTC_TABLE_SIZE] =
{
    32800,31141,29572,28090,26690,25366,24116,22935,21820,20767,
    19773,18835,17949,17113,16323,15578,14874,14208,13579,12983,
    12419,11885,11378,10896,10437,10000, 9513, 9064, 8649, 8263,
     7904, 7570, 7256, 6960, 6682, 6420, 6170, 5932, 5707, 5491,
     5284, 5085, 4895, 4712, 4536, 4367, 4204, 4047, 3896, 3750,
     3610, 3475, 3346, 3221, 3101, 2987, 2876, 2771, 2670, 2573,
     2480, 2391, 2307, 2226, 2148, 2074, 2004, 1936, 1871, 1809,
     1750, 1693, 1586, 1535, 1486, 1439, 1393, 1348, 1305, 1262,
     1220, 1178, 1137, 1097, 1056, 1023,  991,  960,  931,  903,
      876,  850,  825,  801,  777,  755,  733,  713,  692,  673
};

static const u8 code table_week[12] = {0,3,3,6,1,4,6,2,5,0,3,5};
static const u8 code mon_table[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

/* Таблица CRC16 Modbus (High byte) */
static const u8 code CRC_HI[] = {
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
    0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40
};

/* Таблица CRC16 Modbus (Low byte) */
static const u8 code CRC_LO[] = {
    0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,0x04,
    0xCC,0x0C,0x0D,0xCD,0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,0x08,0xC8,
    0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC,
    0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,0x11,0xD1,0xD0,0x10,
    0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4,
    0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,0x3B,0xFB,0x39,0xF9,0xF8,0x38,
    0x28,0xE8,0xE9,0x29,0xEB,0x2B,0x2A,0xEA,0xEE,0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C,
    0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,
    0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,0x62,0x66,0xA6,0xA7,0x67,0xA5,0x65,0x64,0xA4,
    0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
    0x78,0xB8,0xB9,0x79,0xBB,0x7B,0x7A,0xBA,0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,
    0xB4,0x74,0x75,0xB5,0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,
    0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,
    0x9C,0x5C,0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,0x99,0x59,0x58,0x98,
    0x88,0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
    0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,0x40
};

/* ---------------------------------------------------------------
 * Структуры
 * ------------------------------------------------------------- */

typedef struct
{
    u8  year;
    u8  month;
    u8  day;
    u8  weekday;
    u8  hour;
    u8  min;
    u8  sec;
} RtcTime;

typedef struct
{
    u8  active;
    u8  prev_speed;
    u16 prev_t_set;
} GraphRuntime;

typedef struct
{
    u8  status;
    u8  days;
    u8  speed;
    u8  temp_int;
    u8  shour;
    u8  smin;
    u8  ehour;
    u8  emin;
} GraphConfig;

/* ---------------------------------------------------------------
 * Глобальные переменные
 * ------------------------------------------------------------- */

static RtcTime      g_rtc = {25, 1, 25, 6, 0, 0, 0}; /* год=25 (2025), месяц=1, день=25, суббота */
static u16   data   SysTick       = 0;
static u16   data   SysTick_RTC   = 0;
static bit          g_rtc_1s_flag = 0;

static u8           g_onoff   = 2;
static u8           g_speed   = 1;
static u8           g_mode    = 2;
static u16          g_t_set_10x  = 220;
static u16          g_t_real_10x = 220;
static bit          g_valve_open = 1;
static u8           g_graph_en   = 1;

static GraphConfig  g_graph_cfg[4];
static GraphRuntime g_graph_rt[4];


/* Modbus */
static u8           g_mb_addr = 247;    /* Адрес устройства */
static u8           g_mb_speed_idx = 7; /* Индекс скорости (7=115200) */
static u8           g_mb_proto = 1;     /* Протокол (1=8N1) */

static u8  xdata    g_mb_rx_buf[MODBUS_RX_BUF_SIZE];
static u8           g_mb_rx_cnt = 0;
static u8  xdata    g_mb_tx_buf[MODBUS_TX_BUF_SIZE];
static u8           g_mb_rx_timeout = 0;
static bit          g_mb_frame_ready = 0;

/* ---------------------------------------------------------------
 * Инициализация системы
 * ------------------------------------------------------------- */

static void init_system(void)
{
    EA = 0;

    PAGESEL   = 0x01;
    D_PAGESEL = 0x02;
    MUX_SEL   = 0x60;
    RAMMODE   = 0x00;
    PORTDRV   = 0x01;

    IEN0 = 0x00;
    IEN1 = 0x00;
    IEN2 = 0x00;

    /* Timer0: для RTC (1ms) */
    TMOD = 0x11;
    TH0 = (u8)(T1MS >> 8);
    TL0 = (u8)(T1MS & 0xFF);
    ET0 = 1;
    TR0 = 1;

    /* Timer2: для delay_ms */
    T2CON  = 0x70;
    TH2    = 0x00;
    TL2    = 0x00;
    TRL2H  = 0xBC;
    TRL2L  = 0xCD;
    IEN0  |= 0x20;
    TR2    = 1;

    EA = 1;
}

/* Настройка UART5 для Modbus RTU */
static void uart5_init(u8 speed_idx, u8 proto)
{
    u16 div_val;

    (void)proto;  /* В T5L аппаратно только 8N1 */

    /* Скорости: 1=2400, 2=4800, 3=9600, 4=19200, 5=38400, 6=57600, 7=115200 */
    switch (speed_idx)
    {
        case 1: div_val = (u16)(FOSC / 8UL / 2400UL);   break;
        case 2: div_val = (u16)(FOSC / 8UL / 4800UL);   break;
        case 3: div_val = (u16)(FOSC / 8UL / 9600UL);   break;
        case 4: div_val = (u16)(FOSC / 8UL / 19200UL);  break;
        case 5: div_val = (u16)(FOSC / 8UL / 38400UL);  break;
        case 6: div_val = (u16)(FOSC / 8UL / 57600UL);  break;
        case 7: div_val = (u16)(FOSC / 8UL / 115200UL); break;
        default: div_val = (u16)(FOSC / 8UL / 115200UL); break; /* 115200 по умолчанию */
    }

    SCON3T = 0x80;      /* TX enable, 8 bit */
    SCON3R = 0x80;      /* RX enable, 8 bit */

    BODE3_DIV_H = (u8)(div_val >> 8);
    BODE3_DIV_L = (u8)(div_val & 0xFF);

    /* TX-вывод UART5 (P0.1) в push-pull */
    P0MDOUT |= 0x02;

    /* RS-485: по умолчанию режим приёма */
    RS485_TX_EN = 0;

    /* Включаем прерывание приёма UART5: ES3R в IEN1 бит 5 */
    ES3R = 1;

    EA = 1;
}

static void init_relays(void)
{
    P1MDOUT |= (RELAY_VALVE_BIT | RELAY_LOW_BIT | RELAY_HIGH_BIT | RELAY_MID_BIT);
    RELAY_VALVE = 1;
    RELAY_LOW   = 0;
    RELAY_MID   = 0;
    RELAY_HIGH  = 0;
    g_valve_open = 1;
}

/* ---------------------------------------------------------------
 * Прерывания таймеров
 * ------------------------------------------------------------- */

void T0_ISR(void) interrupt 1
{
    EA = 0;
    TH0 = (u8)(T1MS >> 8);
    TL0 = (u8)(T1MS & 0xFF);

    ++SysTick_RTC;
    if (SysTick_RTC >= 1000u)
    {
        SysTick_RTC = 0;
        g_rtc_1s_flag = 1;
    }

    /* Таймаут Modbus */
    if (g_mb_rx_timeout > 0)
    {
        --g_mb_rx_timeout;
        if (g_mb_rx_timeout == 0 && g_mb_rx_cnt > 0)
        {
            g_mb_frame_ready = 1;
        }
    }

    EA = 1;
}

void T2_ISR(void) interrupt 5
{
    EA = 0;
    TF2 = 0;
    if (SysTick) --SysTick;
    EA = 1;
}

/* Прерывание приёма UART5 */
void UART5_RX_ISR(void) interrupt 13
{
    u8 res;
    EA = 0;

    if (SCON3R & 0x01)
    {
        res = SBUF3_RX;
        SCON3R &= 0xFE;

        if (g_mb_rx_cnt < MODBUS_RX_BUF_SIZE)
        {
            g_mb_rx_buf[g_mb_rx_cnt++] = res;
        }
        g_mb_rx_timeout = MODBUS_TIMEOUT_MS;
        g_mb_frame_ready = 0;
    }

    EA = 1;
}

static void delay_ms(u16 n)
{
    SysTick = n;
    while (SysTick) { }
}

/* ---------------------------------------------------------------
 * UART5 отправка
 * ------------------------------------------------------------- */

static void uart5_send_byte(u8 dat)
{
    SBUF3_TX = dat;
    while ((SCON3T & 0x01) == 0) ;
    SCON3T &= 0xFE;
}

static void uart5_send_buf(u8 *buf, u8 len)
{
    RS485_TX_EN = 1;
    while (len--)
    {
        uart5_send_byte(*buf++);
    }
    RS485_TX_EN = 0;
}

/* ---------------------------------------------------------------
 * Modbus CRC16
 * ------------------------------------------------------------- */

static u16 modbus_crc16(u8 *buf, u8 len)
{
    u8 crc_hi = 0xFF;
    u8 crc_lo = 0xFF;
    u8 idx;

    while (len--)
    {
        idx = crc_hi ^ *buf++;
        crc_hi = crc_lo ^ CRC_HI[idx];
        crc_lo = CRC_LO[idx];
    }

    return ((u16)crc_hi << 8) | crc_lo;
}

/* ---------------------------------------------------------------
 * Доступ к VP DGUS
 * ------------------------------------------------------------- */

static void read_dgus_vp(u16 addr, u8 *buf, u16 len)
{
    u32 os_addr;
    u16 os_len;
    u8  offset;

    os_addr = (u32)addr / 2U;
    offset  = (u8)(addr & 0x01);

    ADR_H = (u8)((os_addr >> 16) & 0xFFU);
    ADR_M = (u8)((os_addr >> 8)  & 0xFFU);
    ADR_L = (u8)(os_addr & 0xFFU);

    ADR_INC = 1;
    RAMMODE = 0xAF;

    if (offset)
    {
        while (!APP_ACK) ;
        APP_EN = 1;
        while (APP_EN) ;
        *buf++ = DATA1;
        *buf++ = DATA0;
        --len;
    }

    os_len = len / 2U;
    if (len & 0x01) ++os_len;

    while (os_len--)
    {
        if ((len & 0x01) && (os_len == 0))
        {
            while (!APP_ACK) ;
            APP_EN = 1;
            while (APP_EN) ;
            *buf++ = DATA3;
            *buf++ = DATA2;
            break;
        }

        while (!APP_ACK) ;
        APP_EN = 1;
        while (APP_EN) ;
        *buf++ = DATA3;
        *buf++ = DATA2;
        *buf++ = DATA1;
        *buf++ = DATA0;
    }

    RAMMODE = 0x00;
}

static void write_dgus_vp(u16 vp_addr, const u8 *buf, u16 len)
{
    u32 os_addr;
    u16 os_len;
    u8  odd;

    EA = 0;

    os_addr = (u32)vp_addr / 2U;
    odd     = (u8)(vp_addr & 0x01);

    ADR_H = (u8)((os_addr >> 16) & 0xFFU);
    ADR_M = (u8)((os_addr >> 8)  & 0xFFU);
    ADR_L = (u8)(os_addr & 0xFFU);

    ADR_INC = 0x01;
    RAMMODE = 0x8F;

    if (odd)
    {
        ADR_INC = 0x00;
        RAMMODE = 0xAF;
        while (!APP_ACK) ;
        APP_EN = 1;
        while (APP_EN) ;

        ADR_INC = 0x01;
        RAMMODE = 0x8F;
        while (!APP_ACK) ;
        DATA1 = *buf++;
        DATA0 = *buf++;
        APP_EN = 1;
        while (APP_EN) ;
        --len;
    }

    os_len = len / 2U;
    if (len & 0x01) ++os_len;

    while (os_len--)
    {
        if ((len & 0x01) && (os_len == 0))
        {
            ADR_INC = 0x00;
            RAMMODE = 0xAF;
            while (!APP_ACK) ;
            APP_EN = 1;
            while (APP_EN) ;

            ADR_INC = 0x01;
            RAMMODE = 0x8F;
            while (!APP_ACK) ;
            DATA3 = *buf++;
            DATA2 = *buf++;
            APP_EN = 1;
            while (APP_EN) ;
            break;
        }

        while (!APP_ACK) ;
        DATA3 = *buf++;
        DATA2 = *buf++;
        DATA1 = *buf++;
        DATA0 = *buf++;
        APP_EN = 1;
        while (APP_EN) ;
    }

    RAMMODE = 0x00;
    EA = 1;
}

static void write_vp_u16(u16 vp_addr, u16 value)
{
    u8 buf[2];
    buf[0] = (u8)(value >> 8);
    buf[1] = (u8)(value & 0xFF);
    write_dgus_vp(vp_addr, buf, 1);  /* len=1 слово (2 байта) */
}

static u16 read_vp_u16(u16 vp_addr)
{
    u8 buf[2];
    read_dgus_vp(vp_addr, buf, 1);
    return ((u16)buf[0] << 8) | buf[1];
}

/* ---------------------------------------------------------------
 * Modbus: чтение/запись регистров
 * ------------------------------------------------------------- */

/* Чтение значения Modbus регистра по адресу */
static u16 modbus_read_reg(u16 addr)
{
    /* Графики 1..4: просто пробрасываем значения VP */
    if (addr >= 0x2020 && addr <= 0x2057)
    {
        return read_vp_u16(addr);
    }

    switch (addr)
    {
        /* Дата/время (только чтение) */
        case 0x2010: return 2000u + g_rtc.year;
        case 0x2011: return g_rtc.month;
        case 0x2012: return g_rtc.day;
        case 0x2013: return g_rtc.weekday;
        case 0x2014: return g_rtc.hour;
        case 0x2015: return g_rtc.min;
        case 0x2016: return g_rtc.sec;

        /* Основные регистры */
        case 0x2060: return g_onoff;
        case 0x2061: return g_speed;
        case 0x2062: return g_mode;
        case 0x2063: return g_t_real_10x;
        case 0x2064: return g_t_real_10x / 10u;
        case 0x2065: return g_t_set_10x;
        case 0x2066: return g_t_set_10x / 10u;
        case 0x2067: return g_graph_en;

        /* Настройки Modbus */
        case 0x2070: return g_mb_addr;
        case 0x2071: return g_mb_speed_idx;
        case 0x2072: return g_mb_proto;

        default:     return 0;
    }
}

/* Запись значения в Modbus регистр */
static u8 modbus_write_reg(u16 addr, u16 value)
{
    /* Графики 1..4: пишем напрямую в VP, значения проверяются уже логикой графиков */
    if (addr >= 0x2020 && addr <= 0x2057)
    {
        write_vp_u16(addr, value);
        return 1;
    }

    switch (addr)
    {
        case 0x2060:  /* ON/OFF */
            if (value == 1 || value == 2)
            {
                g_onoff = (u8)value;
                write_vp_u16(VP_ONOFF, value);
                return 1;
            }
            break;

        case 0x2061:  /* Скорость */
            if (value >= 1 && value <= 4)
            {
                g_speed = (u8)value;
                write_vp_u16(VP_SPEED, value);
                return 1;
            }
            break;

        case 0x2062:  /* Режим */
            if (value == 1 || value == 2)
            {
                g_mode = (u8)value;
                write_vp_u16(VP_MODE, value);
                return 1;
            }
            break;

        case 0x2065:  /* Уставка температуры (*10) */
            if (value >= 160 && value <= 320)
            {
                g_t_set_10x = value;
                write_vp_u16(VP_T_SET_10X, value);
                write_vp_u16(VP_T_SET_INT, value / 10u);
                return 1;
            }
            break;

        case 0x2067:  /* Графики вкл/выкл */
            if (value == 1 || value == 2)
            {
                g_graph_en = (u8)value;
                write_vp_u16(VP_GRAPH_EN, value);
                return 1;
            }
            break;

        case 0x2070:  /* Адрес Modbus */
            if (value >= 1 && value <= 247)
            {
                g_mb_addr = (u8)value;
                write_vp_u16(VP_MB_ADDR, value);
                return 1;
            }
            break;

        case 0x2071:  /* Скорость Modbus */
            if (value >= 1 && value <= 7)
            {
                g_mb_speed_idx = (u8)value;
                write_vp_u16(VP_MB_SPEED, value);
                uart5_init(g_mb_speed_idx, g_mb_proto);
                return 1;
            }
            break;

        case 0x2072:  /* Протокол Modbus (храним, но физически пока 8N1) */
            if (value >= 1 && value <= 6)
            {
                g_mb_proto = (u8)value;
                write_vp_u16(VP_MB_PROTO, value);
                /* uart5_init(...) оставляем — вдруг потом реализуете поддержку parity */
                uart5_init(g_mb_speed_idx, g_mb_proto);
                return 1;
            }
            break;
    }

    return 0;
}

/* Проверка допустимости адреса регистра */
static u8 modbus_addr_valid(u16 addr)
{
    if (addr >= 0x2010 && addr <= 0x2016) return 1;  /* Дата/время */
    if (addr >= 0x2020 && addr <= 0x2057) return 1;  /* Графики 1..4 */
    if (addr >= 0x2060 && addr <= 0x2067) return 1;  /* Основные регистры */
    if (addr >= 0x2070 && addr <= 0x2072) return 1;  /* Настройки Modbus */
    return 0;
}

/* ---------------------------------------------------------------
 * Обработка Modbus RTU фрейма
 * ------------------------------------------------------------- */

static void modbus_process_frame(void)
{
    u8 addr, func;
    u16 reg_addr, reg_cnt, reg_val;
    u16 crc_recv, crc_calc;
    u8 i, tx_len;

    if (g_mb_rx_cnt < 4) goto cleanup;

    /* Проверка CRC */
    crc_calc = modbus_crc16(g_mb_rx_buf, g_mb_rx_cnt - 2);
    crc_recv = ((u16)g_mb_rx_buf[g_mb_rx_cnt - 1] << 8) | g_mb_rx_buf[g_mb_rx_cnt - 2];

    if (crc_calc != crc_recv) goto cleanup;

    addr = g_mb_rx_buf[0];
    func = g_mb_rx_buf[1];

    /* Проверка адреса */
    if (addr != g_mb_addr && addr != 0) goto cleanup;

    /* Обработка функций */
    switch (func)
    {
        case MB_FUNC_READ_REGS:  /* 0x03: Read Holding Registers */
            if (g_mb_rx_cnt < 8) goto cleanup;

            reg_addr = ((u16)g_mb_rx_buf[2] << 8) | g_mb_rx_buf[3];
            reg_cnt  = ((u16)g_mb_rx_buf[4] << 8) | g_mb_rx_buf[5];

            if (reg_cnt == 0 || reg_cnt > 125) goto send_error_value;

            /* Проверка адресов */
            for (i = 0; i < reg_cnt; ++i)
            {
                if (!modbus_addr_valid(reg_addr + i)) goto send_error_addr;
            }

            /* Формируем ответ */
            g_mb_tx_buf[0] = addr;
            g_mb_tx_buf[1] = func;
            g_mb_tx_buf[2] = (u8)(reg_cnt * 2);
            tx_len = 3;

            for (i = 0; i < reg_cnt; ++i)
            {
                reg_val = modbus_read_reg(reg_addr + i);
                g_mb_tx_buf[tx_len++] = (u8)(reg_val >> 8);
                g_mb_tx_buf[tx_len++] = (u8)(reg_val & 0xFF);
            }

            /* CRC */
            crc_calc = modbus_crc16(g_mb_tx_buf, tx_len);
            g_mb_tx_buf[tx_len++] = (u8)(crc_calc & 0xFF);
            g_mb_tx_buf[tx_len++] = (u8)(crc_calc >> 8);

            if (addr != 0) uart5_send_buf(g_mb_tx_buf, tx_len);
            break;

        case MB_FUNC_WRITE_REG:  /* 0x06: Write Single Register */
            if (g_mb_rx_cnt < 8) goto cleanup;

            reg_addr = ((u16)g_mb_rx_buf[2] << 8) | g_mb_rx_buf[3];
            reg_val  = ((u16)g_mb_rx_buf[4] << 8) | g_mb_rx_buf[5];

            if (!modbus_addr_valid(reg_addr)) goto send_error_addr;

            if (!modbus_write_reg(reg_addr, reg_val)) goto send_error_value;

            /* Ответ = эхо запроса */
            if (addr != 0)
            {
                uart5_send_buf(g_mb_rx_buf, g_mb_rx_cnt);
            }
            break;

        case MB_FUNC_WRITE_REGS:  /* 0x10: Write Multiple Registers */
            if (g_mb_rx_cnt < 9) goto cleanup;

            reg_addr = ((u16)g_mb_rx_buf[2] << 8) | g_mb_rx_buf[3];
            reg_cnt  = ((u16)g_mb_rx_buf[4] << 8) | g_mb_rx_buf[5];

            if (reg_cnt == 0 || reg_cnt > 123) goto send_error_value;
            if (g_mb_rx_cnt < (9 + reg_cnt * 2)) goto cleanup;

            /* Проверка и запись */
            for (i = 0; i < reg_cnt; ++i)
            {
                if (!modbus_addr_valid(reg_addr + i)) goto send_error_addr;
            }

            for (i = 0; i < reg_cnt; ++i)
            {
                reg_val = ((u16)g_mb_rx_buf[7 + i * 2] << 8) | g_mb_rx_buf[8 + i * 2];
                modbus_write_reg(reg_addr + i, reg_val);
            }

            /* Формируем ответ */
            g_mb_tx_buf[0] = addr;
            g_mb_tx_buf[1] = func;
            g_mb_tx_buf[2] = g_mb_rx_buf[2];
            g_mb_tx_buf[3] = g_mb_rx_buf[3];
            g_mb_tx_buf[4] = g_mb_rx_buf[4];
            g_mb_tx_buf[5] = g_mb_rx_buf[5];

            crc_calc = modbus_crc16(g_mb_tx_buf, 6);
            g_mb_tx_buf[6] = (u8)(crc_calc & 0xFF);
            g_mb_tx_buf[7] = (u8)(crc_calc >> 8);

            if (addr != 0) uart5_send_buf(g_mb_tx_buf, 8);
            break;

        default:
            goto send_error_func;
    }

    goto cleanup;

send_error_func:
    g_mb_tx_buf[0] = addr;
    g_mb_tx_buf[1] = func | 0x80;
    g_mb_tx_buf[2] = MB_ERR_ILLEGAL_FUNC;
    crc_calc = modbus_crc16(g_mb_tx_buf, 3);
    g_mb_tx_buf[3] = (u8)(crc_calc & 0xFF);
    g_mb_tx_buf[4] = (u8)(crc_calc >> 8);
    if (addr != 0) uart5_send_buf(g_mb_tx_buf, 5);
    goto cleanup;

send_error_addr:
    g_mb_tx_buf[0] = addr;
    g_mb_tx_buf[1] = func | 0x80;
    g_mb_tx_buf[2] = MB_ERR_ILLEGAL_ADDR;
    crc_calc = modbus_crc16(g_mb_tx_buf, 3);
    g_mb_tx_buf[3] = (u8)(crc_calc & 0xFF);
    g_mb_tx_buf[4] = (u8)(crc_calc >> 8);
    if (addr != 0) uart5_send_buf(g_mb_tx_buf, 5);
    goto cleanup;

send_error_value:
    g_mb_tx_buf[0] = addr;
    g_mb_tx_buf[1] = func | 0x80;
    g_mb_tx_buf[2] = MB_ERR_ILLEGAL_VALUE;
    crc_calc = modbus_crc16(g_mb_tx_buf, 3);
    g_mb_tx_buf[3] = (u8)(crc_calc & 0xFF);
    g_mb_tx_buf[4] = (u8)(crc_calc >> 8);
    if (addr != 0) uart5_send_buf(g_mb_tx_buf, 5);

cleanup:
    g_mb_rx_cnt = 0;
    g_mb_frame_ready = 0;
}

/* ---------------------------------------------------------------
 * Управление реле
 * ------------------------------------------------------------- */

static void relays_speeds_off(void)
{
    RELAY_LOW  = 0;
    RELAY_MID  = 0;
    RELAY_HIGH = 0;
}

static void set_valve(bit open)
{
    RELAY_VALVE = open ? 1 : 0;
    g_valve_open = open;
}

static void set_speed_relays(u8 speed)
{
    relays_speeds_off();
    switch (speed)
    {
        case 1: RELAY_LOW = 1;  break;
        case 2: RELAY_MID = 1;  break;
        case 3: RELAY_HIGH = 1; break;
    }
}

static void apply_off_state(void)
{
    relays_speeds_off();
    set_valve(1);
}

/* ---------------------------------------------------------------
 * RTC
 * ------------------------------------------------------------- */

static bit is_leap_year(u16 year)
{
    if ((year % 400u) == 0u) return 1;
    if ((year % 100u) == 0u) return 0;
    if ((year % 4u) == 0u) return 1;
    return 0;
}

static u8 calc_weekday(u8 year, u8 month, u8 day)
{
    /* Формула Томохико Сакамото - проверенный алгоритм */
    static const u8 code t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    u16 y = (u16)year + 2000u;
    u8 m = month;
    u8 d = day;
    u16 result;
    
    if (m < 3) y--;
    
    result = (y + y/4u - y/100u + y/400u + t[m - 1u] + d) % 7u;
    
    /* Результат: 0=Вс, 1=Пн, 2=Вт, 3=Ср, 4=Чт, 5=Пт, 6=Сб
     * Конвертируем в: 1=Пн, 2=Вт, 3=Ср, 4=Чт, 5=Пт, 6=Сб, 7=Вс */
    if (result == 0) return 7u;  /* Воскресенье */
    return (u8)result;
}

static void rtc_tick_1s(void)
{
    u8 dim;

    g_rtc.sec++;
    if (g_rtc.sec >= 60)
    {
        g_rtc.sec = 0;
        g_rtc.min++;
        if (g_rtc.min >= 60)
        {
            g_rtc.min = 0;
            g_rtc.hour++;
            if (g_rtc.hour >= 24)
            {
                g_rtc.hour = 0;
                g_rtc.day++;

                dim = mon_table[g_rtc.month - 1];
                if ((g_rtc.month == 2) && is_leap_year((u16)g_rtc.year + 2000u))
                    dim = 29;

                if (g_rtc.day > dim)
                {
                    g_rtc.day = 1;
                    g_rtc.month++;
                    if (g_rtc.month > 12)
                    {
                        g_rtc.month = 1;
                        g_rtc.year++;
                    }
                }

                g_rtc.weekday = calc_weekday(g_rtc.year, g_rtc.month, g_rtc.day);
            }
        }
    }
}

static void rtc_read_from_dgus(void)
{
    u16 vp_year, vp_month, vp_day, vp_hour, vp_min, vp_sec;
    u8 year_ok = 0, month_ok = 0, day_ok = 0;
    
    /* Читаем из VP регистров UI (0x2010-0x2016) */
    vp_year  = read_vp_u16(VP_YEAR);
    vp_month = read_vp_u16(VP_MONTH);
    vp_day   = read_vp_u16(VP_DAY);
    vp_hour  = read_vp_u16(VP_HOUR);
    vp_min   = read_vp_u16(VP_MIN);
    vp_sec   = read_vp_u16(VP_SEC);
    
    /* Год: 2000-2100 */
    if (vp_year >= 2000 && vp_year <= 2100)
    {
        g_rtc.year = (u8)(vp_year - 2000);
        year_ok = 1;
    }
    
    /* Месяц: 101-112 (формат +100) или 1-12 (прямой) */
    if (vp_month >= 101 && vp_month <= 112)
    {
        g_rtc.month = (u8)(vp_month - 100);
        month_ok = 1;
    }
    else if (vp_month >= 1 && vp_month <= 12)
    {
        g_rtc.month = (u8)vp_month;
        month_ok = 1;
    }
    
    /* День: 101-131 (формат +100) или 1-31 (прямой) */
    if (vp_day >= 101 && vp_day <= 131)
    {
        g_rtc.day = (u8)(vp_day - 100);
        day_ok = 1;
    }
    else if (vp_day >= 1 && vp_day <= 31)
    {
        g_rtc.day = (u8)vp_day;
        day_ok = 1;
    }
    
    /* Час: 100-123 (формат +100) или 0-23 (прямой) */
    if (vp_hour >= 100 && vp_hour <= 123)
        g_rtc.hour = (u8)(vp_hour - 100);
    else if (vp_hour <= 23)
        g_rtc.hour = (u8)vp_hour;
    
    /* Минуты: 100-159 (формат +100) или 0-59 (прямой) */
    if (vp_min >= 100 && vp_min <= 159)
        g_rtc.min = (u8)(vp_min - 100);
    else if (vp_min <= 59)
        g_rtc.min = (u8)vp_min;
    
    /* Секунды: 100-159 (формат +100) или 0-59 (прямой) */
    if (vp_sec >= 100 && vp_sec <= 159)
        g_rtc.sec = (u8)(vp_sec - 100);
    else if (vp_sec <= 59)
        g_rtc.sec = (u8)vp_sec;

    g_rtc.weekday = calc_weekday(g_rtc.year, g_rtc.month, g_rtc.day);
}

static void rtc_publish_to_vp(void)
{
    u8 day_val   = (g_rtc.day   >= 1 && g_rtc.day   <= 31) ? g_rtc.day   : 1;
    u8 month_val = (g_rtc.month >= 1 && g_rtc.month <= 12) ? g_rtc.month : 1;

    write_vp_u16(VP_YEAR,    2000u + g_rtc.year);
    write_vp_u16(VP_MONTH,   100u + month_val);   /* min 101, max 112 */
    write_vp_u16(VP_DAY,     100u + day_val);     /* min 101, max 131 */
    write_vp_u16(VP_WEEKDAY, g_rtc.weekday);
    write_vp_u16(VP_HOUR,    100u + g_rtc.hour);
    write_vp_u16(VP_MIN,     100u + g_rtc.min);
    write_vp_u16(VP_SEC,     100u + g_rtc.sec);
}

/* ---------------------------------------------------------------
 * Температура
 * ------------------------------------------------------------- */


/* Глобальные переменные для температуры */
static u16 adc_val[8] = {0};
static u16 adc_value[8] = {0};
static u16 R1_value = 0;
static u16 R2_value = 0;
static u16 R1_Temperature = 0;
static u16 R2_Temperature = 0;
static u16 Temperature_Real = 0;
static u16 Temperatrue_Real_Old = 0;
static u16 AD_Count = 0;
static u16 Temp_Pre = 10;           /* Шаг в десятых градуса: 5=0.5°C, 10=1°C */
static s16 Temp_Coef[3] = {0, 0, 0}; /* K1, K2, C - будут загружены из VP или установлены дефолтом */

/* Скользящее среднее для дополнительной фильтрации */
#define TEMP_FILTER_SIZE 16  /* Увеличено для лучшей стабильности */
static u16 temp_filter_buf[TEMP_FILTER_SIZE];
static u8 temp_filter_idx = 0;
static bit temp_filter_init = 0;  /* Флаг первой инициализации */

/* Загрузка калибровочных коэффициентов температуры */
static void init_temperature_calibration(void)
{
    u16 prec;

    /* Точность: 0 → 0.5°C (Temp_Pre=5), 1 → 1.0°C (Temp_Pre=10) */
    prec = read_vp_u16(VP_T_PRECISION);
    if (prec == 0)
    {
        Temp_Pre = 5;
    }
    else if (prec == 1)
    {
        Temp_Pre = 10;
    }
    else
    {
        Temp_Pre = 10;
        write_vp_u16(VP_T_PRECISION, 1);
    }

    /* Коэффициенты: читаем и проверяем */
    read_dgus_vp(VP_T_COEF, (u8 *)Temp_Coef, 3);
    
    /* Проверка: K1 должен быть > 0, K1 должен равняться K2 + 10 */
    if ((Temp_Coef[0] <= 0) || (Temp_Coef[1] < 0) || (Temp_Coef[0] != Temp_Coef[1] + 10))
    {
        /* Устанавливаем дефолтные значения для вашего термостата
         * 
         * КАЛИБРОВКА: Если температура показывает неверно, измените Temp_Coef[2]:
         * - Если показывает ВЫШЕ реальной: уменьшите значение (сделайте более отрицательным)
         * - Если показывает НИЖЕ реальной: увеличьте значение (сделайте менее отрицательным)
         * 
         * Формула: каждые 10 единиц Temp_Coef[2] = 1°C
         * Пример: показывает 26.6°C вместо 22°C → разница +4.6°C
         *         нужно уменьшить на 46 единиц (4.6 * 10 = 46)
         */
        Temp_Coef[0] = 10;
        Temp_Coef[1] = 0;
        Temp_Coef[2] = -1800;  /* Базовое значение. Корректируйте после проверки измерений */
        
        /* Записываем дефолт в VP для сохранения */
        write_dgus_vp(VP_T_COEF, (u8 *)Temp_Coef, 3);
    }
}

/* Чтение ADC значений (точно как в tttt) */
static void Get_ADC_Value(void)
{
    static u16 temp[8] = {0};
    read_dgus_vp(OS_AD_VALUE, (u8 *)temp, 8);
    adc_val[0] += temp[0];
    adc_val[6] += temp[6];
    adc_val[7] += temp[7];
}

/* Расчёт сопротивления NTC (точно как в tttt) */
static void Get_R_Value(u16 n)
{
    float R1_temp = 0.0f, R2_temp = 0.0f;
    
    adc_value[0] = adc_val[0] / n;
    adc_value[6] = adc_val[6] / n;
    adc_value[7] = adc_val[7] / n;
    
    R1_temp = adc_value[7] * 1.0f / (adc_value[6] - adc_value[7]);
    R2_temp = adc_value[0] * 1.0f / (adc_value[6] - adc_value[0]);
    
    R1_value = (u16)(R1_temp * 10000);
    R2_value = (u16)(R2_temp * 10000);
}

/* Поиск в таблице NTC (точно как в tttt) */
static u8 FindTab(const u16 *pTab, u8 Tablong, u16 dat)
{
    u8 st = 0, ed = 0, m = 0;
    u8 i = 0;
    
    ed = NTC_TABLE_SIZE - 1;
    
    if (dat >= pTab[st])
        return st;
    else if (dat <= pTab[ed])
        return ed;
    
    while (st < ed)
    {
        m = (st + ed) / 2;
        if (dat == pTab[m])
            break;
        if ((dat < pTab[m]) && (dat > pTab[m + 1]))
            break;
        if (dat > pTab[m])
            ed = m;
        else
            st = m;
        if (i++ > Tablong)
            break;
    }
    
    if (st > ed)
        return 0;
    return m;
}

/* Скользящее среднее для фильтрации температуры */
static u16 apply_temp_filter(u16 new_val)
{
    u8 j;
    u32 sum = 0;
    
    /* При первом вызове заполняем весь буфер начальным значением */
    if (!temp_filter_init)
    {
        for (j = 0; j < TEMP_FILTER_SIZE; j++)
            temp_filter_buf[j] = new_val;
        temp_filter_init = 1;
        temp_filter_idx = 0;
        return new_val;
    }
    
    /* Добавляем новое значение в буфер */
    temp_filter_buf[temp_filter_idx] = new_val;
    temp_filter_idx = (temp_filter_idx + 1) % TEMP_FILTER_SIZE;
    
    /* Вычисляем среднее по всему буферу */
    for (j = 0; j < TEMP_FILTER_SIZE; j++)
        sum += temp_filter_buf[j];
    
    return (u16)(sum / TEMP_FILTER_SIZE);
}

/* Расчёт температуры - МИНИМАЛЬНАЯ ВЕРСИЯ */
static void Get_Temperature(u16 tim, u16 n)
{
    static u16 i = 0;
    u8 idx1;
    u16 delta1;
    s16 adc_diff;
    float temp1 = 0;
    
    if (AD_Count > tim)
    {
        Get_ADC_Value();
        i++;
        if (i >= n)
        {
            Get_R_Value(n);
            
            /* Проверка валидности ADC с запасом */
            adc_diff = (s16)adc_value[6] - (s16)adc_value[7];
            if (adc_diff <= 10)  /* Минимальная разница 10 */
            {
                /* ADC невалидный - пропускаем, но не сбрасываем счётчик */
                adc_val[0] = adc_val[6] = adc_val[7] = 0;
                i = 0;
                AD_Count = 0;
                return;
            }

            /* Используем только ПЕРВЫЙ датчик (ADC7) */
            idx1 = FindTab(NTC1_TABLE, NTC_TABLE_SIZE, R1_value);

            if (idx1 >= (NTC_TABLE_SIZE - 1u)) idx1 = NTC_TABLE_SIZE - 2u;

            delta1 = NTC1_TABLE[idx1] - NTC1_TABLE[idx1 + 1u];
            if (delta1 == 0u) delta1 = 1;  /* Защита от деления на 0 */
            
            temp1 = (NTC1_TABLE[idx1] - R1_value) * 1.0f / delta1;
            R1_Temperature = (u16)idx1 * 10u + (u16)(temp1 * 10.0f);
            
            /* Калибровка: смещение -7°C (70 десятых) */
            #define TEMP_OFFSET 70
            
            if (R1_Temperature > TEMP_OFFSET)
                Temperature_Real = R1_Temperature - TEMP_OFFSET;
         
            Temperatrue_Real_Old = Temperature_Real;
            
            g_t_real_10x = Temperature_Real;
            write_vp_u16(VP_T_REAL_10X, g_t_real_10x);
            write_vp_u16(VP_T_REAL_INT, g_t_real_10x / 10u);
            
            /* Сброс */
            adc_val[0] = 0;
            adc_val[6] = 0;
            adc_val[7] = 0;
            i = 0;
        }
        AD_Count = 0;
    }
}

/* Обёртка для вызова из main */
static void update_temperature(void)
{
    AD_Count++;
    /* tim=40 (@5 ms цикла) * 10 выборок ≈ 2 s на одно измерение */
    Get_Temperature(40, 10);
}

/* ---------------------------------------------------------------
 * Чтение параметров из VP
 * ------------------------------------------------------------- */

static void read_control_params_from_vp(void)
{
    u16 tmp;

    tmp = read_vp_u16(VP_ONOFF);
    if (tmp == 1u || tmp == 2u)
        g_onoff = (u8)tmp;

    tmp = read_vp_u16(VP_SPEED);
    if (tmp >= 1u && tmp <= 4u)
        g_speed = (u8)tmp;

    tmp = read_vp_u16(VP_MODE);
    if (tmp == 1u || tmp == 2u)
        g_mode = (u8)tmp;

    /* Уставка может храниться как *10 (0x2065), так и только целая часть (0x2066) */
    tmp = read_vp_u16(VP_T_SET_10X);
    if (tmp >= 160u && tmp <= 320u)          /* 16.0...32.0 */
    {
        g_t_set_10x = tmp;
    }
    else
    {
        /* Пытаемся взять из целочисленного регистра */
        tmp = read_vp_u16(VP_T_SET_INT);
        if (tmp >= 16u && tmp <= 32u)
        {
            g_t_set_10x = tmp * 10u;
            /* Синхронизируем оба регистра в DGUS */
            write_vp_u16(VP_T_SET_10X, g_t_set_10x);
            write_vp_u16(VP_T_SET_INT, tmp);
        }
    }

    tmp = read_vp_u16(VP_GRAPH_EN);
    if (tmp == 1u || tmp == 2u)
        g_graph_en = (u8)tmp;
}

/* Чтение одного графика из VP в структуру g_graph_cfg[idx] */
static void read_single_graph_from_vp(u8 idx)
{
    GraphConfig *cfg = &g_graph_cfg[idx];
    u16 base;
    u16 v;

    switch (idx)
    {
        case 0: base = VP_G1_STATUS; break;
        case 1: base = VP_G2_STATUS; break;
        case 2: base = VP_G3_STATUS; break;
        case 3: base = VP_G4_STATUS; break;
        default: return;
    }

    v = read_vp_u16(base + 0u);         /* Статус */
    cfg->status = (v == 2u) ? 2u : 1u;

    v = read_vp_u16(base + 1u);         /* Дни (битовая маска) */
    cfg->days = (v <= 127u) ? (u8)v : 0u;

    v = read_vp_u16(base + 2u);         /* Скорость графика */
    if (v >= 1u && v <= 5u) cfg->speed = (u8)v;
    else                    cfg->speed = 5u;

    v = read_vp_u16(base + 3u);         /* Температура графика, 16..32 */
    if (v >= 16u && v <= 32u) cfg->temp_int = (u8)v;
    else                      cfg->temp_int = 22u;

    v = read_vp_u16(base + 4u);         /* Часы начала (100..123) */
    if (v >= 100u && v <= 123u) cfg->shour = (u8)(v - 100u);
    else                        cfg->shour = 0u;

    v = read_vp_u16(base + 5u);         /* Минуты начала (100..159) */
    if (v >= 100u && v <= 159u) cfg->smin = (u8)(v - 100u);
    else                        cfg->smin = 0u;

    v = read_vp_u16(base + 6u);         /* Часы конца (100..123) */
    if (v >= 100u && v <= 123u) cfg->ehour = (u8)(v - 100u);
    else                        cfg->ehour = 0u;

    v = read_vp_u16(base + 7u);         /* Минуты конца (100..159) */
    if (v >= 100u && v <= 159u) cfg->emin = (u8)(v - 100u);
    else                        cfg->emin = 0u;
}

/* Чтение всех четырёх графиков */
static void read_graphs_from_vp(void)
{
    u8 i;
    for (i = 0; i < 4; ++i)
    {
        read_single_graph_from_vp(i);
    }
}

/* ---------------------------------------------------------------
 * Управление
 * ------------------------------------------------------------- */

static void update_valve_with_hysteresis(s16 delta)
{
    if (g_valve_open)
    {
        if (delta < -TEMP_HYST_10X) set_valve(0);
    }
    else
    {
        if (delta > TEMP_HYST_10X) set_valve(1);
    }
}

static void apply_auto_speed_and_valve(s16 delta)
{
    u8 spd;
    bit vlv;

    if (g_mode == 2)
    {
        if (delta >= 0)        { spd = 1; vlv = 1; }
        else if (delta >= -20) { spd = 1; vlv = 0; }
        else if (delta >= -40) { spd = 2; vlv = 0; }
        else                   { spd = 3; vlv = 0; }
    }
    else
    {
        if (delta < 0)        { spd = 1; vlv = 0; }
        else if (delta < 20)  { spd = 1; vlv = 1; }
        else if (delta < 40)  { spd = 2; vlv = 1; }
        else                  { spd = 3; vlv = 1; }
    }

    set_speed_relays(spd);
    set_valve(vlv);
}

static void apply_main_control(void)
{
    s16 delta;

    if (g_onoff == 1)
    {
        apply_off_state();
        return;
    }

    delta = (s16)g_t_real_10x - (s16)g_t_set_10x;

    if (g_speed >= 1 && g_speed <= 3)
    {
        set_speed_relays(g_speed);
        update_valve_with_hysteresis(delta);
    }
    else if (g_speed == 4)
    {
        apply_auto_speed_and_valve(delta);
    }
    else
    {
        set_speed_relays(0);
    }
}

/* ---------------------------------------------------------------
 * Графики
 * ------------------------------------------------------------- */

static bit graph_is_today(const GraphConfig* g)
{
    if (g_rtc.weekday > 6) return 0;
    return (g->days & (1u << g_rtc.weekday)) ? 1 : 0;
}

static void process_single_graph(u8 idx)
{
    GraphConfig* cfg = &g_graph_cfg[idx];
    GraphRuntime* rt = &g_graph_rt[idx];

    if (g_graph_en != 2 || cfg->status != 2 || !graph_is_today(cfg))
    {
        rt->active = 0;
        return;
    }

    if (!rt->active && g_rtc.hour == cfg->shour && g_rtc.min == cfg->smin && g_rtc.sec == 0)
    {
        rt->active = 1;
        rt->prev_speed = g_speed;
        rt->prev_t_set = g_t_set_10x;

        if (cfg->speed == 5) g_onoff = 1;
        else
        {
            g_onoff = 2;
            g_speed = cfg->speed;
            g_t_set_10x = (u16)cfg->temp_int * 10u;
        }
    }

    if (rt->active && g_rtc.hour == cfg->ehour && g_rtc.min == cfg->emin && g_rtc.sec == 0)
    {
        u8 i, other = 0;
        rt->active = 0;

        for (i = 0; i < 4; ++i)
            if (g_graph_rt[i].active) { other = 1; break; }

        if (!other) g_onoff = 1;
        else
        {
            g_speed = rt->prev_speed;
            g_t_set_10x = rt->prev_t_set;
        }
    }
}

static void update_graphs(void)
{
    u8 i;

    /* Каждый раз подтягиваем актуальные настройки графиков из VP */
    read_graphs_from_vp();

    for (i = 0; i < 4; ++i)
    {
        process_single_graph(i);
    }
}

/* ---------------------------------------------------------------
 * Инициализация
 * ------------------------------------------------------------- */

static void init_state_and_vps(void)
{
    u8 i;
    u16 tmp;

    /* Чтение Modbus настроек из VP */
    tmp = read_vp_u16(VP_MB_ADDR);
    if (tmp >= 1 && tmp <= 247) g_mb_addr = (u8)tmp;
    else { g_mb_addr = 247; write_vp_u16(VP_MB_ADDR, 247); }

    tmp = read_vp_u16(VP_MB_SPEED);
    if (tmp >= 1 && tmp <= 7) g_mb_speed_idx = (u8)tmp;
    else { g_mb_speed_idx = 7; write_vp_u16(VP_MB_SPEED, 7); }

    tmp = read_vp_u16(VP_MB_PROTO);
    if (tmp >= 1 && tmp <= 6) g_mb_proto = (u8)tmp;
    else { g_mb_proto = 1; write_vp_u16(VP_MB_PROTO, 1); }

    /* Чтение основных параметров */
    tmp = read_vp_u16(VP_ONOFF);
    if (tmp == 1 || tmp == 2) g_onoff = (u8)tmp;
    else { g_onoff = 2; write_vp_u16(VP_ONOFF, 2); }

    tmp = read_vp_u16(VP_SPEED);
    if (tmp >= 1 && tmp <= 4) g_speed = (u8)tmp;
    else { g_speed = 1; write_vp_u16(VP_SPEED, 1); }

    tmp = read_vp_u16(VP_MODE);
    if (tmp == 1 || tmp == 2) g_mode = (u8)tmp;
    else { g_mode = 2; write_vp_u16(VP_MODE, 2); }

    tmp = read_vp_u16(VP_T_SET_10X);
    if (tmp >= 160 && tmp <= 320) g_t_set_10x = tmp;
    else
    {
        g_t_set_10x = 220;
        write_vp_u16(VP_T_SET_10X, 220);
        write_vp_u16(VP_T_SET_INT, 22);
    }

    tmp = read_vp_u16(VP_GRAPH_EN);
    if (tmp == 1 || tmp == 2) g_graph_en = (u8)tmp;
    else { g_graph_en = 1; write_vp_u16(VP_GRAPH_EN, 1); }

    /* Калибровка температуры */
    init_temperature_calibration();

    /* Инициализация переменных температуры 
     * Ставим 0 чтобы видеть когда первое реальное измерение произойдёт */
    g_t_real_10x = 0;
    Temperature_Real = 0;
    Temperatrue_Real_Old = 0;
    temp_filter_init = 0;  /* Сброс фильтра для первого измерения */
    
    write_vp_u16(VP_T_REAL_10X, g_t_real_10x);
    write_vp_u16(VP_T_REAL_INT, g_t_real_10x / 10u);

    /* Рантайм-часть графиков (что было до активации) */
    for (i = 0; i < 4; ++i)
    {
        g_graph_rt[i].active     = 0;
        g_graph_rt[i].prev_speed = g_speed;
        g_graph_rt[i].prev_t_set = g_t_set_10x;
    }

    /* Читаем фактические настройки графиков из VP */
    read_graphs_from_vp();
}

/* ---------------------------------------------------------------
 * MAIN
 * ------------------------------------------------------------- */

int main(void)
{
    init_system();
    init_relays();
    init_state_and_vps();

    /* Инициализация UART5 для Modbus */
    uart5_init(g_mb_speed_idx, g_mb_proto);

    /* Чтение времени */
    rtc_read_from_dgus();
    rtc_publish_to_vp();

    for (;;)
    {
        /* Modbus обработка */
        if (g_mb_frame_ready)
        {
            modbus_process_frame();
        }

        /* Обновление температуры */
        update_temperature();

        /* Обработка раз в секунду */
        if (g_rtc_1s_flag)
        {
            g_rtc_1s_flag = 0;

            rtc_tick_1s();
            rtc_publish_to_vp();

            read_control_params_from_vp();
            update_graphs();
            apply_main_control();
        }

        delay_ms(5);
    }
}
