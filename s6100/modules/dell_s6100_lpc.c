
#include <linux/module.h>
#include <linux/kernel.h>
//for platform drivers....
#include <linux/platform_device.h>

#define DRIVER_NAME "dell_s6100_lpc"

/* These Addresses defined by the Hardware Designer... 
   Simply added to demostrate.*/

#define RESOURCE1_START_ADDRESS 0x200
#define RESOURCE1_END_ADDRESS   0x260

/* SMF Registers */

#define SMF_REG_VERSION   0x200
#define SMF_MB_ADDR_HI    0x210
#define SMF_MB_ADDR_LO    0x211
#define SMF_MB_READ_DATA  0x212
#define SMF_MB_WRITE_DATA  0x213

/* SMF MB Registers */

#define MAX_NUM_TEMP_SENSORS 0x0013
#define MAX_NUM_PSU          0x0231
#define PSU1_STATUS          0x0237
#define PSU2_STATUS          0x0270
#define SYS_STATUS_LED       0x130
#define TOTAL_PWR            0x0232
#define MAX_PSU1_PWR         0x0234
#define MAX_PSU2_PWR         0x026D

/* SMF temp sensors */

#define TEMP_SENS_1  0x0014
#define TEMP_SENS_2  0x0016
#define TEMP_SENS_3  0x0018
#define TEMP_SENS_4  0x001A
#define TEMP_SENS_5  0x001C
#define TEMP_SENS_6  0x001E
#define TEMP_SENS_7  0x0020
#define TEMP_SENS_8  0x0022
#define TEMP_SENS_9  0x0024
#define TEMP_SENS_10 0x0026
#define TEMP_SENS_11 0x0028

/* FAN Tray */
#define MAX_NUM_FAN_TRAYS    0x00F0
#define MAX_NUM_FAN_PER_TRAY 0x00F1
#define FAN_TRAY_PRES        0x0113

#define FAN_TRAY_1_SPEED  0x00F3
#define FAN_TRAY_2_SPEED  0x00F7
#define FAN_TRAY_3_SPEED  0x00FB
#define FAN_TRAY_4_SPEED  0x00FF


unsigned long  *mmio;

/* SMF read function */

static int sfm_read_byte(int addr)
{
    /* lock is acquired by user before invoking this */
    //printk(KERN_ERR " Addr HI %x ",((addr >> 8) & 0xff));
    outb(((addr >> 8) & 0xff),SMF_MB_ADDR_HI);
    //printk(KERN_ERR "Addr LO %x \n",(addr & 0xff));
    outb((addr & 0xff),SMF_MB_ADDR_LO);
    return inb(SMF_MB_READ_DATA);
}

/* change ver to info */

static int sfm_read_two_byte(u16 offset)
{

    u8 low_byte=0,high_byte=0;
    high_byte = sfm_read_byte(offset);
    //printk(KERN_ERR " low_byte %x\n",high_byte);
    low_byte =  sfm_read_byte(offset+1);
    //printk(KERN_ERR " low_byte %x\n",low_byte);
    //printk ( KERN_ERR " Value %x",((high_byte <<8) | ( low_byte)));

    return ((high_byte <<8) | ( low_byte));

}


/* Specifying my resources information */
static struct resource dell_s6100_lpc_resources[] = {
    {
        .start          = RESOURCE1_START_ADDRESS,
        .end            = RESOURCE1_END_ADDRESS,
        .flags          = IORESOURCE_MEM,
    },
};

static void dell_s6100_lpc_dev_release( struct device * dev)
{
    return ;
}

static struct platform_device dell_s6100_lpc_dev = {
    .name           = DRIVER_NAME,
    .id             = -1,
    .num_resources  = ARRAY_SIZE(dell_s6100_lpc_resources),
    .resource       = dell_s6100_lpc_resources,
    .dev = {
        .release = dell_s6100_lpc_dev_release,

    }
};



/* change ver to info */
static ssize_t get_smfver(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf, "%x\n",inb(SMF_REG_VERSION));
}

static ssize_t get_sys_led(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%x\n", inb(SYS_STATUS_LED));
}

static ssize_t set_sys_led(struct device *dev, struct device_attribute *devattr, const char *buf, 
        size_t count)
{
    unsigned long devdata;
    int err;

    err = kstrtoul(buf,16, &devdata);

    if (err)
        return err;

    outb(devdata,SYS_STATUS_LED);
    return count;
}


static ssize_t get_fan_tray_presence(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%x\n", sfm_read_byte(FAN_TRAY_PRES));
}

static ssize_t get_max_num_temp_sensors(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%x\n", sfm_read_byte(MAX_NUM_TEMP_SENSORS));
}

