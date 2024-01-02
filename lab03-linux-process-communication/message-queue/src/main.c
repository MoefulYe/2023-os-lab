#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>
#include <unistd.h>

#define MQ_KEY 0x1234
#define MAX_MSG_SIZE 1024

sem_t sender_mutex;
sem_t over0;
sem_t over1;

typedef enum : long int {
  Any = 0,
  ToReceiver = 1,
  AcktoSender0 = 2,
  AcktoSender1 = 3,
} Type;

char *type_to_string(Type type) {
  switch (type) {
  case Any:
    return "Any";
  case ToReceiver:
    return "to receiver";
  case AcktoSender0:
    return "ack to sender0";
  case AcktoSender1:
    return "ack to sender1";
  }
}

typedef struct {
  Type type;
  char payload[MAX_MSG_SIZE - sizeof(Type)];
} Message;

Message msg_end0() {
  Message msg;
  msg.type = ToReceiver;
  strcpy(msg.payload, "end0\n");
  return msg;
}

Message msg_end1() {
  Message msg;
  msg.type = ToReceiver;
  strcpy(msg.payload, "end1\n");
  return msg;
}

Message msg_ack0() {
  Message msg;
  msg.type = AcktoSender0;
  strcpy(msg.payload, "over\n");
  return msg;
}

Message msg_ack1() {
  Message msg;
  msg.type = AcktoSender1;
  strcpy(msg.payload, "over\n");
  return msg;
}

_Bool msg_is_end0(Message *msg) {
  return msg->type == ToReceiver && strcmp(msg->payload, "end0\n") == 0;
}

_Bool msg_is_end1(Message *msg) {
  return msg->type == ToReceiver && strcmp(msg->payload, "end1\n") == 0;
}

Message msg_new(Type send_to, char *payload) {
  Message msg;
  msg.type = send_to;
  strcpy(msg.payload, payload);
  return msg;
}

void msg_print(Message *msg) {
  printf("type: %s, \n payload: %s\n", type_to_string(msg->type), msg->payload);
}

_Bool is_exit(char *str) { return strcmp(str, "exit\n") == 0; }

void *sender0() {
  key_t mq = msgget(MQ_KEY, IPC_CREAT | 0666);
  assert(mq != -1);
  for (;;) {
    sem_wait(&sender_mutex);
    printf("sender0> ");
    char *line;
    size_t len;
    len = getline(&line, &len, stdin);
    sem_post(&sender_mutex);
    if (is_exit(line)) {
      Message msg = msg_end0();
      msgsnd(mq, &msg, MAX_MSG_SIZE, 0);
      break;
    }
    Message msg = msg_new(ToReceiver, line);
    msgsnd(mq, &msg, MAX_MSG_SIZE, 0);
  }
  sem_wait(&over0);
  Message msg;
  msgrcv(MQ_KEY, &msg, MAX_MSG_SIZE, AcktoSender0, 0);
  printf("sender1 received ack as follow:\n");
  msg_print(&msg);
  return NULL;
}

void *sender1() {
  key_t mq = msgget(MQ_KEY, IPC_CREAT | 0666);
  assert(mq != -1);
  for (;;) {
    sem_wait(&sender_mutex);
    printf("sender1> ");
    char *line;
    size_t len;
    len = getline(&line, &len, stdin);
    sem_post(&sender_mutex);
    if (is_exit(line)) {
      Message msg = msg_end1();
      msgsnd(mq, &msg, MAX_MSG_SIZE, 0);
      break;
    }
    Message msg = msg_new(ToReceiver, line);
    msgsnd(mq, &msg, MAX_MSG_SIZE, 0);
  }
  sem_wait(&over1);
  Message msg;
  msgrcv(MQ_KEY, &msg, MAX_MSG_SIZE, AcktoSender1, 0);
  printf("sender1 received ack as follow:\n");
  msg_print(&msg);
  return NULL;
}

void *receiver() {
  key_t mq = msgget(MQ_KEY, IPC_CREAT | 0666);
  int done = 0;
  for (;;) {
    sleep(5);
    Message msg;
    for (;;) {
      int res = msgrcv(mq, &msg, MAX_MSG_SIZE, ToReceiver, IPC_NOWAIT);
      if (res == -1) {
        break;
      }
      printf("receiver received message as follow:\n");
      msg_print(&msg);
      if (msg_is_end0(&msg)) {
        Message ack = msg_ack0();
        sem_post(&over0);
        msgsnd(mq, &ack, MAX_MSG_SIZE, 0);
        done++;
        if (done == 2) {
          goto exit;
        }
      } else if (msg_is_end1(&msg)) {
        Message ack = msg_ack1();
        sem_post(&over1);
        msgsnd(mq, &ack, MAX_MSG_SIZE, 0);
        done++;
        if (done == 2) {
          goto exit;
        }
      }
    }
  }
exit:
  msgctl(mq, IPC_RMID, NULL);
  return NULL;
}

void init() {
  sem_init(&sender_mutex, 0, 1);
  sem_init(&over0, 0, 0);
  sem_init(&over1, 0, 0);
}

void drop() {
  sem_destroy(&sender_mutex);
  sem_destroy(&over0);
  sem_destroy(&over1);
}

int main(int argc, char *argv[]) {
  init();
  pthread_t sender0_handler, sender1_handler, receiver_handler;
  pthread_create(&sender0_handler, NULL, sender0, NULL);
  pthread_create(&sender1_handler, NULL, sender1, NULL);
  pthread_create(&receiver_handler, NULL, receiver, NULL);
  pthread_join(sender0_handler, NULL);
  pthread_join(sender1_handler, NULL);
  pthread_join(receiver_handler, NULL);
  drop();
  return 0;
}
