#include <PubSubClient.h>    // header file that allows to send and receive MQTT messages. 
#include <ESP8266WiFi.h>     // provides ESP8266 specific Wi-Fi routines that we are calling to connect to the network.
#include <SoftwareSerial.h>  // allow serial communication on other digital pins 


#define D5 (14)   //Digital pin used as RX
#define D6 (12)   //Digital pin used as TX
#define TX (1)

#define BAUD_RATE 9600

int val;
char ssid[] = "Danish";                                 // Change this to your network SSID (name).
char pass[] = "4164346666";                                // Change this to your network password.
const char* server = "mqtt.thingspeak.com";
char mqttUserName[] = "Train_Crossing_1";                // Use any name.
char mqttPass[] = "YWL5WV1LNGH3YWVX";                 // Change to your MQTT API key from Account > MyProfile.
long readChannelID=1244830;
char readAPIKey[]="BB240NJHSQA7Q6UA";
long writeChannelID=1244830;
char writeAPIKey[]   = "7Z4KE6LK8QE04Y9J";

WiFiClient client;                                    // Initialize the Wi-Fi client library.
PubSubClient mqttClient( client );                    // Initialize the PuBSubClient library.

int fieldsToPublish[8]={1,1,1,1,0,0,0,0};             // Change to allow multiple fields.
// f1: rssi, f2: train arrived, f3: train dep
//f4: panic button 
float dataToPublish[8];                               // Holds your field data.
int changeFlag=0;                                     // Let the main loop know ther is new data to set.

SoftwareSerial swSer;   //object of a structure


//  
// Prototypes
//

// Handle messages from MQTT subscription.
int mqttSubscriptionCallback(char* topic, byte* payload, unsigned int length);  

// Generate a unique client ID and connect to MQTT broker.
void mqttConnect();  

// Subscribe to a field or feed from a ThingSpeak channel.
int mqttSubscribe( long subChannelID,int field,char* readKey, int unSub);

// Publish messages to a channel feed.
void mqttPublish(long pubChannelID, char* pubWriteAPIKey, float dataArray[], int fieldArray[]);

// Connect to a given Wi-Fi SSID
int connectWifi();

// Measure the Wi-Fi signal strength.
void updateRSSIValue();

// Build a random client ID for MQTT connection.
void getID(char clientID[], int idLength);




void setup() {
 
  Serial.begin(9600);
  Serial.println( "Start" );
    // Important: the buffer size optimizations here, in particular the isrBufSize (11) that is only sufficiently
  // large to hold a single 8N1 word, are on the basis that any char written to the loopback SoftwareSerial
  // adapter gets read before another write is performed.
  // Block writes with a size greater than 1 would usually fail.
  swSer.begin(BAUD_RATE, SWSERIAL_8N1, D5, D6, false, 95, 11);

  
  int status = WL_IDLE_STATUS; // Set temporary Wi-Fi status.
         
     // connectWifi();  // Connect to Wi-Fi network.
 WiFi.mode(WIFI_STA);              //wife mode, station and hotspot
  WiFi.begin(ssid, pass);       //setting user name and password and connecting to wifi
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);         //waiting to make a connection
    
  Serial.print(".");
  }
Serial.println("Wifi Connected!!");   //connection established


     
      mqttClient.setServer( server, 1883 ); // Set the MQTT broker details.
      mqttClient.setCallback( mqttSubscriptionCallback );   // Set the MQTT message handler function.
}

void loop() {
   
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
   
    if (!mqttClient.connected())
    {
       
       mqttConnect(); // Connect if MQTT client is not connected.
       
         if(mqttSubscribe( readChannelID,1,readAPIKey,0 )==1 ){
                Serial.println( " Subscribed " );
            }
    }
   
    mqttClient.loop(); // Call the loop to maintain connection to the server.                        

if(swSer.available())        //check wether software serial pins are availabe to read/write
  {
 val = swSer.read();          // read serial data
 yield();

  // if train arrives then publish a message
 if(val == 65)                
 {
    long rssi = WiFi.RSSI();   //function to read the signal strength (RSSI-Received Signal Strength Indication )
    Serial.print("RSSI:");     //(RSSI-Received Signal Strength Indication )
    Serial.println(rssi);
    Serial.println( " mqtt_publishing... " );
    dataToPublish[1] = 1;
    dataToPublish[2] = 0;
    dataToPublish[3] = 0;
    mqttPublish( writeChannelID, writeAPIKey, dataToPublish, fieldsToPublish );
 }
 // if train departs publish a message
 
  if(val == 68)                
 {

    long rssi = WiFi.RSSI();   //function to read the signal strength (RSSI-Received Signal Strength Indication )
    Serial.print("RSSI:");     //(RSSI-Received Signal Strength Indication )
    Serial.println(rssi);
    dataToPublish[1] = 0;
    dataToPublish[2] = 1;
    dataToPublish[3] = 0;
    Serial.println( " mqtt_publishing... " );
    mqttPublish( writeChannelID, writeAPIKey, dataToPublish, fieldsToPublish );
 }
  
  //if PANIC button is pressed then publish a message
  if(val == 112)                
 {
    long rssi = WiFi.RSSI();   //function to read the signal strength (RSSI-Received Signal Strength Indication )
    Serial.print("RSSI:");     //(RSSI-Received Signal Strength Indication )
    Serial.println(rssi);
    dataToPublish[1] = 0;
    dataToPublish[2] = 0;
    dataToPublish[3] = 1;
    Serial.println( " mqtt_publishing... " );
    mqttPublish( writeChannelID, writeAPIKey, dataToPublish, fieldsToPublish );
 }
    delay(10);
}
}

