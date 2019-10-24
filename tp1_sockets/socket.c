#define _POSIX_C_SOURCE 200112L

#include "socket.h"

//Esta función conecta el cliente al servidor
void skt_connect(socket_t *self, char *hostname, char *port){
   int status; //para resolución de dirección
   struct addrinfo hints; //configuración conexión
   struct addrinfo *res; //lista de estructuras de direcciones

   self->status = SKT_CONNECTED;

   //configuro cliente
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_INET; //IPv4
   hints.ai_socktype = SOCK_STREAM; //TCP
   hints.ai_flags = 0; //server

   status = getaddrinfo(hostname, port, &hints, &res);
   if (status != 0)
      self->status = SKT_CONNECTION_FAILURE;

   self->skt = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

   if (self->skt == -1)
      self->status = SKT_CONNECTION_FAILURE;

   status = connect(self->skt, res->ai_addr, res->ai_addrlen);
   if (status == -1){
      close(self->skt);
      self->status = SKT_CONNECTION_FAILURE;
   }

   freeaddrinfo(res);
}

//Esta función crea un servidor
void skt_create(socket_t *self, char *port){
   int c = 1;
   int status; //para resolución de dirección
   struct addrinfo hints; //configuración conexión
   struct addrinfo *res; //lista de estructuras de direcciones

   //configuro servidor
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_INET; //IPv4
   hints.ai_socktype = SOCK_STREAM; //TCP
   hints.ai_flags = AI_PASSIVE; //server
   status = getaddrinfo(NULL, port, &hints, &res);

   if (status != 0)
      self->status = SKT_CREATE_FAILURE;
   self->skt = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
   if (self->skt == -1){
      freeaddrinfo(res); //libera info de lista de direcciones
      self->status = SKT_CREATE_FAILURE;
   }
   status = setsockopt(self->skt, SOL_SOCKET, SO_REUSEADDR, &c, sizeof(&c));
   if (status == -1){
      self->status = SKT_CREATE_FAILURE;
   }
   skt_bind(self, res);
   skt_listen(self);
   freeaddrinfo(res);
}

//Configura cantidad de clientes en espera
void skt_listen(socket_t *self){  
   int status;

   status = listen(self->skt, WAITING_CLIENTS);
   if (status == -1){
      close(self->skt);
      self->status = SKT_CREATE_FAILURE;
   }
}

//Configuro puerto para que no sea aleatorio
void skt_bind(socket_t *self, addrinfo_t *res){
   int status;
   status = bind(self->skt, res->ai_addr, res->ai_addrlen);
   if (status == -1){
      close(self->skt);
      freeaddrinfo(res); //libera info de lista de direcciones
      self->status = SKT_CREATE_FAILURE;
   }
}

//Esta función espera un cliente y lo acepta si es correcto
void skt_accept(socket_t *self){
   self->skt = accept(self->skt, NULL, NULL);   // aceptamos un cliente
   self->status = SKT_CONNECTED;
   printf("%s\n", MSG_NEW_CLIENT);
}

void skt_destroy(socket_t *self){
   self->status = SKT_SHUTDOWN;
   shutdown(self->skt, SHUT_RDWR);
   close(self->skt);
}

//Esta función recibe un string de largo len en el buffer buf.
int skt_receive(socket_t *self, int len, char *buf){
   bool socket_running = true;
   int bytes_received = 0;
   int status = 0;
   while (socket_running && bytes_received < len) {
      status = recv(self->skt, &buf[bytes_received], len-bytes_received, 0);
      if (status == 0) {
         socket_running = false;
      } else if (status == -1) {
         socket_running = false;
      } else {
         bytes_received = bytes_received + status;
      }
   }
   if ( bytes_received > 0 ) {
      self->status = SKT_RECEPTION_SUCCESS;
      return bytes_received;
   } else if (bytes_received == 0){
      self->status = SKT_RECEPTION_END;
      return -1;
   } else {
      self->status = SKT_RECEPTION_FAILURE;
      return -1;
   }
}

//Esta función envía un string de largo len en el buffer buf.
void skt_send(socket_t *self, int len, char *buf){
   bool socket_running = true;
   int bytes_sent = 0;
   int status = 0;

   while (socket_running && bytes_sent < len) {
      status = send(self->skt, &buf[bytes_sent], len-bytes_sent, 0);
      if (status == 0) {
         socket_running = false;
      } else if (status == -1) {
         socket_running = false;
      } else {
         bytes_sent = bytes_sent + status;
      }
   }
   if (socket_running) {
      self->status = SKT_SEND_SUCCESS;
   } else {
      self->status = SKT_SEND_FAILURE;
   }
}