static ssize_t get_max_num_fan_trays(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%x\n", sfm_read_byte(MAX_NUM_FAN_TRAYS));
}

static ssize_t get_max_num_fans_per_tray(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%x\n", sfm_read_byte(MAX_NUM_FAN_PER_TRAY));
}

static ssize_t get_max_num_psu(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%x\n", sfm_read_byte(MAX_NUM_PSU));
}

static ssize_t get_psu_total_pwr(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TOTAL_PWR)/10));
}

static ssize_t get_psu1_max_pwr(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(MAX_PSU1_PWR)/10));
}

static ssize_t get_psu2_max_pwr(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(MAX_PSU2_PWR)/10));
}

static ssize_t get_psu1_status(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%x\n", sfm_read_byte(PSU1_STATUS));
}

static ssize_t get_psu2_status(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%x\n", sfm_read_byte(PSU2_STATUS));
}

/* Temp Sensors */

static ssize_t get_temp_sensor1(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_1)/10));
}
static ssize_t get_temp_sensor2(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_2)/10));
}
static ssize_t get_temp_sensor3(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_3)/10));
}
static ssize_t get_temp_sensor4(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_4)/10));
}
static ssize_t get_temp_sensor5(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_5)/10));
}
static ssize_t get_temp_sensor6(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_6)/10));
}
static ssize_t get_temp_sensor7(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_7)/10));
}
static ssize_t get_temp_sensor8(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_8)/10));
}
static ssize_t get_temp_sensor9(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_9)/10));
}
static ssize_t get_temp_sensor10(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_10)/10));
}
static ssize_t get_temp_sensor11(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", (sfm_read_two_byte(TEMP_SENS_11)/10));
}

static ssize_t get_fan_tray_1_speed(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", sfm_read_two_byte(FAN_TRAY_1_SPEED));
}
static ssize_t get_fan_tray_2_speed(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", sfm_read_two_byte(FAN_TRAY_2_SPEED));
}
static ssize_t get_fan_tray_3_speed(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", sfm_read_two_byte(FAN_TRAY_3_SPEED));
}
static ssize_t get_fan_tray_4_speed(struct device *dev, struct device_attribute *devattr, char *buf)
{
    return sprintf(buf,"%d\n", sfm_read_two_byte(FAN_TRAY_4_SPEED));
}

static DEVICE_ATTR(smf_ver,S_IRUGO,get_smfver, NULL);
static DEVICE_ATTR(max_num_temp_sensors,S_IRUGO,get_max_num_temp_sensors, NULL);
static DEVICE_ATTR(max_num_fan_trays,S_IRUGO,get_max_num_fan_trays, NULL);
static DEVICE_ATTR(max_num_fans_per_tray,S_IRUGO,get_max_num_fans_per_tray, NULL);
static DEVICE_ATTR(fan_tray_presence,S_IRUGO,get_fan_tray_presence, NULL);
static DEVICE_ATTR(max_num_psu,S_IRUGO,get_max_num_psu, NULL);
static DEVICE_ATTR(psu1_status,S_IRUGO,get_psu1_status, NULL);
static DEVICE_ATTR(psu2_status,S_IRUGO,get_psu2_status, NULL);
static DEVICE_ATTR(psu_total_pwr,S_IRUGO,get_psu_total_pwr, NULL);
static DEVICE_ATTR(psu1_max_pwr,S_IRUGO,get_psu1_max_pwr, NULL);
static DEVICE_ATTR(psu2_max_pwr,S_IRUGO,get_psu2_max_pwr, NULL);
static DEVICE_ATTR(sys_led,S_IRUGO | S_IWUSR,get_sys_led,set_sys_led);


static DEVICE_ATTR(temp_sensor_1,S_IRUGO,get_temp_sensor1, NULL);
static DEVICE_ATTR(temp_sensor_2,S_IRUGO,get_temp_sensor2, NULL);
static DEVICE_ATTR(temp_sensor_3,S_IRUGO,get_temp_sensor3, NULL);
static DEVICE_ATTR(temp_sensor_4,S_IRUGO,get_temp_sensor4, NULL);
static DEVICE_ATTR(temp_sensor_5,S_IRUGO,get_temp_sensor5, NULL);
static DEVICE_ATTR(temp_sensor_6,S_IRUGO,get_temp_sensor6, NULL);
static DEVICE_ATTR(temp_sensor_7,S_IRUGO,get_temp_sensor7, NULL);
static DEVICE_ATTR(temp_sensor_8,S_IRUGO,get_temp_sensor8, NULL);
static DEVICE_ATTR(temp_sensor_9,S_IRUGO,get_temp_sensor9, NULL);
static DEVICE_ATTR(temp_sensor_10,S_IRUGO,get_temp_sensor10, NULL);
static DEVICE_ATTR(temp_sensor_11,S_IRUGO,get_temp_sensor11, NULL);

