
#include "include.h"
/*******************************************************************************
*  SDK提供了两种在Noncacheable区定义缓冲区和变量的方法：
*  AT_NONCACHEABLE_SECTION_ALIGN(var, alignbytes)
*  AT_NONCACHEABLE_SECTION(var)
******************************************************************************/
AT_NONCACHEABLE_SECTION_ALIGN(uint16_t lcdFrameBuf[2][LCD_HEIGHT][LCD_WIDTH], FRAME_BUFFER_ALIGN);               //LCD数据缓存区

uint8_t counter;       //LCD有两个缓冲区，一个当前显示用，一个缓冲用

int OFFSET0=0;      //最远处，赛道中心值综合偏移量
int OFFSET1=0;      //第二格
int OFFSET2=0;      //最近，第三格


/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】Z
【功能说明】oled + camera test
【软件版本】V1.0
【最后更新】2018年11月7日 
【函数名】
【返回值】无
【参数值】无
【实例】 TFT1.8显示OV7725 RGB图像测试  7725分辨率设置为320*240
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void Test_SGP18_OV7725(void)
{
    TFTSPI_Init();               //TFT1.8初始化     
    uint32_t fullCameraBufferAddr;     
    My_Camera_Init();
//    if (SCB_CCR_IC_Msk == (SCB_CCR_IC_Msk & SCB->CCR)) {   
//        SCB_DisableICache();
//    }
    if (SCB_CCR_DC_Msk == (SCB_CCR_DC_Msk & SCB->CCR)) {//注意，使用csiFrameBuf数组时，最好关闭Cache 不然上次数据可能会存放在cache里面，造成数据错乱
        SCB_DisableDCache();
    }
    delayms(200);        //延时200毫秒     
    while (1)
    {     
        // Wait to get the full frame buffer to show. 
        while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &fullCameraBufferAddr))  //摄像头CSI缓存区已满
        {
        }   
        CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, fullCameraBufferAddr);//将照相机缓冲区提交到缓冲队列        
        TFTSPI_Set_Pos(20,0,(APP_CAMERA_WIDTH/4-1) + 20,APP_CAMERA_HEIGHT/4);
        for(int i = 0; i < APP_CAMERA_HEIGHT; i+=4)  //隔一行取一行
        {
            for(int j = 0; j < APP_CAMERA_WIDTH ; j+=4)//隔一列取一列
            {
                TFTSPI_Write_Word (*((uint16_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH + j)); //显示数据
            }
        }
        
        LED_Color_Reverse(red); //EVK LED闪烁    
    }
}
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】Z
【功能说明】TFT1.8 + camera test
【软件版本】V1.0
【最后更新】2018年11月7日 
【函数名】
【返回值】无
【参数值】无
【实例】 TFT1.8显示神眼二值化 图像测试  神眼分辨率设置为752*480
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void Test_SGP18_Camera(void)
{
    TFTSPI_Init();               //TFT1.8初始化  
    TFTSPI_CLS(u16BLUE);           //清屏
    uint32_t fullCameraBufferAddr;   
#ifdef LQOV7725
    cameraConfig.pixelFormat = kVIDEO_PixelFormatYUYV;
#endif
    My_Camera_Init();
    delayms(200);        //延时200毫秒  
    uint16_t color = 0;
//    if (SCB_CCR_IC_Msk == (SCB_CCR_IC_Msk & SCB->CCR)) {   
//        SCB_DisableICache();
//    }
    if (SCB_CCR_DC_Msk == (SCB_CCR_DC_Msk & SCB->CCR)) {//注意，使用csiFrameBuf数组时，最好关闭D-Cache 不然上次数据可能会存放在cache里面，造成数据错乱
        SCB_DisableDCache();
    }
    while (1)
    {     
        // Wait to get the full frame buffer to show. 
        while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &fullCameraBufferAddr))  //摄像头CSI缓存区已满
        {
        }   
        
