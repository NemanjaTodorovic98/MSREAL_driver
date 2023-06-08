#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/version.h>

#include "types.h"

MODULE_LICENSE("Dual BSD/GPL");

static const char receivedMsg[] = "RECEIVED";

static int __init GetBlockIP_init(void);
static void __exit GetBlockIP_exit(void);
static int GetBlockIP_open(struct inode *pinode, struct file *pfile);
static int GetBlockIP_close(struct inode *pinode, struct file *pfile);
ssize_t GetBlockIP_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t GetBlockIP_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

static dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

static DeviceUnit currentUnit = IPcore;
static unsigned char currentProgress = 0;
static int endRead = 0;

static unsigned char BramA_BlockWritten = 0;
static unsigned char BramB_BlockWritten = 0;

static uint32 count1 = 4u;
static uint32 count2 = 12u;
static uint32 count4 = 16u;

static uint32 imgoffsetx = 0u;
static uint32 imgoffsety = 0u;
static uint32 imgoffsetx_tmp = 0u;
static uint32 imgoffsety_tmp = 0u;

static uint32 rows = 64u;
static uint32 cols = 64u;

static uint32 ptx = 0u;
static uint32 pty = 0u;

static int ready = 0;
static int start = 0;

static uint32 grad[GRAD_MEMORY_SIZE]; //bramA
static uint32 qangle[QANGLE_MEMORY_SIZE]; //bramB
static uint32 gradOfs[GRADOFS_MEMORY_SIZE]; //bramD
static uint32 gradWeights[GRADWEIGHTS_MEMORY_SIZE]; //bramF
static uint32 histOfs[HISTOFS_MEMORY_SIZE]; //bramG
static uint32 histWeights[HISTWEIGHTS_MEMORY_SIZE];//bramH
static uint32 blockHist[BLOCKHIST_MEMORY_SIZE]; 

struct file_operations my_fops = 
{
	.owner = THIS_MODULE,
	.open = GetBlockIP_open,
	.read = GetBlockIP_read,	
	.write = GetBlockIP_write,
	.release = GetBlockIP_close,
};


static int GetBlockIP_open(struct inode *pinode, struct file *pfile)
{
	//printk(KERN_INFO "Succesfully opened getBlockIP\n");
	return 0;
}


static int GetBlockIP_close(struct inode *pinode, struct file *pfile)
{
	//printk(KERN_INFO "Succesfully closed getBlockIP\n");
	return 0;
}

void getBlock(void)
{
		
   int k = 0;
   int i = 0;

   IntToBytes tmp;
   
   imgoffsetx = imgoffsetx_tmp;
   imgoffsety = imgoffsety_tmp;
					
   uint32 a0, a1, h0, h1, t0, t1, w;
   uint32 hist0[4], hist1[4];
   uint32 pkgw, pkhw;
   uint32 _t0, _t1;
   
   uint32* hist;

   uint32* p_blockHist = (uint32*)kcalloc(36, sizeof(uint32), GFP_KERNEL);		
  
   ptx = imgoffsetx;
   pty = imgoffsety;
   
   for(k = 0; k < count1; k++)
   {
      a0 = grad[2 * (pty * cols + ptx) + gradOfs[k]];
      a1 = grad[2 * (pty * cols + ptx) + gradOfs[k]+1];
      
      w = (gradWeights[k] * histWeights[4 * k]) /0x800;
	
      h0 = qangle[2 * (pty * cols + ptx) + gradOfs[k]];
      h1 = qangle[2 * (pty * cols + ptx) + gradOfs[k] + 1];
	
      hist = p_blockHist + histOfs[4 * k];
      
      t0 = hist[h0] + (a0 * w)/0x800;
      t1 = hist[h1] + (a1 * w)/0x800;
      
      hist[h0] = t0;
      hist[h1] = t1;
   }
   for( ; k < count2; k++)
   {
      a0 = grad[2 * (pty * cols + ptx) + gradOfs[k]];
      a1 = grad[2 * (pty * cols + ptx) + gradOfs[k] + 1];
      
      h0 = qangle[2 * (pty * cols + ptx) + gradOfs[k]];
      h1 = qangle[2 * (pty * cols + ptx) + gradOfs[k] + 1];
	
      for(i = 0; i < 4; ++i)
      {
         pkgw = gradWeights[k];
         pkhw = histWeights[4 * k + i];
         w = (pkgw * pkhw) /0x800;
         _t0 = (a0 * w)/0x800 ;
         _t1 = (a1 * w)/0x800 ;
         hist0[i] = _t0;
         hist1[i] = _t1;
      }

      hist = p_blockHist + histOfs[4 * k];
      t0 = hist[h0] + hist0[0];
      t1 = hist[h1] + hist1[0];

      hist[h0] = t0;
      hist[h1] = t1;

      hist = p_blockHist + histOfs[4 * k + 1];
      t0 = hist[h0] + hist0[1];
      t1 = hist[h1] + hist1[1];

      hist[h0] = t0;
      hist[h1] = t1;
   }
   for( ; k < count4; k++)
   {
      a0 = grad[2 * (pty * cols + ptx) + gradOfs[k]];
      a1 = grad[2 * (pty * cols + ptx) + gradOfs[k] + 1];
      
      h0 = qangle[2 * (pty * cols + ptx) + gradOfs[k]];
      h1 = qangle[2 * (pty * cols + ptx) + gradOfs[k] + 1];
	
      for(i = 0; i < 4; ++i)
      {
         pkgw = gradWeights[k];
         pkhw = histWeights[4 * k + i];
         
         w = (pkgw * pkhw)/0x800;
         _t0 = (a0 * w)/0x800;
         _t1 = (a1 * w)/0x800;
         hist0[i] = _t0;
         hist1[i] = _t1;
      }

      hist = p_blockHist + histOfs[4 * k];
      
      t0 = hist[h0] + hist0[0];
      t1 = hist[h1] + hist1[0];
      
      hist[h0] = t0;
      hist[h1] = t1;

      hist = p_blockHist + histOfs[4 * k + 1];
      
      t0 = hist[h0] + hist0[1];
      t1 = hist[h1] + hist1[1];

      hist[h0] = t0;
      hist[h1] = t1;

      hist = p_blockHist + histOfs[4 * k + 2];
      
      t0 = hist[h0] + hist0[2];
      t1 = hist[h1] + hist1[2];

      hist[h0] = t0;
      hist[h1] = t1;

      hist = p_blockHist + histOfs[4 * k +3];

      t0 = hist[h0] + hist0[3];
      t1 = hist[h1] + hist1[3];

      hist[h0] = t0;
      hist[h1] = t1;
   }
   for(i = 0; i < 36; ++i)
   {
      blockHist[i] = p_blockHist[i];
      //printk(KERN_INFO" blockHist[%u] = %u", i, blockHist[i]);
   }

   kfree(p_blockHist);
    
}


