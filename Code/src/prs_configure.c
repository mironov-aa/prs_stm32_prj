/*
 * prs_configure.c
 *
 *  Created on: Feb 21, 2020
 *      Author: mironov-aa
 */
#include <stdbool.h>
#include "string.h"
#include "stdlib.h"
#include "stm32f0xx.h"
#include "global.h"
#include "sdhc_card_driver.h"
#include "main.h"
#include "prs_configure.h"
#include "ff.h"

static const SettingsUnion defaultGlobalSettings = 
                          {'T','I','M','E',':','1','3','3','7','0','0','\n',
                           'D','A','T','E',':','0','1','0','9','1','1','\n',
                           'S','E','T',':','0','\n',
                           'C','N','T',':','1','6','\n',
                            'P','R','D',':','1','0'};


static uint8_t isHseStart = 0; //< Set if HSE don't start

static void ConfigureRcc(void);
static void ConfigureRtc(void);
static void InitRtc(uint32_t newTime, uint32_t newDate);
static void ConfigureSettings(void);//read settings from sdhc card or add it
static void ConfigureGpio(void);
static void CongigureInterrupts(void);
static void ConfigureSpi1(bool isInitial);
static void ConfigureSpi2(void);
static void ConfigureDma(void);
static void ConfigureTim3(void);

void ConfigurePrs(void)
{
    ConfigureRcc();
    ConfigureGpio();

    if(!isHseStart)
    {
        GPIOC->BSRR |= GPIO_BSRR_BS_7; //Warning if HSE don't start!
    }
    ConfigureRtc();
    CongigureInterrupts();
    ConfigureSpi1(true);
    SdhcCardInitialize();
    ConfigureSpi1(false);
    ConfigureSpi2();
    ConfigureDma();

    ConfigureSettings();
#ifdef FREERTOS_DEBUG
    ConfigureTim3();
#endif
}

static void ConfigureRcc(void)
{
    /* SYSCLK, HCLK, PCLK configuration */
    /* At this stage the HSI is already enabled */
    const uint16_t hseStartupTimeout = 0x5000;
    uint16_t hseStartupCounter = 0;

    /* Enable Prefetch Buffer and set Flash Latency */
    FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

    /* PREDiv = 1*/
    RCC->CFGR2 &= (uint32_t)~RCC_CFGR2_PREDIV;

    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

    /* PCLK = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;

    /* Set HSE, CSS on */
    RCC->CR |= (uint32_t)((RCC_CR_HSEON | RCC_CR_CSSON));
    /* Wait till HSE start*/

    while((!(RCC->CR & RCC_CR_HSERDY)) && 
            (hseStartupCounter < hseStartupTimeout))
    {
        hseStartupCounter++;
    }

    if((RCC->CR & RCC_CR_HSERDY))
    {
        //If HSE start
        isHseStart = 1;//Set HSE Flag true
        /* PLL configuration = (HSE/PREDiv)*PLLMUL = (8/1)*6 = 48 */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL6);
    }
    else
    {
        //If HSE don't start
        isHseStart = 0;//Set HSE Flag false
        /* PLL configuration = (HSI/2) * 12 = ~48 MHz */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL));
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_DIV2 | RCC_CFGR_PLLMUL12);
    }

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0){}

    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL){}
}

static void ConfigureRtc(void)
{
    /* Enable the peripheral clock RTC */
    /* (1) Enable the LSI */
    /* (2) Wait while it is not ready */
    /* (3) Enable PWR clock */
    /* (4) Enable write in RTC domain control register */
    /* (5) LSI for RTC clock */
    /* (6) Disable PWR clock */
    RCC->CSR |= RCC_CSR_LSION; /* (1) */
    while((RCC->CSR & RCC_CSR_LSIRDY)!=RCC_CSR_LSIRDY) /* (2) */
    {
      /* add time out here for a robust application */
    }
    RCC->APB1ENR |= RCC_APB1ENR_PWREN; /* (3) */
    PWR->CR |= PWR_CR_DBP; /* (4) */
    RCC->BDCR = (RCC->BDCR & ~RCC_BDCR_RTCSEL) | RCC_BDCR_RTCEN | 
                                                 RCC_BDCR_RTCSEL_1; /* (5) */
    RCC->APB1ENR &=~ RCC_APB1ENR_PWREN; /* (6) */
}

