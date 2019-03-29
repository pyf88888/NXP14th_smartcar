/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【平    台】龙邱i.MX RT1052核心板-智能车板
【编    写】CHIUSIR
【E-mail  】chiusir@163.com
【软件版本】V1.0，龙邱开源代码，仅供参考，后果自负
【最后更新】2018年11月13日
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
================================================
下载及调试模式：0010
下载      模式：0001
运行      模式：0010
------------------------------------------------
用JLINKV9/OB及以上版本可以下载SDRAM和QSPI FLASH;
用LQDAPLINKV3可以下载SDRAM和QSPI FLASH;
------------------------------------------------
部分函数尚未验证完毕，默认测试OV7725模块和配套4.3吋彩屏
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#include "include.h"
#include "AHRS.h"    //使用姿态解算时需要添加该头文件 
//========================================================================================== 
/*      更新说明                                                                    时间
 *      1.优化 Test_OLED_Camera() 
 *      2.增加systick计时器功能 
 *      3.优化延时函数                                                             2018/11/08
 *
 *      1.优化 PID
 *      2.优化 Scheduler                                                           2018/11/09
 *
 *      1.增加串口+DMA 例程                                                        
 *      2.增加QTMR模块的PWM功能例程                                                2018/11/19
 *
 *      1.增加QTMR模块的正交解码例程                                               
 *      2.优化神眼摄像头SCCB                                                       2018/11/21
 *
 *      1.优化神眼摄像头帧率设置                                                   2018/11/27
 *
 *      1.优化7725摄像头帧率配置
 *      2.增加上位机显示图像功能                                                   2018/11/28
 *
 *      1.解决神眼188*120分辨率不出图问题
 *      2.解决图像抖动问题                                                         2019/01/15
 *
 *      1.优化了Get_Use_Image()函数提取数组的时间                                  2019/01/28
 *
 *      1.更新K车舵机驱动程序                                                      2019/02/17
 *
 *      1.更新GPT定时器延时和计时功能                                              2019/02/18       
 */
//void delayms(uint32_t ms)
//{
//    uint32_t i,j;
//    
//    for (i = 0; i < ms; i++)
//    {
//        for (j = 0; j < 200000; j++)//600M--1ms
//        __NOP();//asm("NOP");   
//    }
//    return ;
//}
//void delayus(uint32_t us) //延时1us 实测为1.18us   延时10us 实测为10.02us   延时100us 实测为98.4us
//{
//    uint32_t i,j;
//    
//    for (i = 0; i < us; i++)
//    {
//        for (j = 0; j < 290; j++)//600M--1us
//        __NOP();//asm("NOP");   
//    }
//    return ;
//} 