ssize_t GetBlockIP_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	char read_buff[RESPONSE_BUFF_SIZE]; 
	int lengthRead;
	IntToBytes tmp;

	DeviceUnit unitToRead = (DeviceUnit)MINOR(pfile->f_inode->i_rdev); 
	
	if(unitToRead == BramC)
	{
		endRead = 0;
	}

	if (endRead)
	{
		endRead = 0;
		return 0;
	}
	

	switch(unitToRead)
	{
		case IPcore:
		{
			
			tmp.word = ready;
			
			read_buff[0] = tmp.byte[3];
			read_buff[1] = tmp.byte[2];
			read_buff[2] = tmp.byte[1];
			read_buff[3] = tmp.byte[0];
			lengthRead = 4;
			
			if (copy_to_user(buffer, read_buff , lengthRead))
			{
				return -EFAULT;
			}
			endRead = 1;
			break;
		}
		case BramA:
		{
			if(BramA_BlockWritten == 1)
			{
				lengthRead = 8;
				if (copy_to_user(buffer, receivedMsg, lengthRead))
				{
					return -EFAULT;
				}
				BramA_BlockWritten = 0;
				
			}
			printk(KERN_INFO "\n Received message sent to app for BRAM A!\n");
			endRead = 1;
			break;
		}
		case BramB:
		{
			if(BramB_BlockWritten == 1)
			{
				lengthRead = 8;
				if (copy_to_user(buffer, receivedMsg, lengthRead))
				{
					return -EFAULT;
				}
				BramB_BlockWritten = 0;
				
				
			}
			printk(KERN_INFO "\n Received message sent to app for BRAM B!\n");
			endRead = 1;
			break;
		}
		case BramC:
		{				
				int i;
				for(i = 0; i < BLOCKHIST_MEMORY_SIZE; i++) 
				{
						
					tmp.word = blockHist[i];
				
					read_buff[i*4]     	= tmp.byte[3];
					read_buff[i*4 + 1] 	= tmp.byte[2];
					read_buff[i*4 + 2] 	= tmp.byte[1];
					read_buff[i*4 + 3] 	= tmp.byte[0];
					lengthRead += 4;
				}			
				if (copy_to_user(buffer, read_buff, lengthRead))
				{
					return -EFAULT;
				}
				
			
			endRead = 1;
			break;
		}
		case BramD:				// fallthrough
		case BramF:
		case BramG:
		case BramH:
		{
			// It is not expected to request read of those BRAMs
			read_buff[0] = 255;
		    	read_buff[1] = 255;
			read_buff[2] = 255;
			read_buff[3] = 255;
			
			printk(KERN_ERR "\nSomething went wrong, this BRAM shall not be read from application side.\n");

			lengthRead = 4;
			if (copy_to_user(buffer, read_buff, lengthRead))
			{
				return -EFAULT;
			}
			endRead = 1;
			break;
		}
		default:
		{
			printk(KERN_ERR "\nUnexpected minor ID\n");
			break;
		}
	}

	return (ssize_t)lengthRead;
}