/**
 * Process messages received from subscribed channel via MQTT broker.
 *   topic - Subscription topic for message.
 *   payload - Field to subscribe to. Value 0 means subscribe to all fields.
 *   mesLength - Message length.
 */

int mqttSubscriptionCallback( char* topic, byte* payload, unsigned int mesLength ) {
   
    char p[mesLength + 1];
    memcpy( p, payload, mesLength );
    p[mesLength] = NULL;
    Serial.print( "Answer: " );
    Serial.println( String(p) );
    
   }

void mqttConnect()
{
    char clientID[ 9 ];
   
    // Loop until connected.
    while ( !mqttClient.connected() )
    {

        getID(clientID,8);
       
        // Connect to the MQTT broker.
        Serial.print( "Attempting MQTT connection..." );
        if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) )
        {
            Serial.println( "Connected with Client ID:  " + String( clientID ) + " User "+ String( mqttUserName ) + " Pwd "+String( mqttPass ) );
           
        } else
        {
            Serial.print( "failed, rc = " );
            // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
            Serial.print( mqttClient.state() );
            Serial.println( " Will try again in 5 seconds" );
            delay( 5000 );
        }
    }
}

/**
 * Build a random client ID.
 *   clientID - Character array for output
 *   idLength - Length of clientID (actual length is one character longer for NULL)
 */

void getID(char clientID[], int idLength){
static const char alphanum[] ="0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";                        // For random generation of the client ID.

    // Generate client ID.
    for (int i = 0; i < idLength ; i++) {
        clientID[ i ] = alphanum[ random( 51 ) ];
    }
    clientID[ idLength ] = '\0';
   
}

/**
 * Subscribe to fields of a channel.
 *   subChannelID - Channel to subscribe to.
 *   field - Field to subscribe to. Value 0 means subscribe to all fields.
 *   readKey - Read API key for the subscribe channel.
 *   unSub - Set to 1 for unsubscribe.
 */
 
int mqttSubscribe( long subChannelID, int field, char* readKey, int unsubSub ){
    String myTopic;
   
    // There is no field zero, so if field 0 is sent to subscribe to, then subscribe to the whole channel feed.
    if (field==0){
        myTopic="channels/"+String( subChannelID )+"/subscribe/json/"+String( readKey );
    }
    else{
        myTopic="channels/"+String( subChannelID )+"/subscribe/fields/field"+String( field )+"/"+String( readKey );
    }
   
    Serial.println( "Subscribing to " +myTopic );
    Serial.println( "State= " + String( mqttClient.state() ) );

    if ( unsubSub==1 ){
        return mqttClient.unsubscribe(myTopic.c_str());
    }
    return mqttClient.subscribe( myTopic.c_str() ,0 );
   
}

/**
 * Unsubscribe channel
 *   subChannelID - Channel to unsubscribe from.
 *   field - Field to unsubscribe subscribe from. The value 0 means subscribe to all fields.
 *   readKey - Read API key for the subscribe channel.
 */

int mqttUnSubscribe(long subChannelID,int field,char* readKey){
    String myTopic;
   
    if (field==0){
        myTopic="channels/"+String( subChannelID )+"/subscribe/json/"+String( readKey );
    }
    else{
        myTopic="channels/"+String( subChannelID )+"/subscribe/fields/field"+String( field )+"/"+String( readKey );
    }
    return mqttClient.unsubscribe( myTopic.c_str() );
   
}

/**
 * Publish to a channel
 *   pubChannelID - Channel to publish to.
 *   pubWriteAPIKey - Write API key for the channel to publish to.
 *   dataArray - Binary array indicating which fields to publish to, starting with field 1.
 *   fieldArray - Array of values to publish, starting with field 1.
 */

void mqttPublish(long pubChannelID, char* pubWriteAPIKey, float dataArray[], int fieldArray[]) {
    int index=0;
    String dataString="";
   
    updateRSSIValue();  // Make sure the stored value is updated.
   
    //
    while (index<8){
       
        // Look at the field array to build the posting string to send to ThingSpeak.
        if (fieldArray[ index ]>0){
         
            dataString+="&field" + String( index+1 ) + "="+String( dataArray [ index ] );
        }
        index++;
    }
   
    Serial.println( dataString );
   
    // Create a topic string and publish data to ThingSpeak channel feed.
    String topicString ="channels/" + String( pubChannelID ) + "/publish/"+String( pubWriteAPIKey );
    mqttClient.publish( topicString.c_str(), dataString.c_str() );
    Serial.println( "Published to channel " + String( pubChannelID ) );
}

int connectWifi()
{
   
    while ( WiFi.status() != WL_CONNECTED ) {
        WiFi.begin( ssid, pass );
        delay( 500 );
        Serial.println( "Connecting to Wi-Fi" );
    }
    Serial.println( "Connected" );

}

void updateRSSIValue(){

   long rssi = WiFi.RSSI();  
   Serial.print( "RSSI:" );
   Serial.println(rssi);
    dataToPublish[ 0]=float( rssi );  //[DATA_FIELD removed to 1]

}
