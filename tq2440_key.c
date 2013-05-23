/*
    file Name:      tq2440_key.c
    Author:         zengweitotty
    version:        V1.0
    Data:           2013/05/10
    Email:          zengweitotty@gmail.com
    Description     Test Interrupt 
*/
#include <linux/init.h>	//use for function module_init,module_exit
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>	//use for struct file and struct file_operations
#include <linux/ioport.h>	//use for request_mem_region
#include <asm/io.h>		//use for ioremap iounmap
#include <linux/interrupt.h>	//use for probe_irq_on or probe_irq_off,request_irq,free_irq
#include <asm/delay.h>	//use for udelay
#include <linux/sched.h>	//

#define GPF_BASE		0x56000050
#define GPF_CON_BASE	0x56000050
#define GPF_UP_BASE		0x56000058
#define GPF_DAT_BASE	0x56000054

#define KEY4_IRQ	IRQ_EINT0

volatile unsigned long *gpf_con = NULL;
volatile unsigned long *gpf_up = NULL;
volatile unsigned long *gpf_dat = NULL;

/*
struct tq2440_key{
	
};
*/
irqreturn_t key_interrupt(int irq,void *dev_id){
	printk(KERN_INFO "[tq2440_key/key_interrupt]Key interrupt occured\n");
	return IRQ_HANDLED;
}
static int __init tq2440_key_init(void){
	int ret = 0;
	//unsigned long mask = 0;
	//unsigned int key_int = 0;
	//int count = 0;
	if(!request_mem_region(GPF_BASE,3,"KEY_INIT")){
		printk(KERN_ERR "[tq2440_key/tq2440_key_init]Can not request mem region to use key init\n");
		return -ENODEV;
	}
	gpf_con = (volatile unsigned long*)ioremap_nocache(GPF_CON_BASE,1);
	gpf_up = (volatile unsigned long*)ioremap_nocache(GPF_UP_BASE,1);
	gpf_dat = (volatile unsigned long*)ioremap_nocache(GPF_DAT_BASE,1);

	*gpf_con &= ~(0x00 << 2);
	*gpf_con |= (0x02 << 2);
	*gpf_up |= 0x2;
	/* Can not automatic find the irq number	
	do{
		mask = probe_irq_on();
		*gpf_dat |= 0x2;
		udelay(5);
		*gpf_dat &= ~(0x2);
		udelay(5);
		key_int = probe_irq_off(mask);
		if(key_int == 0){
			printk(KERN_INFO "[tq2440_key/tq2440_key_init]Tq2440:Can not find int number\n");
			key_int = -1;
		}
	}while(count++ < 5 && key_int < 0);
	if(key_int < 0){
		printk(KERN_INFO "[tq2440_key/tq2440_key_init]Tq2440:probe failed\n");		
	}else{
		printk(KERN_INFO "[tq2440_key/tq2440_key_init]Tq2440:irq number is %d\n",key_int);		
	}
	*/
	ret = request_irq(KEY4_IRQ,key_interrupt,IRQF_DISABLED,"key4",NULL);
	if(ret != 0){
		printk(KERN_ERR "[tq2440_key/tq2440_key_init]Tq2440: Can not request irq number %d\n",KEY4_IRQ);
		goto failed;		
	}
	printk(KERN_INFO "[tq2440_key/tq2440_key_init]Tq2440: Success to request irq number %d\n",KEY4_IRQ);
failed:
	iounmap((void*)gpf_con);
	iounmap((void*)gpf_dat);
	iounmap((void*)gpf_up);
	release_mem_region(GPF_BASE,3);
	return 0;
}
static void __exit tq2440_key_exit(void){
	free_irq(KEY4_IRQ,key_interrupt);
	iounmap((void*)gpf_con);
	iounmap((void*)gpf_dat);
	iounmap((void*)gpf_up);
	release_mem_region(GPF_BASE,3);
}

module_init(tq2440_key_init);
module_exit(tq2440_key_exit);

MODULE_AUTHOR("zengweitotty");
MODULE_DESCRIPTION("Test Interrupt");
MODULE_LICENSE("GPL");