ssize_t GetBlockIP_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
	int ret=0;
	static unsigned char alreadyReceivedDataBlocksCount;
	char buff[length + 1];
	DeviceUnit unitToRead;
	IntToBytes tmp;
	unsigned char* pBuff = &buff[0];
	ret = copy_from_user(buff, buffer, length); 


	if(ret)
	return -EFAULT;

	unitToRead = (DeviceUnit)MINOR(pfile->f_inode->i_rdev);

	switch(unitToRead)
	{
		case IPcore:
		{
			if(start == 0 && ready == 1)
			{
				
				tmp.byte[3] = buff[0];
				tmp.byte[2] = buff[1];
				tmp.byte[1] = buff[2];
				tmp.byte[0] = buff[3];
				
				start = tmp.word; 
				

				currentProgress |= 64; 
				if(currentProgress == 0x7F && start == 1)
				{
			
			        	ready = 0;
					start = 0;
					getBlock();
					if(imgoffsety < 60)
					{
						imgoffsetx_tmp = imgoffsetx;
						imgoffsety_tmp = imgoffsety + 2;
						
					}
					else
					{
						if(imgoffsetx < 60)
						{
							imgoffsety_tmp = 0;
							imgoffsetx_tmp = imgoffsetx + 2;
						}
						else
						{
							printk(KERN_INFO "\n Done with 961 blocks\n");
							
						}
						
					}
					ready = 1; 
				}
				else
				{
					printk(KERN_INFO "Currentprogress = %u" , currentProgress);
					printk(KERN_ERR "\n Something went wrong with IP population with values!\n");
				}
			}
			else
			{
				printk(KERN_ERR "\n Device is currently processing previous input.\n");
			}
			break;
		}
		case BramA:
		{

			int i;
			for(i = 0; i < length/4; i++) 
			{
				
				tmp.byte[3] = buff[4*i];
				tmp.byte[2] = buff[4*i + 1];
				tmp.byte[1] = buff[4*i + 2];
				tmp.byte[0] = buff[4*i + 3];
				
				grad[i + alreadyReceivedDataBlocksCount * 1024] = tmp.word;
								
			}
			alreadyReceivedDataBlocksCount++;
			BramA_BlockWritten = 1;
			
			if(alreadyReceivedDataBlocksCount == 8)
			{
				printk(KERN_INFO "\n All 8 block for BRAM A received\n");
				currentProgress |= 32; 
				alreadyReceivedDataBlocksCount = 0;
				
			}
			break;
		}
		case BramB:
		{
			int i;
			for(i = 0; i < length/4; i++)
			{
				
				tmp.byte[3] = buff[4*i];
				tmp.byte[2] = buff[4*i + 1];
				tmp.byte[1] = buff[4*i + 2];
				tmp.byte[0] = buff[4*i + 3];
				
				qangle[i + alreadyReceivedDataBlocksCount * 1024] = tmp.word;
				
			}
			alreadyReceivedDataBlocksCount++;
			BramB_BlockWritten = 1;
			
			if(alreadyReceivedDataBlocksCount == 8)
			{
				printk(KERN_INFO "\n All 8 block for BRAM B received\n");
				currentProgress |= 16;
				alreadyReceivedDataBlocksCount = 0;
				
			}
			break;
		}
		case BramC:
		{
			// Nothing to write to BRAM C - it is used as algorithm output
			printk(KERN_ERR "\nSomething went wrong, BRAM C shall not be filled from application side.\n");
			break;
		}
		case BramD:
		{
			int i;
			for(i = 0; i < (length/4); i++)
			{
			
				tmp.byte[3] = buff[4*i];
				tmp.byte[2] = buff[4*i + 1];
				tmp.byte[1] = buff[4*i + 2];
				tmp.byte[0] = buff[4*i + 3];
				
				gradOfs[i] = tmp.word;
			}
			currentProgress |= 8;
			printk(KERN_INFO "\n BRAM D received\n");
			break;
		}
		case BramF:
		{
		
			int i;
			for(i = 0; i < (length/4); i++)
			{
				
				tmp.byte[3] = buff[4*i];
				tmp.byte[2] = buff[4*i + 1];
				tmp.byte[1] = buff[4*i + 2];
				tmp.byte[0] = buff[4*i + 3];
				
				gradWeights[i] = tmp.word;
				
			}
			currentProgress |= 4;
			printk(KERN_INFO "\n BRAM F received\n");
			break;
		}
		case BramG:
		{
			int i;
			for(i = 0; i < (length/4); i++)
			{
				tmp.byte[3] = buff[4*i];
				tmp.byte[2] = buff[4*i + 1];
				tmp.byte[1] = buff[4*i + 2];
				tmp.byte[0] = buff[4*i + 3];
				
				histOfs[i] = tmp.word;
			}
			currentProgress |= 2;
			printk(KERN_INFO "\n BRAM G received\n");
			break;
		}
		case BramH:
		{
			int i;
			for(i = 0; i < (length/4); i++)
			{
				tmp.byte[3] = buff[4*i];
				tmp.byte[2] = buff[4*i + 1];
				tmp.byte[1] = buff[4*i + 2];
				tmp.byte[0] = buff[4*i + 3];
			
				histWeights[i] = tmp.word;
				
			}
			currentProgress |= 1;
			printk(KERN_INFO "\n BRAM H received\n");
			break;
		}
		default:
		{
			printk(KERN_ERR "\nUnexpected minor ID\n");
			break;
		}
	}

	return length;
}