static DEVICE_ATTR(fan_tray_1_speed,S_IRUGO,get_fan_tray_1_speed, NULL);
static DEVICE_ATTR(fan_tray_2_speed,S_IRUGO,get_fan_tray_2_speed, NULL);
static DEVICE_ATTR(fan_tray_3_speed,S_IRUGO,get_fan_tray_3_speed, NULL);
static DEVICE_ATTR(fan_tray_4_speed,S_IRUGO,get_fan_tray_4_speed, NULL);


static struct attribute *s6100_lpc_attrs[] = {
    &dev_attr_smf_ver.attr,
    &dev_attr_max_num_temp_sensors.attr,
    &dev_attr_max_num_fan_trays.attr,
    &dev_attr_max_num_fans_per_tray.attr,
    &dev_attr_fan_tray_presence.attr,
    &dev_attr_max_num_psu.attr,
    &dev_attr_psu1_status.attr,
    &dev_attr_psu2_status.attr,
    &dev_attr_psu_total_pwr.attr,
    &dev_attr_psu1_max_pwr.attr,
    &dev_attr_psu2_max_pwr.attr,
    &dev_attr_sys_led.attr,
    &dev_attr_temp_sensor_1.attr,
    &dev_attr_temp_sensor_2.attr,
    &dev_attr_temp_sensor_3.attr,
    &dev_attr_temp_sensor_4.attr,
    &dev_attr_temp_sensor_5.attr,
    &dev_attr_temp_sensor_6.attr,
    &dev_attr_temp_sensor_7.attr,
    &dev_attr_temp_sensor_8.attr,
    &dev_attr_temp_sensor_9.attr,
    &dev_attr_temp_sensor_10.attr,
    &dev_attr_temp_sensor_11.attr,
    &dev_attr_fan_tray_1_speed.attr,
    &dev_attr_fan_tray_2_speed.attr,
    &dev_attr_fan_tray_3_speed.attr,
    &dev_attr_fan_tray_4_speed.attr,
    NULL,
};

static struct attribute_group s6100_lpc_attr_grp = {
    .attrs = s6100_lpc_attrs,
};


static int dell_s6100_lpc_drv_probe(struct platform_device *pdev)
{
    struct resource *res;
    int ret =0;

    res = platform_get_resource(pdev, IORESOURCE_MEM,0);
    if (unlikely(!res)) {
        printk(KERN_ERR " Specified Resource Not Available... 1\n");
        return -1;
    }

    printk(KERN_INFO "Start:%x,  End:%x Size:%d\n", (unsigned long)res->start, 
            (unsigned long)res->end, resource_size(res));

    //Converting the physical Address to virtual for using in driver 
    mmio = ioremap(res->start, resource_size(res));
    if (unlikely(!mmio)) {
        printk(KERN_ERR " (cannot map IO)\n");
        return;
    }

    printk(KERN_INFO "\n Registering Sysfs :%d\n");
    /* Register sysfs hooks */
    ret = sysfs_create_group(&pdev->dev.kobj, &s6100_lpc_attr_grp);
    if (ret) {
        printk(KERN_ERR "Cannot create sysfs\n");
    }

    return 0;  
}

static int dell_s6100_lpc_drv_remove(struct platform_device *pdev)
{

    sysfs_remove_group(&pdev->dev.kobj, &s6100_lpc_attr_grp);

    return 0;
}

static struct platform_driver dell_s6100_lpc_drv = {
    .probe          = dell_s6100_lpc_drv_probe,
    .remove         = __exit_p(dell_s6100_lpc_drv_remove),
    .driver = {
        .name  = DRIVER_NAME,
    },
};

int dell_s6100_lpc_init(void)
{
    platform_device_register(&dell_s6100_lpc_dev);
    platform_driver_register(&dell_s6100_lpc_drv);
    return 0;
}

void dell_s6100_lpc_exit(void)
{
    platform_device_unregister(&dell_s6100_lpc_dev);
    platform_driver_unregister(&dell_s6100_lpc_drv);
    return;
}

module_init(dell_s6100_lpc_init);
module_exit(dell_s6100_lpc_exit);

MODULE_AUTHOR("Srideep Devireddy  <srideep_devireddy@dell.com>");
MODULE_DESCRIPTION("dell_s6100_lpc driver");
MODULE_LICENSE("GPL");

