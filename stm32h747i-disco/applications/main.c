/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-09-17     tyustli   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <string.h>

/* defined the LED0 pin: PI12 */
#define LED1_PIN    GET_PIN(I, 12)
//#define LED2_PIN    GET_PIN(I, 13)
//#define LED3_PIN    GET_PIN(I, 14)

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

static void tid1_entry(void *para)
{
	while(1)
	{
		rt_kprintf("This is 1\n");
		rt_thread_mdelay(1000);
	}
}

static void tid2_entry(void *para)
{
	while(1)
	{
		rt_kprintf("This is 2\n");
		rt_thread_mdelay(500);
	}
}

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
	rt_pin_write(LED1_PIN, PIN_LOW);
	
	tid1 = rt_thread_create("thread1",
							tid1_entry,
							RT_NULL,
							1024,
							5,
							10
	);
	rt_thread_startup(tid1);
	
	tid2 = rt_thread_create("thread2",
							tid2_entry,
							RT_NULL,
							1024,
							6,
							10
	);
	rt_thread_startup(tid2);
	
}
