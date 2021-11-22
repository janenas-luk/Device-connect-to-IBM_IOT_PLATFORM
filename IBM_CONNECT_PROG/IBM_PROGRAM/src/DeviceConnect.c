#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include "DeviceConnect.h"
#include "iotp_device.h"
#include <libubus.h>
#include "invoke.h"
#include <syslog.h>
#include <libubox/blobmsg_json.h>
#include <sys/resource.h>

volatile int interrupt = 0;


void logsclose()
{
    closelog();
    exit(1);
}

void CleanAll(IoTPDevice *device,IoTPConfig *config)
{
   /* Destroy client */
    IoTPDevice_destroy(device);
    /* Clear configuration */
    IoTPConfig_clear(config);
    logsclose();
}

/* Usage text */
void usage(void) {
    syslog(LOG_ERR, "Usage: \"orgId\" \"TypeId\" \"deviceId\" \"authToken\"");
    exit(1);
}

/* Signal handler - to support CTRL-C to quit */
void sigHandler(int signo) {
    signal(SIGINT, NULL);
    syslog(LOG_INFO, "Received signal: %d\n", signo);
    interrupt = 1;
}

void MQTTTraceCallback (int level, char * message)
{
    if ( level > 0 )
        syslog(LOG_INFO, "%s\n", message? message:"NULL");
    
}



void CreateConfigs(IoTPConfig **config,char *argv[])
{
    int rc=0;
    rc = IoTPConfig_create(config, NULL);
        ConfigsCheck(rc,*config);
    rc=IoTPConfig_setProperty(*config, "identity.orgId" , argv[1]);
       ConfigsCheck(rc,*config);
    rc=IoTPConfig_setProperty(*config, "identity.typeId" , argv[2]);
        ConfigsCheck(rc,*config);
    rc=IoTPConfig_setProperty(*config, "identity.DeviceId" , argv[3]);
        ConfigsCheck(rc,*config);
    rc=IoTPConfig_setProperty(*config, "auth.token" , argv[4]);
        ConfigsCheck(rc,*config);
}
void ConfigsCheck(int rc,IoTPConfig *config)
{
    if(rc!=0)
        {
            syslog(LOG_ERR, "WARN: Failed toinitialize configuration: rc=%d\n", rc);
            IoTPConfig_clear(config);
            logsclose();
            
        }
}

void DeviceCreate(IoTPDevice **device,IoTPConfig *config)
{
    int rc =0;
    rc = IoTPDevice_create(device, config);
    if ( rc != 0 ) {
        syslog(LOG_ERR, "ERROR: Failed to configure IoTP device: rc= %d\n", rc);
          printf("ERROR: Failed to configure IoTP device: rc= %d\n",rc);
        CleanAll(*device,config);
    }
}


void Deviceconnect(IoTPDevice **device,IoTPConfig *config)
{
    int rc = 0;
    rc = IoTPDevice_connect(*device);
    if ( rc != 0 ) {
        syslog(LOG_ERR, "ERROR: Failed to connect to Watson IoT Platform: rc=%d\n", rc);
        syslog(LOG_ERR, "ERROR: Returned error reason: %s\n", IOTPRC_toString(rc));
        CleanAll(*device,config);
    }
}


void DisconnectDevice(IoTPDevice *device,IoTPConfig *config)
{
    int rc=0;
    rc = IoTPDevice_disconnect(device);
    if ( rc != IOTPRC_SUCCESS ) {
        syslog(LOG_ERR, "ERROR: Failed to disconnect from  Watson IoT Platform: rc=%d\n", rc);
        CleanAll(device,config);
    }
}


void DeviceSendEvent(IoTPDevice *device,IoTPConfig *config)
{   
    int rc=0;
    struct ubus_context *ctx;
    struct MemoryD memory={0,0};
    if(ConnectUbus(&ctx)!=0)
    {
        syslog(LOG_ERR, "Failed to connect to ubus\n");
        CleanAll(device,config);
    }
    while(!interrupt)
    {
        char data[100];
        if(getmemoryinfo(&ctx, &memory)!=0)
        {
            syslog(LOG_ERR, "cannot request memory info from invoke\n");
            CleanAll(device,config);
        }
      
       
         sprintf(data,"{\"Memory usage\":\"%0.2f MB / %0.2f MB\"}",((memory.total_memory-memory.free_memory)/1000000.0),memory.total_memory/1000000.0);
         rc = IoTPDevice_sendEvent(device,"status", data, "json", QoS0, NULL);
         syslog(LOG_INFO,"RC from DeviceSendEvent(): %d\n",rc);


    }
    ubus_free(ctx);
}   
int main(int argc,char *argv[])
{

    IoTPConfig *config = NULL;
    IoTPDevice *device = NULL;
    syslog(LOG_INFO,"Organization id: %s, Device type: %s, Device ID: %s, Auth_Token: %s\n",argv[1],argv[2],argv[3],argv[4]);

     /* check for args */
    if ( argc !=5 )
        usage();
 
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    /* Set signal handlers */
    CreateConfigs(&config,argv);

   
    DeviceCreate(&device,config);
    Deviceconnect(&device,config);
  
   
    DeviceSendEvent(device,config);
  
    
  
    DisconnectDevice(device,config);
    CleanAll(device,config);
    return 0;

}