void InitRtc(uint32_t newTime, uint32_t newDate)
{
    /* RTC init mode */
    /* Configure RTC */
    /* (1) Write access for RTC registers */
    /* (2) Enable init phase */
    /* (3) Wait until it is allow to modify RTC register values */
    /* (4) set prescaler, 40kHz/128 => 312 Hz, 312Hz/312 => 1Hz*/
    /* (5) New time in TR new date in DR */
    /* (6) Disable init phase */
    /* (7) Disable write access for RTC registers */
    RTC->WPR = 0xCA; /* (1) */
    RTC->WPR = 0x53; /* (1) */
    RTC->ISR |= RTC_ISR_INIT; /* (2) */
    while((RTC->ISR & RTC_ISR_INITF)!=RTC_ISR_INITF) /* (3) */
    {
      /* add time out here for a robust application */
    }
    RTC->PRER = 0x007F0137; /* (4) */
    RTC->TR = newTime; /* (5) */
    RTC->DR = newDate; /* (5) */
    RTC->ISR &=~ RTC_ISR_INIT; /* (6) */
    RTC->WPR = 0xFE; /* (7) */
    RTC->WPR = 0x64; /* (7) */
}

static void ConfigureSettings(void)
{
    FRESULT fResult;
    FATFS fatFs;
    FIL file;
    SettingsUnion savedSettings;
    uint32_t date = 0;
    uint32_t time = 0;
    unsigned int savedBytes = 0;
    f_mount(&fatFs, "", 1);
    fResult = f_open(&file, "settings.cfg", FA_OPEN_EXISTING | FA_READ);
    if(fResult == FR_OK)//if file exist
    {
        f_read(&file, savedSettings.byteArray, 
               sizeof(savedSettings.byteArray), &savedBytes);
        f_close(&file);
    }
    else
    {
        f_open(&file, "settings.cfg", FA_OPEN_APPEND | FA_WRITE);
        f_write(&file, defaultGlobalSettings.byteArray, 
                      sizeof(defaultGlobalSettings.byteArray), &savedBytes);
        savedSettings = defaultGlobalSettings;
        f_close(&file);
        date = strtol(defaultGlobalSettings.Field.dataValue, NULL, 16);
        time = strtol(defaultGlobalSettings.Field.timeValue, NULL, 16);
        InitRtc(time, date);
    }
    g_settings.numberOfCounters = atoi(savedSettings.Field.numberOfCounters);
    g_settings.periodMs = atoi(savedSettings.Field.periodMs);

    if(savedSettings.Field.setTime == '1')
    {
        date = strtol(savedSettings.Field.dataValue, NULL, 16);
        time = strtol(savedSettings.Field.timeValue, NULL, 16);
        InitRtc(time, date);
        savedSettings.Field.setTime = '0';
        f_open(&file, "settings.cfg", FA_CREATE_ALWAYS | FA_WRITE);
        f_write(&file, savedSettings.byteArray, 
                 sizeof(defaultGlobalSettings.byteArray), &savedBytes);
        f_close(&file);
    }
    f_close(&file);
    f_unmount("");
}

static void ConfigureGpio(void)
{
    //Enable clock for port A, B & C
    RCC->AHBENR |= (uint32_t)(RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOAEN | 
                              RCC_AHBENR_GPIOBEN);

//******************************  GPIOA  ******************************//

    //Configure SPI1 ping on GPIO: PA5(SCK)
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    GPIOA->MODER |= GPIO_MODER_MODER5_1; //Alternate function

    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5; //AF0

    //PA4 SPI 1 software chip select
    GPIOA->MODER &= ~GPIO_MODER_MODER4;
    GPIOA->MODER |= GPIO_MODER_MODER4_0; //General Purpose output mode
    GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR4; //High speed
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_4 ; //Push-pull

    //PA0 Configuration. User button
    GPIOA->MODER &= (uint32_t)(~GPIO_MODER_MODER0);    //Input Mode
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;//Pull down

//******************************  GPIOB  ******************************//

    //Configure SPI2 pins on GPIO: PB12(NSS) PB13(SCK) PB14(MISO) PB15(MOSI)    

    GPIOB->MODER &= ~(GPIO_MODER_MODER12 | GPIO_MODER_MODER13 |
                      GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
    //Alternate function
    GPIOB->MODER |= (GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1 |
                     GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1); 

    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL12 | GPIO_AFRH_AFSEL13 |
                       GPIO_AFRH_AFSEL14 | GPIO_AFRH_AFSEL15); //AF0

    //Configure SPI1 pins on GPIO: PB4(MISO) PB5(MOSI)
    //Alternate function
    GPIOB->MODER &= ~(GPIO_MODER_MODER4 | GPIO_MODER_MODER5);
    GPIOB->MODER |= (GPIO_MODER_MODER4_1 | GPIO_MODER_MODER5_1); 

    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL4 | GPIO_AFRL_AFSEL5); //AF0

//******************************  GPIOC  ******************************//