#ifdef LQMT9V034  
#if (IMAGEH == 480/4  && IMAGEW == 752/4)
        TFTSPI_Set_Pos(15,0,(uint8_t)(APP_CAMERA_WIDTH/2-1)+15 ,APP_CAMERA_HEIGHT);//注意 设置显示大小要与下面的实际显示大小相等，不然会显示不出来或者花屏
        for(int i = 0; i < APP_CAMERA_HEIGHT; i+=2)  //  480/4/2/2 = 30
        {
            for(int j = 0; j < APP_CAMERA_WIDTH*2; j+=2)//隔2列取一列  752*2/4/2 = 188   //两行数据 一行94像素  实际显示分辨率  94*60
            {
                //灰度显示
                color = 0;
                color=(((*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j))>>3))<<11;
                color=color|((((*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j))>>2))<<5);
                color=color|(((*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j))>>3));
                TFTSPI_Write_Word(color);
                //二值化显示
//                if(*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j) > 0x60)  //阈值0x60 二值化显示
//                  TFTSPI_Write_Word (0xffff); //显示数据
//                else
//                  TFTSPI_Write_Word (0x0000); //显示数据
            }
        }
#elif (IMAGEH == 480/2  && IMAGEW == 752/2)
        TFTSPI_Set_Pos(15,0,(uint8_t)(APP_CAMERA_WIDTH/4-1)+15 ,APP_CAMERA_HEIGHT/2);//注意 设置显示大小要与下面的实际显示大小相等，不然会显示不出来或者花屏
        for(int i = 0; i < APP_CAMERA_HEIGHT; i+=4)  //  480/2/2/4 = 30
        {
            for(int j = 0; j < APP_CAMERA_WIDTH*2; j+=4)//隔2列取一列  752*2/2/4 = 188   //两行数据 一行94像素  实际显示分辨率  94*60
            {
                //灰度显示
                color = 0;
                color=(((*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j))>>3))<<11;
                color=color|((((*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j))>>2))<<5);
                color=color|(((*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j))>>3));
                TFTSPI_Write_Word(color);
                //二值化显示
//                if(*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j) > 0x60)  //阈值0x60 二值化显示
//                  TFTSPI_Write_Word (0xffff); //显示数据
//                else
//                  TFTSPI_Write_Word (0x0000); //显示数据
            }
        }
#elif (IMAGEH == 480  && IMAGEW == 752)
        TFTSPI_Set_Pos(15,0,(uint8_t)(APP_CAMERA_WIDTH/8-1)+15 ,APP_CAMERA_HEIGHT/4);//注意 设置显示大小要与下面的实际显示大小相等，不然会显示不出来或者花屏
        for(int i = 0; i < APP_CAMERA_HEIGHT; i+=8)  //  480/1/2/8 = 30
        {
            for(int j = 0; j < APP_CAMERA_WIDTH*2; j+=8)//隔2列取一列  752*2/1/8 = 188   //两行数据 一行94像素  实际显示分辨率  94*60
            {
                //灰度显示
                color = 0;
                color=(((*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j))>>3))<<11;
                color=color|((((*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j))>>2))<<5);
                color=color|(((*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j))>>3));
                TFTSPI_Write_Word(color);
                //二值化显示
//                if(*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j) > 0x60)  //阈值0x60 二值化显示
//                  TFTSPI_Write_Word (0xffff); //显示数据
//                else
//                  TFTSPI_Write_Word (0x0000); //显示数据
            }
        }
#endif
#else  // 7725 的灰度图像  注意，看灰度图像时，7725使用YUYV格式 cameraConfig = { .pixelFormat = kVIDEO_PixelFormatYUYV }
//        TFTSPI_Set_Pos(0,0,APP_CAMERA_WIDTH/2,APP_CAMERA_HEIGHT/2);
//        for(int i = 0; i < APP_CAMERA_HEIGHT; i+=2)  //隔2行取一行 240 / 2 = 120
//        {
//            for(int j = 1; j < APP_CAMERA_WIDTH *2 ; j+=4)//隔4列取一列 摄像头UYVY格式  Y是灰度，320/2=160
//            {
//                if(*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j) > 0x60)  //阈值0x60 二值化显示
//                  TFTSPI_Write_Word (0xffff); //显示数据
//                else
//                  TFTSPI_Write_Word (0x0000); //显示数据
//            }
//        }
        TFTSPI_Set_Pos(20,0,(APP_CAMERA_WIDTH/4-1) + 20,APP_CAMERA_HEIGHT/4);
        for(int i = 0; i < APP_CAMERA_HEIGHT; i+=4)  //隔4行取一行 240 / 4 = 60
        {
            for(int j = 1; j < APP_CAMERA_WIDTH *2 ; j+=8)//隔8列取一列 摄像头UYVY格式  Y是灰度，320/4=80
            {
                if(*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j) > 0x60)  //阈值0x60 二值化显示
                  TFTSPI_Write_Word (0xffff); //显示数据
                else
                  TFTSPI_Write_Word (0x0000); //显示数据
            }
        }
