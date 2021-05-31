#include "adc.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103������
//ADC��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/5/29
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

ADC_HandleTypeDef  ADC1_Handler;		//ADC���
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
//��ʼ��ADC
//ch: ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
void MY_ADC_Init(void)
{ 
	 ADC_ChannelConfTypeDef sConfig = {0};

	  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	  */
	  hadc1.Instance = ADC1;
	  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	  hadc1.Init.LowPowerAutoWait = DISABLE;
	  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
	  hadc1.Init.ContinuousConvMode = DISABLE;
	  hadc1.Init.NbrOfConversion = 1;
	  hadc1.Init.DiscontinuousConvMode = DISABLE;
	  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	  hadc1.Init.DMAContinuousRequests = DISABLE;
	  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
	  hadc1.Init.SamplingTimeCommon2 = ADC_SAMPLETIME_160CYCLES_5;
	  hadc1.Init.OversamplingMode = DISABLE;
	  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
	  if (HAL_ADC_Init(&hadc1) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /** Configure Regular Channel
	  */
	  sConfig.Channel = ADC_CHANNEL_16;//ADC_CHANNEL_4;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_1;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /** Configure Regular Channel
	  */
	  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	  sConfig.Rank = ADC_REGULAR_RANK_2;
	  sConfig.SamplingTime = ADC_SAMPLINGTIME_COMMON_2;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
	  /** Configure Regular Channel
	  */
	  sConfig.Channel = ADC_CHANNEL_VREFINT;
	  sConfig.Rank = ADC_REGULAR_RANK_3;
	  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}


//���ADCֵ
//ch: ͨ��ֵ 0~16��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
//����ֵ:ת�����
uint16_t Get_Adc(uint32_t ch)
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //ͨ��
    ADC1_ChanConf.Rank=1;                                       //��1�����У�����1
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_160CYCLES_5 ;//ADC_SAMPLETIME_239CYCLES_5;      //����ʱ��
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //ͨ������
	
    HAL_ADC_Start(&ADC1_Handler);                               //����ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //��ѯת��
 
	return (uint16_t)HAL_ADC_GetValue(&ADC1_Handler);	        	//�������һ��ADC1�������ת�����
}
//��ȡָ��ͨ����ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
uint16_t Get_Adc_Average(uint32_t ch,uint8_t times)
{
	uint32_t temp_val=0;
	uint8_t t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		HAL_Delay(5);
	}
	return temp_val/times;
} 
/*************************************************************************************************************
**
* Function Name:uint8_t Get_Adc_Reslult(void)
* Function: ADC of result 
* Input Ref:NO
* Return Ref: ADC converter resut
*
*************************************************************************************************************/
uint8_t Get_Adc_Result(void)
{

	uint16_t  adcx;
    float temp;
	adcx=Get_Adc_Average(ADC_CHANNEL_16,20);//»ñÈ¡Í¨µÀ1µÄ×ª»»Öµ£¬20´ÎÈ¡Æ½¾ù
	temp=(float)adcx*(3.3/4096);
	temp = (uint8_t)temp;
	return temp;

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**ADC1 GPIO Configuration
    PA4     ------> ADC1_IN4
    PA12    ------> ADC1_IN16
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12;//GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//    /* ADC1 DMA Init */
//    /* ADC1 Init */
//    hdma_adc1.Instance = DMA1_Channel1;
//    hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
//    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
//    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
//    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//    hdma_adc1.Init.Mode = DMA_CIRCULAR;
//    hdma_adc1.Init.Priority = DMA_PRIORITY_HIGH;
//    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
//    {
//      Error_Handler();
//    }
//
//    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc1);

    /* ADC1 interrupt Init */
   // HAL_NVIC_SetPriority(ADC1_IRQn, 0, 0);
   // HAL_NVIC_EnableIRQ(ADC1_IRQn);
  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC_CLK_DISABLE();

    /**ADC1 GPIO Configuration
    PA4     ------> ADC1_IN4
    PA12    ------> ADC1_IN16
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12);// HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4);

    /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);

    /* ADC1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(ADC1_IRQn);
  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}
