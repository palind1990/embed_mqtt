#include <stdio.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port


#define ADDRESS "tcp://rpithib.local:1883"
#define CLIENTID "ExampleClientpub"
#define TOPIC "MQTT"
#define QOS 1
#define TIMEOUT 10000L
int file_i2c;
int length;
unsigned char buf[10];
unsigned int temp;
int address = 0x4a;  
char PAYLOAD[10];                      // Address of TP102

float read_temp(int address, char bus)
{
    unsigned int temp;
    unsigned char msb, lsb;

    int fd; // File descriptor
    char *fileName;
    unsigned char buf[10]; // Buffer for data being read/ written on the i2c bus

    if (bus == 1)
        fileName = "/dev/i2c-1"; // Name of the port we will be using
    else
        fileName = "/dev/i2c-0";

    if ((fd = open(fileName, O_RDWR)) < 0)
    { // Open port for reading and writing
        printf("Failed to open i2c port\n");
        exit(1);
    }

    if (ioctl(fd, I2C_SLAVE, address) < 0)
    { // Set the port options and set the address of the device we wish to speak to
        printf("Unable to get bus access to talk to slave\n");
        exit(1);
    }

    buf[0] = 0; // This is the register we wish to read from

    if ((write(fd, buf, 1)) != 1)
    { // Send register to read from
        printf("Error writing to i2c slave\n");
        exit(1);
    }

    if (read(fd, buf, 1) != 1)
    { // Read back data into buf[]
        printf("Unable to read from slave\n");
        exit(1);
    }
    else
    {
        temp = buf[0];
        printf("The temperature is: %d °C\n",temp);
        sprintf(PAYLOAD, "%d", temp);
        return temp;
    }
}





int main(int argc, char *argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    read_temp(address,1);

    if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID,
        MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
         printf("Failed to create client, return code %d\n", rc);
         exit(EXIT_FAILURE);
    }

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    pubmsg.payload = PAYLOAD;
    pubmsg.payloadlen = (int)strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    if ((rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    {
         printf("Failed to publish message, return code %d\n", rc);
         exit(EXIT_FAILURE);
    }

    printf("Waiting for up to %d seconds for publication of %s\n"
            "on topic %s for client with ClientID: %s\n",
            (int)(TIMEOUT/1000), PAYLOAD , TOPIC, CLIENTID);
    rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    printf("Message with delivery token %d delivered\n", token);

    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    	printf("Failed to disconnect, return code %d\n", rc);
    MQTTClient_destroy(&client);
    return rc;
}