#endif
        CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, fullCameraBufferAddr);//将照相机缓冲区提交到缓冲队列  
        LED_Color_Reverse(red); //EVK LED闪烁  
    }
}

/**/











uint8_t Image_Use[Use_ROWS][Use_Line]; //压缩后之后用于存要使用数据
uint8_t Pixle[Use_ROWS][Use_Line];     //存放二值化后的数据
uint32_t fullCameraBufferAddr;
// 获取需要的图像数据
__ramfunc void Get_Use_Image(void)
{
    if (SCB_CCR_DC_Msk == (SCB_CCR_DC_Msk & SCB->CCR)) {//注意，使用csiFrameBuf数组时，最好刷新一下D-Cache 不然上次数据可能会存放在cache里面，造成数据错乱
        SCB_DisableDCache();
    }
    SCB_EnableDCache();
#ifdef LQMT9V034
#if (IMAGEH == 480  && IMAGEW == 752)
    for(int i = 0, x = 0; i < APP_CAMERA_HEIGHT; i+=8, x++)  //  480/1/2/8 = 30
        {
            for(int j = 0, y = 0; j < APP_CAMERA_WIDTH*2; j+=8, y++)//隔2列取一列  752*2/1/8 = 188   //两行数据 一行94像素  实际显示分辨率  94*60
            {
                if(y < Use_Line)
                    Image_Use[x*2][y] = (*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j));
                else
                    Image_Use[x*2 + 1][y - Use_Line] = (*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j));
            }
        }
    
#elif (IMAGEH == 480/4  && IMAGEW == 752/4)
    for(int i = 0, x = 0; i < APP_CAMERA_HEIGHT; i+=2, x++)  //  480/4/2/2 = 30
        {
            for(int j = 0, y = 0; j < APP_CAMERA_WIDTH*2; j+=2, y++)//隔2列取一列  752*2/4/2 = 188   //两行数据 一行94像素  实际显示分辨率  94*60
            {
                if(y < Use_Line)
                    Image_Use[x*2][y] = (*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j));
                else
                    Image_Use[x*2 + 1][y - Use_Line] = (*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j));
            }
        }
    
#elif (IMAGEH == 480/2  && IMAGEW == 752/2)
    for(int i = 0, x = 0; i < APP_CAMERA_HEIGHT; i+=4, x++)  //  480/2/2/4 = 30
        {
            for(int j = 0, y = 0; j < APP_CAMERA_WIDTH*2; j+=4, y++)//隔2列取一列  752*2/2/4 = 188   //两行数据 一行94像素  实际显示分辨率  94*60
            {
                if(y < Use_Line)
                    Image_Use[x*2][y] = (*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j));
                else
                    Image_Use[x*2 + 1][y - Use_Line] = (*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j));
            }
        }
#endif  
#elif (APP_CAMERA_WIDTH == 320 && APP_CAMERA_HEIGHT == 240)
    for(int i = 0, x = Use_ROWS-1; i < APP_CAMERA_HEIGHT; i+=4, x--)  //隔4行取一行 240 / 4 = 60
        {
            for(int j = 1, y = Use_Line - 1; j < APP_CAMERA_WIDTH *2; j+=8, y--)//隔8列取一列 摄像头UYVY格式  Y是灰度，320/4=80
            {
                Image_Use[x][y] = (*((uint8_t *)fullCameraBufferAddr +  i * APP_CAMERA_WIDTH * 2 + j)); 
            }
        }
    
#endif
}


