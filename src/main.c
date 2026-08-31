void *producer(void *arg)
{
    InputFile input;
    FileArgs *args = (FileArgs *)arg;

    if (input_file_open(&input, args->filename) != 0) {
        return NULL;
    }

    float x_coord;
    float y_coord;

    while (input_file_read(&input, &x_coord, &y_coord)) {

        Message msg = {0};

        /*
         * TODO:
         * Convert the coordinates into a transport message.
         */
        memcpy(msg.data, &x_coord, sizeof(float));
        memcpy(msg.data + sizeof(float), &y_coord, sizeof(float));
        msg.length = 2 * sizeof(float);

        /*
         * TODO:
         * Store the message in the shared buffer safely.Make sure only one message is being stored to avoid duplicates
         * in the buffer. 
         */
        if (message_queue_push(&queue, &msg) != 0) {
            continue;
        }

        /*
         * TODO:
         * Notify waiting consumers.
         */
        pthread_mutex_lock(&message_mutex);
        pthread_cond_broadcast(&message_available);
        pthread_mutex_unlock(&message_mutex);
    }

    /*
     * TODO:
     * Notify consumers that production has finished.
     */
    input_file_close(&input);

    Message end_msg = {0};
    end_msg.length = 0;

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        message_queue_push(&queue, &end_msg);
    }

    pthread_mutex_lock(&message_mutex);
    producer_finished = 1;
    pthread_cond_broadcast(&message_available);
    pthread_mutex_unlock(&message_mutex);

    return NULL;
}

void *consumer(void *arg)
{
    int id = *(int *)arg;

    while (1) {

        Message msg;

        /*
         * TODO:
         * Wait for a new message.
         */

        /*
         * TODO:
         * Safely retrieve the message from the
         * shared buffer.
         */
        if (message_queue_pop(&queue, &msg) != 0) {
            break;
        }

        if (msg.length == 0) {
            break;
        }

        float x_coord;
        float y_coord;

        /*
         * TODO:
         * Decode the message.
         */
        if (msg.length >= 2 * sizeof(float)) {
            memcpy(&x_coord, msg.data, sizeof(float));
            memcpy(&y_coord, msg.data + sizeof(float), sizeof(float));

            /*
             * TODO:
             * Forward the message to the drive queue.
             * make sure the message is shown only once to avoid duplicate entries .
             */
            printf("Consumer %d received: %.2f %.2f\n", id, x_coord, y_coord);
        }
    }

    return NULL;
}

void *drive_write(void *arg){
  FileArgs *args=(FileArgs *) arg;
  InputFile input;
  if (input_file_open(&input, args->filename) != 0) {
    printf("Failed to open %s\n", args->filename);
    return NULL;
  }

  /*
   * TODO :
   * recieve the coordinate message
   * contruct the targte coordinate
   * maintain rover state
   * invoke the drive_to_target function in drive.c
   *
  */
  struct rover_state rover = {
      .position = { .latitude = 0.0f, .longitude = 0.0f, .altitude = 0.0f },
      .heading_rad = 0.0f
  };

  float x_coord;
  float y_coord;
  int final_status = DRIVE_REACHED_TARGET;

  while (input_file_read(&input, &x_coord, &y_coord)) {

    struct coordinate coordinate_target;
    coordinate_target.latitude = x_coord;
    coordinate_target.longitude = y_coord;
    coordinate_target.altitude = 0.0f;

    int status = drive_to_target(&rover, &coordinate_target);

    float dx=coordinate_target.latitude-rover.position.latitude;
    float dy=coordinate_target.longitude-rover.position.longitude;
    float error =hypotf(dx,dy);

    final_status = status;

    input_file_write(&input,&rover.position.latitude,&rover.position.longitude,&error,&status);
  }

  input_file_close(&input);

  if(final_status ==DRIVE_REACHED_TARGET){
      printf("Success \n");
  }else {
      printf("Failed try again \n");
  }
  return NULL;
}
