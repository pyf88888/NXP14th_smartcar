
#include "include.h"



/******************************************************************************/
//【作  者】
//【功能说明】初始化ADC
//【软件版本】V1.0
//【最后更新】
//【函数名】void ADC_Init(ADC_Type *base)
//【参数值】ADC1,ADC2
//【返回值】无
//【简  例】ADC_init（ADC1）初始化ADC1模块 
/******************************************************************************/


void My_ADC_Init(ADC_Type *base)  //ADC初始化
{          
    adc_config_t adcConfigStrcut;   

    ADC_GetDefaultConfig(&adcConfigStrcut);
    //adcConfigStrcut.resolution=kADC_Resolution12Bit;              //默认均为12位
    ADC_Init(base, &adcConfigStrcut);                               //初始化
    
    ADC_SetHardwareAverageConfig(base, kADC_HardwareAverageCount32);//硬件平均值
    /* Do auto hardware calibration. */
    ADC_DoAutoCalibration(base);                                    //硬件校准
}   
 
    
    
 
/******************************************************************************/
//【作  者】
//【功能说明】读取ADC 第 通道数值
//【软件版本】V1.0
//【最后更新】 
//【函数名】uint16_t ReadADC(ADC_Type *base,ADCn_Ch_e CH)
//【参数值】base：ADC1,ADC2；CH：0~31
//【返回值】12位adc值
//【简  例】batv=ReadADC(ADC1,ADC1_CH0)，读取ADC1第0通道数值
/******************************************************************************/


uint16_t ReadADC(ADC_Type *base,ADCn_Ch_e CH)
{     
  adc_channel_config_t adcChannelConfigStruct;                          //配置结构体
  
  adcChannelConfigStruct.channelNumber = CH;
  adcChannelConfigStruct.enableInterruptOnConversionCompleted = false;  //转换完成中断
  ADC_SetChannelConfig(base, 0, &adcChannelConfigStruct);                //开启转换
  while (0U == ADC_GetChannelStatusFlags(base,0)){;}                    //等待转换完成
  return (ADC_GetChannelConversionValue(base,0));                       //返回转换结果
}


/*****************************************************************************/
//【作  者】
//【功能说明】测试读取K12电压值，并转换为小数值，串口和OLED输出
//【软件版本】V1.0
//【最后更新】 
//【函数名】uint16_t ReadADC(ADC_Type *base,ADCn_Ch_e CH)
//【参数值】无
//【返回值】无
//【简  例】Test_ADC()，
/****************************************************************************/