/*************************************************************** 
* 
* 函数名称：uint8_t GetOSTU(uint8_t tmImage[IMAGEH][IMAGEW]) 
* 功能说明：求阈值大小 
* 参数说明： 
* 函数返回：阈值大小 
* 修改时间：2018年3月27日 
* 备 注： 
参考：https://blog.csdn.net/zyzhangyue/article/details/45841255
      https://www.cnblogs.com/moon1992/p/5092726.html
      https://www.cnblogs.com/zhonghuasong/p/7250540.html     
Ostu方法又名最大类间差方法，通过统计整个图像的直方图特性来实现全局阈值T的自动选取，其算法步骤为：
1) 先计算图像的直方图，即将图像所有的像素点按照0~255共256个bin，统计落在每个bin的像素点数量
2) 归一化直方图，也即将每个bin中像素点数量除以总的像素点
3) i表示分类的阈值，也即一个灰度级，从0开始迭代
4) 通过归一化的直方图，统计0~i 灰度级的像素(假设像素值在此范围的像素叫做前景像素) 所占整幅图像的比例w0，并统计前景像素的平均灰度u0；统计i~255灰度级的像素(假设像素值在此范围的像素叫做背景像素) 所占整幅图像的比例w1，并统计背景像素的平均灰度u1；
5) 计算前景像素和背景像素的方差 g = w0*w1*(u0-u1) (u0-u1)
6) i++；转到4)，直到i为256时结束迭代
7）将最大g相应的i值作为图像的全局阈值
缺陷:OSTU算法在处理光照不均匀的图像的时候，效果会明显不好，因为利用的是全局像素信息。
***************************************************************/ 
__ramfunc uint8_t GetOSTU(uint8_t tmImage[Use_ROWS][Use_Line]) 
{ 
    int16_t i,j; 
    uint32_t Amount = 0; 
    uint32_t PixelBack = 0; 
    uint32_t PixelIntegralBack = 0; 
    uint32_t PixelIntegral = 0; 
    int32_t PixelIntegralFore = 0; 
    int32_t PixelFore = 0; 
    double OmegaBack, OmegaFore, MicroBack, MicroFore, SigmaB, Sigma; // 类间方差; 
    int16_t MinValue, MaxValue; 
    uint8_t Threshold = 0;
    uint8_t HistoGram[256];              //  
    
    for (j = 0; j < 256; j++)  HistoGram[j] = 0; //初始化灰度直方图 
    
    for (j = 0; j < Use_ROWS; j++) 
    { 
        for (i = 0; i < Use_Line; i++) 
        { 
            HistoGram[tmImage[j][i]]++; //统计灰度级中每个像素在整幅图像中的个数
        } 
    } 
    
    for (MinValue = 0; MinValue < 256 && HistoGram[MinValue] == 0; MinValue++) ;        //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistoGram[MinValue] == 0; MaxValue--) ; //获取最大灰度的值
    
    if (MaxValue == MinValue)     return MaxValue;         // 图像中只有一个颜色    
    if (MinValue + 1 == MaxValue)  return MinValue;        // 图像中只有二个颜色
    
    for (j = MinValue; j <= MaxValue; j++)    Amount += HistoGram[j];        //  像素总数
    
    PixelIntegral = 0;
    for (j = MinValue; j <= MaxValue; j++)
    {
        PixelIntegral += HistoGram[j] * j;//灰度值总数
    }
    SigmaB = -1;
    for (j = MinValue; j < MaxValue; j++)
    {
        PixelBack = PixelBack + HistoGram[j];    //前景像素点数
        PixelFore = Amount - PixelBack;         //背景像素点数
        OmegaBack = (double)PixelBack / Amount;//前景像素百分比
        OmegaFore = (double)PixelFore / Amount;//背景像素百分比
        PixelIntegralBack += HistoGram[j] * j;  //前景灰度值
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
        MicroBack = (double)PixelIntegralBack / PixelBack;   //前景灰度百分比
        MicroFore = (double)PixelIntegralFore / PixelFore;   //背景灰度百分比
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//计算类间方差
        if (Sigma > SigmaB)                    //遍历最大的类间方差g //找出最大类间方差以及对应的阈值
        {
            SigmaB = Sigma;
            Threshold = j;
        }
    }
    return Threshold;                        //返回最佳阈值;
} 

