//CL2351不需开启SPI3外设，使用普通IO口模式即可；而ZLG7289需开启SPI3外设

/* CL2315数据 */
#define	GPIO_IO_SER 	    	GPIOB	// 引脚定义
#define	GPIO_IO_SER_PIN   		GPIO_Pin_4	// 引脚定义 
#define SET_SER        			GPIO_WriteBit(GPIO_IO_SER, GPIO_IO_SER_PIN, (BitAction)1) 
#define CLEAR_SER     			GPIO_WriteBit(GPIO_IO_SER, GPIO_IO_SER_PIN, (BitAction)0)  

/* CL2315时钟 */
#define	GPIO_IO_SRCLK 	    	GPIOB	// 引脚定义
#define	GPIO_IO_SRCLK_PIN   	GPIO_Pin_3	// 引脚定义 
#define SET_SRCLK        		GPIO_WriteBit(GPIO_IO_SRCLK, GPIO_IO_SRCLK_PIN, (BitAction)1)  
#define CLEAR_SRCLK     		GPIO_WriteBit(GPIO_IO_SRCLK, GPIO_IO_SRCLK_PIN, (BitAction)0)  

/* CL2315锁存 */
#define	GPIO_IO_RCLK 	    	GPIOG	// 引脚定义
#define	GPIO_IO_RCLK_PIN   		GPIO_Pin_15	// 引脚定义 
#define SET_RCLK        		GPIO_WriteBit(GPIO_IO_RCLK, GPIO_IO_RCLK_PIN, (BitAction)1)   
#define CLEAR_RCLK     			GPIO_WriteBit(GPIO_IO_RCLK, GPIO_IO_RCLK_PIN, (BitAction)0)