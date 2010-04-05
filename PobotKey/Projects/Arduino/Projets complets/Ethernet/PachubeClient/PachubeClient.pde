/*
 * Arduino ENC28J60 Ethernet shield pachube client
 * Supports CSV method only due to buffer size limitations.
 *
 * Written by Andrew Lindsay, July 2009.
 * TCP/IP code originally from tuxgraphics.org - combined with ethershield library by me.
 *
 * Changes by Julien Holtzer, April 2010.
 * Clean-up for simple Pachube + comments in french
 */

#include "etherShield.h"

// Code compilation options

// define FULLWEB to display just a full webpage with values or a simple 1 line csv output
#define FULLWEB

// ** Local Network Setup **
// Please modify the following lines. mac and ip have to be unique
// in your local area network. You can not have the same numbers in
// two devices:
static uint8_t mymac[6] = {
  0x80,0x79,0x66,0x79,0x84,0x49};

// The IP address of the arduino.
static uint8_t myip[4] = {
  192,168,0,19};

// Default gateway. The ip address of your DSL/Cable router.
static uint8_t gwip[4] = {
  192,168,0,254};

// ** pachube.com setup **
// IP address of the www.pachube.com server to contact (IP of the first portion of the URL):
static uint8_t pachubeip[4] = {
  209,40,205,190};

// The name of the virtual host which you want to contact at pachubeip (hostname of the first portion of the URL):
#define PACHUBE_VHOST "www.pachube.com"

// setup yor feed url and API keys here.
#define PACHUBEAPIURL "/api/1267.csv"
#define PACHUBEAPIKEY "X-PachubeApiKey: 6bdd53f70381ad2c09b99b8c9fdded79e73f06f6127091daea715be2ae686f62"

// End of configuration 

// listen port for tcp/www:
#define MYWWWPORT 80

static volatile uint8_t start_web_client=0;  // 0=off but enabled, 1=send update, 2=sending initiated, 3=update was sent OK, 4=diable updates
static uint8_t web_client_attempts=0;
static uint8_t web_client_sendok=0;
static uint8_t resend=0;

// déclaration des variables envoyézs à Pachube
int temp = 0;
int lumen = 0;
int visitors = 0;

// nb de 30s depuis le démarrage
// remise à zéro à minuit (normalement)
// pour le décompte du nombre de visiteurs
int deminutes; 


// le buffer complet pour le message Ethernet
#define BUFFER_SIZE 650
static uint8_t buf[BUFFER_SIZE+1];

// le buffer pour la partie capteurs
#define STATUS_BUFFER_SIZE 20
static char statusstr[STATUS_BUFFER_SIZE];

// Instantiate the EtherShield class
EtherShield es=EtherShield();

// prepare the webpage by writing the data to the tcp send buffer
uint16_t print_webpage(uint8_t *buf)
{
  uint16_t plen;
  char vstr[5];

  plen = es.ES_fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
#ifdef FULLWEB
  // Display full info 
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("<h1>Temp&eacute;rature pour Pobot chez Julien</h1><pre>\n"));
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("Il fait: "));
  itoa(temp/100,vstr,10);
  plen=es.ES_fill_tcp_data(buf,plen,vstr);
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("."));
  itoa(temp%100,vstr,10);
  plen=es.ES_fill_tcp_data(buf,plen,vstr);
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("&deg;C<br/><br/>"));
  //
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("La luminosit&eacute; est de: "));
  itoa(lumen/10,vstr,10);
  plen=es.ES_fill_tcp_data(buf,plen,vstr);
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("."));
  itoa(lumen%10,vstr,10);
  plen=es.ES_fill_tcp_data(buf,plen,vstr);
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("%<br/><br/>"));  // lien vers pachube
  
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("<img src=\"http://www.pachube.com/feeds/1267/datastreams/0/history.png\">&nbsp;"));
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("<img src=\"http://www.pachube.com/feeds/1267/datastreams/1/history.png\"><br/>"));
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("<a href=\"http://www.pachube.com/feeds/1267/\">Voir le feed sur Pachube</a><br/>"));
  
  // affichage des visiteurs du site  
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR("D&eacute;j&agrave; "));
  itoa(visitors/2,vstr,10);
  plen=es.ES_fill_tcp_data(buf,plen,vstr);
  plen=es.ES_fill_tcp_data_p(buf,plen,PSTR(" visiteurs aujourd'hui!<br/>"));  // lien vers pachube
  
#else  
  // Display just the csv data
  plen=es.ES_fill_tcp_data(buf,plen,statusstr);
#endif
  return(plen);
}

// Browser callback, where we get to after receiving a reply to an update, should really 
// do somthing here to check all was OK.
void browserresult_callback(uint8_t statuscode,uint16_t datapos){

  if (statuscode==0){
    web_client_sendok++;
  }
  // clear pending state at sucessful contact with the
  // web server even if account is expired:
  if (start_web_client==2) start_web_client=3;
}

// Perform setup on etherent and oneWire
void setup(){

  // initialize enc28j60
  es.ES_enc28j60Init(mymac);
  //init the ethernet/ip layer:
  es.ES_init_ip_arp_udp_tcp(mymac,myip, MYWWWPORT);
  // init the web client:
  es.ES_client_set_gwip(gwip);  // e.g internal IP of dsl router
}

// The business end of things
void loop(){
  uint16_t dat_p;
  int8_t cmd;
  start_web_client=1;
  unsigned long lastSend = millis();
  unsigned long time;

  while(1){
    // handle ping and wait for a tcp packet
    dat_p=es.ES_packetloop_icmp_tcp(buf,es.ES_enc28j60PacketReceive(BUFFER_SIZE, buf));
    if(dat_p==0){
      // Nothing received, jus see if there is anythng to do 

      // update every 30s
      time = millis();
      if( time > (lastSend + 30000) ) {
        resend=1; // resend once if it failed
        // déclenche l'envoi à Pachube
        start_web_client=1;
        // met à jour le chrono pour les 30 secondes suivantes
        lastSend = time;
        // incrémente le compteur de demi-minutes
        deminutes++;
        if (deminutes > 2880) {
          // on démarre une nouvelle journée !
          deminutes = 0;
          // on repart à 0 pour les visiteurs
          visitors = 0; 
        }
      }

      // lorsqu'un envoi à Pachube est demandé
      if (start_web_client==1) {
        start_web_client=2;
        web_client_attempts++;

        // Ici, la mise à jour des valeurs pour Pachube
        temp = 2550;
        lumen = analogRead(0);
        
        // Pachube update
        sprintf( statusstr, "%d.%d,%d.%d,%d", temp / 100, temp % 100,lumen / 10, lumen % 10, visitors/2);
        es.ES_client_set_wwwip(pachubeip);
        es.ES_client_http_post(PSTR(PACHUBEAPIURL),PSTR(PACHUBE_VHOST),PSTR(PACHUBEAPIKEY), PSTR("PUT "), statusstr ,&browserresult_callback);
      }
      continue;
    }
    
    // si on arrive ici, c'est qu'un visiteur a demandé la page
    visitors++;
    lumen = analogRead(0);

    dat_p=print_webpage(buf);
    es.ES_www_server_reply(buf,dat_p); // send data
  }
}



