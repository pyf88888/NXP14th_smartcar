/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】龙邱i.MX RT1052核心板-智能车板
【编    写】CHIUSIR
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2018年2月1日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
------------------------------------------------
【dev.env.】IAR8.20.1及以上版本
【Target 】 i.MX RT1052
【Crystal】 24.000Mhz
【ARM PLL】 1200MHz
【SYS PLL】 528MHz
【USB PLL】 480MHz
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#include "include.h"

volatile bool gptIsrFlag = false;
void GPT2_IRQHandler(void)
{
    /* Clear interrupt flag.*/
    GPT_ClearStatusFlags(GPT2, kGPT_OutputCompare1Flag);
    gptIsrFlag = true;
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F, Cortex-M7, Cortex-M7F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U || __CORTEX_M == 7U)
    __DSB();
#endif
}


// 初始化GPT计数器  GPT1用计时  GPT2用作延时
void LQ_GPT_Init(void)
{
    gpt_config_t gptConfig;
    /*Clock setting for GPT 150M*/
    CLOCK_SetMux(kCLOCK_PerclkMux, 0);
    CLOCK_SetDiv(kCLOCK_PerclkDiv, 0);
    
    GPT_GetDefaultConfig(&gptConfig);
    gptConfig.enableRunInDbg = true;   //开启debug情况下可以使用
    GPT_Init(GPT1, &gptConfig);
    GPT_Init(GPT2, &gptConfig);
    
    GPT_SetClockDivider(GPT1, 150);      // GPT 时钟 150M / 15 = 10M
    GPT_SetClockDivider(GPT2, 15);      // GPT 时钟 150M / 150 = 10M
    
    GPT_SetOutputCompareValue(GPT1, kGPT_OutputCompare_Channel1, 0XFFFFFFFF);   //GPT 是32为计数器 最大 0xFFFF FFFF 
    GPT_SetOutputCompareValue(GPT2, kGPT_OutputCompare_Channel1, 0XFFFFFFFF);   //GPT 是32为计数器 最大 0xFFFF FFFF 

}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】CHIUSIR
【功能说明】测试GPI定时中断功能
【软件版本】V1.0
【最后更新】2018年11月24日 
【函数名】
【返回值】无
【参数值】无
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void Test_GPT_Timer(void)
{
    LQ_GPT_Init();
    printf("GPT1 : %d \n", GPT_GetClockSource(GPT1));
    printf("GPT2 : %d \n", GPT_GetClockSource(GPT2));
    uint32_t time = 0, last_time = 0;
    gpt_time_start();
    while(1)
    {
        delayus(10);
        GPIO_PinReverse (GPIO2, 22); 
        time = gpt_time_get();
        printf("delay 10 us is %u \n", time - last_time);
        last_time = gpt_time_get(); //注意 打印也需要时间
        delayms(10);
        time = gpt_time_get();
        printf("delay 10 ms is %u \n", time - last_time);
        last_time = gpt_time_get(); //注意 打印也需要时间
    }
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】Z
【功能说明】GPT 计数器延时
【软件版本】V1.0
【最后更新】2019年02月18日 
【函数名】
【返回值】无
【参数值】无  最大延时4294967295 / 10 000000 = 429s
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void delayms(uint32_t ms)
{
    delayus(ms * 1000);
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】Z
【功能说明】GPT 计数器延时
【软件版本】V1.0
【最后更新】2019年02月18日 
【函数名】
【返回值】无
【参数值】无 最大延时4294967295 / 10 000000 = 429s
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void delayus(uint32_t us)
{
    /* Start Timer */
    GPT_StartTimer(GPT2);
    uint32_t time = us * 10 - 7;
    while(GPT_GetCurrentTimerCount(GPT2) < time);
    
    GPT_StopTimer(GPT2);
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】Z
【功能说明】GPT 计数器开始计时
【软件版本】V1.0
【最后更新】2019年02月18日 
【函数名】
【返回值】无
【参数值】无  
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void gpt_time_start(void)
{
    GPT_StopTimer(GPT1);
    GPT_StartTimer(GPT1);   //刷新 GPT1
    GPT_StartTimer(GPT1);
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】Z
【功能说明】GPT 计数器得到时长 us
【软件版本】V1.0
【最后更新】2019年02月18日 
【函数名】
【返回值】无
【参数值】无 最大计时 4294967295 / 1 000000 = 4290s  
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
uint32_t gpt_time_get(void)
{
    if(GPT_GetStatusFlags(GPT1, kGPT_RollOverFlag))   //溢出
        return 0;
    else
        return GPT_GetCurrentTimerCount(GPT1) - 1;
}