int main(void)
{        
    
    BOARD_ConfigMPU();                   /* 初始化内存保护单元 */      
    BOARD_InitSDRAMPins();               /* SDRAM初始化 */
    BOARD_BootClockRUN();                /* 初始化开发板时钟 */   
    BOARD_InitDEBUG_UARTPins();          //UART调试口管脚复用初始化 
    BOARD_InitDebugConsole();            //UART调试口初始化 可以使用 PRINTF函数
    LQ_GPT_Init();                       //GPT延时和计时功能初始化
    LED_Init();                          //初始化核心板和开发板上的LED接口
    LQ_UART_Init(LPUART1, 115200);       //串口1初始化 可以使用 printf函数
    _systime.init();                     //开启systick定时器
    NVIC_SetPriorityGrouping(2);/*设置中断优先级组  0: 0个抢占优先级16位个子优先级 
                                 *1: 2个抢占优先级 8个子优先级 2: 4个抢占优先级 4个子优先级 
                                 *3: 8个抢占优先级 2个子优先级 4: 16个抢占优先级 0个子优先级
                                 */

/*    uint64_t now = _systime.get_time_us();         //计时功能  得到当前时间   
 *    代码
 *    uint64_t time = _systime.get_time_us() - now;  //得到时差
 *    printf("time is %llu \r\n", time);
 */    

    /****************打印系统时钟******************/
    printf("\r\n");
    printf("*****LQ_1052*****\r\n");
    printf("CPU:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_CpuClk));
    printf("AHB:             %d Hz\r\n", CLOCK_GetFreq(kCLOCK_AhbClk));
    printf("SEMC:            %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SemcClk));
    printf("SYSPLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllClk));
    printf("Video:           %d Hz\r\n", CLOCK_GetFreq(kCLOCK_VideoPllClk));
    printf("SYSPLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd0Clk));
    printf("SYSPLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd1Clk));
    printf("SYSPLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd2Clk));
    printf("SYSPLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_SysPllPfd3Clk));
    printf("USB1PLL:          %d Hz\r\n", CLOCK_GetFreq(kCLOCK_Usb1PllClk));
    printf("USB1PLLPFD0:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_Usb1PllPfd0Clk));
    printf("USB1PLLPFD1:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_Usb1PllPfd1Clk));
    printf("USB1PLLPFD2:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_Usb1PllPfd2Clk));
    printf("USB1PLLPFD3:      %d Hz\r\n", CLOCK_GetFreq(kCLOCK_Usb1PllPfd3Clk));	
    
//-----------------------------------------------------------------------------------------  
//  测试函数都是死循环，每次只能开启一个，综合应用需自行调用各模块初始化函数
//-----------------------------------------------------------------------------------------
//    Test_ADC();          //测试ADC并显示在TFT1.8上               电磁传感器及电源电压监控  
//    Test_UART();         //测试UART及中断                      蓝牙、USB转TTL调试及通信  
//    Test_LED();          //测试GPIO输出口     LED显示及外设控制             
//    Test_GPIO_KEY();     //测试GPIO输入+TFT1.8                   按键检测功能并显示在TFT1.8上              
//    Test_GPIO_ExInt();   //测试GPIO输入及外部中断              按键、中断检测功能    
//    Test_PIT();          //测试PIT定时中断功能  
//    Test_GPT_Timer();    //测试GPT定时器延时和计时功能
//    Test_OLED();         //测试OLED
//    Test_RNG();          //随机数产生      
//    Test_9AX();          //九轴测试 
//    Test_MPU6050();      //测试I2C及6轴陀螺仪功能              陀螺仪及加速度功能 
//    Test_ahrs();         //测试六轴姿态解算   
//    Test_ANO_DT();       //匿名上位机观察数据波形  不使用DMA模式，请将宏定义#define USE_DMA  0  
//    Test_UART_DMA();     //匿名上位机观察波形，使用DMA传输，不占用CPU时间，点击匿名上位机的陀螺仪校准，核心板红灯亮
//    Test_PWM_nCH();      //测试PWM                             PWM1， 产生PWM波  用示波器观察
//    Test_QTMR_PWM();     //测试QTMR 产生的PWM                  示波器观察
//    Test_QTMR_QD();      //测试QTMR正交解码功能
//    Test_Servo();        //数字舵机测试      注意 初次使用此函数时，舵机不要装舵盘，防止舵机卡死，设置中值后再装车，可以自行设置中值
//    Test_Servo_K();        //K车舵机测试
//    Test_Motor();        //直流电机驱动测试，                  用龙邱全桥驱动板  注意： 电机1： 使用 L5  M5   电机2：使用A8  A9  电机3：使用 C9 B9  电机4：A10 J1
//    Test_ENCoder();      //编码器采集
//    Test_Camera_Reprot();  //上位机看图  7725RGB565 / 7725 灰度 / 神眼灰度
//    Test_SGP18_OV7725();   //测试OV7725RGB 和TFT1.8
//    Test_SGP18_Camera();   //测试神眼 Or 7725 二值化 + TFT1.8  
    Test_OLED_Camera();      //测试神眼 Or 7725 二值化 + OLED 
//----------------------------------------------------------------------------------------- 
     
}