void Test_ADC(void) //测试ADC,获取6个通道的ADC值，并打印
{
    char txt[16];
      
    TFTSPI_Init();                  //LCD初始化 
    TFTSPI_CLS(u16BLUE);           //清屏
    TFTSPI_P8X16Str(2,0,(uint8_t*)"LQ ADC Test Bat",u16RED,u16BLUE);
    printf("\r\nLQ ADC Test Bat");

    My_ADC_Init(ADC1);             //电源低压报警ADC初始化
    uint16_t batv0=(uint16_t)(ReadADC(ADC1,ADC1_CH10)*0.46);  //获取ADC值 K12 电池电压 * 100
    uint16_t batv1=(uint16_t)(ReadADC(ADC1,ADC1_CH4)*0.806);  //获取ADC值 L10
    uint16_t batv2=(uint16_t)(ReadADC(ADC1,ADC1_CH7)*0.806);  //获取ADC值 L11
    uint16_t batv3=(uint16_t)(ReadADC(ADC1,ADC1_CH8)*0.806);  //获取ADC值 M12
    uint16_t batv4=(uint16_t)(ReadADC(ADC1,ADC1_CH3)*0.806);  //获取ADC值 H14
    uint16_t batv5=(uint16_t)(ReadADC(ADC1,ADC1_CH1)*0.806);  //获取ADC值 K14
    uint16_t batv6=(uint16_t)(ReadADC(ADC1,ADC1_CH2)*0.806);  //获取ADC值 L14
    while (1)
    {           
        //测试ADC转换   ADC的基准电压是3.3V的，RT1052母板上，K12通过1K和4.7K的分压电阻测量电池电压 电池电压 =  （ReadADC / 4095） * 3.3 * （1 + 4.7）
        batv0 = (uint16_t)(batv0*0.7 + 0.3*(uint16_t)(ReadADC(ADC1,ADC1_CH10)*0.46));//电压采集 
        
        batv1 = (uint16_t)(batv1*0.7 + 0.3*ReadADC(ADC1,ADC1_CH4)*0.806);             //获取ADC值 L10
        batv2 = (uint16_t)(batv2*0.7 + 0.3*ReadADC(ADC1,ADC1_CH7)*0.806);             //获取ADC值 L11
        batv3 = (uint16_t)(batv3*0.7 + 0.3*ReadADC(ADC1,ADC1_CH8)*0.806);             //获取ADC值 M12
        batv4 = (uint16_t)(batv4*0.7 + 0.3*ReadADC(ADC1,ADC1_CH3)*0.806);             //获取ADC值 H14
        batv5 = (uint16_t)(batv5*0.7 + 0.3*ReadADC(ADC1,ADC1_CH1)*0.806);             //获取ADC值 K14
        batv6 = (uint16_t)(batv6*0.7 + 0.3*ReadADC(ADC1,ADC1_CH2)*0.806);  
        
        printf("\r\n/***********************%3.2fV\r\n ",batv0/100.0f);
        sprintf(txt,"BAT:%3.2fV ",batv0/100.0f);
        TFTSPI_P8X16Str(5,1,(uint8_t*)txt,u16RED,u16BLUE);
                           
//        printf("\r\nADC2_IN10  :%d.%dV ",batv/100,batv%100);     //浮点数计算量比较大，可以采用这种方法输出小数点
//        sprintf(txt,"BAT:%d.%dV ",batv/100,batv%100);
//        LCD_P8x16Str(20,3,(uint8_t*)txt); 
        //LED闪烁
        
        printf("\r\n/***********************%3.2fV\r\n ",batv1/1000.0f);
        sprintf(txt,"L10:%3.2fV ",batv1/1000.0f);
        TFTSPI_P8X16Str(5,2,(uint8_t*)txt,u16RED,u16BLUE);
        
        printf("\r\n/***********************%3.2fV\r\n ",batv2/1000.0f);
        sprintf(txt,"L11:%3.2fV ",batv2/1000.0f);
        TFTSPI_P8X16Str(5,3,(uint8_t*)txt,u16RED,u16BLUE);
        
        printf("\r\n/***********************%3.2fV\r\n ",batv3/1000.0f);
        sprintf(txt,"M12:%3.2fV ",batv3/1000.0f);
        TFTSPI_P8X16Str(5,4,(uint8_t*)txt,u16RED,u16BLUE);
        
        printf("\r\n/***********************%3.2fV\r\n ",batv4/1000.0f);
        sprintf(txt,"H14:%3.2fV ",batv4/1000.0f);
        TFTSPI_P8X16Str(5,5,(uint8_t*)txt,u16RED,u16BLUE);
        
        printf("\r\n/***********************%3.2fV\r\n ",batv5/1000.0f);
        sprintf(txt,"K14:%3.2fV ",batv5/1000.0f);
        TFTSPI_P8X16Str(5,6,(uint8_t*)txt,u16RED,u16BLUE);
        
        printf("\r\n/***********************%3.2fV\r\n ",batv6/1000.0f);
        sprintf(txt,"L14:%3.2fV ",batv6/1000.0f);
        TFTSPI_P8X16Str(5,7,(uint8_t*)txt,u16RED,u16BLUE);
        
        LED_Color_Reverse(blue);    //蓝灯   
        delayms(100);
    }
}
