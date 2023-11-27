#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/msg.h>

#define MQ_KEY 0x1234
#define MAX_MSG_SIZE 1024

sem_t sender_mutex;
sem_t sync_sender;
sem_t sync_receiver;
sem_t over;

typedef enum : long int {
  Any = 0,
  Sender0toReceiver = 1,
  Sender1toReceiver = 2,
  AcktoSender0 = 3,
  AcktoSender1 = 4,
} Type;

char *type_to_string(Type type) {
  switch (type) {
  case Any:
    return "Any";
  case Sender0toReceiver:
    return "sender0 send to receiver";
  case Sender1toReceiver:
    return "sender1 send to receiver";
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
  msg.type = Sender0toReceiver;
  strcpy(msg.payload, "end0\n");
  return msg;
}

Message msg_end1() {
  Message msg;
  msg.type = Sender1toReceiver;
  strcpy(msg.payload, "end1\n");
  return msg;
}

Message msg_ack0() {
  Message msg;
  msg.type = AcktoSender0;
  strcpy(msg.payload, "over0\n");
  return msg;
}

Message msg_ack1() {
  Message msg;
  msg.type = AcktoSender1;
  strcpy(msg.payload, "over1\n");
  return msg;
}

_Bool msg_is_end0(Message *msg) {
  return msg->type == Sender0toReceiver && strcmp(msg->payload, "end0\n") == 0;
}

_Bool msg_is_end1(Message *msg) {
  return msg->type == Sender1toReceiver && strcmp(msg->payload, "end1\n") == 0;
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
_Bool is_yield(char *str) { return strcmp(str, "yield\n") == 0; }

void *sender0() {
  key_t mq = msgget(MQ_KEY, IPC_CREAT | 0666);
  assert(mq != -1);
  sem_wait(&sender_mutex);
  for (;;) {
    sem_wait(&sync_sender);
    for (;;) {
      printf("sender0> ");
      char *line;
      size_t len;
      len = getline(&line, &len, stdin);
      if (is_exit(line)) {
        Message msg = msg_end0();
        msgsnd(mq, &msg, MAX_MSG_SIZE, 0);
        sem_post(&sync_receiver);
        goto exit;
      } else if (is_yield(line)) {
        break;
      }
      Message msg = msg_new(Sender0toReceiver, line);
      msgsnd(mq, &msg, MAX_MSG_SIZE, 0);
    }
    sem_post(&sync_receiver);
  }
exit:
  sem_wait(&over);
  Message msg;
  msgrcv(MQ_KEY, &msg, MAX_MSG_SIZE, AcktoSender0, 0);
  printf("sender0 received ack as follow:\n");
  msg_print(&msg);
  sem_post(&sync_receiver);
  sem_post(&sender_mutex);
  return NULL;
}

void *sender1() {
  key_t mq = msgget(MQ_KEY, IPC_CREAT | 0666);
  assert(mq != -1);
  sem_wait(&sender_mutex);
  for (;;) {
    sem_wait(&sync_sender);
    for (;;) {
      printf("sender1> ");
      char *line;
      size_t len;
      len = getline(&line, &len, stdin);
      if (is_exit(line)) {
        Message msg = msg_end0();
        msgsnd(mq, &msg, MAX_MSG_SIZE, 0);
        sem_post(&sync_receiver);
        goto exit;
      } else if (is_yield(line)) {
        break;
      }
      Message msg = msg_new(Sender0toReceiver, line);
      msgsnd(mq, &msg, MAX_MSG_SIZE, 0);
    }
    sem_post(&sync_receiver);
  }
exit:
  sem_wait(&over);
  Message msg;
  msgrcv(MQ_KEY, &msg, MAX_MSG_SIZE, AcktoSender0, 0);
  printf("sender1 received ack as follow:\n");
  msg_print(&msg);
  sem_post(&sync_receiver);
  sem_post(&sender_mutex);
  return NULL;
}

void *receiver() {
  key_t mq = msgget(MQ_KEY, IPC_CREAT | 0666);
  int done = 0;
  for (;;) {
    Message msg;
    sem_wait(&sync_receiver);
    for (;;) {
      int res = msgrcv(mq, &msg, MAX_MSG_SIZE, Any, IPC_NOWAIT);
      if (res == -1) {
        break;
      }
      printf("receiver received message as follow:\n");
      msg_print(&msg);
      if (msg_is_end0(&msg)) {
        Message ack = msg_ack0();
        sem_post(&over);
        msgsnd(mq, &ack, MAX_MSG_SIZE, 0);
        done++;
        if (done == 2) {
          goto exit;
        }
      } else if (msg_is_end1(&msg)) {
        Message ack = msg_ack1();
        sem_post(&over);
        msgsnd(mq, &ack, MAX_MSG_SIZE, 0);
        done++;
        if (done == 2) {
          goto exit;
        }
      } else {
        sem_post(&sync_sender);
      }
    }
  }
exit:
  msgctl(mq, IPC_RMID, NULL);
  return NULL;
}

void init() {
  sem_init(&sender_mutex, 0, 1);
  sem_init(&sync_sender, 0, 1);
  sem_init(&sync_receiver, 0, 0);
  sem_init(&over, 0, 0);
}

void drop() {
  sem_destroy(&sender_mutex);
  sem_destroy(&sync_sender);
  sem_destroy(&sync_receiver);
  sem_destroy(&over);
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
