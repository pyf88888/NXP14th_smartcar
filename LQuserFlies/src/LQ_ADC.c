
#include "include.h"



/******************************************************************************/
//����  �ߡ�
//������˵������ʼ��ADC
//������汾��V1.0
//�������¡�
//����������void ADC_Init(ADC_Type *base)
//������ֵ��ADC1,ADC2
//������ֵ����
//����  ����ADC_init��ADC1����ʼ��ADC1ģ�� 
/******************************************************************************/


void My_ADC_Init(ADC_Type *base)  //ADC��ʼ��
{          
    adc_config_t adcConfigStrcut;   

    ADC_GetDefaultConfig(&adcConfigStrcut);
    //adcConfigStrcut.resolution=kADC_Resolution12Bit;              //Ĭ�Ͼ�Ϊ12λ
    ADC_Init(base, &adcConfigStrcut);                               //��ʼ��
    
    ADC_SetHardwareAverageConfig(base, kADC_HardwareAverageCount32);//Ӳ��ƽ��ֵ
    /* Do auto hardware calibration. */
    ADC_DoAutoCalibration(base);                                    //Ӳ��У׼
}   
 
    
    
 
/******************************************************************************/
//����  �ߡ�
//������˵������ȡADC �� ͨ����ֵ
//������汾��V1.0
//�������¡� 
//����������uint16_t ReadADC(ADC_Type *base,ADCn_Ch_e CH)
//������ֵ��base��ADC1,ADC2��CH��0~31
//������ֵ��12λadcֵ
//����  ����batv=ReadADC(ADC1,ADC1_CH0)����ȡADC1��0ͨ����ֵ
/******************************************************************************/


uint16_t ReadADC(ADC_Type *base,ADCn_Ch_e CH)
{     
  adc_channel_config_t adcChannelConfigStruct;                          //���ýṹ��
  
  adcChannelConfigStruct.channelNumber = CH;
  adcChannelConfigStruct.enableInterruptOnConversionCompleted = false;  //ת������ж�
  ADC_SetChannelConfig(base, 0, &adcChannelConfigStruct);                //����ת��
  while (0U == ADC_GetChannelStatusFlags(base,0)){;}                    //�ȴ�ת�����
  return (ADC_GetChannelConversionValue(base,0));                       //����ת�����
}


/*****************************************************************************/
//����  �ߡ�
//������˵�������Զ�ȡK12��ѹֵ����ת��ΪС��ֵ�����ں�OLED���
//������汾��V1.0
//�������¡� 
//����������uint16_t ReadADC(ADC_Type *base,ADCn_Ch_e CH)
//������ֵ����
//������ֵ����
//����  ����Test_ADC()��
/****************************************************************************/


void Test_ADC(void) //����ADC,��ȡ6��ͨ����ADCֵ������ӡ
{
    char txt[16];
      
    TFTSPI_Init();                  //LCD��ʼ�� 
    TFTSPI_CLS(u16BLUE);           //����
    TFTSPI_P8X16Str(2,0,(uint8_t*)"LQ ADC Test Bat",u16RED,u16BLUE);
    printf("\r\nLQ ADC Test Bat");

    My_ADC_Init(ADC1);             //��Դ��ѹ����ADC��ʼ��
    uint16_t batv0=(uint16_t)(ReadADC(ADC1,ADC1_CH10)*0.46);  //��ȡADCֵ K12 ��ص�ѹ * 100
    uint16_t batv1=(uint16_t)(ReadADC(ADC1,ADC1_CH4)*0.806);  //��ȡADCֵ L10
    uint16_t batv2=(uint16_t)(ReadADC(ADC1,ADC1_CH7)*0.806);  //��ȡADCֵ L11
    uint16_t batv3=(uint16_t)(ReadADC(ADC1,ADC1_CH8)*0.806);  //��ȡADCֵ M12
    uint16_t batv4=(uint16_t)(ReadADC(ADC1,ADC1_CH3)*0.806);  //��ȡADCֵ H14
    uint16_t batv5=(uint16_t)(ReadADC(ADC1,ADC1_CH1)*0.806);  //��ȡADCֵ K14
    uint16_t batv6=(uint16_t)(ReadADC(ADC1,ADC1_CH2)*0.806);  //��ȡADCֵ L14
    while (1)
    {           
        //����ADCת��   ADC�Ļ�׼��ѹ��3.3V�ģ�RT1052ĸ���ϣ�K12ͨ��1K��4.7K�ķ�ѹ���������ص�ѹ ��ص�ѹ =  ��ReadADC / 4095�� * 3.3 * ��1 + 4.7��
        batv0 = (uint16_t)(batv0*0.7 + 0.3*(uint16_t)(ReadADC(ADC1,ADC1_CH10)*0.46));//��ѹ�ɼ� 
        
        batv1 = (uint16_t)(batv1*0.7 + 0.3*ReadADC(ADC1,ADC1_CH4)*0.806);             //��ȡADCֵ L10
        batv2 = (uint16_t)(batv2*0.7 + 0.3*ReadADC(ADC1,ADC1_CH7)*0.806);             //��ȡADCֵ L11
        batv3 = (uint16_t)(batv3*0.7 + 0.3*ReadADC(ADC1,ADC1_CH8)*0.806);             //��ȡADCֵ M12
        batv4 = (uint16_t)(batv4*0.7 + 0.3*ReadADC(ADC1,ADC1_CH3)*0.806);             //��ȡADCֵ H14
        batv5 = (uint16_t)(batv5*0.7 + 0.3*ReadADC(ADC1,ADC1_CH1)*0.806);             //��ȡADCֵ K14
        batv6 = (uint16_t)(batv6*0.7 + 0.3*ReadADC(ADC1,ADC1_CH2)*0.806);  
        
        printf("\r\n/***********************%3.2fV\r\n ",batv0/100.0f);
        sprintf(txt,"BAT:%3.2fV ",batv0/100.0f);
        TFTSPI_P8X16Str(5,1,(uint8_t*)txt,u16RED,u16BLUE);
                           
//        printf("\r\nADC2_IN10  :%d.%dV ",batv/100,batv%100);     //�������������Ƚϴ󣬿��Բ������ַ������С����
//        sprintf(txt,"BAT:%d.%dV ",batv/100,batv%100);
//        LCD_P8x16Str(20,3,(uint8_t*)txt); 
        //LED��˸
        
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
        
        LED_Color_Reverse(blue);    //����   
        delayms(100);
    }
}
