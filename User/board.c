#include "board.h" 

#include <rthw.h>
#include <rtthread.h>
extern void Debug_USART_Config8266(void);
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE 1024
/* A portion of static memory is allocated from the internal 
        SRAM as rtt's heap space, which is configured as 4KB */
		
static uint32_t rt_heap[RT_HEAP_SIZE];
RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/*device serial begin*/
#include <rtdevice.h>
#include <drivers/serial.h>  // ? ������ؽṹ�塢�궼������
// static rt_device_t serial;
// ����һ�������豸�ṹ��
static struct rt_serial_device serial;

// �������ã������ʡ�����λ�ȣ�
static struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;


// ���ڳ�ʼ������������㲻��Ҫ�����ʼ������ʡ�ԣ�
static rt_err_t uart_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
  // Debug_USART_Config();
  Debug_USART_Config8266();
  // ���������ӳ�ʼ���ײ�Ӳ���Ĵ��룬�������ô��ڼĴ���
  return RT_EOK;
}

// ���ڷ��͵��ֽڣ��ײ�ʵ�֣�
static rt_err_t uart_putc(struct rt_serial_device *serial, char c)
{
    USART_TypeDef *uart = USART2; // ��ʹ�õ��� uart2

    // �ȴ����ͻ�������
    while ((uart->SR & USART_SR_TXE) == 0);
    uart->DR = c;
    return RT_EOK;
}

// ���ڲ�����
static const struct rt_uart_ops uart_ops =
{
    .configure = uart_configure,
    .control = RT_NULL,
    .putc = uart_putc,
    .getc = RT_NULL,
    .dma_transmit = RT_NULL
};
/*device serial end*/


void rt_hw_board_init()
{ 
  SysTick_Config( SystemCoreClock/ RT_TICK_PER_SECOND );	//168000000�ҵ�   
	Debug_USART_Config();
	
	config.baud_rate = 115200;
  config.data_bits = DATA_BITS_8;
	config.stop_bits = STOP_BITS_1;
	config.parity = PARITY_NONE;
	config.bufsz = 128;
	// config.invert = RT_SERIAL_RX_INVERT;
	config.reserved = 0;
  serial.ops = &uart_ops;
  serial.config = config;
	
	// ע�ᴮ���豸������Ϊ uart2
  rt_hw_serial_register(&serial, "uart2",
                        RT_DEVICE_FLAG_RDWR ,
                        RT_NULL);
    
	My_RTC_Init();							//RTC��ʼ��
	LED_Init();						        //LED��ʼ��
	KEY_Init(); 					        //������ʼ��
	BUZZER_Init();					      	//��������ʼ��
	SDP810.SDP8XX_Init(&SDP810);			//�󴫸�����ʼ��
	XGZP6859_Init();				      	//С��������ʼ��
	SHT20_Init();					        //��ʪ�ȴ�������ʼ��
	DS18B20_Init();							//ds18b20��ʼ��
  TIMX_PWM_Init(10000-1,0);		  			//�ķ��PWM��ʼ��
  AIR_BLOWER_CONTROL_GPIO_Init(); 			//�ķ�����ư�����ʼ��
	temperature_PWM_Init(10000-1,7);		//�¿�PWM��ʼ��
	btim_init();                  			//��ʱ��6��ʼ��������Bipap��ʱ
	
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    
#if defined(RT_USING_CONSOLE) && defined(RT_USING_DEVICE)
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

void SysTick_Handler(void)
{   
    rt_interrupt_enter();
   
    rt_tick_increase();
   
    rt_interrupt_leave();
}

void rt_hw_console_output(const char *str)
{
//#error "TODO 3: Output the string 'str' through the uart."
    rt_enter_critical();
	
    while (*str!='\0')
	{
		
        if (*str=='\n')
		{
			USART_SendData(DEBUG_USART, '\r'); 
			while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);
		}

		USART_SendData(DEBUG_USART, *str++); 				
		while (USART_GetFlagStatus(DEBUG_USART, USART_FLAG_TXE) == RESET);	
	}	

    rt_exit_critical();
}
