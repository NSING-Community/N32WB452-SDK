1、功能说明
    1、TIM1输出3路互补波形
2、使用环境
    软件开发环境：  KEIL MDK-ARM V5.26.2.0
    硬件环境：      基于N32WB45xL_EVB_V1.1开发
3、使用说明
    系统配置；
        1、时钟源：
                    HSE=8M,PLL=144M,AHB=144M,APB1=36M,APB2=72M,TIM1 CLK=144M
        2、端口配置：
                    PA8选择为TIM1 CH1输出
                    PA9选择为TIM1 CH2输出
                    PA10选择为TIM1 CH3输出
                    PB13选择为TIM1 CH1N输出
                    PB14选择为TIM1 CH2N输出
                    PB15选择为TIM1 CH3N输出
                    PB12选择为TIM1 Breakin输入
        3、TIM：
                    TIM1 3路互补带死区，可以IOM刹车
    使用方法：
        1、编译后打开调试模式，用示波器或者逻辑分析仪观察TIM1的波形
        2、PB12接到GND，可观察到3路互补波形，PB12接到VDD，3路互补波形刹车。
4、注意事项
    无
    
1. Function description
     1. TIM1 outputs 3 complementary waveforms
2. Use environment
     Software development environment: KEIL MDK-ARM V5.26.2.0
     Hardware environment: Developed based on N32G4XM-STB V1.1
3. Instructions for use
     System Configuration;
         1. Clock source:
                     HSE=8M, PLL=144M, AHB=144M, APB1=36M, APB2=72M, TIM1 CLK=144M
         2. Port configuration:
                     PA8 is selected as TIM1 CH1 output
                     PA9 is selected as TIM1 CH2 output
                     PA10 is selected as TIM1 CH3 output
                     PB13 is selected as TIM1 CH1N output
                     PB14 is selected as TIM1 CH2N output
                     PB15 is selected as TIM1 CH3N output
                     PB12 is selected as TIM1 Breakin input
         3. TIM:
                     TIM1 3 complementary with dead zone, IOM brake
     Instructions:
         1. After compiling, turn on the debug mode, and use an oscilloscope or logic analyzer to observe the waveform of TIM1
         2. PB12 is connected to GND,and 3 complementary waveforms can be observed.PB12 is connected to GND,and 3 complementary will be break .
4. Matters needing attention
     without