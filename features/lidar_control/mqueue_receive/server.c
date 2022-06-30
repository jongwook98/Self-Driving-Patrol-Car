#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main() {
  struct mq_attr attr;
  attr.mq_maxmsg = 200;
  attr.mq_msgsize = 512;
  mqd_t mq;

  mq = mq_open("/T_lidar", O_RDWR | O_CREAT, 0666, &attr);

  while (1) {
    char buf[512] = {
        0,
    };

    if (mq == -1) {
      perror("message queue open error");
      exit(1);
    }
    if ((mq_receive(mq, buf, attr.mq_msgsize, NULL)) == -1) {
      perror("mq_receive error");
      exit(-1);
    }
    printf("mq received : %s\n", buf);

    char *ptr = strtok(buf, ",");

    char size_mq_arr[3];
    int int_size_mq;
    memcpy(size_mq_arr, &ptr[0], 2);
    size_mq_arr[2] = 0;

    int_size_mq = atoi(size_mq_arr) + 1;
    printf("\nsize : %d \n", int_size_mq);

    double *angle_arr = (double *)calloc(int_size_mq, sizeof(double));
    double *dis_arr = (double *)calloc(int_size_mq, sizeof(double));

    for (int i = 0; i < int_size_mq; i++) {
      if (strstr(ptr, "dis") != NULL) {
        char angle[7];
        char dis[8];

        memcpy(angle, &ptr[8], 6);
        angle[6] = 0;
        memcpy(dis, &ptr[22], 7);
        dis[7] = 0;

        angle_arr[i] = atof(angle);
        dis_arr[i] = atof(dis);
        // printf("angle : %.2lf , dis : %.2lf \n", atof(angle), atof(dis));
      } else
        i--;

      if (ptr == NULL)
        break;

      ptr = strtok(NULL, ",");
    }
    for (int j = 0; j < int_size_mq; j++) {
      printf("angle : %.2lf ", angle_arr[j]);
      printf("dis : %.2lf \n", dis_arr[j]);
    }

    free(angle_arr);
    free(dis_arr);
  }
  return 0;
}