static int __init GetBlockIP_init(void)
{
   int ret = 0;

   ret = alloc_chrdev_region(&my_dev_id, 0, 8, "GetBlockIP");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");	
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "GetBlockIP_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 0), NULL, "GetBlockIP") == NULL)
   {
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created GetBlockIP\n");

   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 1), NULL, "bramA") == NULL)
   {
      printk(KERN_ERR "failed to create device\n");
      goto fail_2;
   }
   printk(KERN_INFO "device created bramA\n");

   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 2), NULL, "bramB") == NULL)
   {
      printk(KERN_ERR "failed to create device\n");
      goto fail_3;
   }
   printk(KERN_INFO "device created bramB\n");

   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 3), NULL, "bramC") == NULL)
   {
      printk(KERN_ERR "failed to create device\n");
      goto fail_4;
   }
   printk(KERN_INFO "device created bramC\n");

   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 4), NULL, "bramD") == NULL)
   {
      printk(KERN_ERR "failed to create device\n");
      goto fail_5;
   }
   printk(KERN_INFO "device created bramD\n");

   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 5), NULL, "bramF") == NULL)
   {
      printk(KERN_ERR "failed to create device\n");
      goto fail_6;
   }
   printk(KERN_INFO "device created bramF\n");

   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 6), NULL, "bramG") == NULL)
   {
      printk(KERN_ERR "failed to create device\n");
      goto fail_7;
   }
   printk(KERN_INFO "device created bramG\n");

   if (device_create(my_class, NULL, MKDEV(MAJOR(my_dev_id), 7), NULL, "bramH") == NULL)
   {
      printk(KERN_ERR "failed to create device\n");
      goto fail_8;
   }
   printk(KERN_INFO "device created bramH\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;

	if (cdev_add(my_cdev, my_dev_id, 8) == -1)  
	{
      		printk(KERN_ERR "failed to add cdev\n");
		goto fail_9;
	}
   printk(KERN_INFO "cdev added\n");
   
   ready = 1;
   start = 0;
   
   return 0;
 
   fail_9:
   	  device_destroy(my_class, MKDEV(MAJOR(my_dev_id),7));
   fail_8:
   	  device_destroy(my_class, MKDEV(MAJOR(my_dev_id),6));
   fail_7:
   	  device_destroy(my_class, MKDEV(MAJOR(my_dev_id),5));
   fail_6:
   	  device_destroy(my_class, MKDEV(MAJOR(my_dev_id),4));
   fail_5:
   	  device_destroy(my_class, MKDEV(MAJOR(my_dev_id),3));
   fail_4:
   	  device_destroy(my_class, MKDEV(MAJOR(my_dev_id),2));
   fail_3:
   	  device_destroy(my_class, MKDEV(MAJOR(my_dev_id),1));
   fail_2:
   	  device_destroy(my_class, MKDEV(MAJOR(my_dev_id),0));
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit GetBlockIP_exit(void)
{
	cdev_del(my_cdev);
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),7));
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),6));
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),5));
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),4));
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),3));
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),2));
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),1));
	device_destroy(my_class, MKDEV(MAJOR(my_dev_id),0));
	class_destroy(my_class);
	unregister_chrdev_region(my_dev_id,1);

	printk(KERN_INFO "GetBlock driver closed.\n");
}

module_init(GetBlockIP_init);
module_exit(GetBlockIP_exit);