//二值化处理
__ramfunc void Camera_0_1_Handle(void)
{
    int i = 0,j = 0;
    uint8_t GaveValue;
    uint32_t tv=0;
    uint8_t Threshold = 0;
    //累加
    for(i = 0; i <Use_ROWS; i++)
    {    
        for(j = 0; j <Use_Line; j++)
        {                            
            tv+=Image_Use[i][j];   //累加  
        } 
    }
    GaveValue = tv/Use_ROWS/Use_Line;     //求平均值,光线越暗越小，全黑约35，对着屏幕约160，一般情况下大约100 
//    Threshold = GetOSTU(Image_Use);  //大津法求阈值
    //按照均值的比例进行二值化
    Threshold = GaveValue*7/10+10;        //此处阈值设置，根据环境的光线来设定 
    for(i = 0; i < Use_ROWS; i++)
    {
        for(j = 0; j < Use_Line; j++)
        {                                
            if(Image_Use[i][j] >Threshold) //二值化    
                Pixle[i][j] =1;        
            else                                        
                Pixle[i][j] =0;
        }    
    }
    
}

//三面以上反数围绕清除噪点
__ramfunc void Pixle_Filter(void)
{  
  int nr; //行
  int nc; //列
  
  for(nr=1; nr<Use_ROWS-1; nr++)
  {  	    
    for(nc=1; nc<Use_Line-1; nc=nc+1)
    {
      if((Pixle[nr][nc]==0)&&(Pixle[nr-1][nc]+Pixle[nr+1][nc]+Pixle[nr][nc+1]+Pixle[nr][nc-1]>2))         
      {
        Pixle[nr][nc]=1;
      }	
      else if((Pixle[nr][nc]==1)&&(Pixle[nr-1][nc]+Pixle[nr+1][nc]+Pixle[nr][nc+1]+Pixle[nr][nc-1]<2))         
      {
        Pixle[nr][nc]=0;
      }	
    }	  
  }  
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】Z
【功能说明】oled上显示
【软件版本】V1.0
【最后更新】2018年10月18日 
【函数名】
【返回值】无
【参数值】无
【实例】 
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
void Draw_Road(void)
{ 	 
  uint8_t i = 0, j = 0,temp=0;
  for(i=0;i<56;i+=8)// 56行 
  {
    LCD_Set_Pos(24,i/8+1);
    for(j=0;j<Use_Line;j++) 
    { 
      temp=0;
      if(Pixle[0+i][j]) temp|=1;
      if(Pixle[1+i][j]) temp|=2;
      if(Pixle[2+i][j]) temp|=4;
      if(Pixle[3+i][j]) temp|=8;
      if(Pixle[4+i][j]) temp|=0x10;
      if(Pixle[5+i][j]) temp|=0x20;
      if(Pixle[6+i][j]) temp|=0x40;
      if(Pixle[7+i][j]) temp|=0x80;
      LCD_WrDat(temp); 	  	  	  	  
    }
  }  
}

/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
【作  者】Z
【功能说明】oled + camera test
【软件版本】V1.0
【最后更新】2018年11月7日 
【函数名】
【返回值】无
【参数值】无
【实例】 OLED显示神眼 OV7725二值化 图像测试
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/   
void Test_OLED_Camera(void)
{
    LCD_Init();               //LCD初始化 
    LCD_CLS();                //LCD清屏 
    LCD_Show_Frame94();
#ifdef LQOV7725
    LCD_CLS();                //LCD清屏 
    LCD_Show_Frame80();
    cameraConfig.pixelFormat = kVIDEO_PixelFormatYUYV;
#endif
    My_Camera_Init();
    delayms(200);        //延时200毫秒  
//    if (SCB_CCR_IC_Msk == (SCB_CCR_IC_Msk & SCB->CCR)) {   
//        SCB_DisableICache();
//    }
    
    while (1)
    {     
        // Wait to get the full frame buffer to show. 
        while (kStatus_Success != CAMERA_RECEIVER_GetFullBuffer(&cameraReceiver, &fullCameraBufferAddr))  //摄像头CSI缓存区已满
        {
        }   
        Get_Use_Image();     //获取使用数据
        Camera_0_1_Handle(); //二值化
        Pixle_Filter();      //滤波
        Draw_Road();         //显示
        CAMERA_RECEIVER_SubmitEmptyBuffer(&cameraReceiver, fullCameraBufferAddr);//将照相机缓冲区提交到缓冲队列  
        LED_Color_Reverse(red); //EVK LED闪烁  
    }
}
