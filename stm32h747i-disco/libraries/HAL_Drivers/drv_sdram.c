#include <board.h>
#include "drv_sdram.h"

#ifdef RT_USING_MEMHEAP_AS_HEAP
static struct rt_memheap system_heap;
#endif

static FMC_SDRAM_CommandTypeDef Command;
SDRAM_HandleTypeDef sdramHandle;

static void SDRAM_delay(__IO rt_uint32_t cnt)
{
    __IO rt_uint32_t index = 0;
    for(index = (100000 * cnt); index != 0; index--)
    {
    }
}

static void SDRAM_GPIO_Config(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;

  	__HAL_RCC_GPIOD_CLK_ENABLE();
  	__HAL_RCC_GPIOE_CLK_ENABLE();
  	__HAL_RCC_GPIOF_CLK_ENABLE();
  	__HAL_RCC_GPIOG_CLK_ENABLE();
  	__HAL_RCC_GPIOH_CLK_ENABLE();
  	__HAL_RCC_GPIOI_CLK_ENABLE();
     
	GPIO_InitStructure.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Pull      = GPIO_PULLUP;
	GPIO_InitStructure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStructure.Alternate = GPIO_AF12_FMC;

	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
							 GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOD,&GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
							 GPIO_PIN_11| GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE,&GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | \
							 GPIO_PIN_5 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOF,&GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | \
							 GPIO_PIN_8 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOG,&GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
							 GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOH,&GPIO_InitStructure);

	GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | \
							 GPIO_PIN_5 | GPIO_PIN_6 |GPIO_PIN_7 | GPIO_PIN_9 | GPIO_PIN_10; 
	HAL_GPIO_Init(GPIOI,&GPIO_InitStructure);
}

static void SDRAM_InitSequence(void)
{
	rt_uint32_t tmpr = 0;

	/* Step 1 ----------------------------------------------------------------*/
	/* 配置命令：开启提供给SDRAM的时钟 */
	Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
	Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber = 1;
	Command.ModeRegisterDefinition = 0;
	/* 发送配置命令 */
	HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

	/* Step 2: 延时100us */ 
	SDRAM_delay(1);

	/* Step 3 ----------------------------------------------------------------*/
	/* 配置命令：对所有的bank预充电 */ 
	Command.CommandMode = FMC_SDRAM_CMD_PALL;
	Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber = 1;
	Command.ModeRegisterDefinition = 0;
	/* 发送配置命令 */
	HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);   

	/* Step 4 ----------------------------------------------------------------*/
	/* 配置命令：自动刷新 */   
	Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber = 8;
	Command.ModeRegisterDefinition = 0;
	/* 发送配置命令 */
	HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

	/* Step 5 ----------------------------------------------------------------*/
	/* 设置sdram寄存器配置 */
	tmpr = (rt_uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
				   		SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
				   		SDRAM_MODEREG_CAS_LATENCY_3           |
				   		SDRAM_MODEREG_OPERATING_MODE_STANDARD |
				   		SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	/* 配置命令：设置SDRAM寄存器 */
	Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
	Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber = 1;
	Command.ModeRegisterDefinition = tmpr;
	/* 发送配置命令 */
	HAL_SDRAM_SendCommand(&sdramHandle, &Command, SDRAM_TIMEOUT);

	/* Step 6 ----------------------------------------------------------------*/

	/* 设置刷新计数器 */
	/* 刷新周期=64ms/4096行=15.625us */
	/* COUNT=(15.625us x Freq) - 20 */
	/* 设置自刷新速率 */
	HAL_SDRAM_ProgramRefreshRate(&sdramHandle, 1543); 
}

void SDRAM_Init(void)
{
    FMC_SDRAM_TimingTypeDef SdramTiming;
    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;
	
  	/* 配置FMC接口相关的 GPIO*/
  	SDRAM_GPIO_Config();

	/* 配置SDRAM时钟源 100MHz*/
	RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FMC;
	RCC_PeriphClkInit.PLL2.PLL2M = 5;
	RCC_PeriphClkInit.PLL2.PLL2N = 160;
	RCC_PeriphClkInit.PLL2.PLL2P = 2;
	RCC_PeriphClkInit.PLL2.PLL2Q = 2;
	RCC_PeriphClkInit.PLL2.PLL2R = 8;
	RCC_PeriphClkInit.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_2;
	RCC_PeriphClkInit.PLL2.PLL2VCOSEL = RCC_PLL2VCOWIDE;
	RCC_PeriphClkInit.PLL2.PLL2FRACN = 0;
	RCC_PeriphClkInit.FmcClockSelection = RCC_FMCCLKSOURCE_PLL2;
	if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit) != HAL_OK)
	{
		while(1);
	}
	/* 使能 FMC 时钟 */
	__FMC_CLK_ENABLE();

	/*执行SDRAM1的内存初始化序列 */
	sdramHandle.Instance = FMC_SDRAM_DEVICE;
	/* sdramHandle结构体初始化*/
	sdramHandle.Init.SDBank = FMC_SDRAM_BANK2;
	sdramHandle.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_9;//SDRAM列数
	sdramHandle.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;//SDRAM行数
	sdramHandle.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;//总线数据宽度为32位
	sdramHandle.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;//4个扇区
	sdramHandle.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;//列地址选通信延时
	sdramHandle.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;//禁止写保护
	sdramHandle.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;//SDRAM时钟100MHz
	sdramHandle.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;     //使能突发传输模式
	sdramHandle.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0; //读通道延时
	/* SDRAM时序 */
	SdramTiming.LoadToActiveDelay = 2;//加载模式寄存器命令与行有效或刷新命令之间的延迟
	SdramTiming.ExitSelfRefreshDelay = 7;//退出自我刷新到行有效命令之间的延迟
	SdramTiming.SelfRefreshTime = 4;//行有效与预充电命令之间的延迟
	SdramTiming.RowCycleDelay = 7;//两个刷新命令或两个行有效命令之间的延迟
	SdramTiming.WriteRecoveryTime = 2;//写入命令到预充电命令之间的延迟
	SdramTiming.RPDelay = 2;//预充电与行有效命令之间的延迟
	SdramTiming.RCDDelay = 2;//行有效与列读写命令之间的延迟

	HAL_SDRAM_Init(&sdramHandle, &SdramTiming);  
	/* FMC SDRAM 设备时序初始化 */
	SDRAM_InitSequence(); 
  
#ifdef RT_USING_MEMHEAP_AS_HEAP
  /* If RT_USING_MEMHEAP_AS_HEAP is enabled, SDRAM is initialized to the heap */
  rt_memheap_init(&system_heap, "SDRAM", (void *)SDRAM_BANK_ADDR, SDRAM_SIZE);
#endif
}
INIT_BOARD_EXPORT(SDRAM_Init);