/*LEDs: PC6: Led Indicator
 *         PC7: Warning(lit) or fatal error(blink) LED
 *         PC8: vLed1Task indicator
 *         PC9: vLed2Task indicator
*/
    GPIOC->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7 |
                      GPIO_MODER_MODER8 | GPIO_MODER_MODER9);

    //General Purpose output mode
    GPIOC->MODER |= (GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0 |
                     GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0); 

    //Low speed
    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7 |
                        GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9); 
    //Push-pull
    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7 |
                       GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9); 
}

static void CongigureInterrupts(void)
{
    //PA0 as source input
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;

    //Set up EXTI
    EXTI->RTSR |= EXTI_RTSR_RT0; //Rising Edge for line 0
    EXTI->IMR |= EXTI_IMR_IM0; //Interrupt mask for line 0

    //Configure NVIC for push button on PA0
    NVIC_SetPriority(EXTI0_1_IRQn, 0);

}

static void ConfigureSpi1(bool isInitial)
{
    /* Enable the peripheral clock SPI1 */
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    /* Correct disable SPI1*/
    while((SPI1->SR & SPI_SR_FTLVL) != 0){} //no more data to transmit
    while((SPI1->SR & SPI_SR_BSY) != 0){} //the last data frame is processed
    __MEMORY_BARRIER;
    SPI1->CR1 = 0; //disable SPI
    while((SPI1->SR & SPI_SR_FRLVL) != 0)
    {
        uint16_t data = SPI1->DR;    //read all the received data
    }

    /* Configure SPI1 in master */
    /* (1) Master selection, CPOL and CPHA at 1 , 
           software slave management & Internal slave select */
    /* (2) BR: if need SDHC initialize: Fpclk/256, otherwise Fpclk/4
    /* (3) Slave select output enabled, RXNE IT, 
           8-bit Rx fifo TX RX buffers DMA enable*/
    /* (4) Enable SPI1 */
    SPI1->CR1 = SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | 
                SPI_CR1_CPHA | SPI_CR1_CPOL; /* (1) */
    SPI1->CR1 &= ~SPI_CR1_BR; /* (2) */
    SPI1->CR1 |= (isInitial)? (SPI_CR1_BR) : (SPI_CR1_BR_0);
    SPI1->CR2 = SPI_CR2_FRXTH | SPI_CR2_SSOE | 
                SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | 
                SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN; /* (3) */
    __MEMORY_BARRIER;
    SPI1->CR1 |= SPI_CR1_SPE; /* (4) */
}

static void ConfigureSpi2(void)
{
     /* Enable the peripheral clock SPI2 */
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    /* Configure SPI2 in slave */
    /* Master selection, CPOL at 0 and CPHA at 1 , 
       software slave management & Internal slave select */
    /* (1) 8-bit Rx fifo, enable DMAFRXTH */
    /* (2) Enable SPI2 */
    SPI2->CR1 = SPI_CR1_MSTR | SPI_CR1_CPHA | SPI_CR1_SSM | SPI_CR1_SSI;
    SPI2->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_SSOE |
                SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN | SPI_CR2_FRXTH; /* (1) */
    __MEMORY_BARRIER;
    SPI2->CR1 |= SPI_CR1_SPE; /* (2) */

}

static void ConfigureDma(void)
{
    // Enable the peripheral clock DMA1
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    /* DMA1 Channel2 SPI1_RX config */
    DMA1_Channel2->CPAR = (uint32_t)&(SPI1->DR);//Peripheral address
    /* DMA1 Channel3 SPI1_TX config */
    DMA1_Channel3->CPAR = (uint32_t)&(SPI1->DR); //Peripheral address
    /* DMA1 Channel4 SPI2_RX config */
    DMA1_Channel4->CPAR = (uint32_t)&(SPI2->DR);//Peripheral address
    /* DMA1 Channel5 SPI2_TX config */
    DMA1_Channel5->CPAR = (uint32_t)&(SPI2->DR); //Peripheral address
}

#ifdef FREERTOS_DEBUG
static void ConfigureTim3(void)
{
    //Configure TIM3 for configGENERATE_RUN_TIME_STATS:FreeRTOSConfig.h
    /* Enable the peripheral clock TIM3 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->CR1 |= TIM_CR1_URS;//only counter overflow generate update interrupt
    TIM3->ARR = 480;// Update interrupt each 1/100kHZ.
    TIM3->DIER |= TIM_DIER_UIE;//Update interrupt enable
    __MEMORY_BARRIER;
    TIM3->CR1 |= TIM_CR1_CEN;//Enable counter

    /* Configure IT */
    NVIC_SetPriority(TIM3_IRQn, 0);
    NVIC_EnableIRQ(TIM3_IRQn);
}
#endif





